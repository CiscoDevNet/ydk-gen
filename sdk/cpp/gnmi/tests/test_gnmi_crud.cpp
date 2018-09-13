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

#include "../../core/src/catch.hpp"
#include "../../core/tests/config.hpp"

#include <ydk/crud_service.hpp>
#include <ydk/codec_provider.hpp>
#include <ydk/codec_service.hpp>
#include <ydk/gnmi_provider.hpp>

#include <ydk_ydktest/openconfig_bgp.hpp>
#include <ydk_ydktest/openconfig_interfaces.hpp>

using namespace std;
using namespace ydk;
using namespace path;
using namespace ydktest;

TEST_CASE("gnmi_crud_single_entity")
{
    // session
    Repository repo{TEST_HOME};
    string address = "127.0.0.1"; int port = 50051;

    gNMIServiceProvider provider{repo, address, port, "admin", "admin"};
    CrudService crud{};
    CodecServiceProvider codec_provider{EncodingFormat::JSON};
    CodecService codec_service{};

    auto ifc = make_shared<openconfig_interfaces::Interfaces::Interface>();
    ifc->name = "Loopback10";
    ifc->config->name = "Loopback10";

    openconfig_interfaces::Interfaces ifcs{};
    ifcs.interface.append(ifc);

    // Set-replace Request
    auto reply = crud.create(provider, ifcs);
    REQUIRE(reply);

    ifc->config->description = "Test";
    reply = crud.update(provider, ifcs);
    REQUIRE(reply);

    openconfig_interfaces::Interfaces filter{};
    auto ifc_read = crud.read(provider, filter);
    REQUIRE(ifc_read != nullptr);
    REQUIRE(*ifc_read == ifcs);

    ifc_read = crud.read_config(provider, filter);
    REQUIRE(ifc_read != nullptr);

    reply = crud.delete_(provider, ifcs);
    REQUIRE(reply);
}

void config_bgp(openconfig_bgp::Bgp bgp);

TEST_CASE("gnmi_crud_multiple_entities")
{
    // session
    Repository repo{TEST_HOME};
    string address = "127.0.0.1"; int port = 50051;

    gNMIServiceProvider provider{repo, address, port, "admin", "admin"};
    CrudService crud{};
    CodecServiceProvider codec_provider{EncodingFormat::JSON};
    CodecService codec_service{};

    // Configure Interfaces
    auto ifc = make_shared<openconfig_interfaces::Interfaces::Interface>();
    ifc->name = "Loopback10";
    ifc->config->name = "Loopback10";
    ifc->config->description = "Test";

    openconfig_interfaces::Interfaces ifcs{};
    ifcs.interface.append(ifc);

    // Configure BGP
    openconfig_bgp::Bgp bgp{};
    config_bgp(bgp);

    vector<Entity*> create_entities;
    create_entities.push_back(&bgp);
    create_entities.push_back(&ifcs);

    // Set-replace Request
    auto reply = crud.create(provider, create_entities);
    REQUIRE(reply);

    reply = crud.update(provider, create_entities);
    REQUIRE(reply);

    openconfig_bgp::Bgp bgp_filter{};
    openconfig_interfaces::Interfaces int_filter{};
    vector<Entity*> filter;
    filter.push_back(&bgp_filter);
    filter.push_back(&int_filter);

    auto read_entities = crud.read(provider, filter);
    REQUIRE(read_entities.size() == 2);

    read_entities = crud.read_config(provider, filter);
    REQUIRE(read_entities.size() == 2);

    reply = crud.delete_(provider, create_entities);
    REQUIRE(reply);
}