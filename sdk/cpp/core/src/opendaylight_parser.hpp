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

#ifndef _OPENDAYLIGHT_PARSER_H_
#define _OPENDAYLIGHT_PARSER_H_

#include <memory>
#include <string>
#include <vector>

#include "capabilities_parser.hpp"
#include "network_topology.hpp"
#include "types.hpp"


namespace ydk
{
class OpenDaylightCapabilitiesJsonParser
{
	public:
		OpenDaylightCapabilitiesJsonParser();
		~OpenDaylightCapabilitiesJsonParser();

		std::map<std::string, std::unique_ptr<network_topology::NetworkTopology::Topology::Node> > parse(const std::string & capabilities_xml);

};

class OpenDaylightCapabilitiesParser : public CapabilitiesParser
{
	public:
		OpenDaylightCapabilitiesParser();
		~OpenDaylightCapabilitiesParser();

		std::vector<path::Capability> parse(std::vector<std::string> & capabilities) const;
};

}

#endif /* _OPENDAYLIGHT_PARSER_H_ */
