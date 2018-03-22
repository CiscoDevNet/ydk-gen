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
#include "../src/path/netconf_model_provider.hpp"
#include "../src/path_api.hpp"
#include "../src/path/path_private.hpp"
#include "config.hpp"
#include "catch.hpp"
#include "common_utilities.hpp"

TEST_CASE( "test_segmentalize"  )
{
    std::string test_string = "Cisco-IOS-XR-clns-isis-cfg:isis/instances/instance/interfaces[active='act'][interface-name='GigabitEthernet0/0/0/0']";
    std::vector<std::string> segments = ydk::path::segmentalize(test_string);
    std::vector<std::string> expected {"Cisco-IOS-XR-clns-isis-cfg:isis", "instances", "instance", "interfaces[active='act'][interface-name='GigabitEthernet0/0/0/0']"};

    REQUIRE(segments==expected);
}

TEST_CASE( "test_segmentalize_relative_path"  )
{
    std::string test_string = "interface-configuration[active='act'][interface-name='GigabitEthernet0/0/0/0']";
    std::vector<std::string> segments = ydk::path::segmentalize(test_string);
    std::vector<std::string> expected {"interface-configuration[active='act'][interface-name='GigabitEthernet0/0/0/0']"};

    REQUIRE(segments == expected);
}

TEST_CASE( "test_replace_xml_escape_sequences"  )
{
    std::string source   = R"(Testing: &lt;tag&gt;; ampersand - &amp;; &quot;quotes&quot;; huawei end-of-line&#13;)";
    std::string expected = R"(Testing: <tag>; ampersand - &; "quotes"; huawei end-of-line)";

    REQUIRE( ydk::has_xml_escape_sequences(source));
    REQUIRE( !ydk::has_xml_escape_sequences(expected));

    std::string converted = ydk::replace_xml_escape_sequences(source);

    REQUIRE(converted == expected);
}

class TestClient: public ydk::NetconfClient
{
public:
    TestClient()
    {
    }
    ~TestClient()
    {
    }

    int connect()
    {
        return 0;
    }
    std::string execute_payload(const std::string & payload)
    {
        return payload;
    }
    std::vector<std::string> get_capabilities()
    {
        return {};
    }
    std::string get_hostname_port()
    {
        return "";
    }
};

TEST_CASE("static_model_provider")
{
    TestClient t{};
    ydk::path::StaticModelProvider s{t};
    REQUIRE_NOTHROW(s.get_model("","",ydk::path::ModelProvider::Format::YANG));
    REQUIRE_NOTHROW(s.get_hostname_port());
}
