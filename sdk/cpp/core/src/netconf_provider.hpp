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

#ifndef _NETCONF_PROVIDER_H_
#define _NETCONF_PROVIDER_H_

#include <memory>
#include <string>

#include "netconf_client.hpp"
#include "path_api.hpp"
#include "service_provider.hpp"

namespace ydk {

class NetconfServiceProvider : public ServiceProvider {
public:
        NetconfServiceProvider(path::Repository & repo,
                               const std::string& address,
                               const std::string& username,
                               const std::string& password,
                               int port = 830,
                               const std::string& protocol = "ssh",
                               bool on_demand = true,
                               int timeout = -1);
        NetconfServiceProvider(const std::string& address,
                               const std::string& username,
                               const std::string& password,
                               int port = 830,
                               const std::string& protocol = "ssh",
                               bool on_demand = true,
                               bool common_cache = false,
                               int timeout = -1);

        NetconfServiceProvider(path::Repository& repo,
                               const std::string& address,
                               const std::string& username,
                               const std::string& private_key_path,
                               const std::string& public_key_path,
                               int port = 830,
                               bool on_demand = true,
                               int timeout = -1);
        NetconfServiceProvider(const std::string& address,
                               const std::string& username,
                               const std::string& private_key_path,
                               const std::string& public_key_path,
                               int port = 830,
                               bool on_demand = true,
                               bool common_cache = false,
                               int timeout = -1);
        ~NetconfServiceProvider();
        EncodingFormat get_encoding() const;
        const path::Session& get_session() const;
        std::vector<std::string> get_capabilities() const;

private:
        const path::NetconfSession session;
};
}

#endif /*_NETCONF_PROVIDER_H_*/
