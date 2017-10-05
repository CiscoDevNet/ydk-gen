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

#include <ydk_cisco_ios_xr/Cisco_IOS_XR_clns_isis_cfg.hpp>
#include <ydk_cisco_ios_xr/Cisco_IOS_XR_clns_isis_datatypes.hpp>
#include <spdlog/spdlog.h>

#include "args_parser.h"

using namespace ydk;
using namespace cisco_ios_xr::Cisco_IOS_XR_clns_isis_cfg;
using namespace cisco_ios_xr::Cisco_IOS_XR_clns_isis_datatypes;
using namespace std;

void config_isis(Isis* isis)
{
     //Add config data to isis object.
    // global configuration
    auto instance = make_unique<Isis::Instances::Instance>();
    instance->instance_name = "DEFAULT";
    instance->running = Empty();
    instance->is_type = IsisConfigurableLevels::level2;
    auto net = make_unique<Isis::Instances::Instance::Nets::Net>();
    net->net_name = "49.0000.1720.1625.5001.00";
    net->parent = instance->nets.get();
    instance->nets->net.push_back(move(net));

    // global address family
    auto af = make_unique<Isis::Instances::Instance::Afs::Af>();
    af->af_name = IsisAddressFamily::ipv4;
    af->saf_name = IsisSubAddressFamily::unicast;
    af->af_data = make_unique<Isis::Instances::Instance::Afs::Af::AfData>(); // instantiate the presence node
    af->af_data->parent = af.get(); // set the parent
    auto metric_style = make_unique<Isis::Instances::Instance::Afs::Af::AfData::MetricStyles::MetricStyle>();
    metric_style->style = IsisMetricStyle::new_metric_style;
    metric_style->level = IsisInternalLevel::not_set;
    metric_style->transition_state = IsisMetricStyleTransition::disabled;
    metric_style->parent = af->af_data->metric_styles.get();
    af->af_data->metric_styles->metric_style.push_back(move(metric_style));
    af->parent = instance->afs.get();
    instance->afs->af.push_back(move(af));

    // loopback interface
    auto interface = make_unique<Isis::Instances::Instance::Interfaces::Interface>();
    interface->interface_name = "Loopback0";
    interface->running = Empty();
    interface->state = IsisInterfaceState::passive;
    // interface address family
    auto interface_af = make_unique<Isis::Instances::Instance::Interfaces::Interface::InterfaceAfs::InterfaceAf>();
    interface_af->af_name = IsisAddressFamily::ipv4;
    interface_af->saf_name = IsisSubAddressFamily::unicast;
    interface_af->interface_af_data->running = Empty();
    interface_af->parent = interface->interface_afs.get();
    interface->interface_afs->interface_af.push_back(move(interface_af));
    interface->parent = instance->interfaces.get();
    instance->interfaces->interface.push_back(move(interface));

    // gi0/0/0/0 interface
    interface = make_unique<Isis::Instances::Instance::Interfaces::Interface>();
    interface->interface_name = "GigabitEthernet0/0/0/0";
    interface->running = Empty();
    interface->point_to_point = Empty();
    // interface address familiy
    interface_af = make_unique<Isis::Instances::Instance::Interfaces::Interface::InterfaceAfs::InterfaceAf>();
    interface_af->af_name = IsisAddressFamily::ipv4;
    interface_af->saf_name = IsisSubAddressFamily::unicast;
    interface_af->interface_af_data->running = Empty();
    interface_af->parent = interface->interface_afs.get();
    interface->interface_afs->interface_af.push_back(move(interface_af));
    interface->parent = instance->interfaces.get();
    instance->interfaces->interface.push_back(move(interface));

    instance->parent = isis->instances.get();
    isis->instances->instance.push_back(move(instance));
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

    auto isis = make_unique<Isis>();
    config_isis(isis.get());
    bool reply = crud.create(provider, *isis);

    if(reply) cout << "Create yfilter success" << endl << endl; else cout << "Operation failed" << endl << endl;

}
