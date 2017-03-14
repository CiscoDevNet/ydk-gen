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

namespace mock {
class MockServiceProvider : public ydk::path::ServiceProvider
{
public:
    MockServiceProvider(const std::string searchdir, const std::vector<ydk::path::Capability> capabilities) : m_searchdir{searchdir}, m_capabilities{capabilities}
    {
        ydk::path::Repository repo{m_searchdir};
        root_schema = repo.create_root_schema(m_capabilities);
    }

    virtual ~MockServiceProvider()
    {
    }


    ydk::path::RootSchemaNode& get_root_schema() const
    {
        return *root_schema;
    }

	ydk::EncodingFormat get_encoding() const
	{
		return ydk::EncodingFormat::XML;
	}

	std::shared_ptr<ydk::path::DataNode> invoke(ydk::path::Rpc& rpc) const
	{
        ydk::path::CodecService s{};

        std::cout << s.encode(rpc.input(), ydk::EncodingFormat::XML, true) << std::endl;

		return nullptr;
	}
private:
    std::string m_searchdir;
    std::vector<ydk::path::Capability> m_capabilities;
    std::shared_ptr<ydk::path::RootSchemaNode> root_schema;

};
}


std::vector<ydk::path::Capability> test_openconfig {
    {"openconfig-bgp-types", "" },
    {"openconfig-bgp", ""},
    {"openconfig-extensions", ""},
    {"openconfig-interfaces", ""},
    {"openconfig-policy-types", ""},
    {"openconfig-routing-policy", ""},
    {"openconfig-types", ""},
    {"ietf-interfaces", ""},
    {"ydk", ""},
    {"ydktest-sanity", ""}

};
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
    std::string searchdir{TEST_HOME};
    mock::MockServiceProvider sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto & bgp = schema.create("openconfig-bgp:bgp", "");

    auto & as = bgp.create("global/config/as", "65172");

    auto & l3vpn_ipv4_unicast = bgp.create("global/afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");

    auto & afi_safi_name = l3vpn_ipv4_unicast.create("config/afi-safi-name", "openconfig-bgp-types:L3VPN_IPV4_UNICAST");

    //set the enable flag
    auto & enable = l3vpn_ipv4_unicast.create("config/enabled","true");

    //bgp/neighbors/neighbor
    auto & neighbor = bgp.create("neighbors/neighbor[neighbor-address='172.16.255.2']", "");

    auto & neighbor_address = neighbor.create("config/neighbor-address", "172.16.255.2");

    auto & peer_as = neighbor.create("config/peer-as","65172");

    //bgp/neighbors/neighbor/afi-safis/afi-safi
    auto & neighbor_af = neighbor.create("afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");

    auto & neighbor_afi_safi_name = neighbor_af.create("config/afi-safi-name" , "openconfig-bgp-types:L3VPN_IPV4_UNICAST");

    auto & neighbor_enabled = neighbor_af.create("config/enabled","true");

    ydk::path::CodecService s{};


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


    auto create_rpc = schema.rpc("ydk:create") ;
    create_rpc->input().create("entity", xml);

    //call create
    (*create_rpc)(sp);

}

TEST_CASE( "bgp_validation" )
{
    std::string searchdir{TEST_HOME};
    mock::MockServiceProvider sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto & bgp = schema.create("openconfig-bgp:bgp", "");

    auto & as = bgp.create("global/config/as", "65172");

    auto & l3vpn_ipv4_unicast = bgp.create("global/afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");

    auto & afi_safi_name = l3vpn_ipv4_unicast.create("config/afi-safi-name", "openconfig-bgp-types:L3VPN_IPV4_UNICAST");

    //set the enable flag
    auto & enable = l3vpn_ipv4_unicast.create("config/enabled","true");

    //bgp/neighbors/neighbor
    auto & neighbor = bgp.create("neighbors/neighbor[neighbor-address='172.16.255.2']", "");

    //auto & peer_group = neighbor.create("config/peer-group", "IBGP");

    auto & neighbor_address = neighbor.create("config/neighbor-address", "172.16.255.2");

    auto & peer_as = neighbor.create("config/peer-as","65172");

    auto & neighbor_remove_as = neighbor.create("config/remove-private-as", "openconfig-bgp-types:PRIVATE_AS_REMOVE_ALL");

    //bgp/neighbors/neighbor/afi-safis/afi-safi
    auto & neighbor_af = neighbor.create("afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");

    auto & neighbor_afi_safi_name = neighbor_af.create("config/afi-safi-name" , "openconfig-bgp-types:L3VPN_IPV4_UNICAST");

    auto & neighbor_enabled = neighbor_af.create("config/enabled","true");

    auto & peer_group = bgp.create("peer-groups/peer-group[peer-group-name='IBGP']", "");
    auto & peer_group_name = peer_group.create("config/peer-group-name", "IBGP");
    peer_as = peer_group.create("config/peer-as", "65172");

    ydk::path::ValidationService validation_service{};

    validation_service.validate(bgp, ydk::ValidationService::Option::EDIT_CONFIG);
}

TEST_CASE( "decode_remove_as" )
{
    std::string searchdir{TEST_HOME};
    mock::MockServiceProvider sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    ydk::path::CodecService s{};

    //XML Codec Test
    auto xml = "<bgp xmlns=\"http://openconfig.net/yang/bgp\"><neighbors><neighbor><neighbor-address>1.2.3.4</neighbor-address><config><neighbor-address>1.2.3.4</neighbor-address><remove-private-as xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:PRIVATE_AS_REMOVE_ALL</remove-private-as></config></neighbor></neighbors></bgp>";

    auto bgp = s.decode(schema, xml, ydk::EncodingFormat::XML);

    REQUIRE( bgp != nullptr);

    auto new_xml = s.encode(*bgp, ydk::EncodingFormat::XML, false);

    REQUIRE(xml == new_xml);

}

TEST_CASE( "bits_order" )
{
    std::string searchdir{TEST_HOME};
    mock::MockServiceProvider sp{searchdir, test_openconfig};
    ydk::path::CodecService s{};

    auto & schema = sp.get_root_schema();


    auto & runner = schema.create("ydktest-sanity:runner", "");

    auto  & bits = runner.create("ytypes/built-in-t/bits-value", "auto-sense-speed disable-nagle");

    auto new_xml = s.encode(
        runner, ydk::EncodingFormat::XML, false);

    auto expected = "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><bits-value>disable-nagle auto-sense-speed</bits-value></built-in-t></ytypes></runner>";
    REQUIRE( new_xml == expected );
}

TEST_CASE( "submodule" )
{//TODO fix issue with submodule
//    std::string searchdir{TEST_HOME};
//    mock::MockServiceProvider sp{searchdir, test_openconfig};
//    ydk::path::CodecService s{};
//
//    std::unique_ptr<ydk::path::RootSchemaNode> schema{sp.get_root_schema()};
//
//    REQUIRE(schema.get() != nullptr);
//
//    auto subtest = schema->create("ydktest-sanity:sub-test", "");
//    std::cout<<subtest->schema()->path()<<std::endl;
//
//    REQUIRE( subtest != nullptr );
//
//    //get the root
//    std::unique_ptr<const ydk::path::DataNode> data_root{subtest->root()};
//
//    REQUIRE( data_root != nullptr );
//
//    auto name = subtest->create("ydktest-sanity:sub-test/one-aug/name", "test");
//    REQUIRE( name!= nullptr );
//
//    auto number = subtest->create("ydktest-sanity:sub-test/one-aug/number", "3");
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
