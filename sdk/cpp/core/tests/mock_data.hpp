
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
#include "config.hpp"
#include "catch.hpp"

namespace mock {
class MockServiceProvider : public ydk::path::ServiceProvider
{
public:
    MockServiceProvider(const std::string searchdir, const std::vector<ydk::path::Capability> capabilities) : m_searchdir{searchdir}, m_capabilities{capabilities}
    {
        ydk::path::Repository repo{m_searchdir};
        root_schema = repo.create_root_schema(m_capabilities);
    }

    virtual ~MockServiceProvider()
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
    {"ydktest-sanity", ""}

};
