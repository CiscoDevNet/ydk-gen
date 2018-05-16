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

#include <iostream>
#include <sstream>
#include <string.h>

#include "ydk/netconf_provider.hpp"
#include "ydk/crud_service.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk_ydktest/ydktest_sanity_types.hpp"
#include "config.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace ydktest;
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
    r_1->ytypes->built_in_t->embeded_enum = ydktest_sanity::Runner::Ytypes::BuiltInT::EmbededEnum::zero;
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
    r_1->ytypes->built_in_t->enum_value = ydktest_sanity::YdkEnumTest::none;
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
    r_1->ytypes->built_in_t->younion = ydktest_sanity::YdkEnumTest::none;
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
    r_1->ytypes->built_in_t->identity_ref_value = ydktest_sanity_types::Other();
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
    r_1->ytypes->built_in_t->identity_ref_value = ydktest_sanity_types::YdktestType();
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
    r_1->ytypes->built_in_t->enum_llist.append(ydktest_sanity::YdkEnumTest::local);
    r_1->ytypes->built_in_t->enum_llist.append(ydktest_sanity::YdkEnumTest::remote);
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
    r_1->ytypes->built_in_t->identity_llist.append(ydktest_sanity::ChildIdentity());
    r_1->ytypes->built_in_t->identity_llist.append(ydktest_sanity::ChildChildIdentity());
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
//  reply = crud.create(provider, *r_1); //TODO: netsim issue
//  REQUIRE(reply);
//
//  //READ
//  auto filter = make_unique<ydktest_sanity::Runner>();
//  auto r_read = crud.read(provider, *filter);
//  REQUIRE(r_read!=nullptr);
//  ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
//  REQUIRE(r_1->ytypes->built_in_t->bits_llist == r_2->ytypes->built_in_t->bits_llist);
}

TEST_CASE("string_leaflist")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ytypes->built_in_t->llstring.append("0");
    r_1->ytypes->built_in_t->llstring.append("1");
    r_1->ytypes->built_in_t->llstring.append("2");
    r_1->ytypes->built_in_t->llstring.append("3");
    r_1->ytypes->built_in_t->llstring.append("4");
    r_1->ytypes->built_in_t->llstring.append("5");
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto filter = make_unique<ydktest_sanity::Runner>();
    auto r_read = crud.read(provider, *filter);
    REQUIRE(r_read!=nullptr);
    ydktest_sanity::Runner * r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
    REQUIRE(r_1->ytypes->built_in_t->enum_llist == r_2->ytypes->built_in_t->enum_llist);
}

TEST_CASE("test_cascading_types")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    // DELETE
    auto ctypes = make_unique<ydktest_sanity::CascadingTypes>();
    bool reply = crud.delete_(provider, *ctypes);
    REQUIRE(reply);

    // CREATE
    SECTION ( "unknown" )
    {
        ctypes->comp_insttype = ydktest_sanity::CompInstType::unknown;
        ctypes->comp_nicinsttype = ydktest_sanity::CompInstType_::unknown;
    }

    SECTION ( "phys" )
    {
        ctypes->comp_insttype = ydktest_sanity::CompInstType::phys;
        ctypes->comp_nicinsttype = ydktest_sanity::CompInstType_::phys;
    }

    SECTION ( "virt" )
    {
        ctypes->comp_insttype = ydktest_sanity::CompInstType::virt;
        ctypes->comp_nicinsttype = ydktest_sanity::CompInstType_::virt;
    }

    SECTION ( "hv" )
    {
        ctypes->comp_insttype = ydktest_sanity::CompInstType::hv;
        ctypes->comp_nicinsttype = ydktest_sanity::CompInstType_::hv;
    }
    reply = crud.create(provider, *ctypes);
    REQUIRE(reply);

    // READ
    auto filter = make_unique<ydktest_sanity::CascadingTypes>();
    auto temp = crud.read(provider, *filter);
    REQUIRE(temp!=nullptr);
    ydktest_sanity::CascadingTypes * ctypesRead = dynamic_cast<ydktest_sanity::CascadingTypes*>(temp.get());
    std::cout<<ctypes->comp_insttype<<std::endl;
    std::cout<<ctypesRead->comp_nicinsttype<<std::endl;
    REQUIRE(ctypes->comp_insttype == ctypesRead->comp_nicinsttype);
}

TEST_CASE("test_capital_letters")
{
    NetconfServiceProvider provider{"127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    auto native = make_unique<ydktest_sanity::Native>();
    bool reply = crud.delete_(provider, *native);
    REQUIRE(reply);

    // CREATE
    auto gigabit_eth = make_shared<ydktest_sanity::Native::Interface::GigabitEthernet>();
    gigabit_eth->parent = native->interface.get();
    gigabit_eth->name = "test";

    native->interface->gigabitethernet.append(gigabit_eth);
    reply = crud.create(provider, *native);
    REQUIRE(reply);

    // READ
    auto filter = make_unique<ydktest_sanity::Native>();
    auto read_entity_wrapper = crud.read(provider, *filter);
    REQUIRE(read_entity_wrapper != nullptr);
    ydktest_sanity::Native *read_entity = dynamic_cast<ydktest_sanity::Native*>(read_entity_wrapper.get());
    REQUIRE(*native == *read_entity);
}

static string vector_to_string(vector<string> & string_vector)
{
    ostringstream buf;
    for (auto item : string_vector) {
        if (buf.str().length() > 0)
            buf << ", ";
        buf << "\"" << item << "\"";
    }
    return buf.str();
}

TEST_CASE("test_ylist_two_keys") {
	auto runner = make_shared<ydktest_sanity::Runner>();
	auto l1 = make_shared<ydktest_sanity::Runner::TwoKeyList> (); l1->first = "f1"; l1->second = 11;
	auto l2 = make_shared<ydktest_sanity::Runner::TwoKeyList> (); l2->first = "f2"; l2->second = 22;
	runner->two_key_list.extend({l1, l2});

	auto ldataKeys = runner->two_key_list.keys();
	REQUIRE(vector_to_string(ldataKeys) == R"("f1,11", "f2,22")");

	string count{};
	for (auto lkey : ldataKeys) {
		auto ent = runner->two_key_list[lkey];
		REQUIRE(ent != nullptr);
		auto ldata = dynamic_cast<ydktest_sanity::Runner::TwoKeyList*> (ent.get());
		count += ldata->first;
	}
	REQUIRE(count == "f1f2");

	auto ent = runner->two_key_list["f2,22"];
	REQUIRE(ent != nullptr);
	auto ldata = dynamic_cast<ydktest_sanity::Runner::TwoKeyList*> (ent.get());
    count = ldata->first;
	REQUIRE("f2" == count);
}

TEST_CASE("test_ylist_no_keys") {
	auto runner = make_shared<ydktest_sanity::Runner>();
	auto t1 = make_shared<ydktest_sanity::Runner::NoKeyList> (); t1->test = "t1";
	auto t2 = make_shared<ydktest_sanity::Runner::NoKeyList> (); t2->test = "t2";
	auto t3 = make_shared<ydktest_sanity::Runner::NoKeyList> (); t3->test = "t3";
	runner->no_key_list.extend({t1, t2, t3});

	auto testKeys = runner->no_key_list.keys();
	REQUIRE(vector_to_string(testKeys) == R"("0", "1", "2")");

	string count{};
	for (auto ent : runner->no_key_list.entities()) {
		auto elem = dynamic_cast<ydktest_sanity::Runner::NoKeyList*> (ent.get());
		count += elem->test;
	}
	REQUIRE(count == "t1t2t3");
}
