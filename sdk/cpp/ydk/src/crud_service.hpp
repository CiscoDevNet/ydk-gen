//
// @file crud_service.hpp
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

#ifndef CRUD_SERVICE_HPP
#define CRUD_SERVICE_HPP

#include <map>
#include <memory>
#include <string>
#include "service.hpp"

namespace ydk
{

namespace path
{
class DataNode;
class ServiceProvider;
}

class Entity;

class CrudService : public Service
{
	public:
		CrudService();

		bool create(path::ServiceProvider & provider, Entity & entity);
		bool create(path::ServiceProvider & provider, std::map<std::string, Entity*> entity_map);

		bool update(path::ServiceProvider & provider, Entity & entity);
		bool update(path::ServiceProvider & provider, std::map<std::string, Entity*> entity_map);

		bool delete_(path::ServiceProvider & provider, Entity & entity);
		bool delete_(path::ServiceProvider & provider, std::map<std::string, Entity*> entity_map);

		std::unique_ptr<Entity> read(path::ServiceProvider & provider, Entity & filter);
		std::map<std::string, std::unique_ptr<Entity> > read(path::ServiceProvider & provider, std::map<std::string, Entity*> entity);

		std::unique_ptr<Entity> read_config(path::ServiceProvider & provider, Entity & filter);
		std::map<std::string, std::unique_ptr<Entity> > read_config(path::ServiceProvider & provider, std::map<std::string, Entity*> entity);

	private:
		std::unique_ptr<Entity> read(Entity & filter, path::DataNode* read_data_node);
		std::map<std::string, std::unique_ptr<Entity> > read(std::map<std::string, Entity*> filter_map, path::DataNode* read_data_node);
};

}

#endif /* CRUD_SERVICE_HPP */
