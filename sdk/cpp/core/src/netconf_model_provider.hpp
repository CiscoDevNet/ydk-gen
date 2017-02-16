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

#ifndef _NETCONF_MODEL_PROVIDER_H_
#define _NETCONF_MODEL_PROVIDER_H_

#include <memory>
#include <string>

#include "path_api.hpp"

namespace ydk {

class NetconfClient;

class NetconfModelProvider : public path::ModelProvider {
public:
        NetconfModelProvider(NetconfClient & client);

        ~NetconfModelProvider();

        std::string get_model(const std::string& name, const std::string& version, Format format);
        std::string get_hostname_port();

private:
        NetconfClient & client;
};
}

#endif /*_NETCONF_MODEL_PROVIDER_H_*/
