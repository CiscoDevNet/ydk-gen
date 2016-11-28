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
#include <sstream>

#include "path_api.hpp"
#include "errors.hpp"
#include "types.hpp"

namespace ydk {

static std::string get_bool_string(const std::string & value);

std::string to_str(YType t)
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

LeafData::LeafData(std::string value, EditOperation operation)
	: value(value), operation(operation)
{

}

Value::Value(YType type, std::string name):
		is_set(false),
		operation(EditOperation::not_set),
		name(name),
		value(""),
		type(type)
{
}

Value::Value(const Value& val):
    is_set{val.is_set},
	operation(EditOperation::not_set),
    name{val.name},
    value{val.value},
    type{val.type},
	bits_value{val.bits_value}
{

}


Value::Value(Value&& val):
    is_set{val.is_set},
	operation(EditOperation::not_set),
    name{std::move(val.name)},
    value{std::move(val.value)},
    type{val.type},
	bits_value{val.bits_value}
{
}


Value::~Value()
{
}

const std::string  Value::get() const
{
	if(type == YType::bits)
	{
		return get_bits_string();
	}
	BOOST_LOG_TRIVIAL(trace) <<"Returning value "<<value<<" for leaf "<<name;
	return value;
}

std::pair<std::string, LeafData> Value::get_name_leafdata() const
{
	return {name, {get(), operation}};
}

void Value::operator = (uint8 val)
{
	std::ostringstream value_buffer;

	value_buffer << val;
	BOOST_LOG_TRIVIAL(trace)<<"setting uint8";
	store_value(value_buffer.str());
}

void Value::operator = (uint32 val)
{
	std::ostringstream value_buffer;

	value_buffer << val;
	BOOST_LOG_TRIVIAL(trace)<<"setting uint32";
	store_value(value_buffer.str());
}

void Value::operator = (uint64 val)
{
	std::ostringstream value_buffer;

	value_buffer << val;
	BOOST_LOG_TRIVIAL(trace)<<"setting uint64";
	store_value(value_buffer.str());
}

void Value::operator = (int8 val)
{
	std::ostringstream value_buffer;

	value_buffer << val;
	BOOST_LOG_TRIVIAL(trace)<<"setting int8";
	store_value(value_buffer.str());
}

void Value::operator = (int32 val)
{
	std::ostringstream value_buffer;

	value_buffer << val;
	BOOST_LOG_TRIVIAL(trace)<<"setting int32";
	store_value(value_buffer.str());
}

void Value::operator = (Enum::Value val)
{
	std::ostringstream value_buffer;

	value_buffer << val.name;
	BOOST_LOG_TRIVIAL(trace)<<"setting enum";
	store_value(value_buffer.str());
}

void Value::operator = (Bits val)
{
	std::ostringstream value_buffer;

	bits_value = val;
	value_buffer << get_bits_string();
	store_value(value_buffer.str());
}

void Value::operator = (int64 val)
{
	std::ostringstream value_buffer;

	value_buffer << val;
	BOOST_LOG_TRIVIAL(trace)<<"setting int64";
	store_value(value_buffer.str());
}

void Value::operator = (Empty val)
{
	std::ostringstream value_buffer;

	BOOST_LOG_TRIVIAL(trace)<<"setting empty";
	store_value(value_buffer.str());
}

void Value::operator = (Identity val)
{
	std::ostringstream value_buffer;

	value_buffer << val.to_string();
	BOOST_LOG_TRIVIAL(trace)<<"setting identity: "<<val.to_string();
	store_value(value_buffer.str());
}

void Value::operator = (std::string val)
{
	std::ostringstream value_buffer;

	value_buffer << val;
	BOOST_LOG_TRIVIAL(trace)<<"setting string";
	store_value(value_buffer.str());
}

void Value::operator = (Decimal64 val)
{
	std::ostringstream value_buffer;

	value_buffer << val.value;
	BOOST_LOG_TRIVIAL(trace)<<"setting string";
	store_value(value_buffer.str());
}

void Value::store_value(std::string && val)
{
	is_set=true;
	if(type == YType::boolean)
	{
		value = get_bool_string(val);
	}
	else
	{
		value = val;
	}
	BOOST_LOG_TRIVIAL(trace)<<"storing "<<value;
	BOOST_LOG_TRIVIAL(trace)<<"type of leaf: "<<to_str(type);
}

Value::operator std::string() const
{
	return get();
}

bool Value::operator == (Value & other) const
{
	return get() == other.get();
}

bool Value::operator == (const Value & other) const
{
	return get() == other.get();
}

bool & Value::operator [] (std::string key)
{
	is_set = true;
	return bits_value[key];
}

std::string Value::get_bits_string() const
{
	std::string value;
	for(auto const & entry : bits_value.get_bitmap())
	{
		if(entry.second)
		{
			value += entry.first + " ";
		}
	}

	value = value.substr(0, value.size()-1);

    BOOST_LOG_TRIVIAL(trace)<<value<<": value of bits";

	return (value);
}

std::ostream& operator<< (std::ostream& stream, const Value& value)
{
	stream << value.get();
	return stream;
}

std::string get_bool_string(const std::string & value)
{
	if(value == "1" || value == "true")
	{
		return "true";
	}
	else
	{
		return "false";
	}
}

}
