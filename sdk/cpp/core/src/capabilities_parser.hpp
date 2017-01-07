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
#ifndef _CAPABILITIES_PARSER_H_
#define _CAPABILITIES_PARSER_H_

#include <string>
#include <vector>


namespace ydk
{
namespace path
{
struct Capability;
}

class CapabilitiesXmlParser
{
	public:
		CapabilitiesXmlParser();
		virtual ~CapabilitiesXmlParser();

		virtual std::vector<std::string> parse(const std::string & capabilities_xml) = 0;
};

class CapabilitiesParser
{
	public:
		CapabilitiesParser();
		virtual ~CapabilitiesParser();

		virtual std::vector<path::Capability> parse(std::vector<std::string> & capabilities) const =0;
};
}

#endif /* _CAPABILITIES_PARSER_H_ */
