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

#include "ydk/path_api.hpp"
#include "ydk/netconf_provider.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk/types.hpp"
#include "ydk/validation_service.hpp"
#include "config.hpp"

//test_sanity_types begin

BOOST_AUTO_TEST_CASE( test_sanity_types_int8 )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number8 = static_cast<int8_t>(0);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_int16 )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number16 = static_cast<int16_t>(126);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_int32 )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number32 = 200000;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_int64 )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.number64 = -922337203685477580LL;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint8 )
{
     ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number8 =  static_cast<uint8_t>(0);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );
}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint16 )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number16 = static_cast<uint16_t>(65535);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint32 )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number32 = static_cast<uint32_t>(5927);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );
}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint64 )
{

    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.u_number64 = 18446744073709551615ULL;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );
}


BOOST_AUTO_TEST_CASE( bits )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.bits_value["disable-nagle"] = true;
    builtInT.bits_value["auto-sense-speed"] = true;
    auto r = builtInT.get_entity_path(nullptr);
    BOOST_TEST_MESSAGE(r.path);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);
    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_decimal64 )
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.deci64 = ydk::Decimal64("3.2");

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);
    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_string)
{
    ydk::path::Repository repo{TEST_HOME};

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
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.emptee = empty;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_boolean)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.bool_value = true;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );
}


BOOST_AUTO_TEST_CASE( test_sanity_types_embedded_enum)
{

    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.embeded_enum = ydk::ydktest_sanity::Runner::Ytypes::BuiltInT::EmbededEnumEnum::seven;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_enum)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.enum_value = ydk::ydktest_sanity::YdkEnumTestEnum::none;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_union)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.younion = ydk::ydktest_sanity::YdkEnumTestEnum::none;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_union_enum)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.enum_int_value = ydk::ydktest_sanity::YdkEnumIntTestEnum::any;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );
}

BOOST_AUTO_TEST_CASE( test_sanity_types_union_int)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.enum_int_value = 2;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}


BOOST_AUTO_TEST_CASE( test_union_leaflist)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};
    builtInT.llunion.append( static_cast<uint16_t>(1));
    builtInT.llunion.append( static_cast<uint16_t>(2));

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );

}

BOOST_AUTO_TEST_CASE( test_enum_leaflist)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.enum_llist.append(ydk::ydktest_sanity::YdkEnumTestEnum::local);
    builtInT.enum_llist.append(ydk::ydktest_sanity::YdkEnumTestEnum::remote);

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);


    BOOST_REQUIRE( !diagnostic.has_errors() );
}

BOOST_AUTO_TEST_CASE( test_identity_leaflist)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.identity_llist.append(ydk::ydktest_sanity::ChildIdentityIdentity{});
    builtInT.identity_llist.append(ydk::ydktest_sanity::ChildChildIdentityIdentity{});

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}


BOOST_AUTO_TEST_CASE( test_union_complex_list)
{

    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    builtInT.younion_list.append("123:45");

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}


BOOST_AUTO_TEST_CASE( test_identityref)
{
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};

    ydk::ValidationService validation_service{};

    ydk::ydktest_sanity::Runner::Ytypes::BuiltInT builtInT{};

    auto identity = ydk::ydktest_sanity::ChildChildIdentityIdentity{};
    builtInT.identity_ref_value = identity;

    auto diagnostic = validation_service.validate(sp, builtInT, ydk::ValidationService::Option::EDIT_CONFIG);

    BOOST_REQUIRE( !diagnostic.has_errors() );

}

