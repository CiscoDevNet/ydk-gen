/// YANG Development Kit
// Copyright 2019 Cisco Systems. All rights reserved
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
#include <string>

#include "ydk/path_api.hpp"
#include "ydk/netconf_provider.hpp"
#include "ydk/crud_service.hpp"
#include "ydk/codec_service.hpp"
#include "ydk/codec_provider.hpp"
#include "ydk/filters.hpp"
#include "ydk/json_subtree_codec.hpp"

#include "ydk_ydktest/openconfig_bgp.hpp"
#include "ydk_ydktest/openconfig_bgp_types.hpp"
#include "ydk_ydktest/openconfig_routing_policy.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk_ydktest/ydktest_sanity_types.hpp"


#include "config.hpp"
#include "catch.hpp"
#include "test_utils.hpp"

using namespace ydk;
using namespace ydktest;
using namespace std;

TEST_CASE("json_codec_leaf")
{
    JsonSubtreeCodec jcodec{};
    NetconfServiceProvider provider("127.0.0.1", "admin", "admin", 12022);
    auto & session = provider.get_session();

    auto runner = ydktest_sanity::Runner();
    auto json = jcodec.encode(runner, session.get_root_schema(), false);
    REQUIRE(json == "{\"ydktest-sanity:runner\":null}");

    runner.ytypes->built_in_t->number8 = 10;
    runner.ytypes->built_in_t->number16 = 102;
    json = jcodec.encode(runner, session.get_root_schema(), false);
    REQUIRE(json == R"({"ydktest-sanity:runner":{"ytypes":{"built-in-t":{"number16":102,"number8":10}}}})");

    auto top_entity = make_shared<ydktest_sanity::Runner>();
    auto entity = jcodec.decode(json, top_entity);
    auto runner_d = dynamic_cast<ydktest_sanity::Runner*>(entity.get());
    REQUIRE(*runner_d == runner);
}


TEST_CASE("json_codec_leaf_list")
{
    JsonSubtreeCodec jcodec{};
    NetconfServiceProvider provider("127.0.0.1", "admin", "admin", 12022);
    auto & session = provider.get_session();

    auto runner = ydktest_sanity::Runner();
    runner.ytypes->built_in_t->llstring.append("abc");
    runner.ytypes->built_in_t->llstring.append("klm");
    runner.ytypes->built_in_t->llstring.append("xyz");
    auto json = jcodec.encode(runner, session.get_root_schema(), false);
    REQUIRE(json == R"({"ydktest-sanity:runner":{"ytypes":{"built-in-t":{"llstring":["abc","klm","xyz"]}}}})");

    auto top_entity = make_shared<ydktest_sanity::Runner>();
    auto entity = jcodec.decode(json, top_entity);
    auto runner_d = dynamic_cast<ydktest_sanity::Runner*>(entity.get());
    REQUIRE(*runner_d == runner);
}

TEST_CASE("json_codec_identity")
{
    NetconfServiceProvider provider("127.0.0.1", "admin", "admin", 12022);
    auto & session = provider.get_session();
    JsonSubtreeCodec jcodec{};

    auto bgp = make_unique<openconfig_bgp::Bgp>();
    bgp->global->config->as = 65172;
    bgp->global->config->router_id = "1.2.3.4";

    auto afi_safi = make_shared<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
    afi_safi->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
    afi_safi->config->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
    afi_safi->config->enabled = true;
    bgp->global->afi_safis->afi_safi.append(afi_safi);

    auto json = jcodec.encode(*bgp, session.get_root_schema());
    string expected = R"({
  "openconfig-bgp:bgp": {
    "global": {
      "afi-safis": {
        "afi-safi": [
          {
            "afi-safi-name": "openconfig-bgp-types:L3VPN_IPV4_UNICAST",
            "config": {
              "afi-safi-name": "openconfig-bgp-types:L3VPN_IPV4_UNICAST",
              "enabled": true
            }
          }
        ]
      },
      "config": {
        "as": 65172,
        "router-id": "1.2.3.4"
      }
    }
  }
})";
    REQUIRE(json == expected);

    auto top_entity = make_shared<openconfig_bgp::Bgp>();
    auto entity = jcodec.decode(json, top_entity);
    auto bgp_d = dynamic_cast<openconfig_bgp::Bgp*>(entity.get());
    REQUIRE(*bgp_d == *bgp);
}

TEST_CASE("json_codec_identity_subtree")
{
    NetconfServiceProvider provider("127.0.0.1", "admin", "admin", 12022);
    auto & session = provider.get_session();
    JsonSubtreeCodec jcodec{};

    auto afi_safi = make_shared<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
    afi_safi->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
    afi_safi->config->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
    afi_safi->config->enabled = true;

    auto json = jcodec.encode(*afi_safi, session.get_root_schema());
    string expected = R"({
  "openconfig-bgp:afi-safi": {
    "afi-safi-name": "openconfig-bgp-types:L3VPN_IPV4_UNICAST",
    "config": {
      "afi-safi-name": "openconfig-bgp-types:L3VPN_IPV4_UNICAST",
      "enabled": true
    }
  }
})";
    REQUIRE(json == expected);

    auto top_entity = make_shared<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
    auto entity = jcodec.decode(json, top_entity);
    auto afi_safi_d = dynamic_cast<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi*>(entity.get());
    REQUIRE(*afi_safi_d == *afi_safi);
}

TEST_CASE("json_codec_no_key_list")
{
    NetconfServiceProvider provider("127.0.0.1", "admin", "admin", 12022);
    auto & session = provider.get_session();
    JsonSubtreeCodec jcodec{};

    auto runner = ydktest_sanity::Runner();
    auto t1 = make_shared<ydktest_sanity::Runner::NoKeyList>(); t1->test = "abc";
    auto t2 = make_shared<ydktest_sanity::Runner::NoKeyList>(); t2->test = "xyz";
    runner.no_key_list.extend( { t1, t2 });

    auto json = jcodec.encode(runner, session.get_root_schema(), false);
    REQUIRE(json == R"({"ydktest-sanity:runner":{"no-key-list":[{"test":"abc"},{"test":"xyz"}]}})");

    auto top_entity = make_shared<ydktest_sanity::Runner>();
    auto entity = jcodec.decode(json, top_entity);
    auto runner_d = dynamic_cast<ydktest_sanity::Runner*>(entity.get());
    REQUIRE(*runner_d == runner);
}

TEST_CASE("json_codec_augment_presence")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::path::NetconfSession session{repo, "127.0.0.1", "admin", "admin", 12022};

    auto i = make_shared<ydktest_sanity::Runner::Passive::Interfac>();
    i->test = "abc";

    auto passive = make_shared<ydktest_sanity::Runner::Passive>();
    passive->name = "xyz";
    passive->interfac.append(i);

    auto r_1 = make_shared<ydktest_sanity::Runner>();
    r_1->passive.append(passive);

    passive->testc->xyz = make_shared<ydktest_sanity::Runner::Passive::Testc::Xyz>();
    passive->testc->xyz->parent = passive.get();
    passive->testc->xyz->xyz = 25;

    CodecServiceProvider codec_provider{EncodingFormat::JSON};
    CodecService codec_service{};
    auto jsonC = codec_service.encode(codec_provider, *r_1, false);
    auto cs_runner = codec_service.decode(codec_provider, jsonC, make_shared<ydktest_sanity::Runner>());
    REQUIRE(*r_1 == *cs_runner);

    JsonSubtreeCodec json_codec{};
    auto jsonJ = json_codec.encode(*r_1, session.get_root_schema(), false);
    auto runner = json_codec.decode(jsonC, make_shared<ydktest_sanity::Runner>());
    REQUIRE(*r_1 == *runner);
}
