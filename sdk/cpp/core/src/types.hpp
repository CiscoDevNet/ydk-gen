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

namespace std
{
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
    return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}
}

namespace ydk
{

typedef unsigned short uint8;
typedef unsigned int uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef signed short int8;
typedef signed int int16;
typedef signed int int32;
typedef signed long long int64;

enum class EditOperation
{
    merge,
    create,
    remove,
    delete_,
    replace,
    not_set
};

typedef struct Empty {
    bool set;
} Empty;

class Entity;

class LeafData
{
  public:
    LeafData(std::string value, EditOperation operation, bool is_set);
    ~LeafData();

    bool operator == (LeafData & other) const;
    bool operator == (const LeafData & other) const;

  public:
    std::string value;
    EditOperation operation;
    bool is_set;
};

struct EntityPath {
    std::string path;
    std::vector<std::pair<std::string, LeafData>> value_paths;

    EntityPath(std::string path, std::vector<std::pair<std::string, LeafData> > value_paths)
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

typedef void (*augment_capabilities_function)();

class Entity {
  public:
    Entity();
    virtual ~Entity();

  public:
    //
    // @brief Get the EntityPath relative to the parent passed in
    //
    // Returns the EntityPath relative to the ancestor passed in.
    // The ancestor must either be null, in which case the absolute path
    // from the root is returned, or some other ancestor of this Entity.
    //
    // @param[in] parent The ancestor relative to which the path is calculated or nullptr
    // @return EntityPath
    // @throws YCPPInvalidArgumentError if the parent is invalid
    virtual EntityPath get_entity_path(Entity* ancestor) const = 0;
    virtual std::string get_segment_path() const = 0;

    virtual bool has_data() const = 0;
    virtual bool has_operation() const = 0;

    virtual void set_value(const std::string & value_path, std::string value) = 0;
    virtual Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path="") = 0;

    virtual std::map<std::string, Entity*> & get_children() = 0;
    virtual std::shared_ptr<Entity> clone_ptr() const;

    virtual void set_parent(Entity* p);
    virtual Entity* get_parent() const;

    virtual augment_capabilities_function get_augment_capabilities_function() const;
    virtual std::string get_bundle_yang_models_location() const;
    virtual std::string get_bundle_name() const;

  public:
    Entity* parent;
    std::string yang_name;
    std::string yang_parent_name;
    EditOperation operation;
    std::map<std::string, Entity*> children;
};

class Bits {
  public:
    Bits();
    virtual ~Bits();
    bool & operator [] (std::string key);
    const std::map<std::string, bool> & get_bitmap() const;

  private:
    std::map<std::string, bool> bitmap;
};

class Decimal64 {
  public:
    Decimal64(std::string value)
     : value(value)
    {
    }
    virtual ~Decimal64()
    {
    }

    std::string value;
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
    class YLeaf {
      public:
        YLeaf(int value, std::string name)
            : value(value), name(name)
        {
        }
        ~YLeaf(){}

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

class YLeaf
{
  public:
    YLeaf(YType type, std::string name);
    ~YLeaf();

    YLeaf(const YLeaf& val);
    YLeaf(YLeaf&& val);

    YLeaf& operator=(const YLeaf& val)=delete;
    YLeaf& operator=(YLeaf&& val)=delete;

    const std::string get() const;
    std::pair<std::string, LeafData> get_name_leafdata() const;

    void operator = (uint8 val);
    void operator = (uint32 val);
    void operator = (uint64 val);
    void operator = (long val);
    void operator = (int8 val);
    void operator = (int32 val);
    void operator = (int64 val);
    void operator = (double val);
    void operator = (Empty val);
    void operator = (Identity val);
    void operator = (Bits val);
    void operator = (std::string val);
    void operator = (Enum::YLeaf val);
    void operator = (Decimal64 val);

    void set(uint8 val);
    void set(uint32 val);
    void set(uint64 val);
    void set(long val);
    void set(int8 val);
    void set(int32 val);
    void set(int64 val);
    void set(double val);
    void set(Empty val);
    void set(Identity val);
    void set(Bits val);
    void set(std::string val);
    void set(Enum::YLeaf val);
    void set(Decimal64 val);

    operator std::string() const;
    bool operator == (YLeaf & other) const;
    bool operator == (const YLeaf & other) const;

    bool & operator [] (std::string key);

  public:
    bool is_set;
    EditOperation operation;

  private:
    void store_value(std::string && val);
    std::string get_bits_string() const;

    std::string name;
    std::string value;
    YType type;
    Bits bits_value;
};

class YLeafList {
  public:
    YLeafList(YType type, std::string name);
    virtual ~YLeafList();

    YLeafList(const YLeafList& val);
    YLeafList(YLeafList&& val);

    YLeafList& operator=(const YLeafList& val);
    YLeafList& operator=(YLeafList&& val);

    virtual void append(uint8 val);
    virtual void append(uint32 val);
    virtual void append(uint64 val);
    virtual void append(long val);
    virtual void append(int8 val);
    virtual void append(int32 val);
    virtual void append(int64 val);
    virtual void append(double val);
    virtual void append(Empty val);
    virtual void append(Identity val);
    virtual void append(Bits val);
    virtual void append(std::string val);
    virtual void append(Enum::YLeaf val);
    virtual void append(Decimal64 val);

    virtual YLeaf & operator [] (size_t index);

    operator std::string() const;
    bool operator == (YLeafList & other) const;
    bool operator == (const YLeafList & other) const;

    virtual std::vector<std::pair<std::string, LeafData> > get_name_leafdata() const;
    virtual std::vector<YLeaf> getYLeafs() const;

  public:
    EditOperation operation;

  private:
    std::vector<YLeaf> values;
    YType type;
    std::string name;
};

std::ostream& operator<< (std::ostream& stream, const YLeaf& value);

enum class EncodingFormat {
    XML,
    JSON
};

std::string to_string(EditOperation operation);

enum class Protocol
{
	restconf,
	netconf
};
}

#endif /* _TYPES_HPP_ */
