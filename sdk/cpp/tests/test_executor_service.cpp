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

#include <ydk/netconf_provider.hpp>
#include <ydk/executor_service.hpp>
#include <ydk_ydktest/ydktest_sanity.hpp>
#include <ydk/types.hpp>

#include "config.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace std;

TEST_CASE("dummy")
{
    bool foo = true;
    REQUIRE(foo);
}

// TEST_CASE("execute_validate_rpc_source_1")
// {
//     ydk::path::Repository repo{TEST_HOME};
//     NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
//     ExecutorService es{};

//     auto r_1 = make_unique<ydktest_sanity::Runner>();
//     ydk::ietf_netconf::ValidateRpc rpc{};
//     rpc.source->candidate = "candidate";
//     bool reply = es.execute_rpc(provider, rpc);
//     REQUIRE(reply);
// }

// TEST_CASE("execute_validate_rpc_source_2")
// {
//     ydk::path::Repository repo{TEST_HOME};
//     NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
//     ExecutorService es{};

//     auto r_1 = make_unique<ydktest_sanity::Runner>();
//     ydk::ietf_netconf::ValidateRpc rpc{};
//     Empty e;
//     e.set = true;
//     rpc.source->running = e;
//     bool reply = es.execute_rpc(provider, rpc);
//     REQUIRE(reply);
// }

// TEST_CASE("execute_validate_rpc_source_3")
// {
//     ydk::path::Repository repo{TEST_HOME};
//     NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
//     ExecutorService es{};

//     auto r_1 = make_unique<ydktest_sanity::Runner>();
//     ydk::ietf_netconf::ValidateRpc rpc{};
//     Empty e;
//     e.set = true;
//     rpc.source->startup = e;
//     bool reply = es.execute_rpc(provider, rpc);
//     REQUIRE(reply);
// }

// TEST_CASE("execute_validate_rpc_source_4")
// {
//     ydk::path::Repository repo{TEST_HOME};
//     NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
//     ExecutorService es{};

//     auto r_1 = make_unique<ydktest_sanity::Runner>();
//     ydk::ietf_netconf::ValidateRpc rpc{};
//     Empty e;
//     e.set = true;
//     rpc.source->url = e;
//     bool reply = es.execute_rpc(provider, rpc);
//     REQUIRE(reply);
// }

// TEST_CASE("execute_validate_rpc_source_5")
// {
//     ydk::path::Repository repo{TEST_HOME};
//     NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
//     ExecutorService es{};

//     auto r_1 = make_unique<ydktest_sanity::Runner>();
//     ydk::ietf_netconf::ValidateRpc rpc{};
//     // rpc.source->config = // openconfg_bgp::Bgp -- create bgp object and assign
//     bool reply = es.execute_rpc(provider, rpc);
//     REQUIRE(reply);
// }

