/*  ----------------------------------------------------------------
 YDK - YANG Development Kit
 Copyright 2016 Cisco Systems. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 -------------------------------------------------------------------
 This file has been modified by Yan Gorelik, YDK Solutions.
 All modifications in original under CiscoDevNet domain
 introduced since October 2019 are copyrighted.
 All rights reserved under Apache License, Version 2.0.
 ------------------------------------------------------------------*/

#include <algorithm>    // std::all_of
#include <ctype.h>  	// std::isdigit

#include "entity_util.hpp"
#include "logger.hpp"
#include "json.hpp"
#include "common_utilities.hpp"
#include "json_subtree_codec.hpp"

using namespace std;
using json = nlohmann::json;

struct json_node_t
{
    string key;
    json value;
    json_node_t(const string & node_key, const json & jvalue);
    void add(const json_node_t & node);
    void put(const json_node_t & node);
};

json_node_t::json_node_t(const string & node_key, const json & jvalue):
		key(node_key), value(jvalue)
{
}

void json_node_t::add(const json_node_t & node)
{
    auto it = value.find(node.key);
    if (it == value.end())
   	    value.emplace(node.key, node.value);
    else if (node.value.is_primitive()) {
        auto child = value[node.key];
        if (child.is_array())
            it->push_back(node.value);
        else if (child.type() == node.value.type()) {
            auto arr = json::array({ child });
            arr.push_back(node.value);
            it->swap(arr);
        }
    }
}

void json_node_t::put(const json_node_t & node)
{
    value[node.key] += node.value;
}

void to_json(json & j, const json_node_t & node)
{
    j.emplace(node.key, node.value);
}

namespace ydk
{
static void decode_json(json & jchildren, Entity & entity, Entity * parent);

static void walk_children(Entity & entity, const path::SchemaNode & parent_schema, json_node_t & json_node, std::string & parent_module_name);
static json_node_t populate_json_node(Entity & entity, const path::SchemaNode & parent_schema, std::string & parent_module_name);
static void populate_json_node_contents(Entity & entity, const path::SchemaNode & parent_schema, EntityPath & path,
                                        json_node_t & json_node,
                                        std::string & parent_module_name);
static const path::SchemaNode* find_child_by_name(const path::SchemaNode & parent_schema, const string & name);

JsonSubtreeCodec::JsonSubtreeCodec()
{
}

JsonSubtreeCodec::~JsonSubtreeCodec()
{
}

//////////////////////////////////////////////////////////////////
// JsonSubtreeCodec::encode
//////////////////////////////////////////////////////////////////
std::string JsonSubtreeCodec::encode(Entity & entity, path::RootSchemaNode & root_schema, bool pretty)
{
    EntityPath root_path = get_entity_path(entity, nullptr);
    auto & root_data_node = root_schema.create_datanode(root_path.path);
    YLOG_DEBUG("JsonCodec: Encoding entity '{}' to JSON string", entity.get_segment_path());
    const path::SchemaNode & schema = root_data_node.get_schema_node();
    auto st = schema.get_statement();
    std::string root_module_name = st.module_name;
    std::string root_node_name = root_module_name;
    root_node_name += ":" + st.arg;

    json_node_t root_json_node(root_node_name, json{});

    populate_json_node_contents(entity, schema, root_path, root_json_node, root_module_name);
    walk_children(entity, schema, root_json_node, root_module_name);

    if (pretty) {
        return json(root_json_node).dump(2);
    }
    else {
        return json(root_json_node).dump();
    }
}

static void walk_children(Entity & entity, const path::SchemaNode & schema, json_node_t & parent_json_node, string & parent_module_name)
{
    std::map<string, shared_ptr<Entity>> children = entity.get_children();
    YLOG_DEBUG("JsonCodec: Children count for '{}': {}", get_entity_path(entity, entity.parent).path, children.size());
    for (auto const& child : children)
    {
        if(child.second == nullptr)
            continue;
        if (child.second->has_operation() || child.second->has_data() || child.second->is_presence_container)
        {
            YLOG_DEBUG("JsonCodec: Populating child entity node '{}'", child.second->yang_name);
            json_node_t child_json_node = populate_json_node(*(child.second), schema, parent_module_name);
            auto bracket_pos = child.first.find("[");
            if (bracket_pos != string::npos) {
                // We have a list
                parent_json_node.put(child_json_node);
            }
            else {
                // We have a container
                parent_json_node.add(child_json_node);
            }
        }
    }
}

static const path::SchemaNode* find_child_by_name(const path::SchemaNode & parent_schema, const string & name)
{
    auto p = const_cast<path::SchemaNode*>(&parent_schema);
    vector<path::SchemaNode*> s = p->find(name);
    if (s.size() == 0) {
        YLOG_ERROR("Could not find node '{}'", name);
        throw YServiceError{"Could not find node " + name};
    }
    return s[0];
}

static json_node_t create_json_node(std::string & parent_module_name, const path::SchemaNode & schema)
{
    auto st = schema.get_statement();
    std::string child_key = st.arg;
    if (st.module_name != parent_module_name) {
        child_key.insert(0, st.module_name + ":");
    }
    json_node_t child(child_key, json{});
    return child;
}

static json_node_t populate_json_node(Entity & entity, const path::SchemaNode & parent_schema, string & parent_module_name)
{
    EntityPath path = get_entity_path(entity, entity.parent);
    const path::SchemaNode* schema = find_child_by_name(parent_schema, entity.get_segment_path());

    auto child = create_json_node(parent_module_name, *schema);

    auto st = schema->get_statement();
    auto child_module_name = st.module_name;
    populate_json_node_contents(entity, *schema, path, child, child_module_name);
    walk_children(entity, *schema, child, child_module_name);
    return child;
}

static std::string get_leafdata_prefix(Entity & entity, const string & leaf_name, LeafData & leaf_data)
{
    std::string module_name;
	if (leaf_data.name_space.size() > 0 && leaf_data.name_space_prefix.size() > 0)
    {
        Entity* p = &entity;
        while (p->parent != nullptr) {
            p = p->parent;
        }
        auto m = p->get_namespace_identity_lookup();
        if (m.find({leaf_name, leaf_data.name_space}) != m.end()) {
            module_name = m[{leaf_name, leaf_data.name_space}] + ":";
        }
    }
    return module_name;
}

static string get_content_from_leafdata(Entity & entity, const string & leaf_name, LeafData & leaf_data)
{
    string content;
    if (leaf_data.is_set)
    {
    	auto prefix = get_leafdata_prefix(entity, leaf_name, leaf_data);
    	content = (prefix + leaf_data.value);
    }
    else if(is_set(leaf_data.yfilter))
    {
        content = "";
    }
    return content;
}

static json get_json_leaf_value(string & leaf_value)
{
    if (leaf_value == "" || leaf_value == "null")
        return json{};
    else if (leaf_value == "true")
        return json(true);
    else if (leaf_value == "false")
        return json(false);
    else {
        bool is_number = all_of(leaf_value.begin(), leaf_value.end(), ::isdigit);
        if (is_number) {
            long long jint = stoll(leaf_value);
            return json(jint);
        }
        else {
            return json(leaf_value);
        }
    }
}

static void populate_json_node_contents(Entity & entity, const path::SchemaNode & parent_schema, EntityPath & path, json_node_t & json_node, std::string & parent_module_name)
{
    for (const std::pair<string, LeafData> & name_value : path.value_paths)
    {
        LeafData leaf_data = name_value.second;
        const path::SchemaNode* schema = find_child_by_name(parent_schema, name_value.first);

        if (leaf_data.is_set || is_set(leaf_data.yfilter))
        {
            string leaf_name = name_value.first;
            string leaf_type = "leaf";
            auto pos = leaf_name.find("[.=\"");
            if (pos != string::npos) {
            	leaf_data.value = leaf_name.substr(pos+4, leaf_name.length()-pos-6);
            	leaf_name = leaf_name.substr(0, pos);
            	leaf_type = "leaf-list";
            }
            YLOG_DEBUG("JsonCodec: Creating {} node '{}' in '{}' with value: '{}'",
                       leaf_type, leaf_name, entity.yang_name, leaf_data.value);
            string content = get_content_from_leafdata(entity, leaf_name, leaf_data);

            auto st = schema->get_statement();
            auto child_module_name = parent_module_name;
            string child_key = st.arg;
            if (st.module_name != child_module_name) {
                child_key.insert(0, child_module_name + ":");
            }
            json_node_t jleaf(child_key, get_json_leaf_value(content));
            json_node.add(jleaf);
        }
    }
}

//////////////////////////////////////////////////////////////////
// JsonSubtreeCodec::decode
//////////////////////////////////////////////////////////////////

static pair<string,string> split_key(string & key)
{
    auto colon_pos = key.find(":");
    if (colon_pos == string::npos)
        return {"", key};
    else
        return {key.substr(0, colon_pos), key.substr(colon_pos+1)};
}

std::shared_ptr<Entity> JsonSubtreeCodec::decode(const std::string & payload, std::shared_ptr<Entity> entity)
{
    YLOG_DEBUG("JsonCodec: Decoding JSON payload\n{}\nTo Entity '{}'", payload, entity->get_segment_path());
	json root_json_node;
    try {
        root_json_node = json::parse(payload);
        if (!root_json_node.is_object()) {
            throw exception();
        }
    }
    catch (exception & e) {
        throw YInvalidArgumentError{"Invalid JSON string"};
    }
    auto it = root_json_node.begin();
    json_node_t root(it.key(), root_json_node[it.key()]);
    auto prefix_key = split_key(root.key);
    if (entity->yang_name != prefix_key.second) {
        throw YInvalidArgumentError{"Wrong entity provided"};
    }
    decode_json(root.value, *entity, nullptr);
    return entity;
}

static void check_and_set_content(Entity & entity, const string & leaf_name, json & leaf_jvalue)
{
    ostringstream jstr_buffer;
    jstr_buffer << leaf_jvalue;
    auto content = jstr_buffer.str();
    if (content.front() == '"' && content.back() == '"')
        content = content.substr(1, content.length()-2);
    content = trim(content);
    if (leaf_name.empty() || content.empty())
        return;

    auto prefix_key = split_key(content);
    string name_space;
    string name_space_prefix;
    if (!prefix_key.first.empty())
    {
        name_space_prefix = prefix_key.first;		// module name
        auto m = entity.get_namespace_identity_lookup();
        for (auto mitem : m) {
            if (mitem.first.first == leaf_name && mitem.second == name_space_prefix) {
                name_space = mitem.first.second;
                break;
            }
        }
    }
    YLOG_DEBUG("JsonCodec: Creating leaf '{}' with value '{}' in entity '{}'", leaf_name, content, entity.yang_name);
    entity.set_value(leaf_name, content, name_space, name_space_prefix);
}

static void check_and_set_leaf(Entity & entity, Entity * parent, const string & node_name, json & json_node)
{
    if (json_node.is_null())
    {
        YLOG_DEBUG("JsonCodec: Creating leaf '{}' with no value in entity '{}'", node_name, entity.yang_name);
        entity.set_filter(node_name, YFilter::read);
    }
    else if (json_node.is_primitive())
    {
    	check_and_set_content(entity, node_name, json_node);
    }
    else {
        decode_json(json_node, entity, parent);
    }
}

static void check_and_set_node(Entity & entity, Entity * parent, const string & node_name, json & node_jvalue)
{
    YLOG_DEBUG("JsonCodec: Looking for child '{}' in '{}'", node_name, entity.yang_name);
    auto child_name = node_name;
    auto pos = child_name.find(":");
    if (pos != string::npos) {
        child_name = child_name.substr(pos+1);
    }
    if (!entity.has_leaf_or_child_of_name(child_name))
    {
        ostringstream os;
        os << "JsonCodec: Wrong payload! No element '" << child_name << "' found in '" << entity.yang_name << "'";
        YLOG_ERROR(os.str().c_str());
        throw YInvalidArgumentError{os.str()};
    }

    auto child = entity.get_child_by_name(node_name);
    if (child != nullptr) {
        YLOG_DEBUG("JsonCodec: Creating child entity '{}' in '{}'", node_name, entity.yang_name);
        if (!child->parent) {
            child->parent = parent;
        }
        decode_json(node_jvalue, *child, &entity);
    }
    else {
        check_and_set_leaf(entity, parent, node_name, node_jvalue);
    }
}

static void decode_json(json & root, Entity & entity, Entity * parent)
{
    for (auto it=root.begin(); it != root.end(); it++)
    {
        string node_key = it.key();
        json node_value = root[node_key];
        if (node_value.is_object()) {      // entity
            check_and_set_node(entity, parent, node_key, node_value);
        }
        else if (node_value.is_array()) {  // list object
            for (auto ar_it=node_value.begin(); ar_it != node_value.end(); ar_it++) {
                check_and_set_node(entity, parent, node_key, *ar_it);
            }
        }
        else {  // leaf
            check_and_set_content(entity, node_key, node_value);
        }
    }
}

}
