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
#include "../core/src/path_api.hpp"
#include "config.hpp"
#include "../core/src/netconf_provider.hpp"

//test_sanity_types begin

BOOST_AUTO_TEST_CASE( test_sanity_types_int8 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto number8 = runner->create("ytypes/built-in-t/number8", "0");


    BOOST_REQUIRE( number8 != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    //find the number8 node
    auto number8_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/number8");

    BOOST_REQUIRE(!number8_read_vec.empty());

    auto number8_read = number8_read_vec[0];

    BOOST_REQUIRE(number8->get() == number8_read->get() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_int16 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto number16 = runner->create("ytypes/built-in-t/number16", "126");


    BOOST_REQUIRE( number16 != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    //find the number8 node
    auto number16_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/number16");

    BOOST_REQUIRE(!number16_read_vec.empty());

    auto number16_read = number16_read_vec[0];

    BOOST_REQUIRE(number16->get() == number16_read->get() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_int32 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto number32 = runner->create("ytypes/built-in-t/number32", "200000");


    BOOST_REQUIRE( number32 != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto number32_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/number32");

    BOOST_REQUIRE(!number32_read_vec.empty());

    auto number32_read = number32_read_vec[0];

    BOOST_REQUIRE(number32->get() == number32_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_int64 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto number64 = runner->create("ytypes/built-in-t/number64", "-9223372036854775808");


    BOOST_REQUIRE( number64 != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto number64_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/number64");

    BOOST_REQUIRE(!number64_read_vec.empty());

    auto number64_read = number64_read_vec[0];

    BOOST_REQUIRE(number64->get() == number64_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint8 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto unumber8 = runner->create("ytypes/built-in-t/u_number8", "0");


    BOOST_REQUIRE( unumber8 != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto unumber8_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/u_number8");

    BOOST_REQUIRE(!unumber8_read_vec.empty());

    auto unumber8_read = unumber8_read_vec[0];

    BOOST_REQUIRE(unumber8->get() == unumber8_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint16 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto unumber16 = runner->create("ytypes/built-in-t/u_number16", "65535");

    BOOST_REQUIRE( unumber16 != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto unumber16_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/u_number16");

    BOOST_REQUIRE(!unumber16_read_vec.empty());

    auto unumber16_read = unumber16_read_vec[0];

    BOOST_REQUIRE(unumber16->get() == unumber16_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint32 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto unumber32 = runner->create("ytypes/built-in-t/u_number32", "5927");

    BOOST_REQUIRE( unumber32 != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto unumber32_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/u_number32");

    BOOST_REQUIRE(!unumber32_read_vec.empty());

    auto unumber32_read = unumber32_read_vec[0];

    BOOST_REQUIRE(unumber32->get() == unumber32_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_uint64 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto unumber64 = runner->create("ytypes/built-in-t/u_number64", "18446744073709551615");

    BOOST_REQUIRE( unumber64 != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto unumber64_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/u_number64");

    BOOST_REQUIRE(!unumber64_read_vec.empty());

    auto unumber64_read = unumber64_read_vec[0];

    BOOST_REQUIRE(unumber64->get() == unumber64_read->get() );


}


BOOST_AUTO_TEST_CASE( test_sanity_types_bits )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto bits = runner->create("ytypes/built-in-t/bits-value", "disable-nagle");

    BOOST_REQUIRE( bits != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto bits_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/bits-value");

    BOOST_REQUIRE(!bits_read_vec.empty());

    auto bits_read = bits_read_vec[0];

    BOOST_REQUIRE(bits->get() == bits_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_decimal64 )
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto deci64 = runner->create("ytypes/built-in-t/deci64", "2.14");


    BOOST_REQUIRE( deci64 != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto deci64_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/deci64");

    BOOST_REQUIRE(!deci64_read_vec.empty());

    auto deci64_read = deci64_read_vec[0];

    //TODO log this
    //std::cout << deci64_read->get() << std::endl;

    BOOST_REQUIRE(deci64->get() == deci64_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_string)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto str = runner->create("ytypes/built-in-t/name", "name_str");


    BOOST_REQUIRE( str != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto str_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/name");

    BOOST_REQUIRE(!str_read_vec.empty());

    auto str_read = str_read_vec[0];

    //std::cout << str_read->get() << std::endl;

    BOOST_REQUIRE(str->get() == str_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_empty)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto emptee = runner->create("ytypes/built-in-t/emptee", "");


    BOOST_REQUIRE( emptee != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto emptee_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/emptee");

    BOOST_REQUIRE(!emptee_read_vec.empty());

    auto emptee_read = emptee_read_vec[0];

    BOOST_REQUIRE(emptee_read );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_boolean)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto bool_val = runner->create("ytypes/built-in-t/bool-value", "true");


    BOOST_REQUIRE( bool_val != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto bool_val_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/bool-value");

    BOOST_REQUIRE(!bool_val_read_vec.empty());

    auto bool_val_read = bool_val_read_vec[0];

    BOOST_REQUIRE(bool_val->get() == bool_val_read->get() );

}


BOOST_AUTO_TEST_CASE( test_sanity_types_embedded_enum)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto embedded_enum = runner->create("ytypes/built-in-t/embeded-enum", "zero");

    BOOST_REQUIRE( embedded_enum != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto embedded_enum_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/embeded-enum");

    BOOST_REQUIRE(!embedded_enum_read_vec.empty());

    auto embedded_enum_read = embedded_enum_read_vec[0];

    BOOST_REQUIRE(embedded_enum->get() == embedded_enum_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_enum)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto enum_value = runner->create("ytypes/built-in-t/enum-value", "none");

    BOOST_REQUIRE( enum_value != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto enum_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-value");

    BOOST_REQUIRE(!enum_value_read_vec.empty());

    auto enum_value_read = enum_value_read_vec[0];


    BOOST_REQUIRE(enum_value->get() == enum_value_read->get() );

}

BOOST_AUTO_TEST_CASE( test_sanity_types_union)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto union_value = runner->create("ytypes/built-in-t/younion", "none");


    BOOST_REQUIRE( union_value != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto union_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/younion");

    BOOST_REQUIRE(!union_value_read_vec.empty());

    auto union_value_read = union_value_read_vec[0];

    BOOST_REQUIRE(union_value->get() == union_value_read->get() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_union_enum)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto enum_int_value = runner->create("ytypes/built-in-t/enum-int-value", "any");

    BOOST_REQUIRE( enum_int_value != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);

    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);


    auto enum_int_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-int-value");

    BOOST_REQUIRE(!enum_int_value_read_vec.empty());

    auto enum_int_value_read = enum_int_value_read_vec[0];


    BOOST_REQUIRE(enum_int_value->get() == enum_int_value_read->get() );


}

BOOST_AUTO_TEST_CASE( test_sanity_types_union_int)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto enum_int_value = runner->create("ytypes/built-in-t/enum-int-value", "2");


    BOOST_REQUIRE( enum_int_value != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);


    auto enum_int_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-int-value");

    BOOST_REQUIRE(!enum_int_value_read_vec.empty());

    auto enum_int_value_read = enum_int_value_read_vec[0];

    BOOST_REQUIRE(enum_int_value->get() == enum_int_value_read->get() );

}


BOOST_AUTO_TEST_CASE( test_union_leaflist)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto llunion1 = runner->create("ytypes/built-in-t/llunion[.='1']", "");


    BOOST_REQUIRE( llunion1 != nullptr );

    auto llunion2 = runner->create("ytypes/built-in-t/llunion[.='2']", "");

    BOOST_REQUIRE(llunion2 != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto llunion1_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/llunion[.='1']");

    BOOST_REQUIRE(!llunion1_read_vec.empty());

    auto llunion2_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/llunion[.='2']");

    BOOST_REQUIRE(!llunion2_read_vec.empty());

}

BOOST_AUTO_TEST_CASE( test_enum_leaflist)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto local = runner->create("ytypes/built-in-t/enum-llist[.='local']", "");


    BOOST_REQUIRE( local != nullptr );

    auto remote = runner->create("ytypes/built-in-t/enum-llist[.='remote']", "");

    BOOST_REQUIRE(remote != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto enumllist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-llist[.='local']");

    BOOST_REQUIRE(!enumllist_read_vec.empty());

    enumllist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/enum-llist[.='remote']");

    BOOST_REQUIRE(!enumllist_read_vec.empty());

}

BOOST_AUTO_TEST_CASE( test_identity_leaflist)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto child_identity = runner->create("ytypes/built-in-t/identity-llist[.='ydktest-sanity:child-identity']", "");


    BOOST_REQUIRE( child_identity != nullptr );

    auto child_child_identity = runner->create("ytypes/built-in-t/identity-llist[.='ydktest-sanity:child-child-identity']", "");

    BOOST_REQUIRE(child_child_identity != nullptr );

    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto identityllist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/identity-llist[.='ydktest-sanity:child-identity']");

    BOOST_REQUIRE(!identityllist_read_vec.empty());

    identityllist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/identity-llist[.='ydktest-sanity:child-child-identity']");

    BOOST_REQUIRE(!identityllist_read_vec.empty());

}


BOOST_AUTO_TEST_CASE( test_union_complex_list)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto younion = runner->create("ytypes/built-in-t/younion-list[.='123:45']", "");

    BOOST_REQUIRE( younion != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto younionlist_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/younion-list[.='123:45']");

    BOOST_REQUIRE(!younionlist_read_vec.empty());

}

BOOST_AUTO_TEST_CASE( test_identityref)
{
    std::string searchdir{TEST_HOME};
    ydk::path::Repository repo{TEST_HOME};

    ydk::NetconfServiceProvider sp{&repo,"127.0.0.1", "admin", "admin",  12022};
    ydk::path::RootSchemaNode* schema = sp.get_root_schema();

    BOOST_REQUIRE(schema != nullptr);

    auto s = ydk::path::CodecService{};

    auto runner = schema->create("ydktest-sanity:runner", "");

    BOOST_REQUIRE( runner != nullptr );

    //get the root
    std::unique_ptr<const ydk::path::DataNode> data_root{runner->root()};

    BOOST_REQUIRE( data_root != nullptr );

    //first delete
    std::unique_ptr<ydk::path::Rpc> delete_rpc { schema->rpc("ydk:delete") };

    auto xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    delete_rpc->input()->create("entity", xml);

    //call delete
    (*delete_rpc)(sp);

    auto identity_ref_value = runner->create("ytypes/built-in-t/identity-ref-value", "ydktest-sanity:child-child-identity");

    BOOST_REQUIRE( identity_ref_value != nullptr );


    xml = s.encode(runner, ydk::path::CodecService::Format::XML, false);

    BOOST_CHECK_MESSAGE( !xml.empty(),
                        "XML output :" << xml);


    //call create
    std::unique_ptr<ydk::path::Rpc> create_rpc { schema->rpc("ydk:create") };
    create_rpc->input()->create("entity", xml);
    (*create_rpc)(sp);

    //call read
    std::unique_ptr<ydk::path::Rpc> read_rpc { schema->rpc("ydk:read") };
    auto runner_read = schema->create("ydktest-sanity:runner", "");
    BOOST_REQUIRE( runner_read != nullptr );
    std::unique_ptr<const ydk::path::DataNode> data_root2{runner_read->root()};

    xml = s.encode(runner_read, ydk::path::CodecService::Format::XML, false);
    BOOST_REQUIRE( !xml.empty() );
    read_rpc->input()->create("filter", xml);

    auto read_result = (*read_rpc)(sp);

    BOOST_REQUIRE(read_result != nullptr);

    auto identityref_value_read_vec = read_result->find("ydktest-sanity:runner/ytypes/built-in-t/identity-ref-value");

    BOOST_REQUIRE(!identityref_value_read_vec.empty());

    auto val = identityref_value_read_vec[0]->get();
    //std::cout <<  val << std::endl;

    BOOST_REQUIRE(val == "child-child-identity");

}

