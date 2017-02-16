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
#include <string>
#include "../core/src/restconf_client.hpp"
#include "../core/src/errors.hpp"
#include <iostream>

#include "catch.hpp"

using namespace ydk;
using namespace std;


TEST_CASE("PostGetPatchGet")
{
	string response;
	RestconfClient client{"http://localhost", "admin", "admin", 12306, "application/json"};

	string json = "{\"id\": \"2\",\"network-topology\": {\"topology\": \"netconf\"}}";
	CHECK_NOTHROW(client.execute("POST", "/test", json));
	CHECK_NOTHROW((response = client.execute("GET", "/test", "")));
	REQUIRE(response.find(json) != string::npos);

	json = "{\"id\": \"2\",\"network-topology\": {\"topology\": \"restconf\"}}";
	CHECK_NOTHROW(client.execute("PATCH", "/test/1", json));
	CHECK_NOTHROW((response = client.execute("GET", "/test/1", "")));
	REQUIRE(response.find(json) != string::npos);

	json = "{\"id\": \"2\",\"network-topology\": {\"topology\": \"odl\"}}";
	CHECK_NOTHROW(client.execute("PUT", "/test/2", json));
	CHECK_NOTHROW((response = client.execute("GET", "/test/2", "")));
	REQUIRE(response.find(json) != string::npos);

	CHECK_NOTHROW(client.execute("DELETE", "/test/2", ""));
}
