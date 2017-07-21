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
#include "../src/path_api.hpp"
#include "config.hpp"
#include "catch.hpp"
#include "mock_data.hpp"

using namespace std;
using namespace ydk;

TEST_CASE( "codec_collection" )
{
    std::string searchdir{TEST_HOME};
    mock::MockServiceProvider sp{searchdir, test_openconfig};
    ydk::path::Codec s{};
    auto & schema = sp.get_root_schema();

    auto & bgp = schema.create_datanode("openconfig-bgp:bgp", "");
    bgp.create_datanode("global/config/as", "65172");

    auto & rp = schema.create_datanode("openconfig-routing-policy:routing-policy", "");
    rp.create_datanode("policy-definitions/policy-definition[name='POLICY1']");

    std::vector<path::DataNode*> dn {&bgp, &rp};

    path::DataNodeCollection d{dn};

    auto buf = s.encode(d, EncodingFormat::XML, false);

    auto b_1 = buf.at("openconfig-bgp:bgp");
    auto b_2 = buf.at("openconfig-routing-policy:routing-policy");

    REQUIRE(b_1 == "<bgp xmlns=\"http://openconfig.net/yang/bgp\"><global><config><as>65172</as></config></global></bgp>");
    REQUIRE(b_2 == "<routing-policy xmlns=\"http://openconfig.net/yang/routing-policy\"><policy-definitions><policy-definition><name>POLICY1</name></policy-definition></policy-definitions></routing-policy>");

    REQUIRE(buf.size() == 2);

    auto data = s.decode(schema, buf, EncodingFormat::XML);

    auto b = data.get_data_nodes();

    auto r_1 = b.at("openconfig-bgp:bgp");
    auto r_2 = b.at("openconfig-routing-policy:routing-policy");

    REQUIRE(r_1 != nullptr);
    REQUIRE(r_2 != nullptr);

    REQUIRE(s.encode(*r_1, EncodingFormat::XML, false) == b_1);
    REQUIRE(s.encode(*r_2, EncodingFormat::XML, false) == b_2);
    REQUIRE(b.size() == 2);
}


TEST_CASE( "test_codec_rpc" )
{
    ydk::path::Codec s{};
    std::string searchdir{TEST_HOME};
    mock::MockServiceProvider sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto a = s.decode(schema, "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><bits-value>disable-nagle auto-sense-speed</bits-value></built-in-t></ytypes></runner>", EncodingFormat::XML);
    REQUIRE(a!=nullptr);

    string pl2 = R"( <data xmlns="urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring">module xyz { } </data>)";
    auto d2 = s.decode_rpc_output(schema, pl2, "/ietf-netconf-monitoring:get-schema", EncodingFormat::XML);
    REQUIRE(d2!=nullptr);
    auto x2 = s.encode(*d2, EncodingFormat::XML, false);
    REQUIRE(x2=="<get-schema xmlns=\"urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring\"><data>module xyz { } </data></get-schema>");
}
