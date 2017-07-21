//
// @file types.hpp
// @brief Header for ydk entity
//
// YANG Development Kit
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

#include "../src/path/path_private.hpp"
#include "../src/types.hpp"
#include "catch.hpp"
#include "config.hpp"
#include "mock_data.hpp"
#include <iostream>

using namespace ydk;
using namespace std;

TEST_CASE("datanode_collection")
{
    std::string searchdir{TEST_HOME};
    mock::MockServiceProvider sp{searchdir, test_openconfig};

    auto & schema = sp.get_root_schema();

    auto & bgp = schema.create_datanode("openconfig-bgp:bgp", "");
    bgp.create_datanode("global/config/as", "65172");

    auto & rp = schema.create_datanode("openconfig-routing-policy:routing-policy", "");
    rp.create_datanode("policy-definitions/policy-definition[name='POLICY1']");

    std::vector<path::DataNode*> dn {&bgp, &rp};
    path::DataNodeCollection a{dn};

    auto b = a.get_data_nodes();

    auto r_1 = b.at("openconfig-bgp:bgp");
    auto r_2 = b.at("openconfig-routing-policy:routing-policy");

    REQUIRE(r_1 != nullptr);
    REQUIRE(r_2 != nullptr);

    REQUIRE(r_1->get_path() == bgp.get_path());
    REQUIRE(r_2->get_path() == rp.get_path());
    REQUIRE(b.size() == 2);
}

TEST_CASE("entity_collection")
{
    auto test = make_shared<TestEntity>();
    auto child = make_shared<TestEntity::Child>();

    EntityCollection a{{test, child}};
    auto b = a.get_entities();

    auto r_1 = b.at("test");
    auto r_2 = b.at("child");

    REQUIRE(r_1 != nullptr);
    REQUIRE(r_2 != nullptr);

    REQUIRE(r_1->get_segment_path() == test->get_segment_path());
    REQUIRE(r_2->get_segment_path() == child->get_segment_path());
    REQUIRE(b.size() == 2);
}
