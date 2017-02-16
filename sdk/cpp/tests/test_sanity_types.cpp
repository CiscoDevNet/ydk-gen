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

#include <string.h>
#include <iostream>

#include "ydk/netconf_provider.hpp"
#include "ydk/crud_service.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk_ydktest/ydktest_sanity_types.hpp"
#include "config.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace std;

TEST_CASE("test_types_int8")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->number8 = 10;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->number8 == r_2->ytypes->built_in_t->number8);
}

TEST_CASE("test_types_int16")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->number16 = 10;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->number16 == r_2->ytypes->built_in_t->number16);
}

TEST_CASE("test_types_int32")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->number32 = 10;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->number32 == r_2->ytypes->built_in_t->number32);
}

TEST_CASE("test_types_int64")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->number64 = -193933810;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->number64 == r_2->ytypes->built_in_t->number64);
}

TEST_CASE("test_types_uint8")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->u_number8 = 10;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->u_number8 == r_2->ytypes->built_in_t->u_number8);
}

TEST_CASE("test_types_uint16")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->u_number16 = 10;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->u_number16 == r_2->ytypes->built_in_t->u_number16);
}

TEST_CASE("test_types_uint32")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->u_number32 = 10;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->u_number32 == r_2->ytypes->built_in_t->u_number32);
}

TEST_CASE("test_types_uint64")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->u_number64 = 10;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->u_number64 == r_2->ytypes->built_in_t->u_number64);
}

TEST_CASE("test_types_bits")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->bits_value["auto-sense-speed"] = true;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->bits_value == r_2->ytypes->built_in_t->bits_value);
}

TEST_CASE("test_types_deci64")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->deci64 = Decimal64("23.14");
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->deci64 == r_2->ytypes->built_in_t->deci64);
}

TEST_CASE("test_types_string")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->name = "testing";
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->name == r_2->ytypes->built_in_t->name);
}

TEST_CASE("test_types_empty")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->emptee = Empty();
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->emptee == r_2->ytypes->built_in_t->emptee);
}

TEST_CASE("test_types_bool")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->bool_value = true;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	std::cout<<r_1->ytypes->built_in_t->bool_value<<std::endl;
	std::cout<<r_2->ytypes->built_in_t->bool_value<<std::endl;
	REQUIRE(r_1->ytypes->built_in_t->bool_value == r_2->ytypes->built_in_t->bool_value);
}

TEST_CASE("test_types_embeded_enum")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->embeded_enum = ydktest_sanity::Runner::Ytypes::BuiltInT::EmbededEnumEnum::zero;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->embeded_enum == r_2->ytypes->built_in_t->embeded_enum);
}

TEST_CASE("test_types_enum")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->enum_value = ydktest_sanity::YdkEnumTestEnum::none;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->enum_value == r_2->ytypes->built_in_t->enum_value);
}

TEST_CASE("test_types_younion")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->younion = ydktest_sanity::YdkEnumTestEnum::none;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->younion == r_2->ytypes->built_in_t->younion);
}

TEST_CASE("test_types_identity")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->identity_ref_value = ydktest_sanity_types::OtherIdentity();
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);

	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
    cout<<r_2->ytypes->built_in_t->identity_ref_value<<endl;
	REQUIRE(r_1->ytypes->built_in_t->identity_ref_value == r_2->ytypes->built_in_t->identity_ref_value);
}

TEST_CASE("test_types_submodule")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::SubTest>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->one_aug->name = "test";
	r_1->one_aug->number = 3;
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::SubTest>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::SubTest * r_2 = dynamic_cast<ydktest_sanity::SubTest*>(r_read.get());
	REQUIRE(r_1->one_aug->name == r_2->one_aug->name);
	REQUIRE(r_1->one_aug->number == r_2->one_aug->number);
}


TEST_CASE("test_types_identity_other_module")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->identity_ref_value = ydktest_sanity_types::YdktestTypeIdentity();
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->identity_ref_value == r_2->ytypes->built_in_t->identity_ref_value);
}

TEST_CASE("test_types_enum_leaflist")
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
	r_1->ytypes->built_in_t->enum_llist.append(ydktest_sanity::YdkEnumTestEnum::remote);
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->enum_llist == r_2->ytypes->built_in_t->enum_llist);
}

TEST_CASE("test_types_identity_leaflist")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->identity_llist.append(ydktest_sanity::ChildIdentityIdentity());
	r_1->ytypes->built_in_t->identity_llist.append(ydktest_sanity::ChildChildIdentityIdentity());
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->enum_llist == r_2->ytypes->built_in_t->enum_llist);
}

TEST_CASE("test_types_union_complex_list")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->younion_list.append("123:45");
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->younion_list == r_2->ytypes->built_in_t->younion_list);
}

TEST_CASE("test_types_list")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	r_1->ytypes->built_in_t->llunion.append(1);
	r_1->ytypes->built_in_t->llunion.append(2);
	r_1->ytypes->built_in_t->llunion.append(3);
	r_1->ytypes->built_in_t->llunion.append(4);
	r_1->ytypes->built_in_t->llunion.append(5);
	r_1->ytypes->built_in_t->llunion.append(6);
	r_1->ytypes->built_in_t->llunion.append(7);
	r_1->ytypes->built_in_t->llunion.append(8);
	r_1->ytypes->built_in_t->llunion.append(9);
	r_1->ytypes->built_in_t->llunion.append(10);
	r_1->ytypes->built_in_t->llunion.append(11);
	reply = crud.create(provider, *r_1);
	REQUIRE(reply);

	//READ
	auto filter = make_unique<ydktest_sanity::Runner>();
	auto r_read = crud.read(provider, *filter);
	REQUIRE(r_read!=nullptr);
	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
	REQUIRE(r_1->ytypes->built_in_t->younion_list == r_2->ytypes->built_in_t->younion_list);
}

TEST_CASE("test_types_bits_list")
{
	ydk::path::Repository repo{TEST_HOME};
	NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
	CrudService crud{};

	//DELETE
	auto r_1 = make_unique<ydktest_sanity::Runner>();
	bool reply = crud.delete_(provider, *r_1);
	REQUIRE(reply);

	//CREATE
	Bits bits1{};
	bits1["disable-nagle"] = true;
	Bits bits2{};
	bits2["disable-nagle"] = true;
	r_1->ytypes->built_in_t->bits_llist.append(bits1);
	r_1->ytypes->built_in_t->bits_llist.append(bits2);
//	reply = crud.create(provider, *r_1); //TODO: netsim issue
//	REQUIRE(reply);
//
//	//READ
//	auto filter = make_unique<ydktest_sanity::Runner>();
//	auto r_read = crud.read(provider, *filter);
//	REQUIRE(r_read!=nullptr);
//	ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
//	REQUIRE(r_1->ytypes->built_in_t->bits_llist == r_2->ytypes->built_in_t->bits_llist);
}
