//
// @file value.hpp
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

#include <iostream>
#include <sstream>

#include "errors.hpp"
#include "logger.hpp"
#include "types.hpp"

using namespace std;

namespace ydk {

string to_string(YType t)
{
#define TOSTRING(t) case YType::t: return #t
    switch(t)
    {
        TOSTRING(uint8);
        TOSTRING(uint16);
        TOSTRING(uint32);
        TOSTRING(uint64);
        TOSTRING(int8);
        TOSTRING(int16);
        TOSTRING(int32);
        TOSTRING(int64);
        TOSTRING(empty);
        TOSTRING(identityref);
        TOSTRING(str);
        TOSTRING(boolean);
        TOSTRING(enumeration);
        TOSTRING(bits);
        TOSTRING(decimal64);
    }
    return "";
}

YLeafList::YLeafList(YType type, const std::string & name)
    : yfilter(YFilter::not_set), type(type), name(name)
{
}

YLeafList::YLeafList(const YLeafList& other)
    : yfilter(YFilter::not_set), values(other.getYLeafs()), type(other.type), name(other.name)
{
}

YLeafList::YLeafList(YLeafList&& other)
    : yfilter(YFilter::not_set), values(other.getYLeafs()), type(other.type), name(other.name)
{
}

ydk::YLeafList&
YLeafList::operator=(const YLeafList& other)
{
    type = other.type;
    name = other.name;
    values = other.getYLeafs();
    yfilter = other.yfilter;
    return *this;
}

ydk::YLeafList&
YLeafList::operator=(YLeafList&& other)
{
    type = other.type;
    name = other.name;
    values = other.getYLeafs();
    yfilter = other.yfilter;
    return *this;
}

YLeafList::~YLeafList()
{
}

void YLeafList::append(uint8 val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(uint32 val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(uint64 val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(long val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(double val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(int8 val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(int32 val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(Enum::YLeaf val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(int64 val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(Empty val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(Identity val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(Bits val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(string val)
{
    YLeaf value {type, name};
    value = val;


    values.push_back(value);
}

void YLeafList::append(Decimal64 val)
{
    YLeaf value {type, name};
    value = val.value;


    values.push_back(value);
}

bool YLeafList::operator == (YLeafList & other) const
{
    return type == other.type && name == other.name && values == other.getYLeafs();
}

bool YLeafList::operator == (const YLeafList & other) const
{
    return type == other.type && name == other.name && values == other.getYLeafs();
}

YLeaf & YLeafList::operator [] (size_t key)
{
    if(key >= values.size())
    {
        throw(YInvalidArgumentError{"List index out of range"});
    }
    return values[key];
}

vector<YLeaf> YLeafList::getYLeafs() const
{
    return values;
}

void YLeafList::clear()
{
    values.clear();
}

std::vector<std::pair<std::string, LeafData> > YLeafList::get_name_leafdata() const
{
    std::vector<std::pair<std::string, LeafData> > name_values;
    for (auto value : values)
    {
        name_values.push_back(
                            {
                                (value.get_name_leafdata().first+"[.=\""+value.get()+"\"]"),
                                {"", yfilter, value.is_set, value.value_namespace, value.value_namespace_prefix}
                            }
                            );
    }
    return name_values;
}

using namespace std;

YList::YList(Entity* parent_entity, initializer_list<string> key_names)
	: parent(parent_entity),counter(1000000)
{
    ylist_key_names = vector<string>{};
    entity_map = map<string,shared_ptr<Entity>>{};
    key_vector = vector<string>{};

    for (auto key : key_names) {
        ylist_key_names.push_back(key);
    }
}

YList::~YList()
{
}

string
YList::build_key(shared_ptr<Entity> ep)
{
    ostringstream value_buffer;
    string key;
    vector< pair<string, LeafData> > name_leaf_data_vector = ep->get_name_leaf_data();
    for (auto key : ylist_key_names) {
        for (auto name_leaf_data : name_leaf_data_vector) {
            if (key == name_leaf_data.first) {
                key = value_buffer.str();
                if (key.length() > 0) {
                    value_buffer << ",";
                }
                value_buffer << name_leaf_data.second.value;
                break;
            }
        }
    }
    key = value_buffer.str();
    if (key.length() == 0) {
        // No key list or no matching key, use internal counter
        value_buffer << counter++;
        key = value_buffer.str();
    }
    return key;
}

void
YList::append(shared_ptr<Entity> ep)
{
    ep->parent = parent;

	string key = build_key(ep);
    if (!entity_map[key]) {
        key_vector.push_back(key);
    }
    entity_map[key] = ep;
}

void
YList::extend(initializer_list<shared_ptr<Entity>> ep_list)
{
    for (auto ep : ep_list) {
        append(ep);
    }
}

shared_ptr<Entity>
YList::operator [] (const string& key) const
{
	return entity_map.at(key);
}

shared_ptr<Entity>
YList::operator [] (const std::size_t item) const
{
    if (item < key_vector.size()) {
        auto key =  key_vector[item];
        return entity_map.at(key);
    }
    else {
        YLOG_ERROR("Index value {} is out of range [0,{}]", item, key_vector.size());
        throw(YInvalidArgumentError{"Index value is out of range"});
    }
    return nullptr;
}

vector<shared_ptr<Entity>>
YList::entities() const
{
    vector<shared_ptr<Entity>> ev{};
    for (auto key : key_vector) {
        ev.push_back(entity_map.at(key));
    }
    return ev;
}

vector<string>
YList::keys() const
{
	return key_vector;
}

size_t
YList::len() const
{
    return key_vector.size();
}

shared_ptr<Entity>
YList::pop(const string& key)
{
    for (vector<string>::iterator it = key_vector.begin() ; it != key_vector.end(); ++it) {
        if (*it == key) {
            shared_ptr<Entity> found = entity_map[key];
            entity_map.erase(key);
            key_vector.erase(it);
            return found;
        }
    }
    return nullptr;
}

shared_ptr<Entity>
YList::pop(const std::size_t item)
{
    if (item < key_vector.size()) {
        vector<string>::iterator it = key_vector.begin();
        for (size_t i = 0; i < item; i++) ++it;
        shared_ptr<Entity> found = entity_map[*it];
        entity_map.erase(*it);
        key_vector.erase(it);
        return found;
    }
    else {
        YLOG_ERROR("Index value {} is out of range [0,{}]", item, key_vector.size());
        throw(YInvalidArgumentError{"Index value is out of range"});
    }
    return nullptr;
}

}
