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

#define TEST_MODULE LevelsTests
#include <string.h>
#include <iostream>

#include <ydk/netconf_provider.hpp>
#include <ydk/crud_service.hpp>
#include <ydk_ydktest/ydktest_sanity.hpp>
#include <ydk_ydktest/ydktest_sanity_types.hpp>

#include "catch.hpp"
#include "config.hpp"

using namespace ydk;
using namespace std;

#define CONTAINS_ERROR_MESSAGE \
    		Catch::Contains("Invalid value") \
    		|| Catch::Contains("Failed to resolve") \
			|| Catch::Contains("Unexpected character") \
			|| Catch::Contains("does not satisfy the constraint") \
			|| Catch::Contains("YCPPModelError")


TEST_CASE("int8_invalid")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->number8 = "test";
	CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("int16_invalid")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->number16 = "test";
	CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("int64_invalid")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->number64 = "test";
	CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("bits_invalid")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->bits_value["invalid"] = true;
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("deci_invalid")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->deci64 = "xyz";
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("empty_invalid")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->emptee = 1;
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("enum_invalid")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->embeded_enum = ydktest_sanity::YdkEnumTestEnum::none;
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);

    //CREATE
    r_1->ytypes->built_in_t->embeded_enum = "wrong";
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("identity_invalid")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->identity_ref_value = "wrong";
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("leaflist_invalid")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->identity_llist.append(1);
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("empty_invalid_1")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->emptee = 143;
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}


TEST_CASE("enum_leaflist_invalid")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->enum_llist.append(Empty());
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("int8_invalid_1")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->number8 = Empty();
	CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("leafref_invalid")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->leaf_ref->one->name_of_one = "test";
	r_1->leaf_ref->one->two->self_ref_one_name = "test";
	CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}

TEST_CASE("leaflist_max_elements")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->enum_llist.append(ydktest_sanity::YdkEnumTestEnum::local);
    r_1->ytypes->built_in_t->enum_llist.append(ydktest_sanity::YdkEnumTestEnum::not_set);
    r_1->ytypes->built_in_t->enum_llist.append(ydktest_sanity::YdkEnumTestEnum::remote);
    r_1->ytypes->built_in_t->enum_llist.append(ydktest_sanity::YdkEnumTestEnum::none);
    CHECK_THROWS_WITH(crud.create(provider, *r_1), CONTAINS_ERROR_MESSAGE);
}
