/*  ----------------------------------------------------------------
 YDK - YANG Development Kit
 Copyright 2016 Cisco Systems. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 -------------------------------------------------------------------
 This file has been modified by Yan Gorelik, YDK Solutions.
 All modifications in original under CiscoDevNet domain
 introduced since October 2019 are copyrighted.
 All rights reserved under Apache License, Version 2.0.
 ------------------------------------------------------------------*/

#include <iostream>
#include <string>

#include "ydk/path_api.hpp"
#include "ydk/netconf_provider.hpp"
#include "ydk/crud_service.hpp"
#include "ydk/codec_service.hpp"
#include "ydk/codec_provider.hpp"
#include "ydk/xml_subtree_codec.hpp"
#include "ydk/filters.hpp"
#include "ydk/common_utilities.hpp"

#include "ydk_ydktest/openconfig_bgp.hpp"
#include "ydk_ydktest/openconfig_bgp_types.hpp"
#include "ydk_ydktest/openconfig_routing_policy.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk_ydktest/ydktest_sanity_types.hpp"

#include "config.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace ydktest;
using namespace std;

TEST_CASE("subtree_encode_leaf")
{
CodecService c{};
CodecServiceProvider cp{EncodingFormat::XML};

ydk::path::Repository repo{TEST_HOME};
ydk::NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
const ydk::path::Session& session = provider.get_session();

CrudService crud{};
auto r_1 = make_shared<ydktest_sanity::Runner>();
bool result = crud.delete_(provider, *r_1);
REQUIRE(result);

XmlSubtreeCodec codec{};
r_1->ytypes->built_in_t->number16 = 102;

auto s = codec.encode(*r_1, session.get_root_schema());
REQUIRE(s == R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <ytypes>
    <built-in-t>
      <number16>102</number16>
    </built-in-t>
  </ytypes>
</runner>)");

auto s1 = c.encode(cp, *r_1, true, true);
REQUIRE(s1==s);

auto r = make_shared<ydktest_sanity::Runner>();
auto g = codec.decode(s, r);
result = crud.create(provider, *g);
REQUIRE(result);

result = crud.delete_(provider, *r);
REQUIRE(result);
}

TEST_CASE("subtree_encode_yfilter_delete")
{
ydk::path::Repository repo{TEST_HOME};
NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
const path::Session& session = provider.get_session();

CrudService crud{};

XmlSubtreeCodec codec{};
auto r_1 = make_shared<ydktest_sanity::Runner>();
bool result = crud.delete_(provider, *r_1);
REQUIRE(result);

auto ld = make_shared<ydktest_sanity::Runner::OneList::Ldata>();
ld->name = "xyz";
ld->number = YFilter::delete_;
r_1->one_list->ldata.append(ld);

auto s = codec.encode(*r_1, session.get_root_schema());
REQUIRE(s == R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <one-list>
    <ldata>
      <number operation="delete"/>
      <name>xyz</name>
    </ldata>
  </one-list>
</runner>)");

}

TEST_CASE("subtree_encode_yfilter_read")
{
ydk::path::Repository repo{TEST_HOME};
NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
const path::Session& session = provider.get_session();

XmlSubtreeCodec codec{};
auto r_1 = make_shared<ydktest_sanity::Runner>();
r_1->ytypes->built_in_t->number16 = YFilter::read;

auto s = codec.encode(*r_1, session.get_root_schema());
REQUIRE(s == R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <ytypes>
    <built-in-t>
      <number16/>
    </built-in-t>
  </ytypes>
</runner>)");
}

TEST_CASE("subtree_encode_namespaces")
{
ydk::path::Repository repo{TEST_HOME};
NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
const path::Session& session = provider.get_session();

CodecService c{};
CodecServiceProvider cp{EncodingFormat::XML};

XmlSubtreeCodec codec{};
auto pd = make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition>();
pd->name = "xyz";

auto stmt = make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement>();
stmt->actions->bgp_actions->config->set_local_pref = 1233;

auto rp = make_shared<openconfig_routing_policy::RoutingPolicy>();
pd->statements->statement.append(stmt);

rp->policy_definitions->policy_definition.append(pd);

auto s = codec.encode(*rp, session.get_root_schema());
REQUIRE(s==R"(<routing-policy xmlns="http://openconfig.net/yang/routing-policy">
  <policy-definitions>
    <policy-definition>
      <name>xyz</name>
      <statements>
        <statement>
          <actions>
            <bgp-actions xmlns="http://openconfig.net/yang/bgp-policy">
              <config>
                <set-local-pref>1233</set-local-pref>
              </config>
            </bgp-actions>
          </actions>
        </statement>
      </statements>
    </policy-definition>
  </policy-definitions>
</routing-policy>)");

auto s1 = c.encode(cp, *rp, true, true);
REQUIRE(s==s1);
}

TEST_CASE("subtree_encode_identity")
{
ydk::path::Repository repo{TEST_HOME};
NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
const path::Session& session = provider.get_session();

XmlSubtreeCodec codec{};

CodecService c{};
CodecServiceProvider cp{EncodingFormat::XML};

auto bgp = make_unique<openconfig_bgp::Bgp>();
bgp->global->config->as = 65172;
bgp->global->config->router_id = "1.2.3.4";

auto afi_safi = make_shared<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
afi_safi->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
afi_safi->config->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
afi_safi->config->enabled = true;
bgp->global->afi_safis->afi_safi.append(afi_safi);

auto s = codec.encode(*bgp, session.get_root_schema());
REQUIRE(s==R"(<bgp xmlns="http://openconfig.net/yang/bgp">
  <global>
    <afi-safis>
      <afi-safi>
        <afi-safi-name xmlns:openconfig-bgp-types="http://openconfig.net/yang/bgp-types">openconfig-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>
        <config>
          <afi-safi-name xmlns:openconfig-bgp-types="http://openconfig.net/yang/bgp-types">openconfig-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>
          <enabled>true</enabled>
        </config>
      </afi-safi>
    </afi-safis>
    <config>
      <as>65172</as>
      <router-id>1.2.3.4</router-id>
    </config>
  </global>
</bgp>)");

auto z = c.decode(cp, s, make_shared<openconfig_bgp::Bgp>());
REQUIRE(*z==*bgp);

}

TEST_CASE("subtree_decode_crud_read")
{
ydk::path::Repository repo{TEST_HOME};
NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
CrudService crud{};

XmlSubtreeCodec codec{};
auto r_1 = make_shared<ydktest_sanity::Runner>();

bool result = crud.delete_(provider, *r_1);
REQUIRE(result);

auto s = codec.decode(R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
<ytypes>
    <built-in-t>
    <number8>102</number8>
    </built-in-t>
    </ytypes>
</runner>
)", r_1);
result = crud.create(provider, *s);
REQUIRE(result);

auto g = codec.decode(R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
<ytypes>
    <built-in-t>
    <number8></number8>
    </built-in-t>
    </ytypes>
</runner>
)", r_1);
auto r = crud.read(provider, *g);
REQUIRE(r!=nullptr);

REQUIRE(
        ((ydktest_sanity::Runner&)*s).ytypes->built_in_t->number8
            ==
        ((ydktest_sanity::Runner&)*r).ytypes->built_in_t->number8
       );

result = crud.delete_(provider, *r_1);
REQUIRE(result);
}

TEST_CASE("subtree_decode_identity")
{
ydk::path::Repository repo{TEST_HOME};
NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
const path::Session& session = provider.get_session();

CrudService crud{};

XmlSubtreeCodec codec{};
auto bgp = make_shared<openconfig_bgp::Bgp>();

openconfig_bgp::Bgp bgp_set{};
bool reply = crud.delete_(provider, bgp_set);
REQUIRE(reply);

auto payload = R"(<bgp xmlns="http://openconfig.net/yang/bgp">
          <global>
            <afi-safis>
              <afi-safi>
                <afi-safi-name xmlns:oc-bgp-types="http://openconfig.net/yang/bgp-types">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>
                <config>
                  <afi-safi-name xmlns:oc-bgp-types="http://openconfig.net/yang/bgp-types">oc-bgp-types:L3VPN_IPV4_UNICAST</afi-safi-name>
                  <enabled>false</enabled>
                </config>
              </afi-safi>
            </afi-safis>
            <config>
              <as>65172</as>
              <router-id>1.2.3.4</router-id>
            </config>
          </global>
        </bgp>)";

auto g = codec.decode(payload, bgp);
auto s = codec.encode(*g, session.get_root_schema());
auto a = codec.decode(s, make_shared<openconfig_bgp::Bgp>());

crud.create(provider, *a);
auto v=make_shared<openconfig_bgp::Bgp>();
v->global->yfilter = YFilter::read;
auto r = crud.read_config(provider, *v);
REQUIRE(r!=nullptr);
REQUIRE(*r==*g);
}

TEST_CASE("test_no_key_list")
{
std::string payload=R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <no-key-list>
    <test>abc</test>
  </no-key-list>
  <no-key-list>
    <test>xyz</test>
  </no-key-list>
</runner>)";

ydk::path::Repository repo{TEST_HOME};
ydk::path::NetconfSession session{repo, "127.0.0.1", "admin", "admin", 12022};

XmlSubtreeCodec xml_codec{};

auto no_key = xml_codec.decode(payload, make_shared<ydktest_sanity::Runner>());
auto no_key_payload = xml_codec.encode(*no_key, session.get_root_schema());
REQUIRE(payload==no_key_payload);
}

TEST_CASE("xml_codec_augment_presence")
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

    CodecServiceProvider codec_provider{EncodingFormat::XML};
    CodecService codec_service{};
    auto xmlC = codec_service.encode(codec_provider, *r_1, true);
    auto cs_runner = codec_service.decode(codec_provider, xmlC, make_shared<ydktest_sanity::Runner>());
    REQUIRE(*r_1 == *cs_runner);

    XmlSubtreeCodec xml_codec{};
    auto xmlX = xml_codec.encode(*r_1, session.get_root_schema());
    REQUIRE(trim(xmlC) == xmlX);

    auto runner = xml_codec.decode(xmlC, make_shared<ydktest_sanity::Runner>());
    auto xml_runner = xml_codec.encode(*runner, session.get_root_schema());
    REQUIRE(xmlX == xml_runner);
}

static void config_bgp(openconfig_bgp::Bgp & bgp)
{
    bgp.global->config->as = 65172;
    bgp.global->config->router_id = "1.2.3.4";

    auto neighbor = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    neighbor->neighbor_address = "6.7.8.9";
    neighbor->config->neighbor_address = "6.7.8.9";
    neighbor->config->peer_as = 65001;
    neighbor->config->local_as = 65001;
    neighbor->config->peer_group = "IBGP";
    bgp.neighbors->neighbor.append(neighbor);
}

TEST_CASE( "xml_codec_on_user_bundle" )
{
    auto repo = path::Repository{TEST_HOME};
    std::vector<path::Capability> empty_caps;
    auto root = repo.create_root_schema(empty_caps);

    auto bgp = openconfig_bgp::Bgp{};
    config_bgp(bgp);

    XmlSubtreeCodec xml_codec{};
    auto xml_bgp_config = xml_codec.encode(bgp, *root);

    auto expected = R"(<bgp xmlns="http://openconfig.net/yang/bgp">
  <global>
    <config>
      <as>65172</as>
      <router-id>1.2.3.4</router-id>
    </config>
  </global>
  <neighbors>
    <neighbor>
      <neighbor-address>6.7.8.9</neighbor-address>
      <config>
        <peer-group>IBGP</peer-group>
        <neighbor-address>6.7.8.9</neighbor-address>
        <peer-as>65001</peer-as>
        <local-as>65001</local-as>
      </config>
    </neighbor>
  </neighbors>
</bgp>)";
    REQUIRE(expected == xml_bgp_config);
}
