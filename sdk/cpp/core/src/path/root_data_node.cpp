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


#include "path_private.hpp"
#include "../logger.hpp"


///////////////////////////////////////////////////////////////////////////////
// class ydk::RootDataImpl
//////////////////////////////////////////////////////////////////////////
ydk::path::RootDataImpl::RootDataImpl(const SchemaNode& schema, struct ly_ctx* ctx, const std::string & path) : DataNodeImpl{nullptr, nullptr, nullptr}, m_schema{schema}, m_ctx{ctx}, m_path{path}
{
}

ydk::path::RootDataImpl::RootDataImpl(const SchemaNode& schema, struct ly_ctx* ctx, const std::string & path, const std::shared_ptr<RepositoryPtr> & repo) : DataNodeImpl{nullptr, nullptr, repo}, m_schema{schema}, m_ctx{ctx}, m_path{path}, m_priv_repo{repo}
{
}

ydk::path::RootDataImpl::~RootDataImpl()
{
}

const ydk::path::SchemaNode&
ydk::path::RootDataImpl::get_schema_node() const
{
    return m_schema;
}

std::string
ydk::path::RootDataImpl::get_path() const
{
    return m_schema.get_path();
}

void
ydk::path::RootDataImpl::populate_new_schemas_from_path(const std::string& path)
{
    auto csnode = const_cast<SchemaNode*>(&m_schema);
    auto snode = reinterpret_cast<SchemaNodeImpl*>(csnode);
    snode->populate_new_schemas_from_path(path);
}

ydk::path::DataNode&
ydk::path::RootDataImpl::create_datanode(const std::string& path, const std::string& value)
{
    populate_new_schemas_from_path(path);
    populate_new_schemas_from_path(value);

    if(path.empty())
    {
        YLOG_ERROR("Path is empty");
        throw(YInvalidArgumentError{"Path is empty"});
    }

    //path should not start with /
    if(path.at(0) == '/')
    {
        YLOG_ERROR("Path '{}' should not start with /", path);
        throw(YInvalidArgumentError{"Path should not start with /"});
    }
    std::vector<std::string> segments = segmentalize(path);
    if(segments.size()<=0)
    {
        YLOG_ERROR("Could not segmentalize path");
        throw(YInvalidArgumentError{"Could not segmentalize path"});
    }

    std::string start_seg = m_path + segments[0];
    YLOG_DEBUG("Creating root data node with path '{}'", start_seg);
    struct lyd_node* dnode = lyd_new_path(m_node, m_ctx, start_seg.c_str(),
                                          segments.size() == 1 ? (void*)value.c_str():nullptr, LYD_ANYDATA_SXML, 0);

    if( dnode == nullptr)
    {
        YLOG_ERROR("Path '{}' is invalid", path);
        throw(YInvalidArgumentError{"Path is invalid: " + path});
    }

    DataNodeImpl* dn = nullptr;
    if(m_node == nullptr)
    {
        m_node = dnode;
        child_map.insert(std::make_pair(m_node, std::make_shared<DataNodeImpl>(this, m_node, m_priv_repo)));
        dn = dynamic_cast<DataNodeImpl*>(child_map[m_node].get());
    }
    else
    {
        //dnode is one of the siblings of m_node
        auto iter = child_map.find(dnode);
        if(iter != child_map.end())
        {
            dn = dynamic_cast<DataNodeImpl*>(iter->second.get());

        }
        else
        {
            child_map.insert(std::make_pair(m_node, std::make_shared<DataNodeImpl>(this, m_node, m_priv_repo)));
            dn = dynamic_cast<DataNodeImpl*>(child_map[m_node].get());
        }

    }

    DataNode* rdn = dn;

    //at this stage we have dn so for the remaining segments use dn as the parent
    if(segments.size() > 1)
    {
        std::string remaining_path;
        for(size_t i =1; i< segments.size(); i++)
        {
            if(i!=1)
            {
                remaining_path+="/";
            }
            remaining_path+=segments[i];
        }

        rdn = &(rdn->create_datanode(remaining_path));
    }


    return *rdn;

 }

void
ydk::path::RootDataImpl::set_value(const std::string& value)
{
    if(!value.empty()) {
        YLOG_ERROR("Invalid value being assigned to root");
        throw(YInvalidArgumentError{"Invalid value being assigned to root."});
    }
}

std::string
ydk::path::RootDataImpl::get_value() const
{
    return "";
}


std::vector<std::shared_ptr<ydk::path::DataNode>>
ydk::path::RootDataImpl::get_children() const
{
    std::vector<std::shared_ptr<DataNode>> ret{};

    struct lyd_node* iter = m_node;

    if( iter ){
        do {
            auto p = child_map.find(iter);
            if (p != child_map.end()) {
                ret.push_back(p->second);
            }

            iter=iter->next;

        } while (iter && iter != m_node);
    }

    return ret;
}

const ydk::path::DataNode&
ydk::path::RootDataImpl::get_root() const
{
    return *this;
}

std::vector<std::shared_ptr<ydk::path::DataNode>>
ydk::path::RootDataImpl::find(const std::string& path)
{
    populate_new_schemas_from_path(path);

    std::vector<std::shared_ptr<DataNode>> results;

    if(m_node == nullptr)
    {
        return results;
    }

    std::string schema_path{ this->get_path() };
    if(schema_path.size()!= 1)
    {
        schema_path+="/";
    }

    auto s = get_schema_node().get_statement();
    if(s.keyword == "rpc" || s.keyword == "action")
    {
        schema_path+="input/";
    }

    schema_path+=path;

    YLOG_DEBUG("Looking for schema nodes path in root: '{}'", schema_path);
    const struct lys_node* found_snode = ly_ctx_get_node(m_node->schema->module->ctx, nullptr, schema_path.c_str(), 1);

    if(found_snode)
    {
        struct ly_set* result_set = lyd_find_instance(m_node, found_snode);
        if( result_set )
        {
            if (result_set->number > 0)
            {
                for(size_t i=0; i < result_set->number; i++)
                {
                    struct lyd_node* node_result = result_set->set.d[i];
                    results.push_back(get_dn_for_desc_node(node_result));
                }
            }
            ly_set_free(result_set);
        }

    }

    return results;
}
