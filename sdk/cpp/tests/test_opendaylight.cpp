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

#include <ydk/crud_service.hpp>
#include <ydk/opendaylight_provider.hpp>
#include <ydk/path_api.hpp>
#include <ydk_ydktest/openconfig_bgp.hpp>
#include "config.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace std;

using namespace ydk::openconfig_bgp;

TEST_CASE("ReadODL")
{
	ydk::path::Repository repo{TEST_HOME};
	OpenDaylightServiceProvider provider{repo, "localhost", "admin", "admin", 12306, EncodingFormat::JSON};
	CrudService crud {};

	auto bgp_filter = make_unique<Bgp>();

	auto node_ids = provider.get_node_ids();

	auto bgp_read = crud.read_config(provider.get_node_provider("xr"), *bgp_filter);

	REQUIRE(bgp_read!=nullptr);

	if(bgp_read == nullptr)
	{
		INFO("==================================================");
		INFO("No entries found");
		INFO("==================================================");

	}

	Bgp * bgp_read_ptr = dynamic_cast<Bgp*>(bgp_read.get());
	REQUIRE(bgp_read_ptr!=nullptr);
	INFO("==================================================");
	INFO("BGP configuration: ");
	INFO("AS: " << bgp_read_ptr->global->config->as);
	INFO("Router ID: " << bgp_read_ptr->global->config->router_id);

}

TEST_CASE("CreateODL")
{
	ydk::path::Repository repo{TEST_HOME};
	OpenDaylightServiceProvider provider{repo, "localhost", "admin", "admin", 12306, EncodingFormat::JSON};
	CrudService crud {};

	auto bgp = make_unique<Bgp>();
	bgp->global->config->as = 65172;
	bgp->global->config->router_id = "1.2.3.4";

	//Commented because of XR 611 issue with OC identity
	//	auto afi_safi = make_unique<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
	//	afi_safi->afi_safi_name = openconfig_bgp_types::L3Vpn_Ipv4_Unicast();
	//	afi_safi->config->afi_safi_name = openconfig_bgp_types::L3Vpn_Ipv4_Unicast();
	//	afi_safi->config->enabled = false;
	//	afi_safi->parent = bgp->global->afi_safis.get();
	//	bgp->global->afi_safis->afi_safi.push_back(move(afi_safi));

	auto neighbor = make_unique<openconfig_bgp::Bgp::Neighbors::Neighbor>();
	neighbor->neighbor_address = "6.7.8.9";
	neighbor->config->neighbor_address = "6.7.8.9";
	neighbor->config->peer_as = 65001;
	neighbor->config->local_as = 65001;
	neighbor->config->peer_group = "IBGP";
	//neighbor->config->peer_type = "INTERNAL";
	//neighbor->config->remove_private_as = openconfig_bgp_types::Private_As_Remove_All();
	neighbor->parent = bgp->neighbors.get();
	bgp->neighbors->neighbor.push_back(move(neighbor));

	auto peer_group = make_unique<openconfig_bgp::Bgp::PeerGroups::PeerGroup>();
	peer_group->peer_group_name = "IBGP";
	peer_group->config->peer_group_name = "IBGP";
	//peer_group->config->auth_password = "password";
	peer_group->config->description = "test description";
	peer_group->config->peer_as = 65001;
	peer_group->config->local_as = 65001;
	//peer_group->config->peer_type = "INTERNAL";
	//peer_group->config->remove_private_as = openconfig_bgp_types::Private_As_Remove_All();
	peer_group->parent = bgp->peer_groups.get();
	bgp->peer_groups->peer_group.push_back(move(peer_group));

    auto & prov = provider.get_node_provider("xr");
	bool result = crud.create(prov, *bgp);
	REQUIRE(result);
}
//AUTO_TEST_SUITE_END()

