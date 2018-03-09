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


/////////////////////////////////////////////////////////////////////////
/// Rpc
////////////////////////////////////////////////////////////////////////
ydk::path::Rpc::~Rpc()
{
}
////////////////////////////////////////////////////////////////////////////////
// class RpcImpl
////////////////////////////////////////////////////////////////////////////////


ydk::path::RpcImpl::RpcImpl(SchemaNodeImpl& sn, struct ly_ctx* ctx, const std::shared_ptr<RepositoryPtr> & repo) : schema_node{sn}, m_priv_repo{repo}
{

    struct lyd_node* dnode = lyd_new_path(nullptr, ctx, sn.get_path().c_str(), (void*)"", LYD_ANYDATA_SXML, 0);

    if(!dnode){
        YLOG_ERROR("Cannot find RPC with path {}", sn.get_path());
        throw(YModelError{"Invalid RPC"});
    }

    data_node = std::make_unique<DataNodeImpl>(nullptr, dnode, m_priv_repo);

}

ydk::path::RpcImpl::~RpcImpl()
{
}

std::shared_ptr<ydk::path::DataNode>
ydk::path::RpcImpl::operator()(const ydk::path::Session& session)
{
    return session.invoke(*this);
}


ydk::path::DataNode&
ydk::path::RpcImpl::get_input_node() const
{
    return *data_node;
}

ydk::path::SchemaNode&
ydk::path::RpcImpl::get_schema_node() const
{
    return schema_node;
}

static bool is_output(lys_node* node)
{
    return std::string(node->name) == "output" && node->nodetype == LYS_OUTPUT ;
}

bool ydk::path::RpcImpl::has_output_node() const
{
    std::string node_path = lys_path( data_node->m_node->schema);
    std::string search_path = node_path + "//*";	// Patterns includes only descendants of the node

    ly_verb(LY_LLSILENT); //turn off libyang logging at the beginning
    ly_set* result_set = lys_find_path(data_node->m_node->schema->module, data_node->m_node->schema, search_path.c_str());
    ly_verb(LY_LLVRB); // enable libyang logging after find has completed

    auto result = false;
    if(result_set && result_set->number > 0)
    {
        for(size_t i=0; i < result_set->number; i++)
        {
            lys_node* node_result = result_set->set.s[i];
            if (is_output(node_result) && node_result->child != NULL)
            {
            	result = true;
            	break;
            }
        }
    }
    return result;
}
