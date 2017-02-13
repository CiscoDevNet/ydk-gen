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

#include "types.hpp"

using namespace std;

namespace ydk
{
Entity::Entity()
  : parent(nullptr), operation(EditOperation::not_set)
{
}

Entity::~Entity()
{
}

unique_ptr<Entity> Entity::clone_ptr()
{
    return nullptr;
}

void Entity::set_child(const std::string & yang_name, Entity* entity)
{
    children[yang_name] = entity;
}

Entity* Entity::get_child(const std::string & yang_name)
{
    return children[yang_name];
}

std::map<std::string, Entity*> & Entity::get_protected_children()
{
    return children;
}


}
