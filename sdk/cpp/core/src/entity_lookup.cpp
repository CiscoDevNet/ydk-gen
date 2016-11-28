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

#include "entity_lookup.hpp"
#include "path_api.hpp"

namespace ydk
{

TopEntityLookUp ydk_top_entities_table;
std::vector<path::Capability> ydk_global_capabilities;

TopEntityLookUp::TopEntityLookUp ()
{
    m_entities = std::map<std::string, std::unique_ptr<Entity>>{};
}

TopEntityLookUp::~TopEntityLookUp () {}

std::unique_ptr<Entity>
TopEntityLookUp::lookup(const std::string & path)
{
    return m_entities.at(path)->clone_ptr();
}

void
TopEntityLookUp::insert(std::string path, std::unique_ptr<Entity> top_entity)
{
    m_entities[path] = std::move(top_entity);
}

std::unique_ptr<Entity> lookup_top_entity(const std::string & lookup_key)
{
	return ydk_top_entities_table.lookup(lookup_key);
}

const std::vector<path::Capability> get_global_capabilities()
{
	return ydk_global_capabilities;
}

}
