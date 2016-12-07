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

bool correct_message(const YCPPError& ex)
{
    BOOST_CHECK_EQUAL(
    		string(ex.what()).find("Invalid value")!=string::npos
    		|| string(ex.what()).find("Failed to resolve")!=string::npos
			|| string(ex.what()).find("Unexpected character")!=string::npos
			|| string(ex.what()).find("does not satisfy the constraint")!=string::npos,
			true);
    return true;
}

BOOST_AUTO_TEST_CASE(int8_invalid)
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	BOOST_REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->number8 = "test";
	BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(int16_invalid)
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	BOOST_REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->number16 = "test";
	BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(int64_invalid)
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	BOOST_REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->number64 = "test";
	BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(bits_invalid)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->bits_value["invalid"] = true;
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(deci_invalid)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->deci64 = "xyz";
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(empty_invalid)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->emptee = 1;
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(enum_invalid)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->embeded_enum = ydktest_sanity::YdkEnumTestEnum::none;
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);

    //CREATE
    r_1->ytypes->built_in_t->embeded_enum = "wrong";
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(identity_invalid)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->identity_ref_value = "wrong";
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(leaflist_invalid)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->identity_llist.append(1);
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(empty_invalid_1)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->emptee = 143;
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}


BOOST_AUTO_TEST_CASE(enum_leaflist_invalid)
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    BOOST_REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->enum_llist.append(Empty());
    BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(int8_invalid_1)
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	BOOST_REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->number8 = Empty();
	BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}

BOOST_AUTO_TEST_CASE(leafref_invalid)
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	BOOST_REQUIRE(reply);

	//CREATE
	r_1->leaf_ref->one->name_of_one = "test";
	r_1->leaf_ref->one->two->self_ref_one_name = "test";
	BOOST_CHECK_EXCEPTION(crud.create(provider, *r_1), YCPPModelError, correct_message);
}
