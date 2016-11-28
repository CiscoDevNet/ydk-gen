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


/////////////////////////////////////////////////////////////////////////
/// Rpc
////////////////////////////////////////////////////////////////////////
ydk::path::Rpc::~Rpc()
{

}
////////////////////////////////////////////////////////////////////////////////
// class RpcImpl
////////////////////////////////////////////////////////////////////////////////


ydk::path::RpcImpl::RpcImpl(SchemaNodeImpl* sn, struct ly_ctx* ctx) : m_sn{sn}
{

    struct lyd_node* dnode = lyd_new_path(nullptr, ctx, sn->path().c_str(), (void*)"", LYD_ANYDATA_SXML, 0);

    if(!dnode){
        BOOST_LOG_TRIVIAL(error) << "Cannot find DataNode with path " << sn->path();
        BOOST_THROW_EXCEPTION(YCPPIllegalStateError{"Illegal state"});
    }

    m_input_dn = new DataNodeImpl{nullptr, dnode};

}

ydk::path::RpcImpl::~RpcImpl()
{
    if(m_input_dn){
        delete m_input_dn;
        m_input_dn = nullptr;
    }

}

ydk::path::DataNode*
ydk::path::RpcImpl::operator()(const ydk::path::ServiceProvider& provider)
{
    return provider.invoke(this);
}


ydk::path::DataNode*
ydk::path::RpcImpl::input() const
{
    return m_input_dn;
}

ydk::path::SchemaNode*
ydk::path::RpcImpl::schema() const
{
    return m_sn;
}
