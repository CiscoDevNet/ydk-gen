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

#include "logger.hpp"
#include "opendaylight_parser.hpp"
#include "opendaylight_provider.hpp"
#include "path_api.hpp"
#include "restconf_client.hpp"
#include "restconf_provider.hpp"


using namespace std;

namespace ydk
{
#define ODLNode opendaylight::network_topology::NetworkTopology::Topology::Node

static string get_encoding_string(EncodingFormat encoding);

OpenDaylightServiceProvider::OpenDaylightServiceProvider(path::Repository & repo, const string & address,
           const string & username, const string & password, int port, EncodingFormat encoding, Protocol protocol)
    : m_repo{repo}, address(address), username(username), password(password),
      port(port), encoding(encoding)
{
    if(protocol != Protocol::restconf)
    {
        YLOG_ERROR("Netconf protocol currently not supported");
        throw(YServiceProviderError{"Netconf protocol currently not supported"});
    }

    RestconfClient client{address, username, password, port, get_encoding_string(encoding)};

    OpenDaylightCapabilitiesJsonParser odl_parser{};
    string odl_caps_xml = client.get_capabilities("/operational/network-topology:network-topology/", get_encoding_string(EncodingFormat::JSON));
    odl_nodes = odl_parser.parse(odl_caps_xml);
    for(const auto & n:odl_nodes)
    {
        node_ids.push_back(n.first);
    }
}

OpenDaylightServiceProvider::~OpenDaylightServiceProvider()
{
}

unique_ptr<ydk::ServiceProvider> OpenDaylightServiceProvider::create_provider_for_node(const string & node_id)
{
    if(odl_nodes.find(node_id) == odl_nodes.end())
    {
        YLOG_ERROR("Invalid node id {}", node_id);
        throw(YServiceProviderError{"Invalid node id " + node_id});
    }
    YLOG_DEBUG("Creating and returning provider for {}", node_id);
    vector<string> node_capabilities{};
    const auto & node = odl_nodes[node_id];

    for(const auto & c : node->available_capabilities->available_capability)
    {
        const string & c1 = c->capability.get();
        if(c1.size() > 0)
        {
            node_capabilities.push_back(c1);
        }
    }
    OpenDaylightCapabilitiesParser capabilities_parser{};
    auto capabilities = capabilities_parser.parse(node_capabilities);
    return make_unique<RestconfServiceProvider>(
                            make_unique<RestconfClient>(address, username, password, port, get_encoding_string(encoding)),
                            m_repo.create_root_schema(capabilities),
                            "PUT",
                            string("/config/network-topology:network-topology/topology/topology-netconf/node/")+node_id+string("/yang-ext:mount"),
                            string("/operational/network-topology:network-topology/topology/topology-netconf/node/")+node_id+string("/yang-ext:mount"),
                            encoding
                            );
}

ydk::ServiceProvider & OpenDaylightServiceProvider::get_node_provider(const string & node_id)
{
    if(providers.find(node_id) != providers.end())
    {
        YLOG_DEBUG("Returning existing provider for {}", node_id);
        return *(providers[node_id]);
    }

    providers[node_id] = create_provider_for_node(node_id);
    return *(providers[node_id]);
}

const std::vector<std::string> & OpenDaylightServiceProvider::get_node_ids()
{
    return node_ids;
}

static string get_encoding_string(EncodingFormat encoding)
{
    return (encoding == EncodingFormat::XML)?
        ("application/yang.data+xml"):
        ("application/yang.data+json");
}

#undef ODLNode
}
