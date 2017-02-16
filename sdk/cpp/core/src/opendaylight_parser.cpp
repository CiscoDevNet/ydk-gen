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
#include <sstream>
#include <string>
#include <iostream>

#include "json.hpp"
#include "opendaylight_parser.hpp"
#include "path_api.hpp"
#include "ydk_yang.hpp"


using namespace std;
using json = nlohmann::json;

namespace ydk
{
#define ODLNode network_topology::NetworkTopology::Topology::Node

//////////////////////////////////////////
//// OpenDaylightCapabilitiesParser
//////////////////////////////////////////
OpenDaylightCapabilitiesParser::OpenDaylightCapabilitiesParser()
{
}

OpenDaylightCapabilitiesParser::~OpenDaylightCapabilitiesParser()
{
}

vector<path::Capability> OpenDaylightCapabilitiesParser::parse(vector<string> & capabilities) const
{
	vector<path::Capability> path_capabilities{};
	for(const auto & c : capabilities)
	{
		if(c.find("calvados") != string::npos || c.find("tailf") != string::npos || c.find("tail-f") != string::npos)
		{
			continue;
		}
		auto q = c.find("revision=");
		if(q != string::npos)
		{
			auto close = c.find(')');
			if(close != string::npos)
			{
                auto rsize = string("revision=").size();
                auto revision = c.substr(q+rsize,close-q-rsize);
                auto module = c.substr(close+1);

				path_capabilities.push_back({module, revision});
			}
		}
	}
	//add ydk capability
	path::Capability ydk_cap{ydk::path::YDK_MODULE_NAME, ydk::path::YDK_MODULE_REVISION, {}, {}};
	auto result = find(path_capabilities.begin(), path_capabilities.end(), ydk_cap);
	if(result == path_capabilities.end()){
		path_capabilities.push_back(ydk_cap);
	}

	//add ietf-netconf capability
	path::Capability ietf_netconf_cap{ydk::IETF_NETCONF_MODULE_NAME, ydk::IETF_NETCONF_MODULE_REVISION, {}, {}};
	result = find(path_capabilities.begin(), path_capabilities.end(), ietf_netconf_cap);
	if(result == path_capabilities.end()){
		path_capabilities.push_back(ietf_netconf_cap);
	}

	return path_capabilities;
}

//////////////////////////////////////////
//// OpenDaylightCapabilitiesJsonParser
//////////////////////////////////////////
OpenDaylightCapabilitiesJsonParser::OpenDaylightCapabilitiesJsonParser()
{

}

OpenDaylightCapabilitiesJsonParser::~OpenDaylightCapabilitiesJsonParser()
{
}

static void print_to_node(const json& pt, ODLNode & odl_node)
{
    for (auto it = pt.begin(); it != pt.end(); ++it)
    {
        if(it.key().find("node-id") != string::npos)
        {
            odl_node.node_id = it->get<string>();
        }
        else if(it.key().find("host") != string::npos)
        {
            odl_node.host = it->get<string>();
        }
        else if(it.key().find("connection-status") != string::npos)
        {
            odl_node.connection_status = it->get<string>();
        }
        else if(it.key().find("port") != string::npos)
        {
            odl_node.port = it->get<int>();
        }
        else if(it.key().find(":available-capabilities") != string::npos)
        {
            for(auto p = it->begin(); p != it->end(); ++p)
            {
                for(auto p1 = p->begin(); p1 != p->end(); ++p1)
                {
                    auto a = make_unique<ODLNode::AvailableCapabilities::AvailableCapability>();
                    a->capability = p1->get<string>();
                    odl_node.available_capabilities->available_capability.push_back(move(a));
                }
            }
        }
    }
}

static void parse_json(const json& pt, std::map<std::string, std::unique_ptr<ODLNode>> & odl_nodes)
{
    auto network_topology = pt.find("network-topology");
    if(network_topology == pt.end())
        return;

    auto topology = network_topology->find("topology");
    if(topology == network_topology->end())
        return;

    for(auto it = topology->begin(); it != topology->end(); it++)
    {
        for(auto child = it->begin(); child != it->end(); child++)
        {
            if(child->type_name() == "array")
            {
                for(size_t i=0; i<child->size(); i++)
                {
                    auto node = make_unique<ODLNode>();
                    print_to_node(child->at(i), *node);
                    if(node->node_id.get().size()>0)
                    {
                        string s = node->node_id.get();
                        odl_nodes[s] = move(node);
                     }
                }
            }
        }
    }
}

std::map<std::string, std::unique_ptr<ODLNode>> OpenDaylightCapabilitiesJsonParser::parse(const string & capabilities_buffer)
{
	std::map<std::string, std::unique_ptr<ODLNode>> odl_nodes;

	istringstream ss(capabilities_buffer);
	json pt;
	ss >> pt;

	parse_json(pt, odl_nodes);

	return odl_nodes;
}
#undef ODLNode
}
