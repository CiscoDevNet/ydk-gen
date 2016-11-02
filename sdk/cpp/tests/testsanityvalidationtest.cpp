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

#define BOOST_TEST_MODULE OCBgpTest
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "ydk/core.hpp"
#include "ydk/netconf_provider.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk/types.hpp"
#include "ydk/validation_service.hpp"
#include "config.hpp"

//test_sanity_types begin

BOOST_AUTO_TEST_CASE( test_sanity_types_int8 )
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number8 = static_cast<int8_t>(0);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_int16 )
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number16 = static_cast<int16_t>(126);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_int32 )
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number32 = 200000;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}
/*
BOOST_AUTO_TEST_CASE( test_sanity_types_int64 )
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number64 = static_cast<int64_t>(-922337203685477580LL);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}
*/
BOOST_AUTO_TEST_CASE( test_sanity_types_uint8 )
{
     ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number8 =  static_cast<uint8_t>(0);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );
}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint16 )
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number16 = static_cast<uint16_t>(65535);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint32 )
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number32 = static_cast<uint32_t>(5927);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );
}
/*
BOOST_AUTO_TEST_CASE( test_sanity_types_uint64 )
{

    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number64 = static_cast<uint64_t>(18446744073709551615ULL);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );
}
*/

BOOST_AUTO_TEST_CASE( test_sanity_types_bits )
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.bits_value["disable-nagle"] = true;
    auto r = builtInT.get_entity_path(nullptr);
    BOOST_TEST_MESSAGE(r.path);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);
    //BOOST_REQUIRE( !diagnostic.has_errors() ); //TODO: possible bug in core not able to validate bits

}

BOOST_AUTO_TEST_CASE( test_sanity_types_decimal64 )
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.deci64 = "3.2";
    auto r = builtInT.get_entity_path(nullptr);
    BOOST_TEST_MESSAGE(r.path);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);
    BOOST_REQUIRE( !diagnostic.has_errors() ); //TODO: possible bug in core not able to validate bits

}

BOOST_AUTO_TEST_CASE( test_sanity_types_string)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.name = "name_str";

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_empty)
{
    ydk::Empty empty{};
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.emptee = empty;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_boolean)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.bool_value = true;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );
}


BOOST_AUTO_TEST_CASE( test_sanity_types_embedded_enum)
{

    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.embeded_enum = ydk::ydktest_sanity::Runner::Ytypes::BuiltInT::EmbededEnumEnum::seven;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_enum)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.enum_value = ydk::ydktest_sanity::YdkEnumTestEnum::none;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_union)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.younion = ydk::ydktest_sanity::YdkEnumTestEnum::none;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_union_enum)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.enum_int_value = ydk::ydktest_sanity::YdkEnumIntTestEnum::any;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );
}

BOOST_AUTO_TEST_CASE( test_sanity_types_union_int)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.enum_int_value = 2;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}


BOOST_AUTO_TEST_CASE( test_union_leaflist)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    ydk::Value value1{ydk::YType::int16, "llunion"};
    value1 = (ydk::int16)1;

    ydk::Value value2{ydk::YType::int16, "llunion"};
    value2 = (ydk::int16)2;

    builtInT.llunion.append(value1);
    builtInT.llunion.append(value2);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_enum_leaflist)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    ydk::Value value1{ydk::YType::enumeration, "enum-llist"};
    value1 = ydk::ydktest_sanity::YdkEnumTestEnum::local;
    builtInT.enum_llist.append(value1);


    ydk::Value value2{ydk::YType::enumeration, "enum-llist"};
    value2 = ydk::ydktest_sanity::YdkEnumTestEnum::remote;
    builtInT.enum_llist.append(value2);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );
}

BOOST_AUTO_TEST_CASE( test_identity_leaflist)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    ydk::Value value1{ydk::YType::identityref, "identity-llist"};
    value1 = ydk::ydktest_sanity::ChildIdentityIdentity{};

    ydk::Value value2{ydk::YType::identityref, "identity-llist"};
    value2 = ydk::ydktest_sanity::ChildChildIdentityIdentity{};

    builtInT.identity_llist.append(value1);
    builtInT.identity_llist.append(value2);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}


BOOST_AUTO_TEST_CASE( test_union_complex_list)
{

    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    ydk::Value value{ydk::YType::str, "younion-list"};
    value = "123:45";

    builtInT.younion_list.append(value);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}


BOOST_AUTO_TEST_CASE( test_identityref)
{
    ydk::core::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    auto identity = ydk::ydktest_sanity::ChildChildIdentityIdentity{};
    builtInT.identity_ref_value = identity;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}

