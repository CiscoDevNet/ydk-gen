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

#include "ydk_cisco_ios_xr/Cisco_IOS_XR_clns_isis_cfg.hpp"
#include "ydk_cisco_ios_xr/Cisco_IOS_XR_clns_isis_datatypes.hpp"
#include <spdlog/spdlog.h>

#include "args_parser.h"

using namespace ydk;
using namespace cisco_ios_xr::Cisco_IOS_XR_clns_isis_cfg;
using namespace cisco_ios_xr::Cisco_IOS_XR_clns_isis_datatypes;
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

    auto isis = make_unique<Isis>();
    auto isis_read = crud.read_config(provider, *isis);
    if(isis_read == nullptr)
    {
        cout << "=================================================="<<endl;
        cout << "No entries found"<<endl<<endl;
        cout << "=================================================="<<endl;
        return 0;
    }

    Isis* isis_read_ptr = dynamic_cast<Isis*>(isis_read.get());

    cout << "=================================================="<<endl;
    cout << "ISIS configuration: " << endl<<endl;

    for(size_t i=0; i < isis_read_ptr->instances->instance.size(); i++)
    {
        auto instance = dynamic_cast<Isis::Instances::Instance*>(isis_read_ptr->instances->instance[i].get());
        cout << "Instance: " << instance->instance_name << endl;
        cout << "Running: " << ((instance->running.is_set)?"Yes":"No") << endl;
        cout << "IS type: " << instance->is_type << endl;

        for(size_t j=0; j < instance->nets->net.size(); j++)
        {
            cout << "Net name: " << instance->nets->net[j]->net_name << endl;
        }

        for(size_t j=0; j < instance->afs->af.size(); j++)
        {
            cout << "AF name: " << instance->afs->af[j]->af_name << endl;
            cout << "SAF name: " << instance->afs->af[j]->saf_name << endl;
        }

        for(size_t j=0; j < instance->interfaces->interface.size(); j++)
        {
            cout<<endl << "Interface name: " << instance->interfaces->interface[j]->interface_name << endl;
            cout << "Interface running: " << ((instance->interfaces->interface[j]->running.is_set)?"Yes":"No") << endl;
            cout<< "Interface state: " << instance->interfaces->interface[j]->state << endl;
            cout<< "Interface point-to-point: " << ((instance->interfaces->interface[j]->point_to_point.is_set)?"Yes":"No") << endl;

            for(size_t k=0; k < instance->interfaces->interface[j]->interface_afs->interface_af.size(); k++)
            {
                cout<< "Interface AF name: " << instance->interfaces->interface[j]->interface_afs->interface_af[k]->af_name << endl;
                cout<< "Interface SAF name: " << instance->interfaces->interface[j]->interface_afs->interface_af[k]->saf_name << endl;
            }
        }
    }

    cout << "=================================================="<<endl<<endl;
}
