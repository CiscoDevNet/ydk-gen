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
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>
#include <string>
#include <iostream>

#include "opendaylight_parser.hpp"
#include "path_api.hpp"
#include "ydk_yang.hpp"


using namespace std;
using namespace ydk::network_topology;

namespace ydk
{
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

static vector<string> get_caps(boost::property_tree::ptree const& pt, int count)
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    vector<string> v{};
    for (ptree::const_iterator it = pt.begin(); it != end; ++it)
    {
        if(count == 1)
            return get_caps(it->second, count+1);
       	v.push_back(it->second.get_value<std::string>());
    }
    return v;
}

static void print_to_node(boost::property_tree::ptree const& pt, NetworkTopology::Topology::Node & odl_node)
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();
    for (ptree::const_iterator it = pt.begin(); it != end; ++it)
    {
        if(it->first.find("node-id") != string::npos)
        {
        	odl_node.node_id = it->second.get_value<std::string>();
        }
        else if(it->first.find("host") != string::npos)
        {
        	odl_node.host = it->second.get_value<std::string>();
        }
        else if(it->first.find("connection-status") != string::npos)
	{
		odl_node.connection_status = it->second.get_value<std::string>();
	}
        else if(it->first.find("port") != string::npos)
	{
		odl_node.port = it->second.get_value<int>();
	}
        else if(it->first.find("available-capabilities") != string::npos)
        {
            vector<string> caps = get_caps(it->second, 1);
            for(const auto & c : caps)
            {
            	auto a = make_unique<NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability>();
            	a->capability = c;
            	odl_node.available_capabilities->available_capability.push_back(move(a));
            }
        }

    }
}

static void parse_json(boost::property_tree::ptree const& pt, std::map<std::string, std::unique_ptr<NetworkTopology::Topology::Node>> & odl_nodes)
{
    using boost::property_tree::ptree;
    ptree::const_iterator end = pt.end();

    for (ptree::const_iterator it = pt.begin(); it != end; ++it)
    {
        if(it->first.find("node") != string::npos)
        {
            auto node = make_unique<NetworkTopology::Topology::Node>();
            print_to_node(pt, *node);
            if(node->node_id.get().size()>0)
            {
            	string s = node->node_id.get();
                odl_nodes[s] = move(node);
            }
        }
        parse_json(it->second, odl_nodes);
    }
}

std::map<std::string, std::unique_ptr<NetworkTopology::Topology::Node>> OpenDaylightCapabilitiesJsonParser::parse(const string & capabilities_buffer)
{
	std::map<std::string, std::unique_ptr<NetworkTopology::Topology::Node>> odl_nodes;

	boost::property_tree::ptree pt;
	istringstream ss(capabilities_buffer);
	read_json(ss, pt);

	parse_json(pt, odl_nodes);

	return odl_nodes;
}

}
