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


#include "core_private.hpp"
#include <boost/log/trivial.hpp>


//////////////////////////////////////////////////////////////////////////////
/// RootSchemaNode
/////////////////////////////////////////////////////////////////////////////
ydk::core::RootSchemaNode::~RootSchemaNode()
{

}

std::string
ydk::core::RootSchemaNode::path() const
{
    return "/";
}

ydk::core::SchemaNode*
ydk::core::RootSchemaNode::parent() const noexcept
{
    return nullptr;
}

const ydk::core::SchemaNode*
ydk::core::RootSchemaNode::root() const noexcept
{
    return this;
}

ydk::core::Statement
ydk::core::RootSchemaNode::statement() const
{
    return Statement{};
}

std::vector<ydk::core::Statement>
ydk::core::RootSchemaNode::keys() const
{
    return std::vector<Statement>{};

}


/////////////////////////////////////////////////////////////////////////////////////
// class RootSchemaNodeImpl
/////////////////////////////////////////////////////////////////////////////////////
ydk::core::RootSchemaNodeImpl::RootSchemaNodeImpl(struct ly_ctx* ctx) : m_ctx{ctx}
{
    //populate the tree
    uint32_t idx = 0;
    while( auto p = ly_ctx_get_module_iter(ctx, &idx)) {
        const struct lys_node *last = nullptr;
        while( auto q = lys_getnext(last, nullptr, p, 0)) {
            m_children.push_back(new SchemaNodeImpl{this, const_cast<struct lys_node*>(q)});
            last = q;
        }
    }

}

ydk::core::RootSchemaNodeImpl::~RootSchemaNodeImpl()
{
    if(m_ctx){
        ly_ctx_destroy(m_ctx, nullptr);
        m_ctx = nullptr;
    }
}

std::vector<ydk::core::SchemaNode*>
ydk::core::RootSchemaNodeImpl::find(const std::string& path) const
{
    if(path.empty()) {
        BOOST_LOG_TRIVIAL(error) << "path is empty";
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"path is empty"});
    }

    //has to be a relative path
    if(path.at(0) == '/') {
        BOOST_LOG_TRIVIAL(error) << "path must be a relative path";
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"path must be a relative path"});
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

std::vector<ydk::core::SchemaNode*>
ydk::core::RootSchemaNodeImpl::children() const
{
    return m_children;
}

ydk::core::DataNode*
ydk::core::RootSchemaNodeImpl::create(const std::string& path) const
{
    return create(path, "");
}

ydk::core::DataNode*
ydk::core::RootSchemaNodeImpl::create(const std::string& path, const std::string& value) const
{
    RootDataImpl* rd = new RootDataImpl{this, m_ctx, "/"};

    if (rd){
        return rd->create(path, value);
    }

    return nullptr;
}

ydk::core::DataNode*
ydk::core::RootSchemaNodeImpl::from_xml(const std::string& xml) const
{
    struct lyd_node *root = lyd_parse_mem(m_ctx, xml.c_str(), LYD_XML, 0);
    RootDataImpl* rd = new RootDataImpl{this, m_ctx, "/"};
    DataNodeImpl* nodeImpl = new DataNodeImpl{rd,root};

    return nodeImpl;

}



ydk::core::Rpc*
ydk::core::RootSchemaNodeImpl::rpc(const std::string& path) const
{
    auto c = find(path);
    if(c.empty()){
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"Path is invalid"});
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
        BOOST_LOG_TRIVIAL(error) << "Path " << path << " does not refer to an rpc node.";
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"Path does not refer to an rpc node"});
    }
    SchemaNodeImpl* sn = dynamic_cast<SchemaNodeImpl*>(rpc_sn);
    if(!sn){
        BOOST_LOG_TRIVIAL(error) << "Schema Node case failed";
        BOOST_THROW_EXCEPTION(YDKIllegalStateException("Internal error occurred"));
    }
    return new RpcImpl{sn, m_ctx};

}
