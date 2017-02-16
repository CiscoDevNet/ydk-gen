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
#include "../core/src/path_api.hpp"
#include "config.hpp"
#include "catch.hpp"
#include "../core/src/netconf_provider.hpp"

//test_sanity_types begin

TEST_CASE("test_sanity_types_int8 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & number8 = runner.create("ytypes/built-in-t/number8", "0");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    //find the number8 node
    auto number8_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/number8");

    REQUIRE(!number8_read_vec.empty());

    auto number8_read = number8_read_vec[0];

    REQUIRE(number8.get() == number8_read->get() );


}

TEST_CASE("test_sanity_types_int16 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & number16 = runner.create("ytypes/built-in-t/number16", "126");


    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    //find the number8 node
    auto number16_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/number16");

    REQUIRE(!number16_read_vec.empty());

    auto & number16_read = number16_read_vec[0];

    REQUIRE(number16.get() == number16_read->get() );


}

TEST_CASE("test_sanity_types_int32 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & number32 = runner.create("ytypes/built-in-t/number32", "200000");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto number32_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/number32");

    REQUIRE(!number32_read_vec.empty());

    auto number32_read = number32_read_vec[0];

    REQUIRE(number32.get() == number32_read->get() );

}

TEST_CASE("test_sanity_types_int64 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & number64 = runner.create("ytypes/built-in-t/number64", "-9223372036854775808");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto number64_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/number64");

    REQUIRE(!number64_read_vec.empty());

    auto number64_read = number64_read_vec[0];

    REQUIRE(number64.get() == number64_read->get() );

}

TEST_CASE("test_sanity_types_uint8 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & unumber8 = runner.create("ytypes/built-in-t/u_number8", "0");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto unumber8_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/u_number8");

    REQUIRE(!unumber8_read_vec.empty());

    auto unumber8_read = unumber8_read_vec[0];

    REQUIRE(unumber8.get() == unumber8_read->get() );

}

TEST_CASE("test_sanity_types_uint16 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & unumber16 = runner.create("ytypes/built-in-t/u_number16", "65535");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty()
                        );

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto unumber16_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/u_number16");

    REQUIRE(!unumber16_read_vec.empty());

    auto unumber16_read = unumber16_read_vec[0];

    REQUIRE(unumber16.get() == unumber16_read->get() );

}

TEST_CASE("test_sanity_types_uint32 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & unumber32 = runner.create("ytypes/built-in-t/u_number32", "5927");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty()
                        );


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto unumber32_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/u_number32");

    REQUIRE(!unumber32_read_vec.empty());

    auto unumber32_read = unumber32_read_vec[0];

    REQUIRE(unumber32.get() == unumber32_read->get() );

}

TEST_CASE("test_sanity_types_uint64 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & unumber64 = runner.create("ytypes/built-in-t/u_number64", "18446744073709551615");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto unumber64_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/u_number64");

    REQUIRE(!unumber64_read_vec.empty());

    auto unumber64_read = unumber64_read_vec[0];

    REQUIRE(unumber64.get() == unumber64_read->get() );


}


TEST_CASE("test_sanity_types_bits ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & bits = runner.create("ytypes/built-in-t/bits-value", "disable-nagle");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto bits_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/bits-value");

    REQUIRE(!bits_read_vec.empty());

    auto bits_read = bits_read_vec[0];

    REQUIRE(bits.get() == bits_read->get() );

}

TEST_CASE("test_sanity_types_decimal64 ")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & deci64 = runner.create("ytypes/built-in-t/deci64", "2.14");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto deci64_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/deci64");

    REQUIRE(!deci64_read_vec.empty());

    auto deci64_read = deci64_read_vec[0];

    //TODO log this
    //std::cout << deci64_read->get() << std::endl;

    REQUIRE(deci64.get() == deci64_read->get() );

}

TEST_CASE("test_sanity_types_string")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & str = runner.create("ytypes/built-in-t/name", "name_str");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto str_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/name");

    REQUIRE(!str_read_vec.empty());

    auto str_read = str_read_vec[0];

    //std::cout << str_read->get() << std::endl;

    REQUIRE(str.get() == str_read->get() );

}

TEST_CASE("test_sanity_types_empty")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & emptee = runner.create("ytypes/built-in-t/emptee", "");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto emptee_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/emptee");

    REQUIRE(!emptee_read_vec.empty());

    auto emptee_read = emptee_read_vec[0];

    REQUIRE(emptee_read );

}

TEST_CASE("test_sanity_types_boolean")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & bool_val = runner.create("ytypes/built-in-t/bool-value", "true");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto bool_val_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/bool-value");

    REQUIRE(!bool_val_read_vec.empty());

    auto bool_val_read = bool_val_read_vec[0];

    REQUIRE(bool_val.get() == bool_val_read->get() );

}


TEST_CASE("test_sanity_types_embedded_enum")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & embedded_enum = runner.create("ytypes/built-in-t/embeded-enum", "zero");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto embedded_enum_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/embeded-enum");

    REQUIRE(!embedded_enum_read_vec.empty());

    auto embedded_enum_read = embedded_enum_read_vec[0];

    REQUIRE(embedded_enum.get() == embedded_enum_read->get() );

}

TEST_CASE("test_sanity_types_enum")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & enum_value = runner.create("ytypes/built-in-t/enum-value", "none");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto enum_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-value");

    REQUIRE(!enum_value_read_vec.empty());

    auto enum_value_read = enum_value_read_vec[0];


    REQUIRE(enum_value.get() == enum_value_read->get() );

}

TEST_CASE("test_sanity_types_union")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & union_value = runner.create("ytypes/built-in-t/younion", "none");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto union_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/younion");

    REQUIRE(!union_value_read_vec.empty());

    auto union_value_read = union_value_read_vec[0];

    REQUIRE(union_value.get() == union_value_read->get() );


}

TEST_CASE("test_sanity_types_union_enum")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & enum_int_value = runner.create("ytypes/built-in-t/enum-int-value", "any");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);


    auto enum_int_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-int-value");

    REQUIRE(!enum_int_value_read_vec.empty());

    auto enum_int_value_read = enum_int_value_read_vec[0];


    REQUIRE(enum_int_value.get() == enum_int_value_read->get() );


}

TEST_CASE("test_sanity_types_union_int")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & enum_int_value = runner.create("ytypes/built-in-t/enum-int-value", "2");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);


    auto enum_int_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-int-value");

    REQUIRE(!enum_int_value_read_vec.empty());

    auto enum_int_value_read = enum_int_value_read_vec[0];

    REQUIRE(enum_int_value.get() == enum_int_value_read->get() );

}


TEST_CASE("test_union_leaflist")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & llunion1 = runner.create("ytypes/built-in-t/llunion[.='1']", "");

    auto & llunion2 = runner.create("ytypes/built-in-t/llunion[.='2']", "");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto llunion1_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/llunion[.='1']");

    REQUIRE(!llunion1_read_vec.empty());

    auto llunion2_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/llunion[.='2']");

    REQUIRE(!llunion2_read_vec.empty());

}

TEST_CASE("test_enum_leaflist")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & local = runner.create("ytypes/built-in-t/enum-llist[.='local']", "");

    auto & remote = runner.create("ytypes/built-in-t/enum-llist[.='remote']", "");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto enumllist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-llist[.='local']");

    REQUIRE(!enumllist_read_vec.empty());

    enumllist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-llist[.='remote']");

    REQUIRE(!enumllist_read_vec.empty());

}

TEST_CASE("test_identity_leaflist")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & child_identity = runner.create("ytypes/built-in-t/identity-llist[.='ydktest-sanity:child-identity']", "");

    auto & child_child_identity = runner.create("ytypes/built-in-t/identity-llist[.='ydktest-sanity:child-child-identity']", "");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto identityllist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/identity-llist[.='ydktest-sanity:child-identity']");

    REQUIRE(!identityllist_read_vec.empty());

    identityllist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/identity-llist[.='ydktest-sanity:child-child-identity']");

    REQUIRE(!identityllist_read_vec.empty());

}


TEST_CASE("test_union_complex_list")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & younion = runner.create("ytypes/built-in-t/younion-list[.='123:45']", "");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");

    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto younionlist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/younion-list[.='123:45']");

    REQUIRE(!younionlist_read_vec.empty());

}

TEST_CASE("test_identityref")
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode& schema = sp.get_root_schema();


    ydk::path::CodecService s{};

    auto & runner = schema.create("ydktest-sanity:runner", "");



    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{&runner.root()};

    REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema.rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    delete_rpc->input().create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto & identity_ref_value = runner.create("ytypes/built-in-t/identity-ref-value", "ydktest-sanity:child-child-identity");

    xml = s.encode(runner, ydk::EncodingFormat::XML, false);

    CHECK( !xml.empty());

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema.rpc("ydk:create") };
    create_rpc->input().create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema.rpc("ydk:read") };
    auto & runner_read = schema.create("ydktest-sanity:runner", "");


    std::unique_ptr<const ydk::path::DataNode> data_root2{&runner_read.root()};

    xml = s.encode(runner_read, ydk::EncodingFormat::XML, false);
    REQUIRE( !xml.empty() );
    read_rpc->input().create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    REQUIRE(read_result != nullptr);

    auto identityref_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/identity-ref-value");

    REQUIRE(!identityref_value_read_vec.empty());

    auto val = identityref_value_read_vec[0]->get();
    //std::cout <<  val << std::endl;

    REQUIRE(val == "ydktest-sanity:child-child-identity");

}

