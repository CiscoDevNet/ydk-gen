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

#include <ydk/types.hpp>
#include <ydk/netconf_provider.hpp>
#include <ydk/crud_service.hpp>

#include <ydk_cisco_ios_xr/Cisco_IOS_XR_ipv4_bgp_cfg.hpp>
#include <spdlog/spdlog.h>

#include "args_parser.h"

using namespace ydk;
using namespace cisco_ios_xr::Cisco_IOS_XR_ipv4_bgp_cfg;
using namespace std;

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
    auto bgp_read = crud.read_config(provider, *bgp);
    if(bgp_read == nullptr)
    {
        cout << "=================================================="<<endl;
        cout << "No entries found"<<endl<<endl;
        cout << "=================================================="<<endl;
        return 0;
    }
    Bgp* bgp_read_ptr = dynamic_cast<Bgp*>(bgp_read.get());

    cout << "=================================================="<<endl;
    cout << "BGP configuration: " << endl<<endl;
    for(size_t i=0; i < bgp_read_ptr->instance.size(); i++)
    {
        auto instance = dynamic_cast<Bgp::Instance*>(bgp_read_ptr->instance[i].get());
        cout << "Instance: " << instance->instance_name << endl;
        for(size_t j=0; j < instance->instance_as.size(); j++)
        {
            auto instance_as = dynamic_cast<Bgp::Instance::InstanceAs*>(instance->instance_as[j].get());
            cout << "AS: " << instance_as->as << endl;
            for(size_t k=0; k < instance_as->four_byte_as.size(); k++)
            {
                auto four_byte_as = dynamic_cast<Bgp::Instance::InstanceAs::FourByteAs*>(instance_as->four_byte_as[k].get());
                cout << "Four Byte AS: " << four_byte_as->as << endl;
                cout << "BGP running: " << ((four_byte_as->bgp_running.is_set)?"Yes":"No") << endl;
                for(size_t l=0; l < four_byte_as->default_vrf->global->global_afs->global_af.size(); l++)
                {
                    auto global_af = dynamic_cast<Bgp::Instance::InstanceAs::FourByteAs::DefaultVrf::Global::GlobalAfs::GlobalAf*>
                                        (four_byte_as->default_vrf->global->global_afs->global_af[l].get());
                    cout << "Global AF name: " << global_af->af_name << endl;
                    cout << "Global AF enabled: " << (global_af->enable.is_set?"Yes":"No") << endl;
                }
                for(size_t l=0; l < four_byte_as->default_vrf->bgp_entity->neighbor_groups->neighbor_group.size(); l++)
                {
                    auto neighbor_group = dynamic_cast<Bgp::Instance::InstanceAs::FourByteAs::DefaultVrf::BgpEntity::NeighborGroups::NeighborGroup*>
                                            (four_byte_as->default_vrf->bgp_entity->neighbor_groups->neighbor_group[l].get());
                    cout << "Neighbor Group name: " << neighbor_group->neighbor_group_name << endl;
                    cout << "Neighbor Group create: " << (neighbor_group->create.is_set?"Yes":"No") << endl;
                    cout << "Neighbor Group remote AS XX: " << neighbor_group->remote_as->as_xx<< endl;
                    cout << "Neighbor Group remote AS YY: " << neighbor_group->remote_as->as_yy << endl;
                    cout << "Neighbor Group update source interface: " << neighbor_group->update_source_interface << endl;

                    for(size_t m=0; m < neighbor_group->neighbor_group_afs->neighbor_group_af.size(); m++)
                    {
                        auto neighbor_group_af = dynamic_cast<Bgp::Instance::InstanceAs::FourByteAs::DefaultVrf::BgpEntity::NeighborGroups::NeighborGroup::NeighborGroupAfs::NeighborGroupAf*>
                                                                    (neighbor_group->neighbor_group_afs->neighbor_group_af[m].get());
                        cout << "Neighbor Group AF name: " << neighbor_group_af->af_name << endl;
                        cout << "Neighbor Group AF activate: " << (neighbor_group_af->activate.is_set?"Yes":"No") << endl;
                    }
                }
                cout<<endl;
            }
        }
    }

    cout << "=================================================="<<endl<<endl;
}
