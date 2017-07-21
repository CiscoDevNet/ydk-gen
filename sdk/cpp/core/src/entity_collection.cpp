//
// @file any_data.cpp
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

#include <vector>

#include "logger.hpp"
#include "types.hpp"

using namespace std;

namespace ydk
{
EntityCollection::EntityCollection()
{
}

EntityCollection::EntityCollection(std::vector<Entity*> data)
{
    for(auto d : data)
    {
        if(d!=nullptr)
        {
            entities[d->get_segment_path()] = shared_ptr<Entity>(d);
        }
    }
}

EntityCollection::EntityCollection(std::vector<std::shared_ptr<Entity>> data)
{
    for(auto d : data)
    {
        if(d!=nullptr)
        {
            entities[d->get_segment_path()] = d;
        }
    }
}

EntityCollection::~EntityCollection()
{
}

const std::map<std::string, std::shared_ptr<Entity>> & EntityCollection::get_entities() const
{
    return entities;
}
}
