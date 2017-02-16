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

#include <string.h>
#include <iostream>

#include "../../src/netconf_provider.hpp"
#include "../../src/make_unique.hpp"
#include "../models_gen/openconfig_bgp.h"

using namespace ydk;
using namespace std;
#define MODELS_PATH "/Users/abhirame/Cisco/003/ydk-gen/sdk/cpp/ydk/tests/models/openconfig"

TEST_CASE(bgp_as)
{
	ydk::NetconfServiceProvider provider{ "127.0.0.1", "admin", "admin", "12022", "", "", MODELS_PATH};
	auto bgp = make_unique<ydk::openconfig_bgp::Bgp>();

	// Set the Global AS
	bgp->global_->config->as_ = 65001;

	string payload = provider.encode(*bgp, "create");
	INFO(payload);

	string result = provider.execute_payload(payload, "create");
	REQUIRE(strstr(result.c_str(),"<ok/>") != NULL);
}
