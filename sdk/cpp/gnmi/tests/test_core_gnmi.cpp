/*  ----------------------------------------------------------------
 YDK - YANG Development Kit
 Copyright 2016 Cisco Systems. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 -------------------------------------------------------------------
 This file has been modified by Yan Gorelik, YDK Solutions.
 All modifications in original under CiscoDevNet domain
 introduced since October 2019 are copyrighted.
 All rights reserved under Apache License, Version 2.0.
 ------------------------------------------------------------------*/

#include <iostream>
#include <spdlog/spdlog.h>

#include <ydk/gnmi_path_api.hpp>
#include <ydk/gnmi_util.hpp>
#include <ydk/gnmi_provider.hpp>
#include <ydk/filters.hpp>

#include "test_utils.hpp"

#include "../../core/src/path/path_private.hpp"
#include "../../core/src/catch.hpp"
#include "../../core/tests/config.hpp"
#include "../../core/tests/mock_data.hpp"

#include <ydk_ydktest/openconfig_interfaces.hpp>
#include <ydk_ydktest/ydktest_sanity.hpp>

TEST_CASE( "test_gnmi_entity_to_path" )
{
    auto ifc = ydktest::openconfig_interfaces::Interfaces::Interface();
    ifc.name = "Loopback10";
    ifc.config->yfilter = ydk::YFilter::read;

    gnmi::Path* path = new gnmi::Path();
    ydk::parse_entity_to_path(ifc, path);

    std::string expected = R"(origin: "openconfig-interfaces"
elem {
  name: "interfaces"
}
elem {
  name: "interface"
  key {
    key: "name"
    value: "Loopback10"
  }
}
elem {
  name: "config"
}
)";
    REQUIRE(path->DebugString() == expected);

    path->Clear();
    delete path;
}

TEST_CASE( "test_gnmi_entity_leaf_to_path" )
{
    auto ifc = std::make_shared<ydktest::openconfig_interfaces::Interfaces::Interface>();
    ifc->name = "Loopback10";
    ifc->config->description.yfilter = ydk::YFilter::read;

    ydktest::openconfig_interfaces::Interfaces ifcs{};
    ifcs.interface.append(ifc);

    gnmi::Path* path = new gnmi::Path();
    ydk::parse_entity_to_path(ifcs, path);

    std::string expected = R"(origin: "openconfig-interfaces"
elem {
  name: "interfaces"
}
elem {
  name: "interface"
  key {
    key: "name"
    value: "Loopback10"
  }
}
elem {
  name: "config"
}
elem {
  name: "description"
}
)";
    REQUIRE(path->DebugString() == expected);

    path->Clear();
    delete path;
}

TEST_CASE( "test_gnmi_entity_key_and_leaf_to_path" )
{
    auto l_1 = std::make_shared<ydktest::ydktest_sanity::Runner::TwoList::Ldata>();
    l_1->number = 11;
    l_1->name.yfilter = ydk::YFilter::read;

    ydktest::ydktest_sanity::Runner runner{};
    runner.two_list->ldata.append(l_1);

    gnmi::Path* path = new gnmi::Path();
    ydk::parse_entity_to_path(runner, path);

    std::string expected = R"(origin: "ydktest-sanity"
elem {
  name: "runner"
}
elem {
  name: "two-list"
}
elem {
  name: "ldata"
  key {
    key: "number"
    value: "11"
  }
}
elem {
  name: "name"
}
)";
    REQUIRE(path->DebugString() == expected);

    path->Clear();
    delete path;
}

TEST_CASE( "test_gnmi_entity_key_and_leaf_to_path2" )
{
    auto l_1 = std::make_shared<ydktest::ydktest_sanity::Runner::TwoList::Ldata>();
    l_1->number = 11;
    l_1->name = "l1-name";

    ydktest::ydktest_sanity::Runner runner{};
    runner.two_list->ldata.append(l_1);

    auto s_11 = std::make_shared<ydktest::ydktest_sanity::Runner::TwoList::Ldata::Subl1>();
    s_11->number = 211;
    s_11->name.yfilter = ydk::YFilter::read;
    l_1->subl1.append(s_11);

    gnmi::Path* path = new gnmi::Path();
    ydk::parse_entity_to_path(runner, path);

    std::string expected = R"(origin: "ydktest-sanity"
elem {
  name: "runner"
}
elem {
  name: "two-list"
}
elem {
  name: "ldata"
  key {
    key: "number"
    value: "11"
  }
}
elem {
  name: "subl1"
  key {
    key: "number"
    value: "211"
  }
}
elem {
  name: "name"
}
)";
    REQUIRE(path->DebugString() == expected);

    path->Clear();
    delete path;
}

TEST_CASE( "test_gnmi_datanode_to_path" )
{
    ydk::path::Repository repo{TEST_HOME};
    mock::MockSession sp{TEST_HOME, test_openconfig};
    auto & schema = sp.get_root_schema();

    auto & bgp = schema.create_datanode("openconfig-bgp:bgp");
    auto & neighbor = bgp.create_datanode("neighbors/neighbor[neighbor-address='172.16.255.2']");
    auto & neighbor_address = neighbor.create_datanode("config/neighbor-address", "172.16.255.2");
    auto & peer_as = neighbor.create_datanode("config/peer-as","65172");

    gnmi::Path* path = new gnmi::Path();
    ydk::path::parse_datanode_to_path(&peer_as, path);

    std::string expected = R"(origin: "openconfig-bgp"
elem {
  name: "bgp"
}
elem {
  name: "neighbors"
}
elem {
  name: "neighbor"
  key {
    key: "neighbor-address"
    value: "172.16.255.2"
  }
}
elem {
  name: "config"
}
elem {
  name: "peer-as"
}
)";
    REQUIRE(path->DebugString() == expected);

    path->Clear();
    delete path;
}

TEST_CASE( "test_gnmi_datanode_to_path_2" )
{
    ydk::path::Repository repo{TEST_HOME};
    mock::MockSession sp{TEST_HOME, test_openconfig};
    auto & schema = sp.get_root_schema();

    auto & runner = schema.create_datanode("ydktest-sanity:runner");
    auto & two_pr  = runner.create_datanode("two-key-list[first='GigabitEthernet0/0/0/0'][second='222']");

    gnmi::Path* path = new gnmi::Path();
    ydk::path::parse_datanode_to_path(&two_pr, path);

    std::string expected = R"(origin: "ydktest-sanity"
elem {
  name: "runner"
}
elem {
  name: "two-key-list"
  key {
    key: "first"
    value: "GigabitEthernet0/0/0/0"
  }
  key {
    key: "second"
    value: "222"
  }
}
)";
    REQUIRE(path->DebugString() == expected);

    path->Clear();
    delete path;
}

TEST_CASE("gnmi_test_json_payload"  )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::gNMISession session{repo, "127.0.0.1", 50051, "admin", "admin"};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    ydk::path::Codec s{};

    auto bgp = s.decode(schema, R"(
    {"openconfig-bgp:bgp":
      {
         "@": {
            "ietf-netconf:operation":"merge"
        },
        "global": {
          "config": {
            "as": 65172,
            "router-id": "1.2.3.4"
          }
        },
        "neighbors": {
          "neighbor": [
            {
              "neighbor-address": "6.7.8.9",
              "config": {
                "local-as": 65001,
                "neighbor-address": "6.7.8.9",
                "peer-as": 65001,
                "peer-group": "IBGP"
              }
            }
          ]
        },
        "peer-groups": {
          "peer-group": [
            {
              "peer-group-name": "IBGP",
              "config": {
                "description": "test description",
                "local-as": 65001,
                "peer-as": 65001,
                "peer-group-name": "IBGP"
              }
            }
          ]
        }
      }
    })", ydk::EncodingFormat::JSON);

    std::string json = s.encode(*bgp, ydk::EncodingFormat::JSON, false);
    //std::cout<< json<<std::endl;

    auto edit_config = schema.create_rpc("ietf-netconf:get-config");
    edit_config->get_input_node().create_datanode("source/candidate");
    edit_config->get_input_node().create_datanode("filter",json);

    std::string rpc_json = s.encode(edit_config->get_input_node(), ydk::EncodingFormat::JSON, false);
    std::cout<< rpc_json<<std::endl;
}

void gnmi_service_subscribe_multiples_callback(const char *);

TEST_CASE("gnmi_rpc_subscribe")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::gNMIServiceProvider provider{repo, "127.0.0.1", 50051, "admin", "admin"};
    const ydk::path::gNMISession& session = dynamic_cast<const ydk::path::gNMISession&> (provider.get_session());
    ydk::path::RootSchemaNode& schema = session.get_root_schema();
    ydk::path::Codec codec{};

    // Build configuration on the server
    build_int_config(provider);
    build_bgp_config(provider);

    std::shared_ptr<ydk::path::Rpc> rpc { schema.create_rpc("ydk:gnmi-subscribe") };
    auto & subscription = rpc->get_input_node().create_datanode("subscription", "");
    subscription.create_datanode("mode", "ONCE");
    subscription.create_datanode("qos", "10");
    subscription.create_datanode("encoding", "JSON_IETF");

    auto & ifs = schema.create_datanode("openconfig-interfaces:interfaces", "");
    auto int_json = codec.encode(ifs, ydk::EncodingFormat::JSON, false);

    auto & bgp = schema.create_datanode("openconfig-bgp:bgp", "");
    bgp.create_datanode("neighbors");
    auto bgp_json = codec.encode(bgp, ydk::EncodingFormat::JSON, false);

    auto & int_subscription = subscription.create_datanode("subscription-list[alias='int']", "");
    int_subscription.create_datanode("entity", int_json);
    int_subscription.create_datanode("subscription-mode", "ON_CHANGE");
    int_subscription.create_datanode("sample-interval", "10000000");
    int_subscription.create_datanode("suppress-redundant", "true");
    int_subscription.create_datanode("heartbeat-interval", "1000000000");

    auto & bgp_subscription = subscription.create_datanode("subscription-list[alias='bgp']", "");
    bgp_subscription.create_datanode("entity", bgp_json);
    // bgp_subscription.create_datanode("subscription-mode", "ON_CHANGE");	// can be skipped as default
    bgp_subscription.create_datanode("sample-interval", "20000000");
    // bgp_subscription.create_datanode("suppress-redundant", "true");  	// can be skipped as default
    // bgp_subscription.create_datanode("heartbeat-interval", "2000000000");// can be skipped as it is calculated from sample-interval

    // auto json = codec.encode(rpc->get_input_node(), ydk::EncodingFormat::JSON, true);
    // std::cout << "Built RPC:" << std::endl << json << std::endl;

    session.invoke_subscribe(*rpc, gnmi_service_subscribe_multiples_callback);
}

TEST_CASE("gnmi_rpc_caps")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::path::gNMISession session{repo, "127.0.0.1", 50051, "admin", "admin"};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    std::shared_ptr<ydk::path::Rpc> cap_rpc { schema.create_rpc("ydk:gnmi-caps") };
    auto caps = (*cap_rpc)(session);

    ydk::path::Codec codec{};
    auto json = codec.encode(*caps, ydk::EncodingFormat::JSON, true);
    //std::cout << "Server capabilities:" << std::endl << json << std::endl;
}

TEST_CASE("gnmi_rpc_set_get_bgp")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::path::gNMISession session{repo, "127.0.0.1", 50051, "admin", "admin"};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    // Configure BGP
    auto & bgp = schema.create_datanode("openconfig-bgp:bgp");
    auto & as = bgp.create_datanode("global/config/as", "65172");
    auto & neighbor = bgp.create_datanode("neighbors/neighbor[neighbor-address='172.16.255.2']");
    auto & neighbor_address = neighbor.create_datanode("config/neighbor-address", "172.16.255.2");
    auto & peer_as = neighbor.create_datanode("config/peer-as","65172");

    // Configure interface
    auto & ifc = schema.create_datanode("openconfig-interfaces:interfaces");
    auto & lo10_config = ifc.create_datanode("interface[name='Loopback10']");

    // Add data-nodes to RPC
    ydk::path::Codec s{};
    auto bgp_json = s.encode(bgp, ydk::EncodingFormat::JSON, true);
    auto int_json = s.encode(ifc, ydk::EncodingFormat::JSON, true);

    std::shared_ptr<ydk::path::Rpc> set_rpc { schema.create_rpc("ydk:gnmi-set") };
    set_rpc->get_input_node().create_datanode("replace[alias='bgp']/entity", bgp_json);
    set_rpc->get_input_node().create_datanode("replace[alias='int']/entity", int_json);

    auto res = (*set_rpc)(session);

	//call read
    auto & bgp_read = schema.create_datanode("openconfig-bgp:bgp", "");
    auto json_bgp = s.encode(bgp_read, ydk::EncodingFormat::JSON, false);
    REQUIRE( !json_bgp.empty() );

    auto & int_read = schema.create_datanode("openconfig-interfaces:interfaces", "");
    auto json_int = s.encode(int_read, ydk::EncodingFormat::JSON, false);
    REQUIRE( !json_int.empty() );

    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ydk:gnmi-get") };
    read_rpc->get_input_node().create_datanode("type", "CONFIG");

    read_rpc->get_input_node().create_datanode("request[alias='bgp']/entity", json_bgp);
    read_rpc->get_input_node().create_datanode("request[alias='int']/entity", json_int);

    auto read_result = (*read_rpc)(session);

    REQUIRE(read_result != nullptr);
}
TEST_CASE("gnmi_rpc_get_bgp_leaf")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::gNMIServiceProvider provider{repo, "127.0.0.1", 50051, "admin", "admin"};
    auto & session = provider.get_session();
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    // Build configuration on the server
    build_bgp_config(provider);

    // Read BGP/global/as
    auto & bgp = schema.create_datanode("openconfig-bgp:bgp");
    auto & as = bgp.create_datanode("global/config/as", "0");
    ydk::path::Codec s{};
    auto json_bgp = s.encode(bgp, ydk::EncodingFormat::JSON, false);

    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ydk:gnmi-get") };
    read_rpc->get_input_node().create_datanode("type", "CONFIG");
    read_rpc->get_input_node().create_datanode("request[alias='bgp']/entity", json_bgp);

    auto read_result = (*read_rpc)(session);
    REQUIRE(read_result != nullptr);
    //std::cout << print_tree(read_result.get(), " ") << std::endl;
    std::string expected = R"( <>
   <bgp>
     <global>
       <config>
         <as>65172</as>
       </config>
     </global>
   </bgp>
 </>
)";
    REQUIRE(print_tree(read_result.get(), " ") == expected);
}
