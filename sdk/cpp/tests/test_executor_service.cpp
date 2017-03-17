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
#include <ydk_ydktest/ietf_netconf.hpp>
#include <ydk_ydktest/openconfig_bgp.hpp>
#include <ydk/types.hpp>

#include "config.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace std;

TEST_CASE("es_close_session_rpc")
{
    // provider
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    ydk::ietf_netconf::CloseSessionRpc rpc{};

    std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);
    bool result = reply == nullptr;
    REQUIRE(result);
}

// persist-id is broken?
TEST_CASE("es_commit_rpc")
{
    // provider
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    // auto r_1 = make_unique<ydktest_sanity::Runner>();
    ydk::ietf_netconf::CommitRpc rpc{};
    Empty e;
    e.set = true;
    rpc.input->confirmed = e;
    rpc.input->confirm_timeout = 5;
    rpc.input->persist = "0";
    // rpc.input->persist_id = "0";

    std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);
    bool result = reply == nullptr;
    REQUIRE(result);
}

TEST_CASE("es_copy_config_rpc")
{
    // provider
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    ydk::ietf_netconf::CopyConfigRpc rpc{};
    Empty e;
    e.set = true;
    rpc.input->target->candidate = Empty();
    rpc.input->source->running = e;

    std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);
    bool result = reply == nullptr;
    REQUIRE(result);
}

// issues in netsim
TEST_CASE("es_delete_config_rpc")
{
    // provider
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    ydk::ietf_netconf::DeleteConfigRpc rpc{};

    rpc.input->target->url = "http://test";
    // std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);

    CHECK_THROWS_AS(es.execute_rpc(provider, rpc), YCPPServiceProviderError);
}

TEST_CASE("es_discard_changes_rpc")
{
    // provider
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    ydk::ietf_netconf::DiscardChangesRpc rpc{};

    std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);
    bool result = reply == nullptr;
    REQUIRE(result);
}

// edit_config, get_config -- no option for config/edit_content in ietf_netconf.EditConfigRpc.Input
// TEST_CASE("es_edit_config_rpc")
// {
//     // provider
//     path::Repository repo{TEST_HOME};
//     NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
//     ExecutorService es{};

//     ydk::ietf_netconf::EditConfigRpc edit_config_rpc{};

//     openconfig_bgp::Bgp filter = {};
//     openconfig_bgp::Bgp bgp = {};
//     bgp.global->config->as = 6500;

//     edit_config_rpc.input->target->candidate = Empty();
//     edit_config_rpc.input->edit_content->config = bgp;

//     std::shared_ptr<Entity> reply = es.execute_rpc(provider, edit_config_rpc);
//     REQUIRE(reply);

//     // auto data = ns.get_config(provider, source, filter);
//     // REQUIRE(data);

//     // auto data_ptr = dynamic_cast<openconfig_bgp::Bgp*>(data.get());
//     // REQUIRE(data_ptr != nullptr);
//     // REQUIRE(data_ptr->global->config->as == bgp.global->config->as);

//     // reply = ns.discard_changes(provider);
//     // REQUIRE(reply);
// }

// get -- no option for filter in ietf_netconf.GetRpc.Input
// TEST_CASE("es_get_rpc")
// {
//     // provider
//     path::Repository repo{TEST_HOME};
//     NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
//     ExecutorService es{};
//     openconfig_bgp::Bgp filter = {};

//     ydk::ietf_netconf::GetRpc rpc{};
//     rpc.input->filter = filter

//     std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);
//     REQUIRE(reply);
// }

// kill_session
TEST_CASE("es_kill_session_rpc")
{
    // provider
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    ydk::ietf_netconf::KillSessionRpc rpc{};
    rpc.input->session_id = 3;

   // std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);
    CHECK_THROWS_AS(es.execute_rpc(provider, rpc), YCPPServiceProviderError);
}

// lock, unlock
TEST_CASE("es_lock_rpc")
{
    // provider
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    ydk::ietf_netconf::LockRpc lock_rpc{};
    lock_rpc.input->target->candidate = Empty();

    std::shared_ptr<Entity> reply = es.execute_rpc(provider, lock_rpc);
    bool result = reply == nullptr;
    REQUIRE(result);

    ydk::ietf_netconf::UnlockRpc unlock_rpc{};
    unlock_rpc.input->target->candidate = Empty();

    reply = es.execute_rpc(provider, unlock_rpc);
    result = reply == nullptr;
    REQUIRE(result);
}

TEST_CASE("es_validate_rpc_1")
{
    path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    auto r_1 = make_unique<ydktest_sanity::Runner>();
    ydk::ietf_netconf::ValidateRpc rpc{};
    rpc.input->source->candidate = Empty();
    // rpc.input->source->config = r_1;
    std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);
    bool result = reply == nullptr;
    REQUIRE(result);
}

TEST_CASE("es_validate_rpc_2")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    ExecutorService es{};

    auto r_1 = make_unique<ydktest_sanity::Runner>();
    ydk::ietf_netconf::ValidateRpc rpc{};
    Empty e;
    e.set = true;
    rpc.input->source->running = e;
    std::shared_ptr<Entity> reply = es.execute_rpc(provider, rpc);
    bool result = reply == nullptr;
    REQUIRE(result);
}

// TEST_CASE("es_validate_rpc_3")
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

// TEST_CASE("es_validate_rpc_4")
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

// TEST_CASE("es_validate_rpc_5")
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

