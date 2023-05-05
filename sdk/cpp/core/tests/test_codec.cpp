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

#include <iostream>

#include "catch.hpp"
#include "config.hpp"
#include "mock_data.hpp"

#include "../src/types.hpp"
#include "../src/path/path_private.hpp"

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

TEST_CASE( "test_submodule_feature" )
{
    ydk::path::Codec s{};
    std::string searchdir{TEST_HOME};

    // Add 'crypto' feature for 'ydktest-types'
    std::vector<ydk::path::Capability> test_capabilities{};
    for (auto cap : test_openconfig) {
        if (cap.module == "ydktest-types") {
            cap.features.push_back("crypto");
        }
        test_capabilities.push_back(cap);
    }
    mock::MockSession sp{searchdir, test_capabilities};

    auto & schema = sp.get_root_schema();

    auto & native = schema.create_datanode("ydktest-sanity:native");

    auto & tunnel = native.create_datanode("interface/Tunnel[name='521']");
    tunnel.create_datanode("description", "Tunnel521");

    // Protection IPsec
    auto & ipsec = tunnel.create_datanode("ipsec");
    ipsec.create_datanode("profile","ipsec-profile");
    ipsec.create_datanode("ikev2-profile","ipsec-ikev2-profile");

    auto xml = s.encode(native, ydk::EncodingFormat::XML, true);

    auto dn = s.decode(schema, xml, ydk::EncodingFormat::XML);
    REQUIRE(dn != nullptr);
    auto real_dn = dn->get_children()[0];
    REQUIRE(real_dn != nullptr);

    auto xml_rt = s.encode(*real_dn, ydk::EncodingFormat::XML, true);
    REQUIRE(xml == xml_rt);
}

TEST_CASE( "decode_multiple_json" )
{
    std::string searchdir{TEST_HOME};
    mock::MockSession sp{searchdir, test_openconfig};
    auto & schema = sp.get_root_schema();

    std::string json_int_payload = R"({
  "openconfig-interfaces:interfaces": {
    "interface": [
      {
        "name": "Loopback10",
        "config": {
          "name": "Loopback10"
        }
      }
    ]
  }
}
)";
    std::string json_bgp_payload = R"({
  "openconfig-bgp:bgp": {
    "global": {
      "config": {
        "as": 65172
      }
    }
  }
}
)";
    std::vector<std::string> payload_list = {json_int_payload, json_bgp_payload};

    ydk::path::Codec s{};

    auto rdn = s.decode_json_output(schema, payload_list);
    REQUIRE(rdn != nullptr);

    auto dn = dynamic_cast<ydk::path::DataNode*> (rdn.get());
    auto json_str = s.encode(*dn, EncodingFormat::JSON, true);
    REQUIRE(json_str == json_int_payload + json_bgp_payload);
}

TEST_CASE("test_no_key_list_path") {
    std::string searchdir{TEST_HOME};
    mock::MockSession sp{searchdir, test_openconfig};
    auto & schema = sp.get_root_schema();
    ydk::path::Codec codec{};

    auto & runner = schema.create_datanode("ydktest-sanity:runner", "");
    runner.create_datanode("no-key-list[1]/test", "t1");
    runner.create_datanode("no-key-list[2]/test", "t2");

    auto xml = codec.encode(runner, ydk::EncodingFormat::XML, true);
    auto expected = R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <no-key-list>
    <test>t1</test>
  </no-key-list>
  <no-key-list>
    <test>t2</test>
  </no-key-list>
</runner>
)";
    REQUIRE(xml== expected);

    auto dn = codec.decode(schema, xml, ydk::EncodingFormat::XML);
    REQUIRE(dn != nullptr);
    auto real_dn = dn->get_children()[0];
    REQUIRE(real_dn != nullptr);

    auto xml_rt = codec.encode(*real_dn, ydk::EncodingFormat::XML, true);
    REQUIRE(xml == xml_rt);
}

std::string value_error_json = R"({
  "ydktest-sanity:runner": {
    "ytypes": {
      "built-in-t": {
        "identity-llist": [
          "ydktest-sanity:child-identity",
          "ydktest-sanity:child-child-identity"
        ]
      }
    }
  }
}
)";

TEST_CASE("value_error")
{
    ydk::path::Codec s{};
    std::string searchdir{TEST_HOME};
    mock::MockSession sp{searchdir, test_openconfig};
    auto & schema = sp.get_root_schema();

    CHECK_NOTHROW(s.decode(schema, value_error_json, EncodingFormat::JSON));
}

namespace ydk {
  namespace path {
    std::string get_netconf_output(const string & reply);
  }
}

TEST_CASE( "decode_rpc_with_nc_prefix" )
{
    auto rpc_reply = R"(
<nc:rpc-reply xmlns:nc="urn:ietf:params:xml:ns:netconf:base:1.0" xmlns:junos="http://xml.juniper.net/junos/15.1X49/junos" xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="1">
<nc:data>
<interfaces xmlns="http://openconfig.net/yang/interfaces">
  <interface>
    <name>GigabitEthernet0/0/0/2</name>
    <config>
      <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:ethernetCsmacd</type>
      <name>GigabitEthernet0/0/0/2</name>
    </config>
  </interface>
</interfaces>
</nc:data>
</nc:rpc-reply>
)";
    auto reply_xml = ydk::path::get_netconf_output(rpc_reply);
    REQUIRE(!reply_xml.empty());

    auto rpc_reply_extracted = R"(<interfaces xmlns="http://openconfig.net/yang/interfaces">
  <interface>
    <name>GigabitEthernet0/0/0/2</name>
    <config>
      <type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">ianaift:ethernetCsmacd</type>
      <name>GigabitEthernet0/0/0/2</name>
    </config>
  </interface>
</interfaces>)";
    REQUIRE(reply_xml == rpc_reply_extracted);
}
