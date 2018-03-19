//
// @file types.hpp
// @brief Header for ydk entity
//
// YANG Development Kit
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

#include "../src/types.hpp"
#include "catch.hpp"
#include "mock_data.hpp"
#include <iostream>

using namespace ydk;
using namespace std;

TEST_CASE( "test_codec_rpc" )
{
    ydk::path::Codec s{};
    std::string searchdir{TEST_HOME};
    mock::MockSession sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto a = s.decode(schema, "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><bits-value>disable-nagle auto-sense-speed</bits-value></built-in-t></ytypes></runner>", EncodingFormat::XML);
    REQUIRE(a!=nullptr);

    string pl2 = R"( <data xmlns="urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring">module xyz { } </data>)";
    auto d2 = s.decode_rpc_output(schema, pl2, "/ietf-netconf-monitoring:get-schema", EncodingFormat::XML);
    REQUIRE(d2!=nullptr);
    auto x2 = s.encode(*d2, EncodingFormat::XML, false);
    REQUIRE(x2=="<get-schema xmlns=\"urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring\"><data>module xyz { } </data></get-schema>");
}

TEST_CASE( "test_codec_action" )
{
    ydk::path::Codec s{};
    std::string searchdir{TEST_HOME};
    mock::MockSession sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    string pl2 = "<t xmlns=\"http://cisco.com/ns/yang/ydktest-action\">xyz</t>";
    auto d2 = s.decode_rpc_output(schema, pl2, "/ydktest-sanity-action:data/action-node", EncodingFormat::XML);
    REQUIRE(d2!=nullptr);
    auto x2 = s.encode(*d2, EncodingFormat::XML, false);
    REQUIRE(x2=="<data xmlns=\"http://cisco.com/ns/yang/ydktest-action\"><action-node><t>xyz</t></action-node></data>");
}

TEST_CASE( "test_codec_action_datanode" )
{
    ydk::path::Codec s{};
    std::string searchdir{TEST_HOME};
    mock::MockSession sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto & data = schema.create_datanode("ydktest-sanity-action:data");
    auto & a = data.create_action("action-node");
    a.create_datanode("test", "xyz");
    REQUIRE_NOTHROW((data)(sp));

    REQUIRE(data.get_action_node_path() == "/ydktest-sanity-action:data/action-node");
}
/*
TEST_CASE( "test_codec_ok" )
{
    ydk::path::Codec s{};
    std::string searchdir{TEST_HOME};
    mock::MockSession sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    string pl2 = "<ok/>";
    auto d2 = s.decode_rpc_output(schema, pl2, "/ietf-netconf:edit-config", EncodingFormat::XML);
    REQUIRE(d2!=nullptr);
    auto x2 = s.encode(*d2, EncodingFormat::XML, false);
    REQUIRE(x2=="<ok/>");
}*/