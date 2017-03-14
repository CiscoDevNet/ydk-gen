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


//////////////////////////////////////////////////////////////////////////////
/// RootSchemaNode
/////////////////////////////////////////////////////////////////////////////

ydk::path::RootSchemaNode::~RootSchemaNode()
{

}

std::string
ydk::path::RootSchemaNode::path() const
{
    return "/";
}

ydk::path::SchemaNode*
ydk::path::RootSchemaNode::parent() const noexcept
{
    return nullptr;
}

const ydk::path::SchemaNode&
ydk::path::RootSchemaNode::root() const noexcept
{
    return *this;
}

ydk::path::Statement
ydk::path::RootSchemaNode::statement() const
{
    return Statement{};
}

std::vector<ydk::path::Statement>
ydk::path::RootSchemaNode::keys() const
{
    return std::vector<Statement>{};

}


/////////////////////////////////////////////////////////////////////////////////////
// class RootSchemaNodeImpl
/////////////////////////////////////////////////////////////////////////////////////
ydk::path::RootSchemaNodeImpl::RootSchemaNodeImpl(struct ly_ctx* ctx) : m_ctx{ctx}
{
    // m_root_data_nodes = std::vector<std::unique_ptr<DataNode>>{};

    //populate the tree
    uint32_t idx = 0;
    while( auto p = ly_ctx_get_module_iter(ctx, &idx)) {
        const struct lys_node *last = nullptr;
        while( auto q = lys_getnext(last, nullptr, p, 0)) {
            m_children.push_back(std::make_unique<SchemaNodeImpl>(this, const_cast<struct lys_node*>(q)));
            last = q;
        }
    }

}

ydk::path::RootSchemaNodeImpl::~RootSchemaNodeImpl()
{
    // need to release before destroy context
    for (auto & r: m_root_data_nodes)
        r.release();

    if(m_ctx){
        ly_ctx_destroy(m_ctx, nullptr);
        m_ctx = nullptr;
    }
}

std::vector<ydk::path::SchemaNode*>
ydk::path::RootSchemaNodeImpl::find(const std::string& path) const
{
    if(path.empty()) {
        YLOG_ERROR("path is empty");
        throw(YCPPInvalidArgumentError{"path is empty"});
    }

    //has to be a relative path
    if(path.at(0) == '/') {
        YLOG_ERROR("path must be a relative path");
        throw(YCPPInvalidArgumentError{"path must be a relative path"});
    }

    std::vector<SchemaNode*> ret;

    std::string full_path{"/"};
    full_path+=path;

    const struct lys_node* found_node = ly_ctx_get_node(m_ctx, nullptr, full_path.c_str());

    if (found_node){
        auto p = reinterpret_cast<SchemaNode*>(found_node->priv);
        if(p) {
            ret.push_back(p);
        }
    }

    return ret;
}

const std::vector<std::unique_ptr<ydk::path::SchemaNode>> &
ydk::path::RootSchemaNodeImpl::children() const
{
    return m_children;
}

ydk::path::DataNode&
ydk::path::RootSchemaNodeImpl::create(const std::string& path)
{
    return create(path, "");
}

ydk::path::DataNode&
ydk::path::RootSchemaNodeImpl::create(const std::string& path, const std::string& value)
{
    auto root_data_node = std::make_unique<RootDataImpl>(*this, m_ctx, "/");
    m_root_data_nodes.push_back(std::move(root_data_node));
    return m_root_data_nodes.back()->create(path, value);
}

std::shared_ptr<ydk::path::Rpc>
ydk::path::RootSchemaNodeImpl::rpc(const std::string& path) const
{
    auto c = find(path);
    if(c.empty()){
        throw(YCPPInvalidArgumentError{"Path is invalid: "+path});
    }

    bool found = false;
    SchemaNode* rpc_sn = nullptr;

    for(auto item : c) {
        auto s = item->statement();
        if(s.keyword == "rpc"){
            found = true;
            rpc_sn = item;
            break;
        }
    }

    if(!found){
        YLOG_ERROR("Path {} does not refer to an rpc node.", path);
        throw(YCPPInvalidArgumentError{"Path does not refer to an rpc node"});
    }
    SchemaNodeImpl* sn = dynamic_cast<SchemaNodeImpl*>(rpc_sn);
    if(!sn){
        YLOG_ERROR("Schema Node case failed");
        throw(YCPPIllegalStateError("Internal error occurred"));
    }

    return std::make_shared<RpcImpl>(*sn, m_ctx);
}
