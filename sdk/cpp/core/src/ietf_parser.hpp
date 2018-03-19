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

#ifndef _IETF_PARSER_H_
#define _IETF_PARSER_H_

#include <unordered_map>

#include "capabilities_parser.hpp"

struct _xmlDoc;
typedef struct _xmlDoc xmlDoc;
typedef xmlDoc * xmlDocPtr;


namespace ydk
{

class IetfCapabilitiesXmlParser : public CapabilitiesXmlParser
{
    public:
        IetfCapabilitiesXmlParser();
        ~IetfCapabilitiesXmlParser();

        std::vector<std::string> parse(const std::string & buffer);
        std::vector<std::string> parse_yang_1_1(const std::string & buffer);

    private:
        xmlDocPtr doc;
};


class IetfCapabilitiesParser : public CapabilitiesParser
{
    public:
        IetfCapabilitiesParser();
        ~IetfCapabilitiesParser();

        std::vector<path::Capability> parse(std::vector<std::string> & capabilities) const;
        std::unordered_map<std::string, path::Capability> get_lookup_table(std::vector<std::string>& capabilities) const;
        std::unordered_map<std::string, path::Capability> get_lookup_table(std::vector<path::Capability>& capabilities) const;
        std::vector<std::pair<std::string, path::Capability>> segmentalize_capabilities(std::vector<std::string>& capabilities) const;
};
}

#endif /* _IETF_PARSER_H_ */
