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

#include <iostream>
#include <sstream>
#include <memory>
#include <libyang/libyang.h>

#include "entity_data_node_walker.hpp"
#include "errors.hpp"
#include "ietf_parser.hpp"
#include "netconf_ssh_client.hpp"
#include "netconf_tcp_client.hpp"
#include "netconf_provider.hpp"
#include "types.hpp"
#include "ydk_yang.hpp"
#include "logger.hpp"

using namespace std;
using namespace ydk;

namespace ydk
{

NetconfServiceProvider::NetconfServiceProvider(const string& address,
                                               const string& username,
                                               const string& password,
                                               int port,
                                               const string& protocol,
                                               bool on_demand,
                                               bool common_cache,
                                               int timeout)
    : session{address, username, password, port, protocol, on_demand, common_cache, timeout}
{
}

NetconfServiceProvider::NetconfServiceProvider(path::Repository & repo,
                                               const string& address,
                                               const string& username,
                                               const string& password,
                                               int port,
                                               const string& protocol,
                                               bool on_demand,
                                               int timeout)
    : session{repo, address, username, password, port, protocol, on_demand, timeout}
{
}

NetconfServiceProvider::~NetconfServiceProvider()
{
}

EncodingFormat NetconfServiceProvider::get_encoding() const
{
    return EncodingFormat::XML;
}

const path::Session& NetconfServiceProvider::get_session() const
{
    return session;
}

std::vector<std::string> NetconfServiceProvider::get_capabilities() const
{
    return session.get_capabilities();
}

}
