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
#include "ydk_ydktest/oc_pattern.hpp"
#include "config.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace std;
using namespace ydktest;
using namespace oc_pattern;

TEST_CASE("one_level_pos_set")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    auto r_1 = make_unique<ydktest_sanity::Runner>();

    r_1->ydktest_sanity_one->number = 1;
    r_1->ydktest_sanity_one->name = "1.2.3.4";
    bool reply = crud.create(provider, *r_1->ydktest_sanity_one);
    REQUIRE(reply);
}

TEST_CASE("one_level_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    // READ
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    r_1->ydktest_sanity_one->number = 1;
    r_1->ydktest_sanity_one->name = "1.2.3.4";
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    auto filter = make_unique<ydktest_sanity::Runner>();
    auto r_2 = crud.read(provider, *filter);
    REQUIRE(r_2!=nullptr);
    ydktest_sanity::Runner * r_2_ptr = dynamic_cast<ydktest_sanity::Runner*>(r_2.get());
    REQUIRE(r_2_ptr!=nullptr);

    REQUIRE(r_1->ydktest_sanity_one->number == r_2_ptr->ydktest_sanity_one->number);
    REQUIRE(r_1->ydktest_sanity_one->name == r_2_ptr->ydktest_sanity_one->name);

    //UPDATE
    r_1 = make_unique<ydktest_sanity::Runner>();
    r_1->ydktest_sanity_one->number = 10;
    r_1->ydktest_sanity_one->name = "runner/one/name";

    filter = make_unique<ydktest_sanity::Runner>();
    reply = crud.update(provider, *r_1);
    REQUIRE(reply);

    r_2 = crud.read(provider, *filter);
    REQUIRE(r_2!=nullptr);
    r_2_ptr = dynamic_cast<ydktest_sanity::Runner*>(r_2.get());
    REQUIRE(r_2_ptr!=nullptr);

    REQUIRE(r_1->ydktest_sanity_one->number == r_2_ptr->ydktest_sanity_one->number);
    REQUIRE(r_1->ydktest_sanity_one->name == r_2_ptr->ydktest_sanity_one->name);

    // DELETE
    filter = make_unique<ydktest_sanity::Runner>();
    reply = crud.delete_(provider, *filter);
    REQUIRE(reply);
}


TEST_CASE("one_level_aug_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    // READ
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    r_1->ydktest_sanity_augm_one_->twin_number = 1;
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    auto filter = make_unique<ydktest_sanity::Runner>();
    auto r_2 = crud.read(provider, *filter);
    REQUIRE(r_2!=nullptr);
    ydktest_sanity::Runner * r_2_ptr = dynamic_cast<ydktest_sanity::Runner*>(r_2.get());
    REQUIRE(r_2_ptr!=nullptr);

    REQUIRE(r_1->ydktest_sanity_augm_one_->twin_number == r_2_ptr->ydktest_sanity_augm_one_->twin_number);

    //UPDATE
    r_1 = make_unique<ydktest_sanity::Runner>();
    r_1->ydktest_sanity_augm_one_->twin_number = 10;

    filter = make_unique<ydktest_sanity::Runner>();
    reply = crud.update(provider, *r_1);
    REQUIRE(reply);

    r_2 = crud.read(provider, *filter);
    REQUIRE(r_2!=nullptr);
    r_2_ptr = dynamic_cast<ydktest_sanity::Runner*>(r_2.get());
    REQUIRE(r_2_ptr!=nullptr);

    REQUIRE(r_1->ydktest_sanity_augm_one_->twin_number == r_2_ptr->ydktest_sanity_augm_one_->twin_number);

    // DELETE
    filter = make_unique<ydktest_sanity::Runner>();
    reply = crud.delete_(provider, *filter);
    REQUIRE(reply);
}

TEST_CASE("two_level_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    // READ
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    r_1->two->number = 2;
    r_1->two->name = "runner:two:name";
    r_1->two->sub1->number = 21;

    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    auto filter = make_unique<ydktest_sanity::Runner>();
    filter->two->yfilter = YFilter::read;
    auto r_2 = crud.read(provider, *filter);
    REQUIRE(r_2!=nullptr);
    ydktest_sanity::Runner * r_2_ptr = dynamic_cast<ydktest_sanity::Runner*>(r_2.get());
    REQUIRE(r_2_ptr!=nullptr);

    REQUIRE(r_1->two->number == r_2_ptr->two->number);
    REQUIRE(r_1->two->name == r_2_ptr->two->name);
    REQUIRE(r_1->two->sub1->number == r_2_ptr->two->sub1->number);

    // UPDATE
    r_1 = make_unique<ydktest_sanity::Runner>();
    r_1->two->number = 20;
    r_1->two->name = "runner/two/name";
    r_1->two->sub1->number = 210;

    filter = make_unique<ydktest_sanity::Runner>();
    reply = crud.update(provider, *r_1->two);
    REQUIRE(reply);
    r_2 = crud.read(provider, *filter);
    REQUIRE(r_2!=nullptr);
    r_2_ptr = dynamic_cast<ydktest_sanity::Runner*>(r_2.get());
    REQUIRE(r_2_ptr!=nullptr);

    REQUIRE(r_1->two->number == r_2_ptr->two->number);
    REQUIRE(r_1->two->name == r_2_ptr->two->name);
    REQUIRE(r_1->two->sub1->number == r_1->two->sub1->number);

    // DELETE
    filter = make_unique<ydktest_sanity::Runner>();
    reply = crud.delete_(provider, *filter);
    REQUIRE(reply);
}

TEST_CASE("three_level_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    // READ
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    //bool reply = crud.delete_(provider, *r_1);
    //REQUIRE(reply);

    r_1->three->number = 3;
    r_1->three->name = "runner:three:name";
    r_1->three->sub1->number = 31;
    r_1->three->sub1->sub2->number = 311;

    bool reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    auto filter = make_unique<ydktest_sanity::Runner>();
    auto r_2 = crud.read(provider, *filter);
    REQUIRE(r_2!=nullptr);
    ydktest_sanity::Runner * r_2_ptr = dynamic_cast<ydktest_sanity::Runner*>(r_2.get());
    REQUIRE(r_2_ptr!=nullptr);

    REQUIRE(r_1->three->number == r_2_ptr->three->number);
    REQUIRE(r_1->three->name == r_2_ptr->three->name);
    REQUIRE(r_1->three->sub1->number == r_2_ptr->three->sub1->number);
    REQUIRE(r_1->three->sub1->sub2->number == r_2_ptr->three->sub1->sub2->number);

    // UPDATE
    r_1 = make_unique<ydktest_sanity::Runner>();
    r_1->three->number = 30;
    r_1->three->name = "runner/three/name";
    r_1->three->sub1->number = 310;
    r_1->three->sub1->sub2->number = 3110;

    filter = make_unique<ydktest_sanity::Runner>();
    reply = crud.update(provider, *r_1);
    r_2 = crud.read(provider, *filter);
    REQUIRE(r_2!=nullptr);
    r_2_ptr = dynamic_cast<ydktest_sanity::Runner*>(r_2.get());
    REQUIRE(r_2_ptr!=nullptr);

    REQUIRE(r_1->three->number == r_2_ptr->three->number);
    REQUIRE(r_1->three->name == r_2_ptr->three->name);
    REQUIRE(r_1->three->sub1->number == r_2_ptr->three->sub1->number);
    REQUIRE(r_1->three->sub1->sub2->number == r_2_ptr->three->sub1->sub2->number);

    // DELETE
    filter = make_unique<ydktest_sanity::Runner>();
    reply = crud.delete_(provider, *filter);
    REQUIRE(reply);
}

TEST_CASE("onelist")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    auto r_2 = make_unique<ydktest_sanity::Runner>();
    auto e_1 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();
    auto e_2 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();
    e_1->number = 1;
    e_1->name = "foo";
    e_2->number = 2;
    e_2->name = "bar";

    e_1->parent = r_1->one_list.get();
    r_1->one_list->ldata.push_back(move(e_1));

    e_2->parent = r_1->one_list.get();
    r_1->one_list->ldata.push_back(move(e_2));

    reply = crud.create(provider, *(r_1->one_list));
    REQUIRE(reply);
    auto r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);
}

TEST_CASE("test_onelist_neg_update_key_nonexist")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    auto r_2 = make_unique<ydktest_sanity::Runner>();
    auto e_1 = make_unique<ydktest_sanity::Runner::OneList::Ldata>();
    e_1->parent = r_1->one_list.get();
    e_1->number = 1;
    e_1->name = "foo";
    r_1->one_list->ldata.push_back(move(e_1));
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);

    //UPDATE
    r_1->one_list->ldata[0]->name = "2";
    r_1->one_list->ldata[0]->number = 2;
    reply = crud.update(provider, *r_1);
    REQUIRE(reply);
    r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);
}

TEST_CASE("test_twolist_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    auto r_2 = make_unique<ydktest_sanity::Runner>();
    auto l_1 = make_unique<ydktest_sanity::Runner::TwoList::Ldata>();
    auto l_2 = make_unique<ydktest_sanity::Runner::TwoList::Ldata>();
    auto s_1 = make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();
    auto s_2 = make_unique<ydktest_sanity::Runner::TwoList::Ldata::Subl1>();

    l_1->number = 21;
    l_1->name = "21name";
    l_1->parent = r_1->two_list.get();
    l_2->number = 22;
    l_2->name = "22name";
    l_2->parent = r_1->two_list.get();
    s_1->number = 221;
    s_1->name = "221name";
    s_1->parent = l_1.get();
    s_2->number = 222;
    s_2->name = "222name";
    s_2->parent = l_2.get();

    l_1->subl1.push_back(move(s_1));
    l_2->subl1.push_back(move(s_2));
    r_1->two_list->ldata.push_back(move(l_1));
    r_1->two_list->ldata.push_back(move(l_2));

    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);

    //UPDATE
    r_1->two_list->ldata[0]->number = 23;
    r_1->two_list->ldata[0]->name = "23name";
    r_1->two_list->ldata[0]->subl1[0]->number = 223;
    r_1->two_list->ldata[0]->subl1[0]->name = "223name";
    r_1->two_list->ldata[1]->number = 24;
    r_1->two_list->ldata[1]->name = "24name";
    r_1->two_list->ldata[1]->subl1[0]->number = 224;
    r_1->two_list->ldata[1]->subl1[0]->name = "224name";
    reply = crud.update(provider, *r_1);
    REQUIRE(reply);
    r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);
}

TEST_CASE("test_threelist_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    auto r_2 = make_unique<ydktest_sanity::Runner>();
    auto l_1 = make_unique<ydktest_sanity::Runner::ThreeList::Ldata>();
    auto l_2 = make_unique<ydktest_sanity::Runner::ThreeList::Ldata>();
    auto s_1 = make_unique<ydktest_sanity::Runner::ThreeList::Ldata::Subl1>();
    auto s_2 = make_unique<ydktest_sanity::Runner::ThreeList::Ldata::Subl1>();
    auto ss_1 = make_unique<ydktest_sanity::Runner::ThreeList::Ldata::Subl1::SubSubl1>();
    auto ss_2 = make_unique<ydktest_sanity::Runner::ThreeList::Ldata::Subl1::SubSubl1>();

    l_1->number = 21;
    l_1->name = "21name";
    l_1->parent = r_1->three_list.get();
    l_2->number = 22;
    l_2->name = "22name";
    l_2->parent = r_1->three_list.get();
    s_1->number = 221;
    s_1->name = "221name";
    s_1->parent = l_1.get();
    s_2->number = 222;
    s_2->name = "222name";
    s_2->parent = l_2.get();
    ss_1->number = 2221;
    ss_1->parent = s_1.get();
    ss_1->name = "2221name";
    ss_2->number = 2222;
    ss_2->parent = s_2.get();
    ss_2->name = "2222name";

    s_1->sub_subl1.push_back(move(ss_1));
    s_2->sub_subl1.push_back(move(ss_2));
    l_1->subl1.push_back(move(s_1));
    l_2->subl1.push_back(move(s_2));
    r_1->three_list->ldata.push_back(move(l_1));
    r_1->three_list->ldata.push_back(move(l_2));

    reply = crud.create(provider, *r_1->three_list);
    REQUIRE(reply);

    //READ
    auto r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);

    //UPDATE
    r_1->three_list->ldata[0]->number = 23;
    r_1->three_list->ldata[0]->name = "23name";
    r_1->three_list->ldata[0]->subl1[0]->number = 223;
    r_1->three_list->ldata[0]->subl1[0]->name = "223name";
    r_1->three_list->ldata[0]->subl1[0]->sub_subl1[0]->number = 2223;
    r_1->three_list->ldata[0]->subl1[0]->sub_subl1[0]->name = "2223name";
    r_1->three_list->ldata[1]->number = 24;
    r_1->three_list->ldata[1]->name = "24name";
    r_1->three_list->ldata[1]->subl1[0]->number = 224;
    r_1->three_list->ldata[1]->subl1[0]->name = "224name";
    r_1->three_list->ldata[1]->subl1[0]->sub_subl1[0]->number = 2224;
    r_1->three_list->ldata[1]->subl1[0]->sub_subl1[0]->name = "2224name";
    reply = crud.update(provider, *r_1);
    REQUIRE(reply);
    r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);
}

TEST_CASE("test_InbtwList_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    auto r_2 = make_unique<ydktest_sanity::Runner>();
    auto l_1 = make_unique<ydktest_sanity::Runner::InbtwList::Ldata>();
    auto l_2 = make_unique<ydktest_sanity::Runner::InbtwList::Ldata>();
    auto s_1 = make_unique<ydktest_sanity::Runner::InbtwList::Ldata::Subc::SubcSubl1>();
    auto s_2 = make_unique<ydktest_sanity::Runner::InbtwList::Ldata::Subc::SubcSubl1>();

    l_1->number = 21;
    l_1->name = "21name";
    l_1->parent = r_1->inbtw_list.get();
    l_2->number = 22;
    l_2->name = "22name";
    l_2->parent = r_1->inbtw_list.get();
    s_1->number = 221;
    s_1->name = "221name";
    s_1->parent = l_1->subc.get();
    s_2->number = 222;
    s_2->name = "222name";
    s_2->parent = l_2->subc.get();

    l_1->subc->subc_subl1.push_back(move(s_1));
    l_2->subc->subc_subl1.push_back(move(s_2));
    r_1->inbtw_list->ldata.push_back(move(l_1));
    r_1->inbtw_list->ldata.push_back(move(l_2));

    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);

    //UPDATE
    r_1->inbtw_list->ldata[0]->number = 221;
    r_1->inbtw_list->ldata[0]->name = "221name";
    r_1->inbtw_list->ldata[0]->subc->subc_subl1[0]->number = 2221;
    r_1->inbtw_list->ldata[0]->subc->subc_subl1[0]->name = "2221name";
    r_1->inbtw_list->ldata[1]->number = 222;
    r_1->inbtw_list->ldata[1]->name = "222name";
    r_1->inbtw_list->ldata[1]->subc->subc_subl1[0]->number = 2222;
    r_1->inbtw_list->ldata[1]->subc->subc_subl1[0]->name = "2222name";
    reply = crud.update(provider, *r_1);
    REQUIRE(reply);
    r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);
}

TEST_CASE("test_leafref_simple_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    auto r_2 = make_unique<ydktest_sanity::Runner>();

    r_1->ytypes->built_in_t->number8 = 100;
    r_1->ytypes->built_in_t->leaf_ref = r_1->ytypes->built_in_t->number8.get();
    crud.create(provider, *r_1);

    //READ
    auto r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);

    //UPDATE
    r_1->ytypes->built_in_t->number8 = 110;
    r_1->ytypes->built_in_t->leaf_ref = r_1->ytypes->built_in_t->number8.get();
    crud.update(provider, *r_1);
    r_read = crud.read(provider, *r_2);
    REQUIRE(r_read != nullptr);
}

TEST_CASE("test_leafref_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ydktest_sanity_one->name = "1.2.3.4";
    r_1->two->sub1->number = 21;
    r_1->three->sub1->sub2->number = 311;
    auto e_1  = make_unique<ydktest_sanity::Runner::InbtwList::Ldata>();
    auto e_2  = make_unique<ydktest_sanity::Runner::InbtwList::Ldata>();
    e_1->parent = r_1->inbtw_list.get();
    e_2->parent = r_1->inbtw_list.get();
    e_1->number = 11;
    e_2->number = 21;
    e_1->name = "11name";
    e_2->name = "21name";
    e_1->subc->number = 111;
    e_2->subc->number = 121;
    e_1->subc->name = "111name";
    e_2->subc->name = "121name";
    auto e_11 = make_unique<ydktest_sanity::Runner::InbtwList::Ldata::Subc::SubcSubl1>();
    auto e_12 = make_unique<ydktest_sanity::Runner::InbtwList::Ldata::Subc::SubcSubl1>();
    e_11->number = 111;
    e_12->number = 112;
    e_11->name = "111name";
    e_12->name = "112name";
    e_11->parent = e_1->subc.get();
    e_12->parent = e_1->subc.get();
    e_1->subc->subc_subl1.push_back(move(e_11));
    e_1->subc->subc_subl1.push_back(move(e_12));
    auto e_21 = make_unique<ydktest_sanity::Runner::InbtwList::Ldata::Subc::SubcSubl1>();
    auto e_22 = make_unique<ydktest_sanity::Runner::InbtwList::Ldata::Subc::SubcSubl1>();
    e_21->number = 121;
    e_22->number = 122;
    e_21->name = "121name";
    e_22->name = "122name";
    e_21->parent = e_2->subc.get();
    e_22->parent = e_2->subc.get();
    e_2->subc->subc_subl1.push_back(move(e_21));
    e_2->subc->subc_subl1.push_back(move(e_22));
    r_1->inbtw_list->ldata.push_back(move(e_1));
    r_1->inbtw_list->ldata.push_back(move(e_2));

    r_1->leaf_ref->ref_one_name = "1.2.3.4";
    r_1->leaf_ref->ref_two_sub1_number = 21;
    r_1->leaf_ref->ref_three_sub1_sub2_number = r_1->three->sub1->sub2->number.get();
    r_1->leaf_ref->one->name_of_one = "1.2.3.4";
    r_1->leaf_ref->one->two->self_ref_one_name = "1.2.3.4";
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    auto r_filter = make_unique<ydktest_sanity::Runner>();
    auto r_read = crud.read(provider, *r_filter);
    ydktest_sanity::Runner* r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
    REQUIRE(r_1->ydktest_sanity_one->name == r_2->ydktest_sanity_one->name);
    REQUIRE(r_1->ydktest_sanity_one->number == r_2->ydktest_sanity_one->number);
    REQUIRE(r_1->inbtw_list->ldata[0]->number == r_2->inbtw_list->ldata[0]->number);
    REQUIRE(r_1->inbtw_list->ldata[0]->name == r_2->inbtw_list->ldata[0]->name);
    REQUIRE(r_1->inbtw_list->ldata[1]->number == r_2->inbtw_list->ldata[1]->number);
    REQUIRE(r_1->inbtw_list->ldata[1]->name == r_2->inbtw_list->ldata[1]->name);
    REQUIRE(r_1->inbtw_list->ldata[0]->subc->subc_subl1[0]->number == r_1->inbtw_list->ldata[0]->subc->subc_subl1[0]->number);
    REQUIRE(r_1->inbtw_list->ldata[0]->subc->subc_subl1[0]->name == r_1->inbtw_list->ldata[0]->subc->subc_subl1[0]->name);
    REQUIRE(*r_1 == *r_2);
}

TEST_CASE("aug_one_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ydktest_sanity_one->one_aug->number = 1;
    r_1->ydktest_sanity_one->one_aug->name = "one";
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto r_filter = make_unique<ydktest_sanity::Runner>();
    auto r_read = crud.read(provider, *r_filter);
    REQUIRE(r_read!=nullptr);
    ydktest_sanity::Runner* r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
    REQUIRE(r_1->ydktest_sanity_one->one_aug->number == r_2->ydktest_sanity_one->one_aug->number);
    REQUIRE(r_1->ydktest_sanity_one->one_aug->name == r_2->ydktest_sanity_one->one_aug->name);

    //UPDATE
    r_1->ydktest_sanity_one->one_aug->number = 10;
    r_1->ydktest_sanity_one->one_aug->name = "onenone";
    reply = crud.update(provider, *r_1);
    REQUIRE(reply);
}

TEST_CASE("aug_onelist_pos")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    auto e_1 = make_unique<ydktest_sanity::Runner::OneList::OneAugList::Ldata>();
    auto e_2 = make_unique<ydktest_sanity::Runner::OneList::OneAugList::Ldata>();
    e_1->number = 1;
    e_1->name = "1name";
    e_1->parent = r_1->one_list->one_aug_list.get();
    e_2->number = 2;
    e_2->name = "2name";
    e_2->parent = r_1->one_list->one_aug_list.get();
    r_1->one_list->one_aug_list->ldata.push_back(move(e_1));
    r_1->one_list->one_aug_list->ldata.push_back(move(e_2));
    r_1->one_list->one_aug_list->enabled = true;
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto r_filter = make_unique<ydktest_sanity::Runner>();
    auto r_read = crud.read(provider, *r_filter);
    REQUIRE(r_read!=nullptr);
    ydktest_sanity::Runner* r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
    REQUIRE(r_1->one_list->one_aug_list->ldata[0]->name == r_2->one_list->one_aug_list->ldata[0]->name);
    REQUIRE(r_1->one_list->one_aug_list->ldata[0]->number == r_2->one_list->one_aug_list->ldata[0]->number);
}

TEST_CASE("parent_empty")
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
    auto r_filter = make_unique<ydktest_sanity::Runner>();
    auto r_read = crud.read(provider, *r_filter);
    REQUIRE(r_read!=nullptr);
}

TEST_CASE("aug_leaf")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->ydktest_sanity_one->augmented_leaf = "test";
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto r_filter = make_unique<ydktest_sanity::Runner>();
    auto r_read = crud.read(provider, *r_filter);
    REQUIRE(r_read != nullptr);
    ydktest_sanity::Runner* r_2 = dynamic_cast<ydktest_sanity::Runner*>(r_read.get());
    REQUIRE(r_2->ydktest_sanity_one->augmented_leaf==r_1->ydktest_sanity_one->augmented_leaf);
}

TEST_CASE("oc_pattern")
{
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    ydk::path::RootSchemaNode& schema = provider.get_session().get_root_schema();
    ydk::path::Codec s{};

    auto & oc = schema.create_datanode("oc-pattern:oc-A[a='xyz']", "");
    oc.create_datanode("B/b","xyz");
    auto x = s.encode(oc, ydk::EncodingFormat::XML, false);
    REQUIRE(x=="<oc-A xmlns=\"http://cisco.com/ns/yang/oc-pattern\"><a>xyz</a><B><b>xyz</b></B></oc-A>");

    oc_pattern::OcA o{};
    o.a = "xyz";
    o.b->b = "xyz";

    bool reply = crud.create(provider, o);
    REQUIRE(reply);

    oc_pattern::OcA o_f{};
    auto d = crud.read(provider, o_f);
    REQUIRE((*d)==o);

    o_f.a = "xyz";
    reply = crud.delete_(provider, o_f);
    REQUIRE(reply);
}

TEST_CASE("mtus")
{
    NetconfServiceProvider provider{"127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    auto mt = make_shared<ydktest_sanity::Runner::Mtus::Mtu>();
    mt->owner = "test";
    mt->mtu = 12;
    mt->parent = r_1->mtus.get();
    r_1->mtus->mtu.push_back(mt);
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto r_filter = make_unique<ydktest_sanity::Runner>();
    auto r_read = crud.read(provider, *r_filter);
    REQUIRE(r_read!=nullptr);

    //DELETE
    r_1 = make_unique<ydktest_sanity::Runner>();
    reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);
}

TEST_CASE("passive")
{
    NetconfServiceProvider provider{"127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    //DELETE
    auto r_1 = make_unique<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    auto passive = make_shared<ydktest_sanity::Runner::Passive>();
    passive->name = "abc";
    auto i = make_shared<ydktest_sanity::Runner::Passive::Interfac>();
    i->test = "xyz";
    i->parent = passive.get();
    passive->interfac.push_back(i);
    passive->testc->xyz = make_shared<ydktest_sanity::Runner::Passive::Testc::Xyz>();
    passive->testc->xyz->parent = passive.get();
    passive->testc->xyz->xyz = 25;
    r_1->passive.push_back(passive);

    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    auto r_filter = make_unique<ydktest_sanity::Runner>();
    auto r_read = crud.read(provider, *r_filter);
    REQUIRE(r_read!=nullptr);

    //DELETE
    r_1 = make_unique<ydktest_sanity::Runner>();
    reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);
}


TEST_CASE("inner_pres")
{
    NetconfServiceProvider provider{"127.0.0.1", "admin", "admin", 12022};
    CrudService crud{};

    ydk::path::RootSchemaNode& schema = provider.get_session().get_root_schema();
    ydk::path::Codec s{};

    auto & r = schema.create_datanode("ydktest-sanity:runner");
    r.create_datanode("outer/inner");
    auto x = s.encode(r, ydk::EncodingFormat::XML, false);
    REQUIRE(x=="<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><outer><inner/></outer></runner>");

    auto r_1 = make_shared<ydktest_sanity::Runner>();
    bool reply = crud.delete_(provider, *r_1);
    REQUIRE(reply);

    //CREATE
    r_1->outer->inner = make_shared<ydktest_sanity::Runner::Outer::Inner>();
    r_1->outer->inner->parent = r_1->outer.get();
    reply = crud.create(provider, *r_1);
    REQUIRE(reply);

    //READ
    ydktest_sanity::Runner f{};
    auto r_read = crud.read(provider, f);
    REQUIRE(r_read != nullptr);
    REQUIRE(*r_read == *r_1);
}
