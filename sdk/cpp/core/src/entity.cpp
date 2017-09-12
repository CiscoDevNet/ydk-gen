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

#include "entity_util.hpp"
#include "logger.hpp"
#include "types.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

namespace ydk
{

//////////////////////////////////////////////////////////////////
/// Entity
//////////////////////////////////////////////////////////////////
Entity::Entity()
  : parent(nullptr), yfilter(YFilter::not_set), is_presence_container(false), is_top_level_class(false), has_list_ancestor(false)
{
}

Entity::~Entity()
{
}

shared_ptr<Entity> Entity::clone_ptr() const
{
    return nullptr;
}

void Entity::set_parent(Entity* p)
{
    parent = p;
}

Entity* Entity::get_parent() const
{
    return parent;
}

augment_capabilities_function Entity::get_augment_capabilities_function() const
{
    return nullptr;
}

std::string Entity::get_bundle_yang_models_location() const
{
    return "";
}

std::string Entity::get_bundle_name() const
{
    return "";
}

std::map<std::pair<std::string, std::string>, std::string> Entity::get_namespace_identity_lookup() const
{
    return {};
}

std::string Entity::get_absolute_path() const
{
    return "";
}

bool Entity::operator == (Entity & other) const
{
    if(!has_data() && !other.has_data())
        return true;

    if(!has_data() || !other.has_data())
        return false;

    YLOG_DEBUG("Comparing equality {} and {}", get_segment_path(), other.get_segment_path());

    auto const this_children = get_children();
    auto const other_children = other.get_children();

    if(get_entity_path(*this, parent) == get_entity_path(other, other.parent))
    {
        if(this_children.size() == other_children.size())
        {
            for(map<std::string, shared_ptr<Entity>>::const_iterator rit = this_children.begin(), lit = other_children.begin() ;
                    rit!=this_children.end() && lit!=other_children.end();
                    ++rit, ++lit)
            {
                if(*(rit->second) != *(lit->second))
                {
                    YLOG_DEBUG("Children not equal {} and {}", rit->first, lit->first);
                    return false;
                }
            }
        }
    }
    else
    {
        YLOG_DEBUG("Entity path not equal {} and {}", yang_name, other.yang_name);
        return false;
    }

    return true;
}

bool Entity::operator != (Entity & other) const
{
    if(has_data() && !other.has_data())
        return true;

    if(!has_data() && other.has_data())
        return true;

    YLOG_DEBUG("Comparing inequality {} and {}", get_segment_path(), other.get_segment_path());

    auto const & this_children = get_children();
    auto const & other_children = other.get_children();

    if(get_entity_path(*this, parent) == get_entity_path(other, other.parent))
    {
        if(this_children.size() == other_children.size())
        {
            for(map<std::string, shared_ptr<Entity>>::const_iterator rit = this_children.begin(), lit = other_children.begin() ;
                    rit!=this_children.end() && lit!=other_children.end();
                    ++rit, ++lit)
            {
                if(*(rit->second) != *(lit->second))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        YLOG_DEBUG("Entity path not equal: {} and {}", yang_name, other.yang_name);
        return true;
    }

    return false;
}

std::ostream& operator<< (std::ostream& stream, Entity& entity)
{
    stream<<get_entity_path(entity, entity.parent);
    auto const & children = entity.get_children();
    if(entity.has_data() && children.size() > 0)
        stream<<endl;
    for(auto const& entry : children)
    {
        if(entry.second->has_data())
            stream<<"  { "<<*(entry.second)<<" }"<<endl;
    }
    return stream;
}

//////////////////////////////////////////////////////////////////
/// EntityPath
//////////////////////////////////////////////////////////////////

EntityPath::EntityPath(const std::string & path, std::vector<std::pair<std::string, LeafData> > & value_paths)
    : path(path), value_paths(value_paths)
{
}

EntityPath::~EntityPath()
{
}

bool EntityPath::operator == (EntityPath & other) const
{
    ostringstream os1, os2;
    os1<<*this;
    os2<<other;
    YLOG_DEBUG("Comparing equality {} and {}", os1.str(), os2.str());
    return path == other.path && value_paths == other.value_paths;
}

bool EntityPath::operator == (const EntityPath & other) const
{
    ostringstream os1, os2;
    os1<<*this;
    os2<<other;
    YLOG_DEBUG("Comparing const equality {} and {}", os1.str(), os2.str());
    return path == other.path && value_paths == other.value_paths;
}

bool EntityPath::operator != (EntityPath & other) const
{
    ostringstream os1, os2;
    os1<<*this;
    os2<<other;
    YLOG_DEBUG("Comparing inequality {} and {}", os1.str(), os2.str());
    return path != other.path || value_paths != other.value_paths;
}

bool EntityPath::operator != (const EntityPath & other) const
{
    ostringstream os1, os2;
    os1<<*this;
    os2<<other;
    YLOG_DEBUG("Comparing const inequality {} and {}", os1.str(), os2.str());
    return path != other.path || value_paths != other.value_paths;
}

std::ostream& operator<< (std::ostream& stream, const EntityPath& path)
{
    stream << path.path << " ( ";
    for (size_t index=0; index<path.value_paths.size(); index++)
    {
        auto const & value_path = path.value_paths[index];
        stream << value_path.first<<":"<<value_path.second;
        if(index!= path.value_paths.size()-1)
        {
            stream<<", ";
        }
    }
    stream << " )";
    return stream;
}
}
