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

#include <boost/log/trivial.hpp>

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

ValueList::ValueList(YType type, std::string name)
	: operation(EditOperation::not_set), type(type), name(name)
{
}

ValueList::ValueList(const ValueList& other)
	: operation(EditOperation::not_set), values(other.getValues()), type(other.type), name(other.name)
{
}

ValueList::ValueList(ValueList&& other)
	: operation(EditOperation::not_set), values(other.getValues()), type(other.type), name(other.name)
{
}

ydk::ValueList&
ValueList::operator=(const ValueList& other)
{
	type = other.type;
	name = other.name;
	values = other.getValues();
	operation = other.operation;
    return *this;
}

ydk::ValueList&
ValueList::operator=(ValueList&& other)
{
	type = other.type;
	name = other.name;
	values = other.getValues();
	operation = other.operation;
    return *this;
}

ValueList::~ValueList()
{
}

void ValueList::append(uint8 val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(uint32 val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(uint64 val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(int8 val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(int32 val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(Enum::Value val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(int64 val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(Empty val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(Identity val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(Bits val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(std::string val)
{
	Value value {type, name};
	value = val;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

void ValueList::append(Decimal64 val)
{
	Value value {type, name};
	value = val.value;
	BOOST_LOG_TRIVIAL(trace)<<"appending "<<value.get();
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_string(type);
	values.push_back(value);
}

bool ValueList::operator == (ValueList & other) const
{
	return type == other.type && name == other.name && values == other.getValues();
}

bool ValueList::operator == (const ValueList & other) const
{
	return type == other.type && name == other.name && values == other.getValues();
}

Value & ValueList::operator [] (size_t key)
{
	if(key >= values.size())
	{
		BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"List index out of range"});
	}
	return values[key];
}

std::vector<Value> ValueList::getValues() const
{
	return values;
}

std::vector<std::pair<std::string, LeafData> > ValueList::get_name_leafdata() const
{
	std::vector<std::pair<std::string, LeafData> > name_values;
    for (auto value : values)
    {
    	name_values.push_back(
    						{
								(value.get_name_leafdata().first+"[.='"+value.get()+"']"),
								{"", operation}
    						}
    						);
    }
    return name_values;
}

}
