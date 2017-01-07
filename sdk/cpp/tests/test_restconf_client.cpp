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
#define BOOST_TEST_MODULE RestconfProviderTest
#include <boost/test/unit_test.hpp>
#include <string>
#include "../core/src/restconf_client.hpp"
#include "../core/src/errors.hpp"
#include <iostream>
#include "config.hpp"

using namespace ydk;
using namespace std;


BOOST_AUTO_TEST_CASE(PostGetPatchGet)
{
	string response;
	RestconfClient client{"http://localhost", "admin", "admin", 12306, "application/json"};

	string json = "{\"id\": \"2\",\"network-topology\": {\"topology\": \"netconf\"}}";
	BOOST_CHECK_NO_THROW(client.execute("POST", "/test", json));
	BOOST_CHECK_NO_THROW((response = client.execute("GET", "/test", "")));
	BOOST_REQUIRE(response.find(json) != string::npos);

	json = "{\"id\": \"2\",\"network-topology\": {\"topology\": \"restconf\"}}";
	BOOST_CHECK_NO_THROW(client.execute("PATCH", "/test/1", json));
	BOOST_CHECK_NO_THROW((response = client.execute("GET", "/test/1", "")));
	BOOST_REQUIRE(response.find(json) != string::npos);

	json = "{\"id\": \"2\",\"network-topology\": {\"topology\": \"odl\"}}";
	BOOST_CHECK_NO_THROW(client.execute("PUT", "/test/2", json));
	BOOST_CHECK_NO_THROW((response = client.execute("GET", "/test/2", "")));
	BOOST_REQUIRE(response.find(json) != string::npos);

	BOOST_CHECK_NO_THROW(client.execute("DELETE", "/test/2", ""));
}
