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

#define BOOST_TEST_MODULE CoreTest
#include <boost/test/unit_test.hpp>
#include <iostream>
#include "../../src/path_api.hpp"
#include "../../src/path/path_private.hpp"
#include "../config.hpp"

BOOST_AUTO_TEST_CASE( test_segmentalize  )
{
	std::string test_string = "Cisco-IOS-XR-clns-isis-cfg:isis/instances/instance/interfaces[interface-name='GigabitEthernet0/0/0/0']";
	std::vector<std::string> segments = ydk::path::segmentalize(test_string);
	std::vector<std::string> expected {"Cisco-IOS-XR-clns-isis-cfg:isis", "instances", "instance", "interfaces[interface-name='GigabitEthernet0/0/0/0']"};

	for (size_t index = 0;index<segments.size();index++)
	{
		BOOST_REQUIRE(segments[index] == expected[index]);
	}
}
