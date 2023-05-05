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

const char* m = "\
<bgp xmlns=\"http://openconfig.net/yang/bgp\">\
<global>\
<config>\
<as>65172</as>\
</config>\
<afi-safis>\
<afi-safi>\
<afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>\
<config>\
<afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>\
<enabled>true</enabled>\
</config>\
</afi-safi>\
</afi-safis>\
</global>\
<neighbors>\
<neighbor>\
<neighbor-address\"(!error!)\"\
<config>\
<neighbor-address>172.16.255.2</neighbor-address>\
<peer-as>65172</peer-as>\
</config>\
<afi-safis>\
<afi-safi>\
<afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>\
<config>\
<afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>\
<enabled>true</enabled>\
</config>\
</afi-safi>\
</afi-safis>\
</neighbor>\
</neighbors>\
</bgp>";


const char* expected_bgp_output ="\
<bgp xmlns=\"http://openconfig.net/yang/bgp\">\
<global>\
<config>\
<as>65172</as>\
</config>\
<afi-safis>\
<afi-safi>\
<afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>\
<config>\
<afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>\
<enabled>true</enabled>\
</config>\
</afi-safi>\
</afi-safis>\
</global>\
<neighbors>\
<neighbor>\
<neighbor-address>172.16.255.2</neighbor-address>\
<config>\
<neighbor-address>172.16.255.2</neighbor-address>\
<peer-as>65172</peer-as>\
</config>\
<afi-safis>\
<afi-safi>\
<afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>\
<config>\
<afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>\
<enabled>true</enabled>\
</config>\
</afi-safi>\
</afi-safis>\
</neighbor>\
</neighbors>\
</bgp>";

const char* expected_bgp_json = "\
{\
\"openconfig-bgp:bgp\":{\
\"global\":{\
\"config\":{\
\"as\":65172\
},\
\"afi-safis\":{\
\"afi-safi\":[\
{\
\"afi-safi-name\":\"openconfig-bgp-types:L3VPN_IPV4_UNICAST\",\
\"config\":{\
\"afi-safi-name\":\"openconfig-bgp-types:L3VPN_IPV4_UNICAST\",\
\"enabled\":true\
}\
}\
]\
}\
},\
\"neighbors\":{\
\"neighbor\":[\
{\
\"neighbor-address\":\"172.16.255.2\",\
\"config\":{\
\"neighbor-address\":\"172.16.255.2\",\
\"peer-as\":65172\
},\
\"afi-safis\":{\
\"afi-safi\":[\
{\
\"afi-safi-name\":\"openconfig-bgp-types:L3VPN_IPV4_UNICAST\",\
\"config\":{\
\"afi-safi-name\":\"openconfig-bgp-types:L3VPN_IPV4_UNICAST\",\
\"enabled\":true\
}\
}\
]\
}\
}\
]\
}\
}\
}";

const char* expected_bgp_peer_xml = "<bgp xmlns=\"http://openconfig.net/yang/bgp\"><global><config><as>65172</as></config><afi-safis><afi-safi><afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name><config><afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name><enabled>true</enabled></config></afi-safi></afi-safis></global><peer-groups><peer-group><peer-group-name>IBGP</peer-group-name><config><peer-group-name>IBGP</peer-group-name><peer-as>65001</peer-as></config><afi-safis><afi-safi><afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name><config><afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name><enabled>true</enabled></config><apply-policy><config><export-policy>POLICY2</export-policy></config></apply-policy></afi-safi></afi-safis></peer-group></peer-groups><neighbors><neighbor><neighbor-address>172.16.255.2</neighbor-address><config><neighbor-address>172.16.255.2</neighbor-address><peer-group>IBGP</peer-group></config></neighbor></neighbors></bgp>";
const char* expected_bgp_peer_json = "{\"openconfig-bgp:bgp\":{\"global\":{\"config\":{\"as\":65172},\"afi-safis\":{\"afi-safi\":[{\"afi-safi-name\":\"openconfig-bgp-types:L3VPN_IPV4_UNICAST\",\"config\":{\"afi-safi-name\":\"openconfig-bgp-types:L3VPN_IPV4_UNICAST\",\"enabled\":true}}]}},\"peer-groups\":{\"peer-group\":[{\"peer-group-name\":\"IBGP\",\"config\":{\"peer-group-name\":\"IBGP\",\"peer-as\":65001},\"afi-safis\":{\"afi-safi\":[{\"afi-safi-name\":\"openconfig-bgp-types:L3VPN_IPV4_UNICAST\",\"config\":{\"afi-safi-name\":\"openconfig-bgp-types:L3VPN_IPV4_UNICAST\",\"enabled\":true},\"apply-policy\":{\"config\":{\"export-policy\":[\"POLICY2\"]}}}]}}]},\"neighbors\":{\"neighbor\":[{\"neighbor-address\":\"172.16.255.2\",\"config\":{\"neighbor-address\":\"172.16.255.2\",\"peer-group\":\"IBGP\"}}]}}}";

TEST_CASE( "bgp" )
{
    ydk::path::Repository repo{TEST_HOME};
    mock::MockSession sp{TEST_HOME, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto & bgp = schema.create_datanode("openconfig-bgp:bgp", "");

    auto & as = bgp.create_datanode("global/config/as", "65172");

    auto & l3vpn_ipv4_unicast = bgp.create_datanode("global/afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");

    auto & afi_safi_name = l3vpn_ipv4_unicast.create_datanode("config/afi-safi-name", "openconfig-bgp-types:L3VPN_IPV4_UNICAST");

    //set the enable flag
    auto & enable = l3vpn_ipv4_unicast.create_datanode("config/enabled","true");

    //bgp/neighbors/neighbor
    auto & neighbor = bgp.create_datanode("neighbors/neighbor[neighbor-address='172.16.255.2']", "");

    auto & neighbor_address = neighbor.create_datanode("config/neighbor-address", "172.16.255.2");

    auto & peer_as = neighbor.create_datanode("config/peer-as","65172");

    //bgp/neighbors/neighbor/afi-safis/afi-safi
    auto & neighbor_af = neighbor.create_datanode("afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");

    auto & neighbor_afi_safi_name = neighbor_af.create_datanode("config/afi-safi-name" , "openconfig-bgp-types:L3VPN_IPV4_UNICAST");

    auto & neighbor_enabled = neighbor_af.create_datanode("config/enabled","true");

    ydk::path::Codec s{};


    //XML Codec Test
    auto xml = s.encode(bgp, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    REQUIRE(xml == expected_bgp_output);

    auto new_bgp = s.decode(schema, xml, ydk::EncodingFormat::XML);

    auto new_xml = s.encode(*new_bgp, ydk::EncodingFormat::XML, false);

    CHECK(!new_xml.empty());



    REQUIRE(new_xml == expected_bgp_output);


    //JSON codec test
    auto json = s.encode(bgp, ydk::EncodingFormat::JSON, false);

    CHECK( !json.empty());

    REQUIRE(json == expected_bgp_json);

    auto new_bgp1 = s.decode(schema, json, ydk::EncodingFormat::JSON);

    REQUIRE( new_bgp1 != nullptr);

    auto new_json = s.encode(*new_bgp1, ydk::EncodingFormat::JSON, false);


    CHECK(!new_json.empty());

    REQUIRE(new_json == expected_bgp_json);


    auto create_rpc = schema.create_rpc("ydk:create") ;
    create_rpc->get_input_node().create_datanode("entity", xml);

    //call create
    (*create_rpc)(sp);

}

TEST_CASE( "bgp_validation" )
{
    ydk::path::Repository repo{TEST_HOME};

    mock::MockSession sp{TEST_HOME, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto & bgp = schema.create_datanode("openconfig-bgp:bgp", "");

    auto & as = bgp.create_datanode("global/config/as", "65172");

    auto & l3vpn_ipv4_unicast = bgp.create_datanode("global/afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");

    auto & afi_safi_name = l3vpn_ipv4_unicast.create_datanode("config/afi-safi-name", "openconfig-bgp-types:L3VPN_IPV4_UNICAST");

    //set the enable flag
    auto & enable = l3vpn_ipv4_unicast.create_datanode("config/enabled","true");

    //bgp/neighbors/neighbor
    auto & neighbor = bgp.create_datanode("neighbors/neighbor[neighbor-address='172.16.255.2']", "");

    //auto & peer_group = neighbor.create_datanode("config/peer-group", "IBGP");

    auto & neighbor_address = neighbor.create_datanode("config/neighbor-address", "172.16.255.2");

    auto & peer_as = neighbor.create_datanode("config/peer-as","65172");

    auto & neighbor_remove_as = neighbor.create_datanode("config/remove-private-as", "openconfig-bgp-types:PRIVATE_AS_REMOVE_ALL");

    //bgp/neighbors/neighbor/afi-safis/afi-safi
    auto & neighbor_af = neighbor.create_datanode("afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");

    auto & neighbor_afi_safi_name = neighbor_af.create_datanode("config/afi-safi-name" , "openconfig-bgp-types:L3VPN_IPV4_UNICAST");

    auto & neighbor_enabled = neighbor_af.create_datanode("config/enabled","true");

    auto & peer_group = bgp.create_datanode("peer-groups/peer-group[peer-group-name='IBGP']", "");
    auto & peer_group_name = peer_group.create_datanode("config/peer-group-name", "IBGP");
    peer_as = peer_group.create_datanode("config/peer-as", "65172");

    ydk::path::ValidationService validation_service{};

    validation_service.validate(bgp, ydk::ValidationService::Option::EDIT_CONFIG);
}

TEST_CASE( "decode_remove_as" )
{
    ydk::path::Repository repo{TEST_HOME};

    mock::MockSession sp{TEST_HOME, test_openconfig};

    auto & schema = sp.get_root_schema();

    ydk::path::Codec s{};

    //XML Codec Test
    auto xml = "<bgp xmlns=\"http://openconfig.net/yang/bgp\"><neighbors><neighbor><neighbor-address>1.2.3.4</neighbor-address><config><neighbor-address>1.2.3.4</neighbor-address><remove-private-as xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:PRIVATE_AS_REMOVE_ALL</remove-private-as></config></neighbor></neighbors></bgp>";

    auto bgp = s.decode(schema, xml, ydk::EncodingFormat::XML);

    REQUIRE( bgp != nullptr);

    auto new_xml = s.encode(*bgp, ydk::EncodingFormat::XML, false);

    REQUIRE(xml == new_xml);

}

TEST_CASE( "bits_order" )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::Codec s{};

    mock::MockSession sp{TEST_HOME, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto & runner = schema.create_datanode("ydktest-sanity:runner", "");

    auto  & bits = runner.create_datanode("ytypes/built-in-t/bits-value", "auto-sense-speed disable-nagle");

    auto new_xml = s.encode(
        runner, ydk::EncodingFormat::XML, false);

    auto expected = "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><bits-value>disable-nagle auto-sense-speed</bits-value></built-in-t></ytypes></runner>";
    REQUIRE( new_xml == expected );
}

TEST_CASE("rpc_output")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::Codec s{};

    mock::MockSession sp{TEST_HOME, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto getc = schema.create_rpc("ietf-netconf:get-config");
    REQUIRE(getc->has_output_node() == true);
    auto get = schema.create_rpc("ietf-netconf:get");
    REQUIRE(get->has_output_node() == true);
    auto editc = schema.create_rpc("ietf-netconf:edit-config");
    REQUIRE(editc->has_output_node() == false);
    auto val = schema.create_rpc("ietf-netconf:validate");
    REQUIRE(val->has_output_node() == false);
    auto com = schema.create_rpc("ietf-netconf:commit");
    REQUIRE(com->has_output_node() == false);
    auto lo = schema.create_rpc("ietf-netconf:lock");
    REQUIRE(lo->has_output_node() == false);
}

TEST_CASE( "submodule" )
{//TODO fix issue with submodule
    // ydk::path::Repository repo{TEST_HOME};
//    mock::MockServiceProvider sp{repo, test_openconfig};
//    ydk::path::Codec s{};
//
//    std::unique_ptr<ydk::path::RootSchemaNode> schema{sp.get_root_schema()};
//
//    REQUIRE(schema.get_value() != nullptr);
//
//    auto subtest = schema->create_datanode("ydktest-sanity:sub-test", "");
//    std::cout<<subtest->get_schema_node()->get_path()<<std::endl;
//
//    REQUIRE( subtest != nullptr );
//
//    //get the root
//    std::unique_ptr<const ydk::path::DataNode> data_root{subtest->get_root()};
//
//    REQUIRE( data_root != nullptr );
//
//    auto name = subtest->create_datanode("ydktest-sanity:sub-test/one-aug/name", "test");
//    REQUIRE( name!= nullptr );
//
//    auto number = subtest->create_datanode("ydktest-sanity:sub-test/one-aug/number", "3");
//    REQUIRE( number!= nullptr );

//    auto ne1w_xml = s.encode(*subtest, ydk::EncodingFormat::XML, false);

//    auto expected = "<sub-test xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><one-aug><name>test</name></one-aug><one-aug><number>3</number></one-aug></sub-test>";
////    REQUIRE( new_xml == expected );
//
//    auto new_bgp = s.decode(schema, expected, ydk::EncodingFormat::XML);
//
//    auto new_xml = s.encode(*new_bgp, ydk::EncodingFormat::XML, false);
//    std::cout<<new_xml<<std::endl;
//
//    auto bgp = s.decode(schema, new_xml, ydk::EncodingFormat::XML);
}
