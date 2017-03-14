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
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sstream>

#include "ietf_parser.hpp"
#include "path_api.hpp"
#include "ydk_yang.hpp"
#include "logger.hpp"


using namespace std;

namespace ydk
{
//////////////////////////////////////////
//// IetfCapabilitiesXmlParser
//////////////////////////////////////////
IetfCapabilitiesXmlParser::IetfCapabilitiesXmlParser()
			: doc (NULL)
{
}

IetfCapabilitiesXmlParser::~IetfCapabilitiesXmlParser()
{
	if(doc != NULL)
	{
		xmlFreeDoc(doc);
	}
}

vector<string> IetfCapabilitiesXmlParser::parse(const string & capabilities_buffer)
{
	doc = xmlReadMemory(capabilities_buffer.c_str(), capabilities_buffer.size(), "noname.xml", NULL, 0);

	vector<string> capabilities{};
	if (doc == NULL)
	{
		YLOG_INFO("Empty capabilities");
		return {};
	}
	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (cur == NULL)
	{
		YLOG_INFO("Empty capabilities");
		return {};
	}
	if (xmlStrcmp(cur->name, (const xmlChar *) "capabilities") != 0)
	{
		YLOG_INFO("Unexpected XML");
		return {};
	}
	cur = cur->xmlChildrenNode;
	while(cur != NULL)
	{
		if (xmlStrcmp(cur->name, (const xmlChar *) "capability") == 0)
		{
			xmlChar * capability_buffer = xmlNodeGetContent(cur);
			if(capability_buffer != NULL)
			{
				ostringstream capability;
				capability << capability_buffer;
				xmlFree(capability_buffer);
				capabilities.push_back(capability.str());
			}
		}
		cur = cur->next;
	}
	return capabilities;
}

//////////////////////////////////////////
//// IetfCapabilitiesParser
//////////////////////////////////////////
IetfCapabilitiesParser::IetfCapabilitiesParser()
{

}

IetfCapabilitiesParser::~IetfCapabilitiesParser()
{

}

vector<path::Capability> IetfCapabilitiesParser::parse(vector<string> & capabilities) const
{
	vector<path::Capability> yang_caps {};
	for(string c : capabilities )
	{
		if(c.find("calvados") != string::npos || c.find("tailf") != string::npos || c.find("tail-f") != string::npos)
		{
			continue;
		}

		auto p = find(c.begin(), c.end(),'?');

		if(p == c.end())
			continue;

		auto module_start = c.find("module=");

		if(module_start == string::npos)
			continue;

		auto revision_start = c.find("revision=");
		if(revision_start == string::npos)
			continue;

		vector<string> c_features{};
		vector<string> c_deviations{};

		auto module_end = c.find("&", module_start);

		module_start+=sizeof("module=");
		auto size = module_end;
		if(size != string::npos ){
			size = module_end - module_start + 1;
		}

		string c_module = c.substr( module_start - 1, size );


		auto revision_end = c.find("&", revision_start);
		revision_start+=sizeof("revision=");
		size = revision_end;
		if(size!= string::npos) {
			size= revision_end - revision_start + 1;
		}
		string c_revision = c.substr(revision_start - 1, size);

		auto features_start = c.find("features=");
		if(features_start != string::npos){
			auto features_end = c.find("&", features_start);
			features_start+=sizeof("features=");
			size=features_end;
			if(size!=string::npos){
				size = features_end - features_start + 1;
			}
			string features = c.substr(features_start - 1 , size);
			istringstream iss{features};
			string feature;
			while(getline(iss, feature, ',')) {
				c_features.push_back(move(feature));
			}

		}

		auto deviations_start = c.find("deviations=");
		if(deviations_start != string::npos){
			auto deviations_end = c.find("&", deviations_start);
			deviations_start+=sizeof("deviations=");
			size=deviations_end;
			if(size!=string::npos){
				size = deviations_end - deviations_start + 1;
			}
			string deviations = c.substr(deviations_start - 1, size);
			istringstream iss{deviations};
			string deviation;
			while(getline(iss, deviation, ',')) {
				c_deviations.push_back(move(deviation));
			}

		}
		if(c_module.find("tailf") != string::npos) {
			continue;
		}
		path::Capability core_cap{c_module, c_revision, c_features, c_deviations};
		yang_caps.emplace_back(core_cap);
	}

	//add ydk capability
	path::Capability ydk_cap{ydk::path::YDK_MODULE_NAME, ydk::path::YDK_MODULE_REVISION, {}, {}};
	auto result = find(yang_caps.begin(), yang_caps.end(), ydk_cap);
	if(result == yang_caps.end()){
		yang_caps.push_back(ydk_cap);
	}
	//add ietf-netconf capability
	path::Capability ietf_netconf_cap{ydk::IETF_NETCONF_MODULE_NAME, ydk::IETF_NETCONF_MODULE_REVISION, {}, {}};
	result = find(yang_caps.begin(), yang_caps.end(), ietf_netconf_cap);
	if(result == yang_caps.end()){
		yang_caps.push_back(ietf_netconf_cap);
	}
	return yang_caps;
}
}
