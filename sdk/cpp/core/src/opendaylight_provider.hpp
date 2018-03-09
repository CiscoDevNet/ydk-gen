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

#ifndef _OPENDAYLIGHT_PROVIDER_H_
#define _OPENDAYLIGHT_PROVIDER_H_

#include <map>
#include <string>
#include <vector>

#include "network_topology.hpp"
#include "path_api.hpp"

namespace ydk
{
class ServiceProvider;
}
namespace ydk
{

class OpenDaylightServiceProvider
{
    public:
        OpenDaylightServiceProvider(path::Repository & repo,
                                   const std::string & address,
                                   const std::string & username,
                                   const std::string & password,
                                   int port = 8181,
                                   EncodingFormat encoding = EncodingFormat::JSON,
                                   Protocol protocol = Protocol::restconf);

        ~OpenDaylightServiceProvider();

        ydk::ServiceProvider & get_node_provider(const std::string & node_id);
        const std::vector<std::string> & get_node_ids();

    private:
        std::unique_ptr<ydk::ServiceProvider> create_provider_for_node(const std::string & node_id);

    private:
        std::unique_ptr<path::Repository> m_repo_ptr;
        path::Repository & m_repo;

        std::string address;
        std::string username;
        std::string password;
        int port;
        EncodingFormat encoding;

        std::map<std::string, std::unique_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node>> odl_nodes;
        std::map<std::string, std::unique_ptr<ydk::ServiceProvider>> providers;
        std::vector<std::string> node_ids;
};

}

#endif /* _OPENDAYLIGHT_PROVIDER_H_ */
