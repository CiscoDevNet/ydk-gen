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

#include <unordered_set>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <json.hpp>

#include "../logger.hpp"
#include "../common_utilities.hpp"
#include "path_private.hpp"


static void get_namespaces_from_xml_doc(xmlNodePtr root, std::unordered_set<std::string>& namespaces)
{
    xmlNodePtr curr = nullptr;
    for (curr = root; curr; curr = curr->next)
    {
        if (curr->type == XML_ELEMENT_NODE && curr->ns && curr->ns->href)
        {
            namespaces.insert(std::string{reinterpret_cast<const char*>(curr->ns->href)});
            if (curr->nsDef && curr->nsDef->href) {
            	namespaces.insert(std::string{reinterpret_cast<const char*>(curr->nsDef->href)});
            }
        }
        get_namespaces_from_xml_doc(curr->children, namespaces);
    }
}

namespace ydk
{

using json = nlohmann::json;

    static std::unordered_set<std::string>
    get_namespaces_from_xml_payload(const std::string& payload)
    {
        YLOG_DEBUG("Extracting module namespaces from XML payload");
        xmlDocPtr doc = xmlParseDoc(reinterpret_cast<const xmlChar*>(payload.c_str()));
        xmlNodePtr root = xmlDocGetRootElement(doc);
        std::unordered_set<std::string> namespaces;

        get_namespaces_from_xml_doc(root, namespaces);

        xmlFreeDoc(doc);

        return namespaces;
    }

    static void
    get_module_names_from_json_object(json& o, std::unordered_set<std::string>& module_names)
    {
        for (json::iterator it = o.begin(); it != o.end(); ++it)
        {
            // extract module name from key
            auto identifier = std::string(it.key());
            auto found = identifier.find(":");
            if (found != std::string::npos && found > 0)
            {
                module_names.insert(identifier.substr(0, found));
            }
            if (it->is_array())
            {
                for (auto i = it->begin(); i != it->end(); i++)
                {
                    if (i->is_primitive())
                    {
                        auto v = i->dump();
                        if (v.find("\"") == 0 && v.rfind("\"") == v.length()-1) {
                            v = v.substr(1, v.length()-2);
                        }
                        auto ns = path::segmentalize_module_names(v);
                        if (ns.size() > 0)
                            module_names.insert(ns.begin(), ns.end());
                    }
                    else
                    {
                        get_module_names_from_json_object(*i, module_names);
                    }
                }
            }
            else
            {
                // extract module name from value
                if (it->is_primitive())
                {
                    auto v = it->dump();
                    if (v.length() > 2 &&
                        v.find("\"") == 0 && v.rfind("\"") == v.length()-1) {
                        v = v.substr(1, v.length()-2);
                    }
                    auto ns = path::segmentalize_module_names(v);
                    if (ns.size() > 0)
                        module_names.insert(ns.begin(), ns.end());
                }
                else
                {
                    get_module_names_from_json_object(*it, module_names);
                }
            }
        }
    }

    static std::unordered_set<std::string>
    get_module_names_from_json_payload(const std::string& payload)
    {
        YLOG_DEBUG("Extracting module names from JSON payload");
        std::unordered_set<std::string> module_names;
        auto o = json::parse(payload);
        get_module_names_from_json_object(o, module_names);
        return module_names;
    }
}

//////////////////////////////////////////////////////////////////////////////
/// RootSchemaNode
/////////////////////////////////////////////////////////////////////////////

ydk::path::RootSchemaNode::~RootSchemaNode()
{

}

std::string
ydk::path::RootSchemaNode::get_path() const
{
    return "/";
}

ydk::path::SchemaNode*
ydk::path::RootSchemaNode::get_parent() const noexcept
{
    return nullptr;
}

const ydk::path::SchemaNode&
ydk::path::RootSchemaNode::get_root() const noexcept
{
    return *this;
}

ydk::path::Statement
ydk::path::RootSchemaNode::get_statement() const
{
    return Statement{};
}

std::vector<ydk::path::Statement>
ydk::path::RootSchemaNode::get_keys() const
{
    return std::vector<Statement>{};

}


/////////////////////////////////////////////////////////////////////////////////////
// class RootSchemaNodeImpl
/////////////////////////////////////////////////////////////////////////////////////
ydk::path::RootSchemaNodeImpl::RootSchemaNodeImpl(struct ly_ctx* ctx, const std::shared_ptr<RepositoryPtr> & repo)
    : m_ctx{ctx}, m_priv_repo{repo}, m_name_namespace_lookup()
{
    populate_all_module_schemas();
}

ydk::path::RootSchemaNodeImpl::RootSchemaNodeImpl(struct ly_ctx* ctx, const std::shared_ptr<RepositoryPtr> & repo,
                                                  const std::unordered_map<std::string, path::Capability>& lookup_table)
    : m_ctx{ctx}, m_priv_repo{repo}, m_name_namespace_lookup(lookup_table)
{
    populate_all_module_schemas();
}

ydk::path::RootSchemaNodeImpl::~RootSchemaNodeImpl()
{
    // release resource before destroying libyang context
    m_root_data_nodes.clear();

    if(m_ctx){
        ly_ctx_destroy(m_ctx, nullptr);
        m_ctx = nullptr;
    }
}

void
ydk::path::RootSchemaNodeImpl::populate_all_module_schemas()
{
    uint32_t idx = 0;
    while( auto m = ly_ctx_get_module_iter(m_ctx, &idx)) {
        populate_module_schema(m);
    }
}

void
ydk::path::RootSchemaNodeImpl::populate_module_schema(const struct lys_module* module) {
    YLOG_DEBUG("Populating new module schema '{}'", module->name);
    const struct lys_node *last = nullptr;
    while( auto q = lys_getnext(last, nullptr, module, 0)) {
        m_children.push_back(std::make_unique<SchemaNodeImpl>(this, const_cast<struct lys_node*>(q)));
        last = q;
    }
}

void
ydk::path::RootSchemaNodeImpl::populate_new_schemas_from_payload(const std::string& payload, ydk::EncodingFormat format)
{
    YLOG_DEBUG("Populating new schema from payload:\n{}", payload);
    std::vector<const lys_module*> modules;
    if (format == ydk::EncodingFormat::XML)
    {
        std::string xml_str = trim(payload);
        if (xml_str.substr(0, 5) != "<?xml")
            xml_str = "<data>" + payload + "</data>";
        auto namespaces = get_namespaces_from_xml_payload(xml_str);
        modules = m_priv_repo->get_new_ly_modules_from_lookup(m_ctx, namespaces, m_name_namespace_lookup);
    }
    else
    {
        auto module_names = get_module_names_from_json_payload(payload);
        modules = m_priv_repo->get_new_ly_modules_from_lookup(m_ctx, module_names, m_name_namespace_lookup);
    }

    populate_new_schemas(modules);
}

void
ydk::path::RootSchemaNodeImpl::populate_new_schemas_from_path(const std::string& path)
{
    if (path.empty()) return;
    YLOG_DEBUG("Getting new modules for path '{}'", path);
    auto new_modules = m_priv_repo->get_new_ly_modules_from_path(m_ctx, path, m_name_namespace_lookup);
    populate_new_schemas(new_modules);
}

void
ydk::path::RootSchemaNodeImpl::populate_new_schemas(std::vector<const lys_module*>& new_modules)
{
    for (auto m: new_modules) {
        populate_module_schema(m);
        populate_augmented_schema_nodes(m);
    }
}

void
ydk::path::RootSchemaNodeImpl::populate_augmented_schema_nodes(const struct lys_module* module)
{
    for (int i = 0; i < module->augment_size; i++)
    {
        auto aug = module->augment[i];
        std::vector<lys_node*> ancestors;
        lys_node* node = aug.target;

        while(node)
        {
            if (node->nodetype != LYS_USES)
            {
                ancestors.emplace_back(node);
            }
            node = node->parent;
        }

        if(aug.child == nullptr)
        {
            YLOG_DEBUG("Augment child {} is null", aug.target_name);
        }
        else
        {
            populate_augmented_schema_node(ancestors, aug.child);
        }
    }
}

void
ydk::path::RootSchemaNodeImpl::populate_augmented_schema_node(std::vector<lys_node*>& ancestors, struct lys_node* target) {
    YLOG_DEBUG("Populating augmented schema node '{}'", std::string(target->name));

    lys_node* root = ancestors.back();
    // quick fix: populate augmented top node if we have not already done so
    bool found = false;
    for (auto&c: m_children)
    {
        if (c->get_statement().arg == root->name)
        {
            found = true;
            break;
        }
    }
    if (found == false)
    {
        m_children.push_back(std::make_unique<SchemaNodeImpl>(this, const_cast<struct lys_node*>(root)));
    }

    // populate the rest of augmented schema nodes
    ancestors.pop_back();
    for (auto& c: m_children)
    {
        if (c->get_statement().arg == root->name)
        {
            reinterpret_cast<SchemaNodeImpl*>(c.get())->populate_augmented_schema_node(ancestors, target);
        }
    }
}

std::vector<ydk::path::SchemaNode*>
ydk::path::RootSchemaNodeImpl::find(const std::string& path)
{
    populate_new_schemas_from_path(path);

    if(path.empty()) {
        YLOG_ERROR("Path is empty");
        throw(YInvalidArgumentError{"path is empty"});
    }

    //has to be a relative path
    if(path.at(0) == '/') {
        YLOG_ERROR("Path must be a relative path");
        throw(YInvalidArgumentError{"path must be a relative path"});
    }

    std::vector<SchemaNode*> ret;

    std::string full_path{"/"};
    full_path+=path;

    const struct lys_node* found_node = ly_ctx_get_node(m_ctx, nullptr, full_path.c_str(), 0);

    if (found_node){
        auto p = reinterpret_cast<SchemaNode*>(found_node->priv);
        if(p) {
            ret.push_back(p);
        }
    }

    return ret;
}

const std::vector<std::unique_ptr<ydk::path::SchemaNode>> &
ydk::path::RootSchemaNodeImpl::get_children() const
{
    return m_children;
}

ydk::path::DataNode&
ydk::path::RootSchemaNodeImpl::create_datanode(const std::string& path)
{
    return create_datanode(path, "");
}

ydk::path::DataNode&
ydk::path::RootSchemaNodeImpl::create_datanode(const std::string& path, const std::string& value)
{
    populate_new_schemas_from_path(path);

    auto root_data_node = std::make_unique<RootDataImpl>(*this, m_ctx, "/", m_priv_repo);
    m_root_data_nodes.push_back(std::move(root_data_node));
    return m_root_data_nodes.back()->create_datanode(path, value);
}

std::shared_ptr<ydk::path::Rpc>
ydk::path::RootSchemaNodeImpl::create_rpc(const std::string& path)
{
    auto c = find(path);
    if(c.empty()){
        throw(YInvalidArgumentError{"Path is invalid: "+ path});
    }

    bool found = false;
    SchemaNode* rpc_sn = nullptr;

    for(auto item : c) {
        auto s = item->get_statement();
        if(s.keyword == "rpc"){
            found = true;
            rpc_sn = item;
            break;
        }
    }

    if(!found){
        YLOG_ERROR("Path {} does not refer to an rpc node.", path);
        throw(YInvalidArgumentError{"Path does not refer to an rpc node"});
    }
    SchemaNodeImpl* sn = dynamic_cast<SchemaNodeImpl*>(rpc_sn);
    if(!sn){
        YLOG_ERROR("Schema Node case failed");
        throw(YIllegalStateError("Internal error occurred"));
    }

    return std::make_shared<RpcImpl>(*sn, m_ctx, m_priv_repo);
}
