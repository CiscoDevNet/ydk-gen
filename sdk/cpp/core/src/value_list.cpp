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

#include "errors.hpp"
#include "types.hpp"

namespace ydk {

std::string to_string(YType t)
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

YLeafList::YLeafList(YType type, std::string name)
    : operation(EditOperation::not_set), type(type), name(name)
{
}

YLeafList::YLeafList(const YLeafList& other)
    : operation(EditOperation::not_set), values(other.getYLeafs()), type(other.type), name(other.name)
{
}

YLeafList::YLeafList(YLeafList&& other)
    : operation(EditOperation::not_set), values(other.getYLeafs()), type(other.type), name(other.name)
{
}

ydk::YLeafList&
YLeafList::operator=(const YLeafList& other)
{
    type = other.type;
    name = other.name;
    values = other.getYLeafs();
    operation = other.operation;
    return *this;
}

ydk::YLeafList&
YLeafList::operator=(YLeafList&& other)
{
    type = other.type;
    name = other.name;
    values = other.getYLeafs();
    operation = other.operation;
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

void YLeafList::append(std::string val)
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
		throw(YCPPInvalidArgumentError{"List index out of range"});
	}
	return values[key];
}

std::vector<YLeaf> YLeafList::getYLeafs() const
{
    return values;
}

std::vector<std::pair<std::string, LeafData> > YLeafList::get_name_leafdata() const
{
    std::vector<std::pair<std::string, LeafData> > name_values;
    for (auto value : values)
    {
        name_values.push_back(
                            {
                                (value.get_name_leafdata().first+"[.='"+value.get()+"']"),
                                {"", operation, value.is_set}
                            }
                            );
    }
    return name_values;
}

}
