/* ----------------------------------------------------------------
 YDK - YANG Development Kit
 Copyright 2016 Cisco Systems, All rights reserved.

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

#include <string.h>
#include <iostream>

#include "netconf_provider.hpp"
#include "crud_service.hpp"
#include "catch.hpp"
#include "entity_data_node_walker.hpp"

#include <ydk_ydktest/openconfig_bgp.hpp>
#include <ydk_ydktest/openconfig_bgp_types.hpp>
#include <ydk_ydktest/openconfig_platform.hpp>
#include <ydk_ydktest/openconfig_platform_types.hpp>
#include "ydk_ydktest/ydktest_sanity.hpp"

#include "config.hpp"
#include "test_utils.hpp"

using namespace ydk;
using namespace ydktest;
using namespace std;

void config_bgp(openconfig_bgp::Bgp* bgp)
{
    // Set the Global AS
    bgp->global->config->as = 65001;
    bgp->global->config->router_id = "1.2.3.4";

    auto afi_safi = make_shared<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
    afi_safi->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
    afi_safi->config->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
    afi_safi->config->enabled = true;
    bgp->global->afi_safis->afi_safi.append(afi_safi);

    auto neighbor = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    neighbor->neighbor_address = "6.7.8.9";
    neighbor->config->neighbor_address = "6.7.8.9";
    neighbor->config->enabled = true;
    neighbor->config->peer_as = 65001;
    neighbor->config->local_as = 65001;
    neighbor->config->peer_group = "IBGP";
    neighbor->config->peer_type = "INTERNAL";
    bgp->neighbors->neighbor.append(neighbor);

    auto peer_group = make_shared<openconfig_bgp::Bgp::PeerGroups::PeerGroup>();
    peer_group->peer_group_name = "IBGP";
    peer_group->config->peer_group_name = "IBGP";
    peer_group->config->auth_password = "password";
    peer_group->config->description = "test description";
    peer_group->config->peer_as = 65001;
    peer_group->config->local_as = 65001;
    peer_group->config->peer_type = "INTERNAL";
    bgp->peer_groups->peer_group.append(peer_group);
}

TEST_CASE("bgp_create_delete")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};
    auto bgp = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp);
    REQUIRE(reply);

    config_bgp(bgp.get());
    reply = crud.create(provider, *bgp);
    REQUIRE(reply);
}

TEST_CASE("bgp_read_delete")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};
    auto bgp_set = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp_set);
    REQUIRE(reply);

    config_bgp(bgp_set.get());
    reply = crud.create(provider, *bgp_set);

    REQUIRE(reply);

    auto bgp_filter = make_unique<openconfig_bgp::Bgp>();
    auto bgp_read = crud.read_config(provider, *bgp_filter);
    REQUIRE(bgp_read!=nullptr);
    openconfig_bgp::Bgp * bgp_read_ptr = dynamic_cast<openconfig_bgp::Bgp*>(bgp_read.get());
    REQUIRE(bgp_read_ptr!=nullptr);

    REQUIRE(*(bgp_read_ptr) == *(bgp_set));

    CHECK(bgp_set->global->config->as == bgp_read_ptr->global->config->as);
    auto set_neighbor  = dynamic_cast<openconfig_bgp::Bgp::Neighbors::Neighbor*> (bgp_set->neighbors->neighbor[0].get());
    auto read_neighbor = dynamic_cast<openconfig_bgp::Bgp::Neighbors::Neighbor*> (bgp_read_ptr->neighbors->neighbor[0].get());
    CHECK(set_neighbor->neighbor_address == read_neighbor->neighbor_address);
    CHECK(set_neighbor->config->local_as == read_neighbor->config->local_as);

    auto set_afi_safi  = dynamic_cast<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi*> (bgp_set->global->afi_safis->afi_safi[0].get());
    auto read_afi_safi = dynamic_cast<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi*> (bgp_read_ptr->global->afi_safis->afi_safi[0].get());
    CHECK(set_afi_safi->afi_safi_name == read_afi_safi->afi_safi_name);
    CHECK(set_afi_safi->config->afi_safi_name == read_afi_safi->config->afi_safi_name);

    reply = reply && (set_afi_safi->config->enabled  == read_afi_safi->config->enabled);
    REQUIRE(reply);

    cout<<*bgp_set<<endl<<endl;
    cout<<*bgp_read_ptr<<endl;
}

TEST_CASE("bgp_update_delete")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};
    auto bgp = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp);
    REQUIRE(reply);

    config_bgp(bgp.get());
    reply = crud.create(provider, *bgp);
    REQUIRE(reply);

    auto bgp_update = make_unique<openconfig_bgp::Bgp>();
    bgp_update->global->config->as = 65210;
    reply = crud.update(provider, *bgp_update);
    REQUIRE(reply);
}

TEST_CASE("bgp_set_leaf")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};
    auto bgp = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp);
    REQUIRE(reply);

    bgp->global->config->as = 65210;
    reply = crud.update(provider, *bgp);
    REQUIRE(reply);
}

TEST_CASE("bgp_read_create")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};
    auto bgp_set = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp_set);
    REQUIRE(reply);

    bgp_set->global->config->as = 65001;
    bgp_set->global->config->router_id = "1.2.3.4";
    reply = crud.create(provider, *bgp_set);

    REQUIRE(reply);

    auto bgp_filter = make_unique<openconfig_bgp::Bgp>();
    auto bgp_read = crud.read_config(provider, *bgp_filter);
    REQUIRE(bgp_read!=nullptr);
    openconfig_bgp::Bgp * bgp_read_ptr = dynamic_cast<openconfig_bgp::Bgp*>(bgp_read.get());
    REQUIRE(bgp_read_ptr!=nullptr);

    REQUIRE(*(bgp_read_ptr) == *(bgp_set));

    bgp_read_ptr->global->config->as = 65210;
    bgp_read_ptr->global->config->router_id = "6.7.8.9";
    reply = crud.update(provider, *bgp_read_ptr);
    REQUIRE(reply);
}

TEST_CASE("read_leaves")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    auto bgp_delete = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp_delete);
    REQUIRE(reply);

    auto bgp_create = make_unique<openconfig_bgp::Bgp>();
    bgp_create->global->config->as = 65001;
    bgp_create->global->config->router_id = "1.1.1.1";
    reply = crud.create(provider, *bgp_create);
    REQUIRE(reply);

    auto bgp_filter = make_unique<openconfig_bgp::Bgp>();
    bgp_filter->global->config->as = YFilter::read;
    bgp_filter->global->config->router_id = YFilter::read;

    auto bgp_read = crud.read(provider, *bgp_filter);
    REQUIRE(bgp_read!=nullptr);
    REQUIRE(*(bgp_read) == *(bgp_create));
}

TEST_CASE("read_leaf")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    auto bgp_delete = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp_delete);
    REQUIRE(reply);

    auto bgp_create = make_unique<openconfig_bgp::Bgp>();
    bgp_create->global->config->as = 65001;
    bgp_create->global->config->router_id = "1.1.1.1";
    reply = crud.create(provider, *bgp_create);
    REQUIRE(reply);

    auto bgp_cfg = make_unique<openconfig_bgp::Bgp>();
    bgp_cfg->global->config->as = 65001;

    auto bgp_filter = make_unique<openconfig_bgp::Bgp>();
    bgp_filter->global->config->as = YFilter::read;

    auto bgp_read = crud.read(provider, *bgp_filter);
    REQUIRE(bgp_read!=nullptr);
    REQUIRE(*(bgp_read) == *(bgp_cfg));
}

TEST_CASE("bgp_read_container")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};
    auto bgp_set = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp_set);
    REQUIRE(reply);

    bgp_set->global->config->as = 65001;
    bgp_set->global->config->router_id = "1.2.3.4";
    auto d = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    d->neighbor_address = "1.2.3.4";
    d->config->neighbor_address = "1.2.3.4";
    bgp_set->neighbors->neighbor.append(d);

    auto q = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    q->neighbor_address = "1.2.3.5";
    q->config->neighbor_address = "1.2.3.5";
    bgp_set->neighbors->neighbor.append(q);
    reply = crud.create(provider, *bgp_set);
    REQUIRE(reply);

    auto bgp_filter = make_unique<openconfig_bgp::Bgp>();
    bgp_filter->global->yfilter = YFilter::read;
    auto bgp_read = crud.read_config(provider, *(bgp_filter));
    REQUIRE(bgp_read!=nullptr);
    openconfig_bgp::Bgp * bgp_read_ptr = dynamic_cast<openconfig_bgp::Bgp*>(bgp_read.get());
    REQUIRE(bgp_read_ptr->neighbors->neighbor.len() == 0);
    REQUIRE(bgp_read_ptr->global->config->as.value == "65001");

    bgp_filter = make_unique<openconfig_bgp::Bgp>();
    bgp_filter->neighbors->yfilter = YFilter::read;
    bgp_read = crud.read_config(provider, *(bgp_filter));
    REQUIRE(bgp_read!=nullptr);
    bgp_read_ptr = dynamic_cast<openconfig_bgp::Bgp*>(bgp_read.get());
    REQUIRE(bgp_read_ptr->neighbors->neighbor.len() == 2);
    REQUIRE(!bgp_read_ptr->global->has_data());

    bgp_filter->neighbors->yfilter = YFilter::not_set;
    auto z = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    z->neighbor_address = "1.2.3.4";
    bgp_filter->neighbors->neighbor.append(z);
    bgp_read = crud.read_config(provider, *(bgp_filter));
    REQUIRE(bgp_read!=nullptr);
    bgp_read_ptr = dynamic_cast<openconfig_bgp::Bgp*>(bgp_read.get());
    REQUIRE(bgp_read_ptr->neighbors->neighbor.len() == 1);
    REQUIRE(*bgp_read_ptr->neighbors->neighbor["1.2.3.4"] == *d);

    bgp_set = make_unique<openconfig_bgp::Bgp>();
    reply = crud.delete_(provider, *bgp_set);
    REQUIRE(reply);
}

TEST_CASE("bgp_read_non_top")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};
    auto bgp_set = make_unique<openconfig_bgp::Bgp>();
    bool reply = crud.delete_(provider, *bgp_set);
    REQUIRE(reply);

    bgp_set->global->config->as = 65001;
    bgp_set->global->config->router_id = "1.2.3.4";
    auto d = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    d->neighbor_address = "1.2.3.4";
    d->config->neighbor_address = "1.2.3.4";
    bgp_set->neighbors->neighbor.append(d);

    auto q = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    q->neighbor_address = "1.2.3.5";
    q->config->neighbor_address = "1.2.3.5";
    bgp_set->neighbors->neighbor.append(q);
    reply = crud.create(provider, *bgp_set);
    REQUIRE(reply);

    auto bgp_filter = make_unique<openconfig_bgp::Bgp>();
    auto bgp_read = crud.read_config(provider, *(bgp_filter));
    REQUIRE(bgp_read!=nullptr);
    openconfig_bgp::Bgp * bgp_read_ptr = dynamic_cast<openconfig_bgp::Bgp*>(bgp_read.get());
    REQUIRE(bgp_read_ptr!=nullptr);
    REQUIRE(*(bgp_read_ptr) == *(bgp_set));
}

TEST_CASE("oc_platform")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};
    openconfig_platform::Components comps {};
    bool reply = crud.delete_(provider, comps);
    REQUIRE(reply);

    auto comp =  make_shared<openconfig_platform::Components::Component>();
    comp->name = "test";
    comp->config->name = "test";
    comp->transceiver->config->enabled = true;
    comps.component.append(comp);
    reply = crud.update(provider, comps);
    REQUIRE(reply);

    openconfig_platform::Components fil{};
    auto data = crud.read(provider, fil);
    REQUIRE(data != nullptr);
    openconfig_platform::Components * comp_r = dynamic_cast<openconfig_platform::Components*>(data.get());
    REQUIRE(comp_r != nullptr);

    auto read_comp = dynamic_cast<openconfig_platform::Components::Component*> (comp_r->component[0].get());
    REQUIRE(read_comp->name == comp->name);
    REQUIRE(read_comp->config->name == comp->config->name);

    reply = crud.delete_(provider, fil);
    REQUIRE(reply);

}

TEST_CASE( "crud_entity_list_bgp" )
{
    ydk::NetconfServiceProvider provider{"127.0.0.1", "admin", "admin", 12022};
    ydk::CrudService crud{};

    // Create 'native' configuration
    ydktest::ydktest_sanity::Native native{};
    native.hostname = "My Host";
    native.version = "0.1.0";

    // Set the Global AS
    openconfig_bgp::Bgp bgp{};
    bgp.global->config->as = 65001;
    bgp.global->config->router_id = "1.2.3.4";

    // Create entity list
    vector<ydk::Entity*> create_list{};
    create_list.push_back(&native);
    create_list.push_back(&bgp);

    // Execute CRUD operations
    bool result = crud.create(provider, create_list);
    REQUIRE(result == true);

    // Create filter list
    ydktest::ydktest_sanity::Native native_filter{};
    openconfig_bgp::Bgp bgp_filter{};
    vector<ydk::Entity*> filter_list{};
    filter_list.push_back(&native_filter);
    filter_list.push_back(&bgp_filter);

    // Read multiple entities
    auto read_list = crud.read(provider, filter_list);
    REQUIRE(read_list.size() == 2);

//    for (auto item : read_list) {
//        print_entity(item, provider.get_session().get_root_schema());
//    }

    // Delete configuration
    result = crud.delete_(provider, filter_list);
    REQUIRE(result == true);
}

TEST_CASE( "crud_delete_container" )
{
	ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    // Create presence container
    auto runner = ydktest_sanity::Runner();
    runner.runner_2 = make_shared<ydktest_sanity::Runner::Runner2>();
    runner.runner_2->parent = &runner;
    runner.runner_2->some_leaf = "some-leaf";
    auto res = crud.create(provider, runner);
    REQUIRE(res);

    // Read presence container
    runner = ydktest_sanity::Runner();
    runner.runner_2 = make_shared<ydktest_sanity::Runner::Runner2>();
    runner.runner_2->parent = &runner;
    auto r2_ent = crud.read(provider, *runner.runner_2);
    REQUIRE(r2_ent != nullptr);
    auto r2 = dynamic_cast<ydktest_sanity::Runner::Runner2*>(r2_ent.get());
    REQUIRE(r2->some_leaf.value == "some-leaf");

    runner = ydktest_sanity::Runner();
    runner.runner_2 = make_shared<ydktest_sanity::Runner::Runner2>();
    runner.runner_2->parent = &runner;
    res = crud.delete_(provider, *runner.runner_2);
    REQUIRE(res);

    // Delete top container (cleanup)
    runner = ydktest_sanity::Runner();
    res = crud.delete_(provider, runner);
    REQUIRE(res);
}

TEST_CASE( "crud_read_non_top_container" )
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    // CLEANUP
    auto native = ydktest_sanity::Native();
    bool reply = crud.delete_(provider, native);
    REQUIRE(reply);

    // CREATE
    auto loopback = make_shared<ydktest_sanity::Native::Interface::Loopback>();
    loopback->name = "2222";
    native.interface->loopback.append(loopback);
    auto address = make_shared<ydktest_sanity::Native::Interface::Loopback::Ipv4::Address>();
    address->ip = "2.2.2.2";
    address->netmask = "255.255.255.255";
    loopback->ipv4->address.append(address);
    reply = crud.create(provider, native);
    REQUIRE(reply);

    // READ
    auto rnative = ydktest_sanity::Native();
    loopback = make_shared<ydktest_sanity::Native::Interface::Loopback>();
    loopback->name = "2222";
    rnative.interface->loopback.append(loopback);
    auto read_entity_wrapper = crud.read(provider, *loopback->ipv4);
    REQUIRE(read_entity_wrapper != nullptr);
    auto ipv4 = dynamic_cast<ydktest_sanity::Native::Interface::Loopback::Ipv4*>(read_entity_wrapper.get());
    auto ip_address = dynamic_cast<ydktest_sanity::Native::Interface::Loopback::Ipv4::Address*>(ipv4->address["2.2.2.2"].get());
    REQUIRE(ip_address->netmask.value == "255.255.255.255");

    // CLEANUP
    auto dnative = ydktest_sanity::Native();
    reply = crud.delete_(provider, dnative);
    REQUIRE(reply);
}
