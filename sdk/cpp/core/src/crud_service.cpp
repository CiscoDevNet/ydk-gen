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

#include <boost/log/trivial.hpp>

#include "crud_service.hpp"
#include "types.hpp"
#include "path_api.hpp"
#include "entity_data_node_walker.hpp"

using namespace std;

namespace ydk {
static string get_data_payload(Entity & entity, path::RootSchemaNode & root_schema);
static path::DataNode* execute_rpc(path::ServiceProvider & provider, Entity & entity,
		const string & operation, const string & data_tag, bool set_config_flag=false);
static path::DataNode* execute_rpc(path::ServiceProvider & provider, map<string, Entity*> entity_map,
		const string & operation, const string & data_tag, bool set_config_flag=false);
static unique_ptr<Entity> get_top_entity_from_filter(Entity & filter);
static bool operation_succeeded(path::DataNode * node);

CrudService::CrudService()
{
}

bool CrudService::create(path::ServiceProvider & provider, Entity & entity)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD create operation";
	return operation_succeeded(
			execute_rpc(provider, entity, "ydk:create", "entity")
			);
}

bool CrudService::create(path::ServiceProvider & provider, map<string, Entity*> entity_map)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD create operation";
	return operation_succeeded(
			execute_rpc(provider, entity_map, "ydk:create", "entity")
			);
}

bool CrudService::update(path::ServiceProvider & provider, Entity & entity)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD update operation";
	return operation_succeeded(
			execute_rpc(provider, entity, "ydk:update", "entity")
			);
}

bool CrudService::update(path::ServiceProvider & provider, map<string, Entity*> entity_map)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD update operation";
	return operation_succeeded(
			execute_rpc(provider, entity_map, "ydk:update", "entity")
			);
}

bool CrudService::delete_(path::ServiceProvider & provider, Entity & entity)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD delete operation";
	return operation_succeeded(
			execute_rpc(provider, entity, "ydk:delete", "entity")
			);
}

bool CrudService::delete_(path::ServiceProvider & provider, map<string, Entity*> entity_map)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD delete operation";
	return operation_succeeded(
			execute_rpc(provider, entity_map, "ydk:delete", "entity")
			);
}

unique_ptr<Entity> CrudService::read(path::ServiceProvider & provider, Entity & filter)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD read operation";
	path::DataNode* read_data_node = execute_rpc(provider, filter, "ydk:read", "filter");
	return read(filter, read_data_node);
}

map<string, unique_ptr<Entity> > CrudService::read(path::ServiceProvider & provider, map<string, Entity*> filter_map)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD read operation";
	path::DataNode* read_data_node = execute_rpc(provider, filter_map, "ydk:read", "filter");
	return read(filter_map, read_data_node);
}

unique_ptr<Entity> CrudService::read_config(path::ServiceProvider & provider, Entity & filter)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD config read operation";
	path::DataNode* read_data_node = execute_rpc(provider, filter, "ydk:read", "filter", true);
	return read(filter, read_data_node);
}

map<string, unique_ptr<Entity> > CrudService::read_config(path::ServiceProvider & provider, map<string, Entity*> filter_map)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing CRUD config read operation";
	path::DataNode* read_data_node = execute_rpc(provider, filter_map, "ydk:read", "filter", true);
	return read(filter_map, read_data_node);
}

unique_ptr<Entity> CrudService::read(Entity & filter, path::DataNode* read_data_node)
{
	if (read_data_node == nullptr)
		return {};
	unique_ptr<Entity> top_entity = get_top_entity_from_filter(filter);
	get_entity_from_data_node(read_data_node->children()[0], top_entity.get());
	return top_entity;
}

map<string, unique_ptr<Entity> > CrudService::read(map<string, Entity*> filter_map, path::DataNode* read_data_node)
{
	if (read_data_node == nullptr)
		return {};
	int index= 0;
	map<string, unique_ptr<Entity> > entity_map;
	for(auto entry : filter_map)
	{
		unique_ptr<Entity> top_entity = get_top_entity_from_filter(*(entry.second));
		get_entity_from_data_node(read_data_node->children()[index++], top_entity.get());
		entity_map[entry.first] = move(top_entity);
	}
    return entity_map;
}

static bool operation_succeeded(path::DataNode * node)
{
	BOOST_LOG_TRIVIAL(debug) << "Operation " << ((node == nullptr)?"succeeded":"failed");
	return node == nullptr;
}

static unique_ptr<Entity> get_top_entity_from_filter(Entity & filter)
{
	if(filter.parent == nullptr)
		return filter.clone_ptr();

	return get_top_entity_from_filter(*(filter.parent));
}

static path::DataNode* execute_rpc(path::ServiceProvider & provider, Entity & entity,
		const string & operation, const string & data_tag, bool set_config_flag)
{
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };
	string data = get_data_payload(entity, *root_schema);

	if(set_config_flag)
	{
		ydk_rpc->input()->create("only-config");
	}
	ydk_rpc->input()->create(data_tag, data);
	return (*ydk_rpc)(provider);
}

static path::DataNode* execute_rpc(path::ServiceProvider & provider, map<string, Entity*> entity_map,
		const string & operation, const string & data_tag, bool set_config_flag)
{
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };
	string data;

	for(auto entry : entity_map)
	{
	    data += get_data_payload(*(entry.second), *root_schema);
	}

	BOOST_LOG_TRIVIAL(debug) << "Payload to be sent in RPC";
    BOOST_LOG_TRIVIAL(debug) << data<<endl;

	if(set_config_flag)
	{
		ydk_rpc->input()->create("only-config");
	}
	ydk_rpc->input()->create(data_tag, data);

    path::CodecService codec{};
    string pl = codec.encode(ydk_rpc->input(), ydk::path::CodecService::Format::XML, true);
    BOOST_LOG_TRIVIAL(debug) << "Payload to be sent in RPC";
    BOOST_LOG_TRIVIAL(debug) << data<<endl;

	return (*ydk_rpc)(provider);
}

static string get_data_payload(Entity & entity, path::RootSchemaNode & root_schema)
{
	const ydk::path::DataNode* data_node = get_data_node_from_entity(entity, root_schema);
	if (data_node==nullptr)
		return "";
	path::CodecService codec{};
	return codec.encode(data_node, ydk::path::CodecService::Format::XML, true);
}

}
