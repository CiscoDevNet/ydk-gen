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

#include "crud_service.hpp"
#include "logger.hpp"
#include "path_api.hpp"
#include "service_provider.hpp"
#include "types.hpp"
#include "validation_service.hpp"
#include "common_utilities.hpp"

using namespace std;

namespace ydk {

static shared_ptr<Entity>
       execute_rpc(ydk::ServiceProvider & provider, Entity & filter,
                   const string & operation, const string & data_tag, bool set_config_flag);

static vector<shared_ptr<Entity>>
       execute_rpc(ydk::ServiceProvider & provider, vector<Entity*> & filter_list,
            const string & operation, const string & data_tag, bool set_config_flag);

static bool operation_succeeded(shared_ptr<Entity> entity)
{
    YLOG_INFO("Operation {}", ((entity == nullptr)?"succeeded":"failed"));
    return entity == nullptr;
}

static bool operation_succeeded(vector<shared_ptr<Entity>> entity_list)
{
    YLOG_INFO("Operation {}", (entity_list.size() == 0) ? "succeeded" : "failed");
    return entity_list.size() == 0;
}

// Class CrudService implementation
//
CrudService::CrudService()
{
}

CrudService::~CrudService()
{
}

bool CrudService::create(ydk::ServiceProvider & provider, Entity & entity)
{
    YLOG_INFO("Executing CRUD create operation on [{}]", entity.get_segment_path());
    return operation_succeeded( execute_rpc(provider, entity, "ydk:create", "entity", false) );
}

bool CrudService::create(ydk::ServiceProvider & provider, vector<Entity*> & entity_list)
{
    YLOG_INFO("Executing CRUD create operation on {}", entity_vector_to_string(entity_list));
    return operation_succeeded(
            execute_rpc(provider, entity_list, "ydk:create", "entity", false)
            );
}

bool CrudService::update(ydk::ServiceProvider & provider, Entity & entity)
{
    YLOG_INFO("Executing CRUD update operation on [{}]", entity.get_segment_path());
    return operation_succeeded(
            execute_rpc(provider, entity, "ydk:update", "entity", false)
            );
}

bool CrudService::update(ydk::ServiceProvider & provider, vector<Entity*> & entity_list)
{
    YLOG_INFO("Executing CRUD create operation on {}", entity_vector_to_string(entity_list));
    return operation_succeeded(
            execute_rpc(provider, entity_list, "ydk:update", "entity", false)
            );
}

bool CrudService::delete_(ydk::ServiceProvider & provider, Entity & entity)
{
    YLOG_INFO("Executing CRUD delete operation on [{}]", entity.get_segment_path());
    return operation_succeeded(
            execute_rpc(provider, entity, "ydk:delete", "entity", false)
            );
}

bool CrudService::delete_(ydk::ServiceProvider & provider, vector<Entity*> & entity_list)
{
    YLOG_INFO("Executing CRUD delete operation on {}", entity_vector_to_string(entity_list));
    return operation_succeeded(
            execute_rpc(provider, entity_list, "ydk:delete", "entity", false)
            );
}

shared_ptr<Entity>
CrudService::read(ydk::ServiceProvider & provider, Entity & filter)
{
    YLOG_INFO("Executing CRUD read operation on [{}]", filter.get_segment_path());
    return execute_rpc(provider, filter, "ydk:read", "filter", false);
}

vector<shared_ptr<Entity>>
CrudService::read(ydk::ServiceProvider & provider, vector<Entity*> & filter_list)
{
    YLOG_INFO("Executing CRUD read operation on {}", entity_vector_to_string(filter_list));
    return execute_rpc(provider, filter_list, "ydk:read", "filter", false);
}

shared_ptr<Entity> CrudService::read_config(ydk::ServiceProvider & provider, Entity & filter)
{
    YLOG_INFO("Executing CRUD read_config operation on [{}]", filter.get_segment_path());
    return execute_rpc(provider, filter, "ydk:read", "filter", true);
}

vector<shared_ptr<Entity>>
CrudService::read_config(ydk::ServiceProvider & provider, vector<Entity*> & filter_list)
{
    YLOG_INFO("Executing CRUD read operation on {}", entity_vector_to_string(filter_list));
    return execute_rpc(provider, filter_list, "ydk:read", "filter", true);
}

//// end of class ydk::CrudService functions //////////////////////////////////

static shared_ptr<Entity>
execute_rpc(ydk::ServiceProvider & provider, Entity & entity,
            const string & operation, const string & data_tag, bool set_config_flag)
{
	vector<Entity*> input_list{};
	input_list.push_back(&entity);

	vector<shared_ptr<Entity>> output_list = execute_rpc(provider, input_list, operation, data_tag, set_config_flag);
	if (output_list.size() == 0)
		return nullptr;

	return output_list[0];
}

static vector<shared_ptr<Entity>>
execute_rpc(ydk::ServiceProvider & provider, vector<Entity*> & filter_list,
            const string & operation, const string & data_tag, bool set_config_flag)
{
    const path::Session& session = provider.get_session();
    path::RootSchemaNode& root_schema = session.get_root_schema();
    shared_ptr<ydk::path::Rpc> ydk_rpc{ root_schema.create_rpc(operation) };
    vector<shared_ptr<Entity>> result_list{};

    string xml_payload = "";
    for (Entity* entity : filter_list)
    {
        if (data_tag == "filter" && provider.get_encoding() == EncodingFormat::XML) {
            xml_payload += get_xml_subtree_filter_payload(*entity, provider);
        }
        else {
            xml_payload += get_data_payload(*entity, provider);
        }
    }

    if(set_config_flag)
    {
        ydk_rpc->get_input_node().create_datanode("only-config");
    }
    ydk_rpc->get_input_node().create_datanode(data_tag, xml_payload);

    // Get root data node
    shared_ptr<path::DataNode> rnd = (*ydk_rpc)(session);
    if (rnd == nullptr)
        return result_list;

    // Build mapping of entity path to entity; needed to calculate top_entity
    map<string,Entity*> path_to_entity{};
    for (Entity* entity : filter_list) {
        string internal_key = "/" + entity->get_segment_path();
        path_to_entity[internal_key] = entity;
    }

    // Return all children of the root node
    vector<shared_ptr<path::DataNode>> data_nodes = rnd->get_children();
    for (auto dn : data_nodes) {
        string internal_key = dn->get_path();
        Entity* entity;
        if(path_to_entity.find(internal_key) == path_to_entity.end())
        {
            YLOG_DEBUG("Searching for filter using yang name: {}", internal_key);
            bool found = false;
            for(auto e:path_to_entity)
            {
                if(internal_key.find(e.second->yang_name) != string::npos)
                {
                    YLOG_DEBUG("Found filter entity: {}", e.second->yang_name);
                    entity = e.second;
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                YLOG_ERROR("Could not find filter entity: {}", internal_key);
                throw(YServiceProviderError{"Could not find filter entity " + internal_key});
            }
        }
        else
        {
            entity = path_to_entity[internal_key];
        }
        result_list.push_back( read_datanode(*entity, dn));
    }
    return result_list;
}

}
