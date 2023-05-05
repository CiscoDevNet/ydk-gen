/*  ----------------------------------------------------------------
 Copyright 2016 Cisco Systems

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ------------------------------------------------------------------*/

#include <string.h>
#include <iostream>

#include <ydk/netconf_provider.hpp>
#include <ydk/netconf_service.hpp>
#include <ydk/types.hpp>
#include <ydk/filters.hpp>
#include <ydk/common_utilities.hpp>

#include "config.hpp"
#include "catch.hpp"
#include "test_utils.hpp"

#include <ydk_ydktest/ydktest_sanity.hpp>
#include <ydk_ydktest/openconfig_bgp.hpp>
#include <ydk_ydktest/openconfig_interfaces.hpp>
#include <ydk_ydktest/iana_if_type.hpp>

using namespace ydk;
using namespace ydktest;
using namespace std;

// cancel_commit -- issues in netsim
//TEST_CASE("cancel_commit")
//{
//    // session
//    path::Repository repo{TEST_HOME};
//    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
//    NetconfService ns{};
//
//    auto reply = ns.cancel_commit(session);
//    REQUIRE(reply);
//}

// close_session
TEST_CASE("close_session")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    auto reply = ns.close_session(provider);
    REQUIRE(reply);
}

// commit
TEST_CASE("commit")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    auto reply = ns.commit(provider);
    REQUIRE(reply);
}

// copy_config
TEST_CASE("copy_config")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;
    DataStore source = DataStore::running;

    auto reply = ns.copy_config(provider, target, source);
    REQUIRE(reply);
}

// delete_config -- issues in netsim
TEST_CASE("delete_config")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::url;

    CHECK_THROWS_AS(ns.delete_config(provider, target, "http://test"), YError);
}

// discard_changes
TEST_CASE("discard_changes")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    auto reply = ns.discard_changes(provider);
    REQUIRE(reply);
}

TEST_CASE("get_edit_copy_config")
{
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;
    DataStore source = DataStore::running;

    // Buils some configuration
    auto bgp = openconfig_bgp::Bgp();
    bgp.global->config->as = 6500;
    bgp.global->config->router_id = "10.20.30.40";
    auto ifcs = openconfig_interfaces::Interfaces();
    auto ifc  = make_shared<openconfig_interfaces::Interfaces::Interface>();
    ifc->name = "Loopback10";
    ifc->config->name = "Loopback10";
    ifc->config->description = "Test Loopback10 config";
    ifcs.interface.append(ifc);

    vector<ydk::Entity*> config_list;
    config_list.push_back(&bgp);
    config_list.push_back(&ifcs);
    if (ns.edit_config(provider, target, config_list))
        ns.commit(provider);

    // Build filter
    openconfig_interfaces::Interfaces interfaces_filter{};
    openconfig_bgp::Bgp bgp_filter{};
    vector<ydk::Entity*> filter_list{};
    filter_list.push_back(&interfaces_filter);
    filter_list.push_back(&bgp_filter);

    // Read running config
    auto get_config_list = ns.get_config(provider, source, filter_list);

    vector<Entity*> copy_config_list{};
    for (auto ent : get_config_list) {
    	//print_entity(ent, provider.get_session().get_root_schema());
        copy_config_list.push_back(ent.get());
    }

    // Copy config to candidate
    auto result = ns.copy_config(provider, target, copy_config_list);
    REQUIRE(result);

    // Discard changes
    result = ns.discard_changes(provider);
    REQUIRE(result);
}

TEST_CASE("iana_if_type_filter")
{
    NetconfServiceProvider provider{"127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    // Buils some configuration
    auto ifcs_config = openconfig_interfaces::Interfaces();
    auto ifc_config  = make_shared<openconfig_interfaces::Interfaces::Interface>();
    ifc_config->name = "GigabitEthernet0/0/0/2";
    ifc_config->config->name = "GigabitEthernet0/0/0/2";
    ifc_config->config->description = "Test interface";
    ifc_config->config->type = iana_if_type::EthernetCsmacd();
    ifcs_config.interface.append(ifc_config);
    if (ns.edit_config(provider, DataStore::candidate, ifcs_config)) {
        ns.commit(provider);
    }

    auto ifcs = openconfig_interfaces::Interfaces();
    auto ifc  = make_shared<openconfig_interfaces::Interfaces::Interface>();
    ifc->name = "GigabitEthernet0/0/0/2";
    ifc->config->type = YFilter::read;
    ifcs.interface.append(ifc);
    auto ifc_ent = ns.get(provider, *ifc);
    REQUIRE(ifc_ent != nullptr);
    auto ifc_xml = entity2string(ifc_ent, provider.get_session().get_root_schema());
    std::string expected = R"(<interface>
  <name>GigabitEthernet0/0/0/2</name>
  <config>
    <type>iana-if-type:ethernetCsmacd</type>
  </config>
</interface>
)";
    REQUIRE(ifc_xml == expected);
}

// edit_config, get_config
TEST_CASE("edit_config")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;
    DataStore source = DataStore::candidate;
    openconfig_bgp::Bgp filter = {};
    openconfig_bgp::Bgp bgp = {};
    bgp.global->config->as = 6500;

    auto reply = ns.edit_config(provider, target, bgp);
    REQUIRE(reply);

    auto data = ns.get_config(provider, source, filter);
    REQUIRE(data);

    auto data_ptr = dynamic_cast<openconfig_bgp::Bgp*>(data.get());
    REQUIRE(data_ptr != nullptr);
    REQUIRE(data_ptr->global->config->as == bgp.global->config->as);

    reply = ns.discard_changes(provider);
    REQUIRE(reply);
}

TEST_CASE("edit_multiple_config")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;
    DataStore source = DataStore::candidate;

    // Create 'native' configuration
    ydktest::ydktest_sanity::Native native{};
    native.hostname = "My Host";
    native.version = "0.1.2";

    // Set the Global AS
    openconfig_bgp::Bgp bgp{};
    bgp.global->config->as = 65051;
    bgp.global->config->router_id = "10.20.30.40";

    // Create entity list
    vector<ydk::Entity*> edit_list{};
    edit_list.push_back(&native);
    edit_list.push_back(&bgp);

    auto reply = ns.edit_config(provider, target, edit_list);
    REQUIRE(reply);

    // Build filter
    ydktest::ydktest_sanity::Native native_filter{};
    openconfig_bgp::Bgp bgp_filter{};
    vector<ydk::Entity*> filter_list{};
    filter_list.push_back(&native_filter);
    filter_list.push_back(&bgp_filter);

    // Read current configuration and print it
    auto read_list = ns.get_config(provider, source, filter_list);
    REQUIRE(read_list.size() == 2);
    for (auto item : read_list) {
        string path = item->get_segment_path();
        if (path.find("bgp") != string::npos) {
            auto data_ptr = dynamic_cast<openconfig_bgp::Bgp*>(item.get());
            REQUIRE(data_ptr != nullptr);
            REQUIRE(data_ptr->global->config->as == bgp.global->config->as);
            REQUIRE(data_ptr->global->config->router_id == bgp.global->config->router_id);
        }
        if (path.find("native") != string::npos) {
            auto data_ptr = dynamic_cast<ydktest::ydktest_sanity::Native*>(item.get());
            REQUIRE(data_ptr != nullptr);
            REQUIRE(data_ptr->hostname == native.hostname);
            REQUIRE(data_ptr->version == native.version);
        }
    }

    // Discard config changes
    reply = ns.discard_changes(provider);
    REQUIRE(reply);
}

// get
TEST_CASE("get")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    openconfig_bgp::Bgp filter = {};

    auto reply = ns.get(provider, filter);
    REQUIRE(reply);
}

// kill_session
TEST_CASE("kill_session")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    int session_id = 3;

//    auto reply = ns.kill_session(session, session_id);
    CHECK_THROWS_AS(ns.kill_session(provider, session_id), YError);
}

// lock, unlock
TEST_CASE("lock")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;

    auto reply = ns.lock(provider, target);
    REQUIRE(reply);

    reply = ns.unlock(provider, target);
    REQUIRE(reply);
}

// validate
TEST_CASE("validate")
{
    // session
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore source = DataStore::candidate;

    auto reply = ns.validate(provider, source);
    REQUIRE(reply);
}

// read device configuration (no filter)
TEST_CASE("ietf_get_config_rpc")
{
    path::Repository repo{TEST_HOME};
    ydk::path::NetconfSession session{repo, "127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ietf-netconf:get-config") };
    read_rpc->get_input_node().create_datanode("source/running");

    auto read_result = (*read_rpc)(session);
    REQUIRE(read_result != nullptr);

    // Print config
    vector<shared_ptr<ydk::path::DataNode>> data_nodes = read_result->get_children();
    for (auto dn : data_nodes) {
        print_data_node(dn);
    }
}

// read device configuration and state (no filter)
TEST_CASE("ietf_get_rpc")
{
    NetconfServiceProvider provider{"127.0.0.1", "admin", "admin", 12022};
    ydk::path::RootSchemaNode& schema = provider.get_session().get_root_schema();

    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ietf-netconf:get") };
    try {
        (*read_rpc)(provider.get_session());
    }
    catch (YError& ex) {
        cout << "Exception while executing RPC: " << ex.what() << endl;
    }
}

TEST_CASE("create_list_with_yfilter")
{
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ydk::path::RootSchemaNode& schema = provider.get_session().get_root_schema();
    NetconfService ns{};

    auto runner = ydktest_sanity::Runner{};
    runner.yfilter = YFilter::delete_;
    auto reply = ns.edit_config(provider, DataStore::running, runner);
    REQUIRE(reply);

    runner.yfilter = YFilter::merge;
    vector<Entity*> merge_list{};
    merge_list.push_back(&runner);

    reply = ns.edit_config(provider, DataStore::running, merge_list);
    REQUIRE(reply);

    reply = ns.discard_changes(provider);
    REQUIRE(reply);
}

TEST_CASE("create_get_non_top")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    // Create global configuration
    auto bgp_set = openconfig_bgp::Bgp();
    bgp_set.yfilter = YFilter::replace;
    bgp_set.global->config->as = 65001;
    bgp_set.global->config->router_id = "1.2.3.4";
    auto reply = ns.edit_config(provider, DataStore::candidate, bgp_set);
    REQUIRE(reply);

    // Add neighbor configuration
    auto neighbor = openconfig_bgp::Bgp::Neighbors::Neighbor();
    neighbor.neighbor_address = "1.2.3.4";
    neighbor.config->neighbor_address = "1.2.3.4";
    reply = ns.edit_config(provider, DataStore::candidate, neighbor);
    REQUIRE(reply);

    // Get and validate neighbor configuration
    auto bgp_filter = openconfig_bgp::Bgp();
    auto neighbor_filter = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    neighbor_filter->neighbor_address = "1.2.3.4";
    bgp_filter.neighbors->neighbor.append(neighbor_filter);
    auto neighbor_entity = ns.get_config(provider, DataStore::candidate, *neighbor_filter);
    REQUIRE(neighbor_entity!=nullptr);

    auto neighbor_ptr = dynamic_cast<openconfig_bgp::Bgp::Neighbors::Neighbor*>(neighbor_entity.get());
    REQUIRE(neighbor_ptr!=nullptr);
    REQUIRE(*neighbor_ptr == neighbor);

    reply = ns.discard_changes(provider);
    REQUIRE(reply);
}

TEST_CASE("get_openconfig_interfaces_and_bgp")
{
    NetconfServiceProvider provider{"127.0.0.1", "admin", "admin", 12022};
    ydk::path::RootSchemaNode& schema = provider.get_session().get_root_schema();

    // Add BGP and interface configuration
    auto & bgp = schema.create_datanode("openconfig-bgp:bgp");
    bgp.create_datanode("global/config/as", "65172");
    bgp.create_datanode("global/config/router-id", "1.2.3.4");

    auto & interfaces = schema.create_datanode("openconfig-interfaces:interfaces");
    interfaces.create_datanode("interface[name='GigabitEthernet0/0/0/2']/config/name", "GigabitEthernet0/0/0/2");
    interfaces.create_datanode("interface[name='GigabitEthernet0/0/0/2']/config/type", "iana-if-type:ethernetCsmacd");

    ydk::path::Codec s {};
    auto xml = s.encode(bgp, ydk::EncodingFormat::XML, true);
    xml += s.encode(interfaces, ydk::EncodingFormat::XML, true);

    std::shared_ptr<ydk::path::Rpc> edit_rpc { schema.create_rpc("ietf-netconf:edit-config") };
    edit_rpc->get_input_node().create_datanode("target/running");
    edit_rpc->get_input_node().create_datanode("config", xml);

    auto edit_result = (*edit_rpc)(provider.get_session());
    REQUIRE(edit_result == nullptr);

    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ietf-netconf:get-config") };
    read_rpc->get_input_node().create_datanode("source/running");

    // Build filter and read configuration
    openconfig_interfaces::Interfaces interfaces_filter{};
    openconfig_bgp::Bgp bgp_filter{};

    std::string filter_string = get_xml_subtree_filter_payload(bgp_filter, provider);
    filter_string += "\n" + get_xml_subtree_filter_payload(interfaces_filter, provider);

    read_rpc->get_input_node().create_datanode("filter", filter_string);

    auto read_result = (*read_rpc)(provider.get_session());
    REQUIRE(read_result != nullptr);

    // Print configuration
    auto data_nodes = read_result->get_children();
    for (auto dn : data_nodes) {
        print_data_node(dn);
    }
}

namespace ydk {
  namespace path {
     std::shared_ptr<DataNode> handle_rpc_output(const std::string & reply, RootSchemaNode & root_schema, const string& rpc_path);
  }
}

TEST_CASE( "decode_transaction_rpc" )
{
    path::Repository repo{TEST_HOME};
    ydk::path::NetconfSession session{repo, "127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    auto rpc = schema.create_rpc("ydktest-sanity-action:transaction");
    rpc->get_input_node().create_datanode("run-request/out-format", "xml");

    auto rpc_reply = R"(<?xml version="1.0" encoding="UTF-8"?>
<rpc-reply xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="5">
  <run-result xmlns="http://cisco.com/ns/yang/ydktest-action">
    <out-result><![CDATA[
      <device>
        <name>192.168.123.11</name>
        <data>router bgp 7922
 vrf green
  rd 65010:0
 exit
exit
</data>
      </device>]]>
    </out-result>
  </run-result>
</rpc-reply>
)";
    auto reply_dn = ydk::path::handle_rpc_output(rpc_reply, schema, rpc->get_input_node().get_path());
    REQUIRE(reply_dn != nullptr);
    print_data_node(reply_dn->get_children()[0]);
}
