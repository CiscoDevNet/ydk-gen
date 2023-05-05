
/// YANG Development Kit
// Copyright 2016 Cisco Systems. All rights reserved
//
////////////////////////////////////////////////////////////////
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//////////////////////////////////////////////////////////////////

#include <iostream>
#include "../src/path_api.hpp"
#include "../src/ietf_parser.hpp"

// as we are requiring load module from module namespace(test_codec.cpp)
// we have to provide mock data with module namespace to module name mapping
static std::unordered_map<std::string, ydk::path::Capability> test_openconfig_lookup {
    {"http://openconfig.net/yang/bgp-types", {"openconfig-bgp-types", "" }},
    {"http://openconfig.net/yang/bgp", {"openconfig-bgp", ""}},
    {"http://openconfig.net/yang/openconfig-ext", {"openconfig-extensions", ""}},
    {"http://openconfig.net/yang/interfaces", {"openconfig-interfaces", ""}},
    {"http://openconfig.net/yang/policy-types", {"openconfig-policy-types", ""}},
    {"http://openconfig.net/yang/routing-policy", {"openconfig-routing-policy", ""}},
    {"http://openconfig.net/yang/openconfig-types", {"openconfig-types", ""}},
    {"urn:ietf:params:xml:ns:netconf:base:1.0", {"ietf-netconf", ""}},
    {"urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring", {"ietf-netconf-monitoring", ""}},
    {"urn:ietf:params:xml:ns:yang:ietf-interfaces", {"ietf-interfaces", ""}},
    {"http://cisco.com/ns/yang/ydk", {"ydk", ""}},
    {"http://cisco.com/ns/yang/ydktest-sanity", {"ydktest-sanity", ""}},
    {"http://cisco.com/ns/yang/ydktest-action", {"ydktest-sanity-action", ""}},
    {"http://cisco.com/ns/yang/ydktest-types", {"ydktest-types", ""}}
};


namespace mock {
class MockSession : public ydk::path::Session 
{
public:
    MockSession(const std::string & searchdir, const std::vector<ydk::path::Capability> capabilities) : m_searchdir{searchdir}, m_capabilities{capabilities}
    {
        ydk::path::Repository repo{m_searchdir};
        ydk::IetfCapabilitiesParser capabilities_parser{};
        auto lookup_table = capabilities_parser.get_lookup_table(m_capabilities);
        lookup_table.insert(test_openconfig_lookup.begin(), test_openconfig_lookup.end());
        root_schema = repo.create_root_schema(lookup_table, m_capabilities);
    }

    virtual ~MockSession()
    {
    }


    ydk::path::RootSchemaNode& get_root_schema() const
    {
        return *root_schema;
    }

    ydk::EncodingFormat get_encoding() const
    {
        return ydk::EncodingFormat::XML;
    }

    std::shared_ptr<ydk::path::DataNode> invoke(ydk::path::Rpc& rpc) const
    {
        ydk::path::Codec s{};

        std::cout << s.encode(rpc.get_input_node(), ydk::EncodingFormat::XML, true) << std::endl;

        return nullptr;
    }

    std::shared_ptr<ydk::path::DataNode> invoke(ydk::path::DataNode& rpc) const
    {
        ydk::path::Codec s{};

        std::cout << s.encode(rpc, ydk::EncodingFormat::XML, true) << std::endl;

        return nullptr;
    }
private:
    std::string m_searchdir;
    std::vector<ydk::path::Capability> m_capabilities;
    std::shared_ptr<ydk::path::RootSchemaNode> root_schema;

};
}

static std::vector<ydk::path::Capability> test_openconfig {
    {"openconfig-bgp-types", "" },
    {"openconfig-bgp", ""},
    {"openconfig-extensions", ""},
    {"openconfig-interfaces", ""},
    {"openconfig-policy-types", ""},
    {"openconfig-routing-policy", ""},
    {"openconfig-types", ""},
    {"ietf-netconf", ""},
    {"ietf-netconf-monitoring", ""},
    {"ietf-interfaces", ""},
    {"ydk", ""},
    {"ydktest-sanity", ""},
    {"ydktest-sanity-action", ""},
    {"ydktest-types", ""}
};

