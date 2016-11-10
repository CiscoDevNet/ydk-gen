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

#define BOOST_TEST_MODULE CodecTests

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <string.h>

#include "ydk/codec_provider.hpp"
#include "ydk/codec_service.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk_ydktest/ydktest_sanity_types.hpp"
#include "ydk_ydktest/oc_pattern.hpp"

#include "config.hpp"

using namespace ydk;

std::string XML_OC_PATTERN_PAYLOAD = R"(<oc-A xmlns="http://cisco.com/ns/yang/oc-pattern">
  <a>Hello</a>
</oc-A>
)";

std::string XML_ENUM_PAYLOAD_2 = R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <ytypes>
    <built-in-t>
      <enum-value>local</enum-value>
    </built-in-t>
  </ytypes>
</runner>
)";

std::string XML_RUNNER_PAYLOAD_1 = R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <two-list>
    <ldata>
      <number>11</number>
      <name>l11name</name>
      <subl1>
        <number>111</number>
        <name>s111name</name>
      </subl1>
      <subl1>
        <number>112</number>
        <name>s112name</name>
      </subl1>
    </ldata>
    <ldata>
      <number>12</number>
      <name>l12name</name>
      <subl1>
        <number>121</number>
        <name>s121name</name>
      </subl1>
      <subl1>
        <number>122</number>
        <name>s122name</name>
      </subl1>
    </ldata>
  </two-list>
</runner>
)";

std::string XML_RUNNER_PAYLOAD_2 = R"(<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <two-list>
    <ldata>
      <number>21</number>
      <name>modified</name>
      <subl1>
        <number>211</number>
        <name>s211name</name>
      </subl1>
      <subl1>
        <number>212</number>
        <name>s212name</name>
      </subl1>
    </ldata>
    <ldata>
      <number>22</number>
      <name>l22name</name>
      <subl1>
        <number>221</number>
        <name>s221name</name>
      </subl1>
      <subl1>
        <number>222</number>
        <name>s222name</name>
      </subl1>
    </ldata>
  </two-list>
</runner>
)";

std::string JSON_RUNNER_PAYLOAD_1 = R"({
  "ydktest-sanity:runner": {
    "two-list": {
      "ldata": [
        {
          "number": 11,
          "name": "l11name",
          "subl1": [
            {
              "number": 111,
              "name": "s111name"
            },
            {
              "number": 112,
              "name": "s112name"
            }
          ]
        },
        {
          "number": 12,
          "name": "l12name",
          "subl1": [
            {
              "number": 121,
              "name": "s121name"
            },
            {
              "number": 122,
              "name": "s122name"
            }
          ]
        }
      ]
    }
  }
}
)";

std::string JSON_RUNNER_PAYLOAD_2 = R"({
  "ydktest-sanity:runner": {
    "two-list": {
      "ldata": [
        {
          "number": 21,
          "name": "modified",
          "subl1": [
            {
              "number": 211,
              "name": "s211name"
            },
            {
              "number": 212,
              "name": "s212name"
            }
          ]
        },
        {
          "number": 22,
          "name": "l22name",
          "subl1": [
            {
              "number": 221,
              "name": "s221name"
            },
            {
              "number": 222,
              "name": "s222name"
            }
          ]
        }
      ]
    }
  }
}
)";

void
config_runner_2(ydktest_sanity::Runner *runner)
{
    auto l_1 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata>();
    auto l_2 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata>();
    auto s_11 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();
    auto s_12 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();
    auto s_21 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();
    auto s_22 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();

    l_1->number = 21;
    l_1->name = "l21name";
    l_1->parent = runner->two_list.get();
    l_2->number = 22;
    l_2->name = "l22name";
    l_2->parent = runner->two_list.get();
    s_11->number = 211;
    s_11->name = "s211name";
    s_11->parent = l_1.get();
    s_12->number = 212;
    s_12->name = "s212name";
    s_12->parent = l_1.get();
    s_21->number = 221;
    s_21->name = "s221name";
    s_21->parent = l_2.get();
    s_22->number = 222;
    s_22->name = "s222name";
    s_22->parent = l_2.get();

    l_1->subl1.push_back(std::move(s_11));
    l_1->subl1.push_back(std::move(s_12));
    l_2->subl1.push_back(std::move(s_21));
    l_2->subl1.push_back(std::move(s_22));

    runner->two_list->ldata.push_back(std::move(l_1));
    runner->two_list->ldata.push_back(std::move(l_2));
}

void
config_runner_1(ydktest_sanity::Runner *runner)
{
    auto l_1 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata>();
    auto l_2 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata>();
    auto s_11 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();
    auto s_12 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();
    auto s_21 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();
    auto s_22 = std::make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();

    l_1->number = 11;
    l_1->name = "l11name";
    l_1->parent = runner->two_list.get();
    l_2->number = 12;
    l_2->name = "l12name";
    l_2->parent = runner->two_list.get();
    s_11->number = 111;
    s_11->name = "s111name";
    s_11->parent = l_1.get();
    s_12->number = 112;
    s_12->name = "s112name";
    s_12->parent = l_1.get();
    s_21->number = 121;
    s_21->name = "s121name";
    s_21->parent = l_2.get();
    s_22->number = 122;
    s_22->name = "s122name";
    s_22->parent = l_2.get();

    l_1->subl1.push_back(std::move(s_11));
    l_1->subl1.push_back(std::move(s_12));
    l_2->subl1.push_back(std::move(s_21));
    l_2->subl1.push_back(std::move(s_22));

    runner->two_list->ldata.push_back(std::move(l_1));
    runner->two_list->ldata.push_back(std::move(l_2));
}

BOOST_AUTO_TEST_CASE(single_encode)
{
	path::Repository repo{TEST_HOME};
    CodecServiceProvider codec_provider{&repo,EncodingFormat::XML};

    CodecService codec_service{};

    auto runner = std::make_unique<ydktest_sanity::Runner>();

    config_runner_1(runner.get());

    std::string xml = codec_service.encode(codec_provider, *runner, true);
    std::cout << xml << std::endl;
}

BOOST_AUTO_TEST_CASE(multiple_encode)
{
    path::Repository repo{TEST_HOME};
    CodecServiceProvider codec_provider{&repo,EncodingFormat::XML};
    CodecService codec_service{};

    auto runner1 = std::make_unique<ydktest_sanity::Runner>();
    auto runner2 = std::make_unique<ydktest_sanity::Runner>();

    config_runner_1(runner1.get());
    config_runner_2(runner2.get());

    runner2->two_list->ldata[0]->name = "modified";

    std::map<std::string, std::unique_ptr<Entity>> entity_map;
    entity_map["runner1"] = std::move(runner1);
    entity_map["runner2"] = std::move(runner2);

    std::map<std::string, std::string> payload_map = codec_service.encode(codec_provider, entity_map, true);

    BOOST_CHECK_EQUAL(payload_map["runner1"], XML_RUNNER_PAYLOAD_1);
    BOOST_CHECK_EQUAL(payload_map["runner2"], XML_RUNNER_PAYLOAD_2);
}

BOOST_AUTO_TEST_CASE(test_oc_pattern)
{
    path::Repository repo{TEST_HOME};
    CodecServiceProvider codec_provider{&repo,EncodingFormat::XML};
    CodecService codec_service{};

    auto entity = codec_service.decode(codec_provider, XML_OC_PATTERN_PAYLOAD);

    oc_pattern::OcA * entity_ptr = dynamic_cast<oc_pattern::OcA*>(entity.get());
    BOOST_CHECK_EQUAL(entity_ptr->a.get(), "Hello");
}

BOOST_AUTO_TEST_CASE(enum_2)
{
    path::Repository repo{TEST_HOME};
    CodecServiceProvider codec_provider{&repo,EncodingFormat::XML};
    CodecService codec_service{};

    auto entity = codec_service.decode(codec_provider, XML_ENUM_PAYLOAD_2);
    ydktest_sanity::Runner * entity_ptr = dynamic_cast<ydktest_sanity::Runner*>(entity.get());
    BOOST_CHECK_EQUAL(entity_ptr->ytypes->built_in_t->enum_value.get(), "local");
}


BOOST_AUTO_TEST_CASE(encode_decode)
{
//     path::Repository repo{TEST_HOME};
//     CodecServiceProvider codec_provider{&repo,EncodingFormat::XML};
//     CodecService codec_service{};
//
//     auto entity = codec_service.decode(codec_provider, XML_RUNNER_PAYLOAD_1);
//     std::string json = codec_service.encode(codec_provider, *entity);
//
//     std::cout << json << std::endl;
//     BOOST_CHECK_EQUAL(json,JSON_RUNNER_PAYLOAD_1);
}

BOOST_AUTO_TEST_CASE(single_decode)
{
    path::Repository repo{TEST_HOME};
    CodecServiceProvider codec_provider{&repo,EncodingFormat::JSON};
    CodecService codec_service{};

    auto entity = codec_service.decode(codec_provider, JSON_RUNNER_PAYLOAD_1);
    ydktest_sanity::Runner * entity_ptr = dynamic_cast<ydktest_sanity::Runner*>(entity.get());

    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[0]->number.get(), "11");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[0]->name.get(), "l11name");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[0]->subl1[0]->number.get(), "111");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[0]->subl1[0]->name.get(), "s111name");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[0]->subl1[1]->number.get(), "112");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[0]->subl1[1]->name.get(), "s112name");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[1]->number.get(), "12");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[1]->name.get(), "l12name");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[1]->subl1[0]->number.get(), "121");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[1]->subl1[0]->name.get(), "s121name");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[1]->subl1[1]->number.get(), "122");
    BOOST_CHECK_EQUAL(entity_ptr->two_list->ldata[1]->subl1[1]->name.get(), "s122name");
}
