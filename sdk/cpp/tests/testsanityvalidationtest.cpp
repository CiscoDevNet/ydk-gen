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

#include <iostream>

#include "ydk/entity_util.hpp"
#include "ydk/path_api.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk/types.hpp"
#include "ydk/validation_service.hpp"
#include "config.hpp"
#include "catch.hpp"

using namespace ydktest;

TEST_CASE("validation_int8 ")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo, "127.0.0.1", "admin", "admin", 12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number8 = static_cast<int8_t>(0);

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));




}

TEST_CASE("validation_int16 ")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number16 = static_cast<int16_t>(126);

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}

TEST_CASE("validation_int32 ")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number32 = 200000;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}

TEST_CASE("validation_int64 ")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number64 = -922337203685477580LL;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}

TEST_CASE("validation_uint8 ")
{
     ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number8 =  static_cast<uint8_t>(0);

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}

TEST_CASE("validation_uint16 ")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number16 = static_cast<uint16_t>(65535);

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));




}

TEST_CASE("validation_uint32 ")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number32 = static_cast<uint32_t>(5927);

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));


}

TEST_CASE("validation_uint64 ")
{

    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number64 = 18446744073709551615ULL;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));


}


TEST_CASE("validation_bits ")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.bits_value["disable-nagle"] = true;
    builtInT.bits_value["auto-sense-speed"] = true;
    auto r = get_entity_path(builtInT, nullptr);
    INFO(r.path);

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));


}

TEST_CASE("validation_decimal64 ")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.deci64 = ydk::Decimal64("3.12");

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));


}

TEST_CASE("validation_string")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.name = "name_str";

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));




}

TEST_CASE("validation_empty")
{
    ydk::Empty empty{};
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.emptee = empty;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));




}

TEST_CASE("validation_boolean")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.bool_value = true;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));


}


TEST_CASE("validation_embedded_enum")
{

    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.embeded_enum = ydktest_sanity::Runner::Ytypes::BuiltInT::EmbededEnum::seven;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));





}

TEST_CASE("validation_enum")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.enum_value = ydktest_sanity::YdkEnumTest::none;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));





}

TEST_CASE("validation_union")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.younion = ydktest_sanity::YdkEnumTest::none;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));





}

TEST_CASE("validation_union_enum")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.enum_int_value = ydktest_sanity::YdkEnumIntTest::any;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}

TEST_CASE("validation_union_int")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.enum_int_value = 2;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));




}


TEST_CASE("validation_test_v_union_leaflist")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.llunion.append( static_cast<uint16_t>(1));
    builtInT.llunion.append( static_cast<uint16_t>(2));

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));




}

TEST_CASE("validation_test_v_enum_leaflist")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.enum_llist.append(ydktest_sanity::YdkEnumTest::local);
    builtInT.enum_llist.append(ydktest_sanity::YdkEnumTest::remote);

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}

TEST_CASE("validation_test_v_identity_leaflist")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.identity_llist.append(ydktest_sanity::ChildIdentity{});
    builtInT.identity_llist.append(ydktest_sanity::ChildChildIdentity{});

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}


TEST_CASE("validation_test_v_union_complex_list")
{

    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.younion_list.append("123:45");

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}


TEST_CASE("validation_test_v_identityref")
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::path::NetconfSession session{repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    auto identity = ydktest_sanity::ChildChildIdentity{};
    builtInT.identity_ref_value = identity;

    CHECK_NOTHROW(validation_service.validate(session, builtInT, ydk::ValidationService::Option::EDIT_CONFIG));



}
