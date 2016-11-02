//
// @file types.hpp
// @brief Header for ydk entity
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

#ifndef _TYPES_HPP_
#define _TYPES_HPP_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace ydk {

typedef unsigned short uint8;
typedef unsigned int uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef signed short int8;
typedef signed int int16;
typedef signed int int32;
typedef signed long long int64;

typedef struct Empty {
    bool set;
} Empty;

namespace core {
class DataNode;
class RootSchemaNode;
}

class Entity;

struct EntityPath {
	std::string path;
	std::vector<std::pair<std::string, std::string>> value_paths;

	EntityPath(std::string path, std::vector<std::pair<std::string, std::string>> value_paths)
		: path(path), value_paths(value_paths)
	{
	}

	~EntityPath()
	{
	}

	inline bool operator == (EntityPath & other) const
	{
	    return path == other.path && value_paths == other.value_paths;
	}

	inline bool operator == (const EntityPath & other) const
	{
	    return path == other.path && value_paths == other.value_paths;
	}
};

class Entity {
  public:
	Entity():parent(nullptr){}
	virtual ~Entity(){}

  public:
	virtual bool has_data() const = 0;

    //
    // @brief Get the EntityPath relative to the parent passed in
    //
    // Returns the EntityPath relative to the parent passed in.
    // The parent must either be null in which case the absolute path
    // from the root is returned or some parent that is an ancestor of this Entity.
    //
    // @param[in] parent The ancestor relative to which the path is calculated or nullptr
    // @return EntityPath
    // @throws YDKInvalidArgumentException if the parent is invalid
    virtual EntityPath get_entity_path(Entity* parent) const = 0;

    virtual std::string get_segment_path() const = 0;

    virtual Entity* set_child(std::string path) = 0;

    virtual void set_value(std::string value_path, std::string value) = 0;

    virtual std::vector<Entity*> & get_children()
    {
	return children;
    }

    virtual std::unique_ptr<Entity> clone_ptr()
    {
	return nullptr;
    }

    void add_child(Entity* child)
    {
	children.push_back(child);
    }

  public:
	Entity* parent;
	std::vector<Entity*> children;
};

class Bits {
  public:
	Bits()
    {
    }
	virtual ~Bits()
	{
	}
	bool & operator [] (std::string key);
    const std::map<std::string, bool> & get_bitmap() const;

  private:
	std::map<std::string, bool> bitmap;
};

class Identity {
  public:
	Identity(std::string tag) : tag(tag)
	{
	}

	virtual ~Identity(){}

	std::string to_string()
	{
		return tag;
	}

  private:
	std::string tag;
};

class Enum {
  public:
	class Value {
	  public:
		Value(int value, std::string name)
			: value(value), name(name)
		{
		}
		~Value(){}

		int value;
		std::string name;
	};

	Enum()
	{
	}
	~Enum()
	{
	}
};

enum class YType {
	uint8,
	uint16,
	uint32,
	uint64,
	int8,
	int16,
	int32,
	int64,
	empty,
	identityref,
	str,
	boolean,
	enumeration,
	bits,
	decimal64
};

class Value {
  public:
	Value(YType type, std::string name);
	~Value();

    Value(const Value& val);
    Value(Value&& val);

    Value& operator=(const Value& val);
    Value& operator=(Value&& val);

	const std::string get() const;
	std::pair<std::string, std::string> get_name_value() const;

	void operator = (uint8 val);
	void operator = (uint32 val);
	void operator = (uint64 val);
	void operator = (int8 val);
	void operator = (int32 val);
	void operator = (int64 val);
	void operator = (Empty val);
	void operator = (Identity val);
	void operator = (Bits val);
	void operator = (std::string val);
	void operator = (Enum::Value val);

	operator std::string() const;
	bool operator == (Value & other) const;
	bool operator == (const Value & other) const;

	bool & operator [] (std::string key);

	bool is_set;

  private:
	void store_value(std::string && val);
	std::string get_bits_string() const;

	std::string name;
	std::string value;
	YType type;
	Bits bits_value;
};

class ValueList {
  public:
	ValueList(YType type, std::string name);
	~ValueList();

    ValueList(const ValueList& val);
    ValueList(ValueList&& val);

    ValueList& operator=(const ValueList& val);
    ValueList& operator=(ValueList&& val);

	const std::string get() const;
	std::pair<std::string, std::string> get_name_value() const;

	void append(uint8 val);
	void append(uint32 val);
	void append(uint64 val);
	void append(int8 val);
	void append(int32 val);
	void append(int64 val);
	void append(Empty val);
	void append(Identity val);
	void append(Bits val);
	void append(std::string val);
	void append(Enum::Value val);

	Value & operator [] (size_t index);

	operator std::string() const;
	bool operator == (ValueList & other) const;
	bool operator == (const ValueList & other) const;

	std::vector<std::pair<std::string, std::string> > get_name_values() const;
	std::vector<Value> getValues() const;

  private:
	std::vector<Value> values;
	YType type;
	std::string name;
};

std::ostream& operator<< (std::ostream& stream, const Value& value);

}

#endif /* _TYPES_HPP_ */
