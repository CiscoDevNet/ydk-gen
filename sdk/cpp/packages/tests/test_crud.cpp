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

#include "../../src/netconf_provider.hpp"
#include "../../src/crud_service.hpp"
#include "../../src/make_unique.hpp"
#include "../models_gen/openconfig_bgp.h"
#include "../config.hpp"

using namespace ydk;
using namespace std;

#define MODELS_DIR string(TEST_HOME)+string("/openconfig")

void config_bgp(openconfig_bgp::Bgp* bgp)
{
	// Set the Global AS
	bgp->global_->config->as_ = 65001;
	bgp->global_->config->router_id = "1.2.3.4";

	auto afi_safi = make_unique<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
	afi_safi->afi_safi_name = "openconfig-bgp-types:L3VPN_IPV4_UNICAST";
	afi_safi->config->afi_safi_name = "openconfig-bgp-types:L3VPN_IPV4_UNICAST";
	afi_safi->config->enabled = true;
	afi_safi->parent = bgp->global_->afi_safis.get();
	bgp->global_->afi_safis->afi_safi.push_back(move(afi_safi));

	auto neighbor = make_unique<openconfig_bgp::Bgp::Neighbors::Neighbor>();
	neighbor->neighbor_address = "6.7.8.9";
	neighbor->config->neighbor_address = "6.7.8.9";
	neighbor->config->enabled = true;
	neighbor->config->peer_as = 65001;
	neighbor->config->local_as = 65001;
	neighbor->parent = bgp->neighbors.get();
	bgp->neighbors->neighbor.push_back(move(neighbor));

	auto peer_group = make_unique<openconfig_bgp::Bgp::PeerGroups::PeerGroup>();
	peer_group->peer_group_name = "IBGP";
	peer_group->config->peer_group_name = "IBGP";
	peer_group->config->auth_password = "password";
	peer_group->config->description = "test description";
	peer_group->config->peer_as = 65001;
	peer_group->config->local_as = 65001;
	bgp->peer_groups->peer_group.push_back(move(peer_group));
}

TEST_CASE(bgp_create)
{
	ydk::core::Repository repo{MODELS_DIR};
	NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};
	auto bgp = make_unique<openconfig_bgp::Bgp>();
	config_bgp(bgp.get());
	string reply = crud.create(provider, *bgp);
	REQUIRE(strstr(reply.c_str(),"<ok/>") != NULL);
}

TEST_CASE(bgp_read)
{
	ydk::core::Repository repo{MODELS_DIR};
	NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};
	auto bgp_set = make_unique<openconfig_bgp::Bgp>();
	config_bgp(bgp_set.get());
	string reply = crud.create(provider, *bgp_set);

	REQUIRE(strstr(reply.c_str(),"<ok/>") != NULL);

	auto bgp_filter = make_unique<openconfig_bgp::Bgp>();
	auto bgp_read = crud.read(provider, *bgp_filter, true);
	REQUIRE(bgp_read!=nullptr);
	openconfig_bgp::Bgp * bgp_read_ptr = dynamic_cast<openconfig_bgp::Bgp*>(bgp_read.get());
	REQUIRE(bgp_read_ptr!=nullptr);

	CHECK_EQUAL(bgp_set->global_->config->as_, bgp_read_ptr->global_->config->as_);
	CHECK_EQUAL(bgp_set->neighbors->neighbor[0]->neighbor_address, bgp_read_ptr->neighbors->neighbor[0]->neighbor_address);
	CHECK_EQUAL(bgp_set->neighbors->neighbor[0]->config->local_as, bgp_read_ptr->neighbors->neighbor[0]->config->local_as);
	CHECK_EQUAL(bgp_set->global_->afi_safis->afi_safi[0]->afi_safi_name, bgp_read_ptr->global_->afi_safis->afi_safi[0]->afi_safi_name);
	CHECK_EQUAL(bgp_set->global_->afi_safis->afi_safi[0]->config->afi_safi_name, bgp_read_ptr->global_->afi_safis->afi_safi[0]->config->afi_safi_name);

//	cerr<<bgp_set->global_->afi_safis->afi_safi[0]->config->enabled<<","<<bgp_read_ptr->global_->afi_safis->afi_safi[0]->config->enabled<<endl;
//	res &= string(bgp_set->global_->afi_safis->afi_safi[0]->config->enabled)  == string(bgp_read_ptr->global_->afi_safis->afi_safi[0]->config->enabled);
//	REQUIRE(res==1);
}

TEST_CASE(bgp_update)
{
	ydk::core::Repository repo{MODELS_DIR};
	NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};
	auto bgp = make_unique<openconfig_bgp::Bgp>();
	config_bgp(bgp.get());

	string reply = crud.update(provider, *bgp);
	REQUIRE(strstr(reply.c_str(),"<ok/>") != NULL);
}
