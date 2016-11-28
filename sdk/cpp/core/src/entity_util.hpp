/// YANG Development Kit
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
#ifndef ENTITY_UTIL_HPP
#define ENTITY_UTIL_HPP

#include <sstream>
#include "types.hpp"

namespace ydk
{
class Entity;

void get_relative_entity_path(const Entity* current_node, const Entity* ancestor, std::ostringstream & path_buffer);
bool is_set(const EditOperation & operation);

}

#endif /* ENTITY_UTIL_HPP */
