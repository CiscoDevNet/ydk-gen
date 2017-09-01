/// YANG Development Kit
// Copyright 2016 Cisco Systems. All rights reserved
//
////////////////////////////////////////////////////////////////
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//////////////////////////////////////////////////////////////////

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "entity_util.hpp"
#include "logger.hpp"
#include "xml_subtree_codec.hpp"

using namespace std;

namespace ydk
{
static void decode_xml(xmlDocPtr doc, xmlNodePtr root, Entity & entity, Entity * parent, const string & leaf_name);
static string to_string(const xmlChar * s);
static const xmlChar * to_xmlchar(const string & s);

static void walk_children(Entity & entity, const path::SchemaNode & parent_schema, xmlNodePtr root_node);
static void populate_xml_node(Entity & entity, const path::SchemaNode & parent_schema, xmlNodePtr xml_node);
static void populate_xml_node_contents(const path::SchemaNode & parent_schema, EntityPath & path, xmlNodePtr xml_node);
static string to_string(xmlDocPtr doc, xmlNodePtr xml_node);

XmlSubtreeCodec::XmlSubtreeCodec()
{
}

//////////////////////////////////////////////////////////////////
// XmlSubtreeCodec::encode
//////////////////////////////////////////////////////////////////
std::string XmlSubtreeCodec::encode(Entity & entity, path::RootSchemaNode & root_schema)
{
    EntityPath root_path = get_entity_path(entity, nullptr);
    auto & root_data_node = root_schema.create_datanode(root_path.path);
    xmlDocPtr doc = xmlNewDoc(to_xmlchar("1.0"));
    xmlNodePtr root_node = xmlNewNode(NULL, to_xmlchar(entity.yang_name));
    xmlNewProp(root_node, (const unsigned char *)"xmlns", to_xmlchar(root_data_node.get_schema_node().get_statement().name_space));

    populate_xml_node_contents(root_data_node.get_schema_node(), root_path, root_node);
    walk_children(entity, root_data_node.get_schema_node(), root_node);

    return to_string(doc, root_node);
}

static void walk_children(Entity & entity, const path::SchemaNode & schema, xmlNodePtr xml_node)
{
    std::map<string, shared_ptr<Entity>> children = entity.get_children();
    YLOG_DEBUG("XML: Children count for: {} : {}",get_entity_path(entity, entity.parent).path, children.size());
    for(auto const& child : children)
    {
        if(child.second == nullptr)
            continue;
        YLOG_DEBUG("==================");
        YLOG_DEBUG("XML: Looking at child '{}': {}",child.first, get_entity_path(*(child.second), child.second->parent).path);
        if(child.second->has_operation() || child.second->has_data() || child.second->is_presence_container)
            populate_xml_node(*(child.second), schema, xml_node);
        else
            YLOG_DEBUG("XML: Child has no data and no operations");
    }
}

static const path::SchemaNode* find_child_by_name(const path::SchemaNode & parent_schema, const string & name)
{
    auto p = const_cast<path::SchemaNode*>(&parent_schema);
    vector<path::SchemaNode*> s = p->find(name);
    if(s.size()==0)
    {
        YLOG_ERROR("Could not find node '{}'", name);
        throw YCPPServiceProviderError{"Could not find node " + name};
    }
    return s[0];
}

static bool has_same_namespace(const path::SchemaNode & left, const path::SchemaNode & right)
{
    return left.get_statement().name_space == right.get_statement().name_space;
}

static void set_xml_namespace(const string & name_space, xmlNodePtr xml_node)
{
    xmlNewProp(xml_node, to_xmlchar("xmlns"), to_xmlchar(name_space));
}

static void set_operation_from_yfilter(YFilter yfilter, xmlNodePtr xml_node)
{
    if (yfilter != YFilter::read)
    {
        xmlNewProp(xml_node, to_xmlchar("operation"), to_xmlchar(to_string(yfilter)));
    }
}

static xmlNodePtr create_and_populate_xml_node(const path::SchemaNode & parent_schema, const path::SchemaNode & schema,
            YFilter yfilter, xmlNodePtr parent_xml_node, const xmlChar* content)
{
    xmlNodePtr child = xmlNewChild(parent_xml_node, NULL, to_xmlchar(schema.get_statement().arg), content);
    if(!has_same_namespace(schema, parent_schema))
    {
        set_xml_namespace(schema.get_statement().name_space, child);
    }

    if(is_set(yfilter))
    {
        set_operation_from_yfilter(yfilter, child);
    }
    return child;
}

static void populate_xml_node(Entity & entity, const path::SchemaNode & parent_schema, xmlNodePtr xml_node)
{
    EntityPath path = get_entity_path(entity, entity.parent);
    const path::SchemaNode* schema = find_child_by_name(parent_schema, entity.get_segment_path());

    xmlNodePtr child = create_and_populate_xml_node(parent_schema, *schema, entity.yfilter, xml_node, NULL);
    populate_xml_node_contents(*schema, path, child);
    walk_children(entity, *schema, child);
}

static const xmlChar* get_content_from_leafdata(LeafData & leaf_data)
{
    const xmlChar* content = NULL;
    if(leaf_data.is_set)
    {
        content = to_xmlchar(leaf_data.value);
    }
    else if(is_set(leaf_data.yfilter))
    {
        content = NULL;
    }
    return content;
}

static bool leaf_to_be_created(LeafData & leaf_data)
{
    return leaf_data.is_set || is_set(leaf_data.yfilter);
}

static void set_prefixed_namespace_from_leafdata(LeafData & leaf_data, xmlNodePtr xml_node)
{
    if(leaf_data.name_space.size() > 0 && leaf_data.name_space_prefix.size() > 0)
    {
        ostringstream os;
        os << "xmlns:" << leaf_data.name_space_prefix;
        xmlNewProp(xml_node, to_xmlchar(os.str()), to_xmlchar(leaf_data.name_space));
    }
}

static void populate_xml_node_contents(const path::SchemaNode & parent_schema, EntityPath & path, xmlNodePtr xml_node)
{
    YLOG_DEBUG("Leaf count: {}", path.value_paths.size());
    for(const std::pair<std::string, LeafData> & name_value : path.value_paths)
    {
        LeafData leaf_data = name_value.second;
        const path::SchemaNode* schema = find_child_by_name(parent_schema, name_value.first);
        YLOG_DEBUG("Creating child {} of {} with value: '{}', is_set: {}", name_value.first, parent_schema.get_path(),
                leaf_data.value, leaf_data.is_set);

        const xmlChar* content = get_content_from_leafdata(leaf_data);
        if(leaf_to_be_created(leaf_data))
        {
            xmlNodePtr child = create_and_populate_xml_node(parent_schema, *schema, leaf_data.yfilter, xml_node, content);
            set_prefixed_namespace_from_leafdata(leaf_data, child);
            if(is_set(leaf_data.yfilter))
            {
                YLOG_DEBUG("Storing operation '{}' for leaf {}", to_string(leaf_data.yfilter), name_value.first);
            }
        }
    }
}

static string to_string(xmlDocPtr doc, xmlNodePtr root)
{
    string str;
    xmlBufferPtr buf = xmlBufferCreate();
    if (buf != NULL)
    {
        xmlNodeDump(buf, doc, root, 0, 1);
        str = to_string(buf->content);
        xmlBufferFree(buf);
    }
    else
    {
        YLOG_ERROR("Error creating the xml buffer");
        throw YCPPServiceProviderError{"Error creating the xml buffer"};
    }
    return str;
}

//////////////////////////////////////////////////////////////////
// XmlSubtreeCodec::decode
//////////////////////////////////////////////////////////////////
std::shared_ptr<Entity> XmlSubtreeCodec::decode(const std::string & payload, std::shared_ptr<Entity> entity)
{
    xmlDocPtr doc = xmlParseDoc(reinterpret_cast<const xmlChar*>(payload.c_str()));
    xmlNodePtr root = xmlDocGetRootElement(doc);
    if(entity->yang_name != to_string(root->name))
    {
        throw YCPPServiceProviderError{"Wrong entity"};
    }
    decode_xml(doc, root->children, *entity, nullptr, "");
    return entity;
}

static bool isonlywhitespace(xmlChar *	content)
{
    if(content == NULL)
        return true;

    for(auto g:to_string(content))
    {
        if (!isspace(g))
            return false;
    }
    return true;
}

static void check_and_set_leaf(Entity & entity, Entity * parent, xmlNodePtr xml_node, xmlDocPtr doc)
{
    string current_node_name{to_string(xml_node->name)};
    if(xml_node->children == NULL)
    {
        YLOG_DEBUG("XML: Creating leaf '{}' with no value", current_node_name);
        entity.set_filter(current_node_name, YFilter::read);
    }
    else
    {
        decode_xml(doc, xml_node->children, entity, parent, current_node_name);
    }
}

static string resolve_leaf_value_namespace(const string & content, const string & name_space, const string & name_space_prefix, Entity * entity)
{
    string c{content};
    if(name_space.size() > 0 && name_space_prefix.size() > 0)
    {
        if(content.find(name_space_prefix) != string::npos && content.find(":") != string::npos)
        {
            auto s = content.find(":");
            c = content.substr(s+1);
        }
        Entity* p = entity;
        while(p->parent!=nullptr)
        {
            p = p->parent;
        }
        auto m = p->get_namespace_identity_lookup();
        YLOG_DEBUG("XML: Got namespace identity lookup with '{}' elements", m.size());
        if(m.find({c,name_space}) != m.end())
        {
            string module_name = m[{c, name_space}];
            c = module_name + ":" + c;
        }
    }
    return c;
}

static void check_and_set_content(Entity & entity, const string & leaf_name, xmlNodePtr parent_xml_node, xmlChar * content, xmlDocPtr doc)
{
    if(leaf_name.size()>0 && !isonlywhitespace(content))
    {
        xmlNsPtr * nsList = xmlGetNsList(doc, parent_xml_node);
        string name_space;
        string name_space_prefix;
        if (nsList)
        {
            for(xmlNsPtr ns=*nsList; ns; ns=ns->next)
            {
                name_space = to_string(ns->href);
                name_space_prefix = to_string(ns->prefix);
                break;
            }
        }
        string c = resolve_leaf_value_namespace(to_string(content), name_space, name_space_prefix, &entity);

        YLOG_DEBUG("XML: Creating leaf '{}' with value '{}'", leaf_name, c);
        entity.set_value(leaf_name, c, name_space, name_space_prefix);
    }
}

static void check_payload_to_raise_exception(Entity & entity, const xmlChar * name)
{
    string current_node_name{to_string(name)};
    if(!entity.has_leaf_or_child_of_name(current_node_name))
    {
        ostringstream os;os<<"Wrong payload! <" << current_node_name << "> not found";
        YLOG_ERROR(os.str().c_str());
        throw YCPPServiceProviderError{os.str()};
    }
}

static void check_and_set_node(Entity & entity, Entity * parent, xmlNodePtr xml_node, xmlDocPtr doc)
{
    YLOG_DEBUG("Looking for child '{}' in '{}'", to_string(xml_node->name), entity.yang_name);
    check_payload_to_raise_exception(entity, xml_node->name);

    auto child = entity.get_child_by_name(to_string(xml_node->name));
    if(child!=nullptr)
    {
        decode_xml(doc, xml_node->children, *child, &entity, "");
    }
    else
    {
        check_and_set_leaf(entity, parent, xml_node, doc);
    }
}

static void decode_xml(xmlDocPtr doc, xmlNodePtr root, Entity & entity, Entity * parent, const string & leaf_name)
{
    xmlNodePtr xml_node = NULL;

    for (xml_node = root; xml_node; xml_node = xml_node->next)
    {
        if(xml_node->type == XML_TEXT_NODE)
        {
            check_and_set_content(entity, leaf_name, xml_node->parent, xml_node->content, doc);
        }
        else
        {
            check_and_set_node(entity, parent, xml_node, doc);
        }
    }
}

static string to_string(const xmlChar * s)
{
    if(s == NULL) return "";

    const char* buffer = reinterpret_cast<const char*>(s);
    return buffer;
}

static const xmlChar * to_xmlchar(const string & s)
{
    const xmlChar* buffer = reinterpret_cast<const xmlChar*>(s.c_str());
    return buffer;
}
}
