//
// @file test_netconf_operations.cpp
// @brief The main ydk public header.
//
// YANG Development Kit
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

#define BOOST_TEST_MODULE LevelsTests
#include <boost/test/unit_test.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <string.h>
#include <iostream>

#include "ydk/netconf_provider.hpp"
#include "ydk/crud_service.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk_ydktest/ydktest_sanity_types.hpp"
#include "config.hpp"

using namespace ydk;
using namespace std;

BOOST_AUTO_TEST_CASE(test_replace)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->number8 = 10;
	reply = crud.create(provider, *r_1);
	BOOST_REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	BOOST_REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	BOOST_REQUIRE(r_1->ytypes->built_in_t->number8 == r_2->ytypes->built_in_t->number8);

	//REPLACE
	r_1->ytypes->built_in_t->number8 = 25;
	r_1->operation = EditOperation::replace;
	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);

	//READ AGAIN
	filter = make_unique<ydktest_sanity::Runner>();
	r_read = crud.read(provider, *filter);
	BOOST_REQUIRE(r_read!=nullptr);
	r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	BOOST_REQUIRE(r_1->ytypes->built_in_t->number8 == r_2->ytypes->built_in_t->number8);
}

BOOST_AUTO_TEST_CASE(test_create)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
	auto e_1 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();
	auto e_2 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();
	e_1->number = 1;
	e_1->name = "foo";
	e_2->number = 2;
	e_2->name = "bar";

	e_1->parent = r_1->one_list.get();
	e_1->operation = EditOperation::create;
	r_1->one_list->ldata.push_back(move(e_1));

    e_2->parent = r_1->one_list.get();
    e_2->operation = EditOperation::create;
	r_1->one_list->ldata.push_back(move(e_2));

	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);

	//CREATE AGAIN WITH ERROR
	BOOST_CHECK_THROW(crud.update(provider, *r_1), YCPPServiceProviderError);
}

BOOST_AUTO_TEST_CASE(test_delete)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
	auto e_1 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();
	auto e_2 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();
	e_1->number = 1;
	e_1->name = "foo";
	e_2->number = 2;
	e_2->name = "bar";

	e_1->parent = r_1->one_list.get();
	e_1->operation = EditOperation::create;
	r_1->one_list->ldata.push_back(move(e_1));

    e_2->parent = r_1->one_list.get();
    e_2->operation = EditOperation::create;
	r_1->one_list->ldata.push_back(move(e_2));

	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);

	//DELETE
	r_1 = make_unique<ydktest_sanity::Runner>();
	e_1 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();

	e_1->parent = r_1->one_list.get();
	e_1->number = 1;
	e_1->operation = EditOperation::delete_;
	r_1->one_list->ldata.push_back(move(e_1));

	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);

	//DELETE AGAIN WITH ERROR
	r_1 = make_unique<ydktest_sanity::Runner>();
	e_1 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();

	e_1->parent = r_1->one_list.get();
	e_1->number = 1;
	e_1->operation = EditOperation::delete_;
	r_1->one_list->ldata.push_back(move(e_1));
	BOOST_CHECK_THROW(crud.update(provider, *r_1), YCPPServiceProviderError);
}

BOOST_AUTO_TEST_CASE(test_remove)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //MERGE
	r_1->ytypes->built_in_t->number8 = 25;
	r_1->operation = EditOperation::merge;
	reply = crud.create(provider, *r_1);
	BOOST_REQUIRE(reply);

	//REMOVE
	r_1 = make_unique<ydktest_sanity::Runner>();
	r_1->operation = EditOperation::remove;
	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);

	//REMOVE AGAIN WITH NO ERROR
	r_1->operation = EditOperation::remove;
	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);
}

BOOST_AUTO_TEST_CASE(test_merge)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
	r_1->ytypes->built_in_t->number8 = 25;
	reply = crud.create(provider, *r_1);
	BOOST_REQUIRE(reply);

	//MERGE
	r_1->ytypes->built_in_t->number8 = 32;
	r_1->operation = EditOperation::merge;
	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);
}

BOOST_AUTO_TEST_CASE(delete_leaf)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->number8 = 10;
	reply = crud.create(provider, *r_1);
	BOOST_REQUIRE(reply);

	//DELETE
	r_1->ytypes->built_in_t->number8.operation = EditOperation::delete_;
	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);

	//DELETE AGAIN WITH ERROR
	BOOST_CHECK_THROW(crud.update(provider, *r_1), YCPPServiceProviderError);
}


BOOST_AUTO_TEST_CASE(delete_leaflist)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->enum_llist.append(ydktest_sanity::YdkEnumTestEnum::local);
	reply = crud.create(provider, *r_1);
	BOOST_REQUIRE(reply);

	//DELETE
	r_1->ytypes->built_in_t->enum_llist.operation = EditOperation::delete_;
	reply = crud.update(provider, *r_1);
	BOOST_REQUIRE(reply);

	//DELETE AGAIN WITH ERROR
	BOOST_CHECK_THROW(crud.update(provider, *r_1), YCPPServiceProviderError);
}
