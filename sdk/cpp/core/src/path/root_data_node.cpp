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
#include <boost/log/trivial.hpp>


///////////////////////////////////////////////////////////////////////////////
// class ydk::RootDataImpl
//////////////////////////////////////////////////////////////////////////
ydk::path::RootDataImpl::RootDataImpl(const SchemaNode* schema, struct ly_ctx* ctx, const std::string path) : DataNodeImpl{nullptr, nullptr}, m_schema{schema}, m_ctx{ctx}, m_path{path}
{

}

ydk::path::RootDataImpl::~RootDataImpl()
{
    m_node = nullptr;
}

const ydk::path::SchemaNode*
ydk::path::RootDataImpl::schema() const
{
    return m_schema;
}

std::string
ydk::path::RootDataImpl::path() const
{
    return m_schema->path();
}

ydk::path::DataNode*
ydk::path::RootDataImpl::create_filter(const std::string& path, const std::string& value)
{
	BOOST_LOG_TRIVIAL(error) << "Cannot create filters for RootDataNode. You may need to call RootDataNode::create() for path: " << path ;
	BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Cannot create filters for RootDataNode. You may need to call RootDataNode::create()"});
    return nullptr;
}

ydk::path::DataNode*
ydk::path::RootDataImpl::create(const std::string& path, const std::string& value)
{
    if(path.empty())
    {
        BOOST_LOG_TRIVIAL(error) << "Path is empty";
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Path is empty"});
    }

    //path should not start with /
    if(path.at(0) == '/')
    {
        BOOST_LOG_TRIVIAL(error) << "Path " << path << " starts with /";
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Path starts with /"});
    }
    std::vector<std::string> segments = segmentalize(path);
    if(segments.size()<=0)
    {
		BOOST_LOG_TRIVIAL(error) << "Could not segmentalize";
		BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Could not segmentalize"});
    }

    std::string start_seg = m_path + segments[0];
    BOOST_LOG_TRIVIAL(trace) << "Creating root data node with path '"<<start_seg<<"'";
    struct lyd_node* dnode = lyd_new_path(m_node, m_ctx, start_seg.c_str(),
                                          segments.size() == 1 ? (void*)value.c_str():nullptr, LYD_ANYDATA_SXML, 0);

    if( dnode == nullptr)
    {
        BOOST_LOG_TRIVIAL(error) << "Path " << path << " is invalid";
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Path is invalid."});
    }

    DataNodeImpl* dn = nullptr;
    if(m_node == nullptr)
    {
        m_node = dnode;
        dn = new DataNodeImpl{this, m_node};
        child_map.insert(std::make_pair(m_node, dn));
    }
    else
    {
        //dnode is one of the siblings of m_node
        auto iter = child_map.find(dnode);
        if(iter != child_map.end())
        {
            dn = iter->second;

        }
        else
        {
            dn = new DataNodeImpl{this, m_node};
            child_map.insert(std::make_pair(m_node, dn));
        }

    }

    DataNode* rdn = dn;
    // created data node is the last child
    while(!rdn->children().empty())
    {
        rdn = rdn->children()[0];
    }

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

        rdn = rdn->create(remaining_path);
    }


    return rdn;

 }

void
ydk::path::RootDataImpl::set(const std::string& value)
{
    if(!value.empty()) {
        BOOST_LOG_TRIVIAL(error) << "Invalid value being assigned to root";
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Invalid value being assigned to root."});
    }
}

std::string
ydk::path::RootDataImpl::get() const
{
    return "";
}


std::vector<ydk::path::DataNode*>
ydk::path::RootDataImpl::children() const
{
    std::vector<DataNode*> ret{};

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

const ydk::path::DataNode*
ydk::path::RootDataImpl::root() const
{
    return this;
}

std::vector<ydk::path::DataNode*>
ydk::path::RootDataImpl::find(const std::string& path) const
{
    std::vector<DataNode*> results;

    if(m_node == nullptr)
    {
        return results;
    }

    std::string schema_path{ this->path() };
    if(schema_path.size()!= 1)
    {
        schema_path+="/";
    }

    auto s = schema()->statement();
    if(s.keyword == "rpc")
    {
        schema_path+="input/";
    }

    schema_path+=path;

    BOOST_LOG_TRIVIAL(trace) << "Looking for schema nodes path in root: '"<<schema_path<<"'";
    const struct lys_node* found_snode = ly_ctx_get_node(m_node->schema->module->ctx, nullptr, schema_path.c_str());

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
