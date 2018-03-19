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
#include "logger.hpp"
#include "path_api.hpp"
#include "xml_util.hpp"
#include "ydk_yang.hpp"


using namespace std;

namespace ydk
{

static xmlNodePtr drop_hello_tag(xmlNodePtr root)
{
    auto cur = root->xmlChildrenNode;
    while (cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, to_xmlchar("capabilities"))))
            return cur;
        cur = cur->next;
    }
    return root;
}

static xmlNodePtr check_and_return_root(xmlDocPtr doc)
{
    if (doc == NULL)
    {
        YLOG_INFO("Empty capabilities");
        return NULL;
    }
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        YLOG_INFO("Empty capabilities");
        return NULL;
    }
    return cur;
}

static string get_xml_node_content(xmlNodePtr n)
{
    string c;
    xmlChar * buffer = xmlNodeGetContent(n);
    if(buffer != NULL)
    {
        ostringstream os;
        os << buffer;
        xmlFree(buffer);
        c = os.str();
    }
    return c;
}
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

vector<string> IetfCapabilitiesXmlParser::parse_yang_1_1(const std::string & buffer)
{
    vector<string> capabilities{};

    doc = xmlReadMemory(buffer.c_str(), buffer.size(), "noname.xml", NULL, 0);
    xmlNodePtr cur = check_and_return_root(doc);
    if(cur == NULL)
    {
        return {};
    }

    if (xmlStrcmp(cur->name, to_xmlchar("rpc-reply")) != 0)
    {
        YLOG_INFO("Unexpected XML");
        return {};
    }
    cur = cur->xmlChildrenNode;
    while(cur != NULL)
    {
        if (xmlStrcmp(cur->name, to_xmlchar("data")) == 0)
        {
            xmlNodePtr c = cur->xmlChildrenNode;
            while(c != NULL)
            {
                if (xmlStrcmp(c->name, to_xmlchar("modules-state")) == 0)
                {
                    xmlNodePtr c1 = c->xmlChildrenNode;
                    while(c1 != NULL)
                    {
                        if (xmlStrcmp(c1->name, to_xmlchar("module")) == 0)
                        {
                            string module_name, revision, name_space;
                            vector<string> features;
                            vector<string> deviations;

                            xmlNodePtr c2 = c1->xmlChildrenNode;
                            while(c2 != NULL)
                            {
                                if (xmlStrcmp(c2->name, to_xmlchar("name")) == 0)
                                {
                                    module_name = get_xml_node_content(c2);
                                }
                                else if (xmlStrcmp(c2->name, to_xmlchar("revision")) == 0)
                                {
                                    revision = get_xml_node_content(c2);
                                }
                                else if (xmlStrcmp(c2->name, to_xmlchar("namespace")) == 0)
                                {
                                    name_space = get_xml_node_content(c2);
                                }
                                else if (xmlStrcmp(c2->name, to_xmlchar("feature")) == 0)
                                {
                                    features.push_back(get_xml_node_content(c2));
                                }
                                else if (xmlStrcmp(c2->name, to_xmlchar("deviation")) == 0)
                                {
                                    xmlNodePtr c3 = c2->xmlChildrenNode;
                                    while(c3 != NULL)
                                    {
                                        if (xmlStrcmp(c3->name, to_xmlchar("name")) == 0)
                                        {
                                            deviations.push_back(get_xml_node_content(c3));
                                        }
                                        c3 = c3->next;
                                    }
                                }
                                c2 = c2->next;
                            }
                            ostringstream capability;
                            capability<<name_space<<"?module="<<module_name;
                            if(revision.size())
                                capability <<"&revision="<<revision;
                            if(features.size())
                            {
                                capability <<"&features=";
                                for(size_t i=0;i<features.size()-1;i++)
                                {
                                    capability << features[i] << ",";
                                }
                                capability << features[features.size()-1];
                            }
                            if(deviations.size())
                            {
                                capability <<"&deviations=";
                                for(size_t i=0;i<deviations.size()-1;i++)
                                {
                                    capability << deviations[i] << ",";
                                }
                                capability << deviations[deviations.size()-1];
                            }
                            capabilities.push_back(capability.str());
                        }
                        c1 = c1->next;
                    }
                }
                c = c->next;
            }
        }
        cur = cur->next;
    }
    return capabilities;
}

vector<string> IetfCapabilitiesXmlParser::parse(const string & capabilities_buffer)
{
    vector<string> capabilities{};

    doc = xmlReadMemory(capabilities_buffer.c_str(), capabilities_buffer.size(), "noname.xml", NULL, 0);
    xmlNodePtr cur = check_and_return_root(doc);
    if(cur == NULL)
    {
        return {};
    }

    // drop hello
    cur = drop_hello_tag(cur);

    if (xmlStrcmp(cur->name, to_xmlchar("capabilities")) != 0)
    {
        YLOG_INFO("Unexpected XML");
        return {};
    }
    cur = cur->xmlChildrenNode;
    while(cur != NULL)
    {
        if (xmlStrcmp(cur->name, to_xmlchar("capability")) == 0)
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


unordered_map<string, path::Capability>
IetfCapabilitiesParser::get_lookup_table(vector<path::Capability>& capabilities) const
{
    unordered_map<string, path::Capability> name_namespace_lookup;
    for (auto &c: capabilities)
    {
        name_namespace_lookup.insert(make_pair(c.module, c));
    }
    return name_namespace_lookup;
}

unordered_map<string, path::Capability>
IetfCapabilitiesParser::get_lookup_table(vector<string>& capabilities) const
{
    unordered_map<string, path::Capability> name_namespace_lookup;

    auto segs = segmentalize_capabilities(capabilities);

    for (auto &s: segs)
    {
        name_namespace_lookup.insert(make_pair(s.second.module, s.second));
        name_namespace_lookup.insert(make_pair(s.first, s.second));
    }

    return name_namespace_lookup;
}

vector<pair<std::string, path::Capability>> IetfCapabilitiesParser::segmentalize_capabilities(vector<string>& capabilities) const
{
    vector<pair<std::string, path::Capability>> segs;

    for( string &c : capabilities )
    {

        auto p = find(c.begin(), c.end(),'?');

        if(p == c.end())
            continue;

        auto ns_end = c.find("?");

        if (ns_end == string::npos)
            continue;

        auto c_ns = c.substr(0, ns_end);

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

        segs.emplace_back(make_pair(c_ns, path::Capability{c_module, c_revision, c_features, c_deviations}));
    }

    return segs;
}

vector<path::Capability> IetfCapabilitiesParser::parse(vector<string> & capabilities) const
{
    auto segs = segmentalize_capabilities(capabilities);

    vector<path::Capability> yang_caps;
    for (auto &c: segs)
    {
        yang_caps.emplace_back(c.second);
    }

    //add ydk capability
    path::Capability ydk_cap{ydk::path::YDK_MODULE_NAME, ydk::path::YDK_MODULE_REVISION, {}, {}};
    auto result = find(yang_caps.begin(), yang_caps.end(), ydk_cap);
    if(result == yang_caps.end()) {
        yang_caps.push_back(ydk_cap);
    }
    //add ietf-netconf capability
    path::Capability ietf_netconf_cap{ydk::IETF_NETCONF_MODULE_NAME, ydk::IETF_NETCONF_MODULE_REVISION, {}, {}};
    result = find(yang_caps.begin(), yang_caps.end(), ietf_netconf_cap);
    if(result == yang_caps.end()) {
        yang_caps.push_back(ietf_netconf_cap);
    }

    return yang_caps;
}

}
