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
#include <spdlog/spdlog.h>

#include "path_api.hpp"
#include "config.hpp"
#include "catch.hpp"
#include "entity_data_node_walker.hpp"

#include "ydk/netconf_provider.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"

using namespace std;

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


const char* expected_bgp_read ="<bgp xmlns=\"http://openconfig.net/yang/bgp\"><global><config><as>65172</as></config><use-multiple-paths><state><enabled>false</enabled></state><ebgp><state><allow-multiple-as>false</allow-multiple-as><maximum-paths>1</maximum-paths></state></ebgp><ibgp><state><maximum-paths>1</maximum-paths></state></ibgp></use-multiple-paths><route-selection-options><state><always-compare-med>false</always-compare-med><ignore-as-path-length>false</ignore-as-path-length><external-compare-router-id>true</external-compare-router-id><advertise-inactive-routes>false</advertise-inactive-routes><enable-aigp>false</enable-aigp><ignore-next-hop-igp-metric>false</ignore-next-hop-igp-metric></state></route-selection-options><afi-safis><afi-safi><afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name><config><afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name><enabled>true</enabled></config><state><enabled>false</enabled></state><graceful-restart><state><enabled>false</enabled></state></graceful-restart><route-selection-options><state><always-compare-med>false</always-compare-med><ignore-as-path-length>false</ignore-as-path-length><external-compare-router-id>true</external-compare-router-id><advertise-inactive-routes>false</advertise-inactive-routes><enable-aigp>false</enable-aigp><ignore-next-hop-igp-metric>false</ignore-next-hop-igp-metric></state></route-selection-options><use-multiple-paths><state><enabled>false</enabled></state><ebgp><state><allow-multiple-as>false</allow-multiple-as><maximum-paths>1</maximum-paths></state></ebgp><ibgp><state><maximum-paths>1</maximum-paths></state></ibgp></use-multiple-paths><apply-policy><state><default-import-policy>REJECT_ROUTE</default-import-policy><default-export-policy>REJECT_ROUTE</default-export-policy></state></apply-policy></afi-safi></afi-safis><apply-policy><state><default-import-policy>REJECT_ROUTE</default-import-policy><default-export-policy>REJECT_ROUTE</default-export-policy></state></apply-policy></global><neighbors><neighbor><neighbor-address>172.16.255.2</neighbor-address><config><neighbor-address>172.16.255.2</neighbor-address><peer-as>65172</peer-as></config><state><enabled>true</enabled><route-flap-damping>false</route-flap-damping><send-community>NONE</send-community></state><timers><state><connect-retry>30.0</connect-retry><hold-time>90.0</hold-time><keepalive-interval>30.0</keepalive-interval><minimum-advertisement-interval>30.0</minimum-advertisement-interval></state></timers><transport><state><mtu-discovery>false</mtu-discovery><passive-mode>false</passive-mode></state></transport><error-handling><state><treat-as-withdraw>false</treat-as-withdraw></state></error-handling><logging-options><state><log-neighbor-state-changes>true</log-neighbor-state-changes></state></logging-options><ebgp-multihop><state><enabled>false</enabled></state></ebgp-multihop><route-reflector><state><route-reflector-client>false</route-reflector-client></state></route-reflector><as-path-options><state><allow-own-as>0</allow-own-as><replace-peer-as>false</replace-peer-as></state></as-path-options><add-paths><state><receive>false</receive></state></add-paths><use-multiple-paths><state><enabled>false</enabled></state><ebgp><state><allow-multiple-as>false</allow-multiple-as></state></ebgp></use-multiple-paths><apply-policy><state><default-import-policy>REJECT_ROUTE</default-import-policy><default-export-policy>REJECT_ROUTE</default-export-policy></state></apply-policy><afi-safis><afi-safi><afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name><config><afi-safi-name xmlns:oc-bgp-types=\"http://openconfig.net/yang/bgp-types\">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name><enabled>true</enabled></config><state><enabled>false</enabled></state><graceful-restart><state><enabled>false</enabled></state></graceful-restart><apply-policy><state><default-import-policy>REJECT_ROUTE</default-import-policy><default-export-policy>REJECT_ROUTE</default-export-policy></state></apply-policy><use-multiple-paths><state><enabled>false</enabled></state><ebgp><state><allow-multiple-as>false</allow-multiple-as></state></ebgp></use-multiple-paths></afi-safi></afi-safis></neighbor></neighbors></bgp>";

std::string xml_payload_ip =
R"(
    <native xmlns="http://cisco.com/ns/yang/ydktest-sanity">
      <interface>
        <Loopback>
          <name>2449</name>
          <description>Interface Loopback2449</description>
          <ipv4>
            <address>
              <ip>10.10.10.10</ip>
              <netmask>255.255.255.255</netmask>
            </address>
          </ipv4>
        </Loopback>
      </interface>
    </native>
)";


void print_tree(ydk::path::DataNode* dn, const std::string& indent)
{
  try {
    ydk::path::Statement s = dn->get_schema_node().get_statement();
    if(s.keyword == "leaf" || s.keyword == "leaf-list" || s.keyword == "anyxml") {
        auto val = dn->get_value();
        std::cout << indent << "<" << s.arg << ">" << val << "</" << s.arg << ">" << std::endl;
    } else {
        std::string child_indent{indent};
        child_indent+="  ";
        std::cout << indent << "<" << s.arg << ">" << std::endl;
        for(auto c : dn->get_children())
            print_tree(c.get(), child_indent);
        std::cout << indent << "</" << s.arg << ">" << std::endl;
    }
  }
  catch (ydk::path::YCoreError &ex) {
	  std::cout << ex.what() << std::endl;
  }
}

void print_data_node(shared_ptr<ydk::path::DataNode> dn)
{
  try {
	cout << "\n=====>  Printing DataNode: '" << dn->get_path() << "'" << endl;
    print_tree(dn.get(), " ");
  }
  catch (ydk::path::YCoreError &ex) {
	  std::cout << ex.what() << std::endl;
  }
}

TEST_CASE( "decode_encode_interfaces" )
{
    ydk::path::NetconfSession session{"127.0.0.1", "admin", "admin", 12022};
    ydk::path::RootSchemaNode& root = session.get_root_schema();

    ydk::path::Codec s{};

    auto data_node = s.decode(root, xml_payload_ip, ydk::EncodingFormat::XML);
    REQUIRE(data_node != nullptr);

    auto xml = s.encode( *data_node, ydk::EncodingFormat::XML, true);
    std::cout<<xml<<std::endl;
}

TEST_CASE( "rpc_escape" )
{
    ydk::path::NetconfSession session{"127.0.0.1", "admin", "admin", 12022, "ssh"};
    ydk::path::RootSchemaNode& root = session.get_root_schema();
    ydk::path::Codec s{};

    // create RPC in specified path
    std::shared_ptr<ydk::path::Rpc> get_rpc { root.create_rpc("ietf-netconf-monitoring:get-schema") };

    // create data node on RPC
    get_rpc->get_input_node().create_datanode("identifier", "ydktest-sanity");

    // execute RPC
    auto result = (*get_rpc)(session);

    // convert result DataNode to XML and print
    auto yang = s.encode( *result, ydk::EncodingFormat::XML, true);
    //std::cout<<yang<<std::endl;
}

TEST_CASE( "bgp_netconf_create" )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    ydk::path::Codec s{};

    auto & bgp = schema.create_datanode("openconfig-bgp:bgp", "");
    //first delete
    std::shared_ptr<ydk::path::Rpc> delete_rpc { schema.create_rpc("ydk:delete") };

    auto xml = s.encode(bgp, ydk::EncodingFormat::XML, false);

    delete_rpc->get_input_node().create_datanode("entity", xml);

    //call delete
    (*delete_rpc)(session);


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

    xml = s.encode(bgp, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    REQUIRE(xml == expected_bgp_output);

    //call create
    std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    create_rpc->get_input_node().create_datanode("entity", xml);
    (*create_rpc)(session);

    //call read
    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ydk:read") };
    auto & bgp_read = schema.create_datanode("openconfig-bgp:bgp", "");

    xml = s.encode(bgp_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->get_input_node().create_datanode("filter", xml);

    auto read_result = (*read_rpc)(session);

    REQUIRE(read_result != nullptr);

    print_tree(read_result.get(),"");

    xml = s.encode(*read_result, ydk::EncodingFormat::XML, false);

    REQUIRE(xml == expected_bgp_read);

    peer_as.set_value("6500");

    //call update
    std::shared_ptr<ydk::path::Rpc> update_rpc { schema.create_rpc("ydk:update") };
    xml = s.encode(bgp, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    update_rpc->get_input_node().create_datanode("entity", xml);
    (*update_rpc)(session);


}


TEST_CASE("bits")
{
    ydk::path::Repository repo{};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    auto & runner = schema.create_datanode("ydktest-sanity:runner", "");

    auto & ysanity = runner.create_datanode("ytypes/built-in-t/bits-value", "disable-nagle");

    ydk::path::Codec s{};
    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());


    //call create
    std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    create_rpc->get_input_node().create_datanode("entity", xml);
    (*create_rpc)(session);
}

TEST_CASE("core_validate")
{
    ydk::path::Repository repo{};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    auto & runner = schema.create_datanode("ietf-netconf:validate", "");

    auto & ysanity = runner.create_datanode("source/candidate", "");

    ydk::path::Codec s{};
    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    std::cout << xml << std::endl;

    //call create
    // std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    // create_rpc->get_input_node().create_datanode("entity", xml);
    // (*create_rpc)(session);
}


TEST_CASE( "get_schema"  )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    std::shared_ptr<ydk::path::Rpc> get_schema_rpc { schema.create_rpc("ietf-netconf-monitoring:get-schema") };
    get_schema_rpc->get_input_node().create_datanode("identifier", "ydktest-sanity");

    auto res = (*get_schema_rpc)(session);

    ydk::path::Codec s{};

    auto xml = s.encode(*res, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );

}

TEST_CASE( "get_config"  )
{
    ydk::path::Repository repo{};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    std::shared_ptr<ydk::path::Rpc> get_schema_rpc { schema.create_rpc("ietf-netconf:get-config") };
    get_schema_rpc->get_input_node().create_datanode("source/candidate");
    get_schema_rpc->get_input_node().create_datanode("filter", "<bgp xmlns=\"xmlns=http://openconfig.net/yang/bgp\"/>");

    REQUIRE_NOTHROW((*get_schema_rpc)(session));

}

TEST_CASE( "bgp_xr_openconfig"  )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    ydk::path::Codec s{};

    auto & bgp = schema.create_datanode("openconfig-bgp:bgp", "");
    //get the root
    const ydk::path::DataNode* data_root = reinterpret_cast<const ydk::path::DataNode*>(&bgp.get_root());

    REQUIRE( data_root != nullptr );

    //call create
    auto & as = bgp.create_datanode("global/config/as", "65172");
    auto & router_id = bgp.create_datanode("global/config/router-id", "1.2.3.4");
    auto & l3vpn_ipv4_unicast = bgp.create_datanode("global/afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "");
    auto & afi_safi_name = l3vpn_ipv4_unicast.create_datanode("config/afi-safi-name", "openconfig-bgp-types:L3VPN_IPV4_UNICAST");
    auto & enable = l3vpn_ipv4_unicast.create_datanode("config/enabled","true");
    //bgp/neighbors/neighbor
    auto & neighbor = bgp.create_datanode("neighbors/neighbor[neighbor-address='172.16.255.2']", "");
    auto & neighbor_address = neighbor.create_datanode("config/neighbor-address", "172.16.255.2");
    auto & peer_as = neighbor.create_datanode("config/peer-as","65172");
    auto & peer_group = neighbor.create_datanode("config/peer-group","IBGP");
    //bgp/peer-groups/peer-group
    auto & ppeer_group = bgp.create_datanode("peer-groups/peer-group[peer-group-name='IBGP']", "");
    auto & peer_group_name = ppeer_group.create_datanode("config/peer-group-name", "IBGP");
    auto & ppeer_as = ppeer_group.create_datanode("config/peer-as","65172");

    std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    auto xml = s.encode(bgp, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    create_rpc->get_input_node().create_datanode("entity", xml);

    auto res = (*create_rpc)(session);

    //call read
    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ydk:read") };
    auto & bgp_read = schema.create_datanode("openconfig-bgp:bgp", "");


    xml = s.encode(bgp_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->get_input_node().create_datanode("filter", xml);
    read_rpc->get_input_node().create_datanode("only-config");

    auto read_result = (*read_rpc)(session);

    REQUIRE(read_result != nullptr);
}
//
//TEST_CASE( bgp_xr_native  )
//{


//    ydk::path::Repository repo{};
//
//    ydk::path::NetconfSession session{repo,"localhost", "admin", "admin",  1220};
//    ydk::path::RootSchemaNode& schema = session.get_root_schema();
//
//    ydk::path::Codec s{};
//
//    auto & bgp = schema.create_datanode("Cisco-IOS-XR-ipv4-bgp-cfg:bgp", "");
//
//    //call create
//    auto & instance = bgp.create_datanode("instance[instance-name='65172']");
//
//    auto & instance_as = instance->create_datanode("instance-as[as='65172']");
//
//    auto & four_instance_as = instance_as->create_datanode("four-byte-as[as='65172']");
//
//    auto & vrf = four_instance_as->create_datanode("vrfs/vrf[vrf-name='red']");
//
//  std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
//  auto xml = s.encode(bgp, ydk::EncodingFormat::XML, false);
//  REQUIRE( !xml.empty() );
//  create_rpc->get_input_node().create_datanode("entity", xml);
//
//  auto res = (*create_rpc)(session);
//
//  //call read
//    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ydk:read") };
//    auto & bgp_read = schema.create_datanode("Cisco-IOS-XR-ipv4-bgp-cfg:bgp", "");
//    std::shared_ptr<const ydk::path::DataNode> data_root2{&bgp_read.get_root()};
//
//    xml = s.encode(bgp_read, ydk::EncodingFormat::XML, false);
//    REQUIRE( !xml.empty() );
//    read_rpc->get_input_node().create_datanode("filter", xml);
//    read_rpc->get_input_node().create_datanode("only-config");
//
//    auto read_result = (*read_rpc)(session);
//
//    REQUIRE(read_result != nullptr);
//
//
//}

TEST_CASE("oc_interface")
{
    ydk::path::NetconfSession session{"127.0.0.1", "admin", "admin", 12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    auto & runner = schema.create_datanode("openconfig-interfaces:interfaces", "");

    runner.create_datanode("interface[name='GigabitEthernet0/0/0/2']/config/name", "GigabitEthernet0/0/0/2");
    runner.create_datanode("interface[name='GigabitEthernet0/0/0/2']/config/type", "iana-if-type:ethernetCsmacd");

    ydk::path::Codec s{};
    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    create_rpc->get_input_node().create_datanode("entity", xml);
    (*create_rpc)(session);
}

TEST_CASE("oc_routing")
{
    ydk::path::NetconfSession session{"127.0.0.1", "admin", "admin", 12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    auto & runner = schema.create_datanode("openconfig-routing-policy:routing-policy", "");

    runner.create_datanode("defined-sets/openconfig-bgp-policy:bgp-defined-sets/community-sets/community-set[community-set-name='COMMUNITY-SET2']/config/community-set-name", "COMMUNITY-SET2");
    runner.create_datanode("defined-sets/openconfig-bgp-policy:bgp-defined-sets/community-sets/community-set[community-set-name='COMMUNITY-SET2']/config/community-member", "65172:17001");

    ydk::path::Codec s{};
    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    create_rpc->get_input_node().create_datanode("entity", xml);
    (*create_rpc)(session);
}

TEST_CASE("multiple_delete_create_read_rpc")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();
    ydk::path::Codec s{};

    // Build filter using path API
    auto & bgp = schema.create_datanode("openconfig-bgp:bgp", "");
    auto & native = schema.create_datanode("ydktest-sanity:native", "");

    std::vector<ydk::path::DataNode*> filters{};
    filters.push_back(&native);
    filters.push_back(&bgp);
    auto xml = s.encode(filters, ydk::EncodingFormat::XML, false);

    // first delete config
    std::shared_ptr<ydk::path::Rpc> delete_rpc{ schema.create_rpc("ydk:delete") };
    delete_rpc->get_input_node().create_datanode("entity", xml);
    auto result = (*delete_rpc)(session);
    REQUIRE(result == nullptr);

    // Build BGP config
    bgp.create_datanode("global/config/as", "65172");
    bgp.create_datanode("global/config/router-id", "10.20.30.40");

    auto & neighbor = bgp.create_datanode("neighbors/neighbor[neighbor-address='172.16.255.2']", "");
    neighbor.create_datanode("config/neighbor-address", "172.16.255.2");
    neighbor.create_datanode("config/peer-as","65172");

    // Build native config
    native.create_datanode("version", "0.1.0");
    native.create_datanode("hostname", "MyHost");

    // Encode data-nodes
    xml = s.encode(filters, ydk::EncodingFormat::XML, false);

    // Create config on device
    std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    create_rpc->get_input_node().create_datanode("entity", xml);
    result = (*create_rpc)(session);
    REQUIRE(result == nullptr);

    // Read config from device
    auto & bgp_read = schema.create_datanode("openconfig-bgp:bgp", "");
    auto & native_read = schema.create_datanode("ydktest-sanity:native", "");

    filters.clear();
    filters.push_back(&native_read);
    filters.push_back(&bgp_read);
    xml = s.encode(filters, ydk::EncodingFormat::XML, false);

    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ydk:read") };
    read_rpc->get_input_node().create_datanode("only-config");
    read_rpc->get_input_node().create_datanode("filter", xml);

    auto read_result = (*read_rpc)(session);
    REQUIRE(read_result != nullptr);
    REQUIRE(read_result->get_children().size() == 2);

    // Print retrieved data
    for (auto dn : read_result->get_children()) {
        print_data_node(dn);
    }
}

TEST_CASE("action_create")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();
    ydk::path::Codec s{};

    auto & native = schema.create_datanode("ydktest-sanity-action:data", "");
    auto& a = native.create_action("action-node");
    a.create_datanode("test", "xyz");

    auto expected=R"(<data xmlns="http://cisco.com/ns/yang/ydktest-action">
  <action-node>
    <test>xyz</test>
  </action-node>
</data>
)";
    auto x = s.encode(native, ydk::EncodingFormat::XML, true);
    REQUIRE(x==expected);

    // run test knowing confd will reject action because it is not implemented on device
    REQUIRE_THROWS_AS((native)(session), ydk::YServiceProviderError);

    auto & runner = schema.create_datanode("ydktest-sanity:runner");
    // run test knowing session will reject datanode without action
    REQUIRE_THROWS_AS((runner)(session), ydk::YServiceProviderError);
}

TEST_CASE("get_schema_decode")
{
    ydk::path::NetconfSession session{"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();
    ydk::path::Codec s{};

    auto rpc = schema.create_rpc("ietf-netconf-monitoring:get-schema");
    rpc->get_input_node().create_datanode("identifier", "ydktest-sanity-action");

    auto o = (*rpc)(session);

    auto xml = s.encode(*o, ydk::EncodingFormat::XML, true);
    cout<<xml<<endl;
}
/* TODO
TEST_CASE("oc_optic")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::path::NetconfSession session{repo, "127.0.0.1", "admin", "admin", 12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    auto & runner = schema.create_datanode("openconfig-platform:components", "");

    runner.create_datanode("component[name='0/0-OpticalChannel0/0/0/19']/openconfig-terminal-device:optical-channel/config/frequency", "191600000");
    runner.create_datanode("component[name='0/0-OpticalChannel0/0/0/19']/openconfig-terminal-device:optical-channel/config/line-port", "0/0-Optics0/0/0/19");
    runner.create_datanode("component[name='0/0-OpticalChannel0/0/0/19']/openconfig-terminal-device:optical-channel/config/operational-mode", "2");
    runner.create_datanode("component[name='0/0-OpticalChannel0/0/0/19']/openconfig-terminal-device:optical-channel/config/target-output-power", "100");

    ydk::path::Codec s{};
    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    create_rpc->get_input_node().create_datanode("entity", xml);
    (*create_rpc)(session);
}
*/
TEST_CASE("test_embedded_single_quote_path") {
    ydk::path::NetconfSession session{"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();
    ydk::path::Codec s{};

    auto & runner = schema.create_datanode("ydktest-sanity:runner", "");
    runner.create_datanode("two-key-list[first=\"ab'c\"][second='123']", "");

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, true);
    string first = R"(<first>ab'c</first>)";
    REQUIRE(xml.find(first) != string::npos);
}

TEST_CASE("test_embedded_double_quote_path") {
    ydk::path::NetconfSession session{"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();
    ydk::path::Codec s{};

    auto & runner = schema.create_datanode("ydktest-sanity:runner", "");
    runner.create_datanode("two-key-list[first='ab\"c'][second='123']", "");

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, true);
    string first = R"(<first>ab&quot;c</first>)";
    REQUIRE(xml.find(first) != string::npos);
}

TEST_CASE("test_embedded_slash_path") {
    ydk::path::NetconfSession session{"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();
    ydk::path::Codec s{};

    auto & runner = schema.create_datanode("ydktest-sanity:runner", "");
    runner.create_datanode("two-key-list[first='ab/c'][second='123']", "");

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, true);
    string first = R"(<first>ab/c</first>)";
    REQUIRE(xml.find(first) != string::npos);
}
