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
#include <iostream>

#include "ydk/types.hpp"
#include "ydk/netconf_provider.hpp"
#include "ydk/crud_service.hpp"
#include <spdlog/spdlog.h>

#include "ydk_cisco_ios_xr/Cisco_IOS_XR_ipv4_bgp_cfg.hpp"
#include "ydk_cisco_ios_xr/Cisco_IOS_XR_ipv4_bgp_datatypes.hpp"

#include "args_parser.h"

using namespace ydk;
using namespace cisco_ios_xr::Cisco_IOS_XR_ipv4_bgp_cfg;
using namespace cisco_ios_xr::Cisco_IOS_XR_ipv4_bgp_datatypes;

using namespace std;

void config_bgp(Bgp* bgp)
{
    // Add config data to bgp object.
    // global configuration
    auto instance = make_shared<Bgp::Instance>();
    instance->instance_name = "test";
    auto instance_as = make_shared<Bgp::Instance::InstanceAs>();
    instance_as->as = 65001;
    auto four_byte_as = make_shared<Bgp::Instance::InstanceAs::FourByteAs>();
    four_byte_as->as = 65001;
    four_byte_as->bgp_running = Empty();

    // global address family
    auto global_af = make_shared<Bgp::Instance::InstanceAs::FourByteAs::DefaultVrf::Global::GlobalAfs::GlobalAf>();
    global_af->af_name = BgpAddressFamily::ipv4_unicast;
    global_af->enable = Empty();
    global_af->parent = four_byte_as->default_vrf->global->global_afs.get();
    four_byte_as->default_vrf->global->global_afs->global_af.append(global_af);

    // configure IBGP neighbor group
    auto neighbor_group = make_shared<Bgp::Instance::InstanceAs::FourByteAs::DefaultVrf::BgpEntity::NeighborGroups::NeighborGroup>();
    neighbor_group->neighbor_group_name = "IBGP";
    neighbor_group->create = Empty();
    // remote AS
    neighbor_group->remote_as->as_xx = 0;
    neighbor_group->remote_as->as_yy = 65001;
    neighbor_group->update_source_interface = "Loopback0";
    // ipv4 unicast
    auto neighbor_group_af = make_shared<Bgp::Instance::InstanceAs::FourByteAs::DefaultVrf::BgpEntity::NeighborGroups::NeighborGroup::NeighborGroupAfs::NeighborGroupAf>();
    neighbor_group_af->af_name = BgpAddressFamily::ipv4_unicast;
    neighbor_group_af->activate = Empty();
    neighbor_group->neighbor_group_afs->neighbor_group_af.append(neighbor_group_af);
    four_byte_as->default_vrf->bgp_entity->neighbor_groups->neighbor_group.append(neighbor_group);

    // configure IBGP neighbor
    auto neighbor = make_shared<Bgp::Instance::InstanceAs::FourByteAs::DefaultVrf::BgpEntity::Neighbors::Neighbor>();
    neighbor->neighbor_address = "172.16.255.2";
    neighbor->neighbor_group_add_member = "IBGP";
    four_byte_as->default_vrf->bgp_entity->neighbors->neighbor.append(neighbor);

    instance_as->four_byte_as.append(four_byte_as);
    instance->instance_as.append(instance_as);
    bgp->instance.append(instance);
}

int main(int argc, char* argv[])
{
    vector<string> args = parse_args(argc, argv);
    if(args.empty()) return 1;
    string host, username, password;
    int port;

    username = args[0]; password = args[1]; host = args[2]; port = stoi(args[3]);

    bool verbose=(args[4]=="--verbose");
    if(verbose)
    {
        auto logger = spdlog::stdout_color_mt("ydk");
        logger->set_level(spdlog::level::info);
    }

    NetconfServiceProvider provider{host, username, password, port};
    CrudService crud{};

    auto bgp = make_unique<Bgp>();
    config_bgp(bgp.get());
    bool reply = crud.create(provider, *bgp);

    if (reply)
        cout << "Update yfilter success" << endl << endl;
    else
        cout << "Operation failed" << endl << endl;
}
