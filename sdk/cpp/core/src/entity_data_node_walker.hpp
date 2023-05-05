//
// @file entity.hpp
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

#ifndef _WALKER_HPP_
#define _WALKER_HPP_

#include <vector>
#include <map>
#include "path_api.hpp"
#include "filters.hpp"

namespace ydk {

class Entity;

path::DataNode& get_data_node_from_entity(Entity & entity, path::RootSchemaNode & root_schema);

void get_entity_from_data_node(path::DataNode * node, std::shared_ptr<Entity> entity);

YFilter get_data_node_yfilter(path::DataNode * node);
}
#endif /* _WALKER_HPP_ */
