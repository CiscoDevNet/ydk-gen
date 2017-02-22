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
#include <unistd.h>
#include <sys/stat.h>

#include <ydk/codec_provider.hpp>
#include <ydk/codec_service.hpp>
#include <ydk/crud_service.hpp>
#include <ydk/netconf_provider.hpp>
#include <ydk/path_api.hpp>
#include <ydk/types.hpp>

#include <ydk_cisco_ios_xr/Cisco_IOS_XR_ipv4_bgp_cfg.hpp>
#include <ydk_cisco_ios_xr/Cisco_IOS_XR_ipv4_bgp_datatypes.hpp>
#include <spdlog/spdlog.h>

#include "../args_parser.h"

using namespace ydk;
using namespace ydk::Cisco_IOS_XR_ipv4_bgp_cfg;
using namespace std;

#define CONFIG_FILE "/../config.json"
#define MODELS_PATH "/../xr_models"

string read_json_config(string cwd)
{
    string json{};
    struct stat buffer = {0};
    string file_path = cwd + CONFIG_FILE;
    if(stat (file_path.c_str(), &buffer) == 0)
    {
        ifstream config_file (file_path);
        if (config_file.is_open())
        {
            string line{};
            while (getline(config_file, line))
            {
                json += line;
            }
            config_file.close();
        }
    }
    return json;
}

int main(int argc, char* argv[])
{
    string host, username, password;
    int port;
    vector<string> args = parse_args(argc, argv);
    if(args.empty()) return 1;

    username = args[0]; password = args[1]; host = args[2]; port = stoi(args[3]);

    bool verbose = (args[4]=="--verbose");
    if(verbose)
    {
        auto logger = spdlog::stdout_color_mt("ydk");
        logger->set_level(spdlog::level::debug);
    }

    char cwd[PATH_MAX];
    string cwd_str = getcwd(cwd, sizeof(cwd));
    string json = read_json_config(cwd_str);

    try
    {
        path::Repository repo{cwd_str + MODELS_PATH};
        NetconfServiceProvider provider{repo, host, username, password, port};
        CrudService crud{};

        CodecService codec_service{};
        CodecServiceProvider codec_provider{repo, EncodingFormat::JSON};

        auto bgp = codec_service.decode(codec_provider, json, make_unique<Bgp>());

        bool reply = crud.create(provider, *bgp);
        if(reply) cout << "Create operation success" << endl << endl; else cout << "Operation failed" << endl << endl;
    }
    catch(YCPPError & e)
    {
        cerr << "Error details: " << e << endl;
    }
}

#undef CONFIG_FILE
#undef MODELS_PATH
