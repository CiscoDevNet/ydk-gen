/// YANG Development Kit
// Copyright 2017 Cisco Systems. All rights reserved
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

#include <string>
#include <ydk/codec_provider.hpp>
#include <ydk/codec_service.hpp>
#include <ydk_ydktest/ietf_aug_base_1.hpp>
#include "ydk/path_api.hpp"
#include "config.hpp"
#include "catch.hpp"

using namespace ydk;

std::string AUGMENTED_XML_PAYLOAD = R"(<cpython xmlns="http://cisco.com/ns/yang/ietf-aug-base-1">
  <doc>
    <aug-5-identityref xmlns:yaug-five="http://cisco.com/ns/yang/yaug-five">yaug-five:derived-aug-identity</aug-5-identityref>
    <disutils>
      <four-aug-list xmlns="http://cisco.com/ns/yang/yaug-four">
        <enabled>true</enabled>
      </four-aug-list>
    </disutils>
    <ydktest-aug-4 xmlns="http://cisco.com/ns/yang/yaug-four">
      <aug-four>aug four</aug-four>
    </ydktest-aug-4>
    <ydktest-aug-1 xmlns="http://cisco.com/ns/yang/yaug-one">
      <aug-one>aug one</aug-one>
    </ydktest-aug-1>
    <ydktest-aug-2 xmlns="http://cisco.com/ns/yang/yaug-two">
      <aug-two>aug two</aug-two>
    </ydktest-aug-2>
  </doc>
  <lib>
    <ydktest-aug-4 xmlns="http://cisco.com/ns/yang/yaug-four">
      <ydktest-aug-nested-4>
        <aug-four>aug four</aug-four>
      </ydktest-aug-nested-4>
    </ydktest-aug-4>
    <ydktest-aug-1 xmlns="http://cisco.com/ns/yang/yaug-one">
      <ydktest-aug-nested-1>
        <aug-one>aug one</aug-one>
      </ydktest-aug-nested-1>
    </ydktest-aug-1>
    <ydktest-aug-2 xmlns="http://cisco.com/ns/yang/yaug-two">
      <ydktest-aug-nested-2>
        <aug-two>aug two</aug-two>
      </ydktest-aug-nested-2>
    </ydktest-aug-2>
  </lib>
</cpython>)";

std::string AUGMENTED_JSON_PAYLOAD = R"({
  "ietf-aug-base-1:cpython": {
    "doc": {
      "disutils": {
        "ydktest-aug-ietf-4:four-aug-list": {
          "enabled": true
        }
      },
      "ydktest-aug-ietf-4:ydktest-aug-4": {
        "aug-four": "aug four"
      },
      "ydktest-aug-ietf-1:ydktest-aug-1": {
        "aug-one": "aug one"
      },
      "ydktest-aug-ietf-2:ydktest-aug-2": {
        "aug-two": "aug two"
      },
      "ydktest-aug-ietf-5:aug-5-identityref": "ydktest-aug-ietf-5:derived-aug-identity"
    },
    "lib": {
      "ydktest-aug-ietf-4:ydktest-aug-4": {
        "ydktest-aug-nested-4": {
          "aug-four": "aug four"
        }
      },
      "ydktest-aug-ietf-1:ydktest-aug-1": {
        "ydktest-aug-nested-1": {
          "aug-one": "aug one"
        }
      },
      "ydktest-aug-ietf-2:ydktest-aug-2": {
        "ydktest-aug-nested-2": {
          "aug-two": "aug two"
        }
      }
    }
  }
}
)";

TEST_CASE("on_demand_loading_json")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& root_schema = session.get_root_schema();
    ydk::path::Codec codec{};

    auto cpython = codec.decode(root_schema, AUGMENTED_JSON_PAYLOAD, ydk::EncodingFormat::JSON);
    REQUIRE(cpython);
}

TEST_CASE("on_demand_loading_xml")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& root_schema = session.get_root_schema();
    ydk::path::Codec codec{};

    auto cpython = codec.decode(root_schema, AUGMENTED_XML_PAYLOAD, ydk::EncodingFormat::XML);
    REQUIRE(cpython);
}

TEST_CASE("on_demand_provider_loading_xml")
{
    CodecServiceProvider codec_provider{EncodingFormat::XML};
    CodecService codec_service{};
    auto cpython = std::make_shared<ydktest::ietf_aug_base_1::Cpython>();
    codec_service.decode(codec_provider, AUGMENTED_XML_PAYLOAD, cpython);
}

TEST_CASE( "native_tunnel_create" )
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::path::NetconfSession session{repo, "127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = session.get_root_schema();

    auto & native = schema.create_datanode("ydktest-sanity:native");

    auto & tunnelInt = native.create_datanode("interface/Tunnel[name='521']");

    auto & tunnel = tunnelInt.create_datanode("ydktest-sanity-augm:tunnel");

    auto & source = tunnel.create_datanode("source", "Lo222");

    auto & destination = tunnel.create_datanode("destination", "2.3.4.5");

    ydk::path::Codec s{};

    auto xml = s.encode(native, ydk::EncodingFormat::XML, false);

    std::cout << xml << std::endl;
}
