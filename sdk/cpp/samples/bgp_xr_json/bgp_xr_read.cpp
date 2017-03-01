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
#include <fstream>

#include <ydk/codec_provider.hpp>
#include <ydk/codec_service.hpp>
#include <ydk/crud_service.hpp>
#include <ydk/netconf_provider.hpp>
#include <ydk/types.hpp>

#include <ydk_cisco_ios_xr/Cisco_IOS_XR_ipv4_bgp_cfg.hpp>
#include <spdlog/spdlog.h>

#include "../args_parser.h"

using namespace ydk;
using namespace ydk::Cisco_IOS_XR_ipv4_bgp_cfg;
using namespace std;

#define CONFIG_FILE "../config_read.json"

void write_json_config(string json)
{
    ofstream config_file;
    config_file.open (CONFIG_FILE);
    config_file << json;
    config_file.close();
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
        logger->set_level(spdlog::level::debug);
    }

	try
    {
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
        cout << "Read operation success" << endl;

        CodecService codec_service{};
        CodecServiceProvider codec_provider{EncodingFormat::JSON};

        auto json = codec_service.encode(codec_provider, *bgp_read);

        write_json_config(json);
        cout << "Writing JSON config" << endl;
    }
    catch(YCPPError & e)
    {
        cerr << "Error details: " << e << endl;
    }
}
