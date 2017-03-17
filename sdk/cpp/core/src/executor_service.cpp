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

#include <iostream>

#include "executor_service.hpp"
#include "types.hpp"
#include "path_api.hpp"
#include "entity_data_node_walker.hpp"
#include "logger.hpp"

using namespace std;

namespace ydk{

// static string get_data_payload(Entity & entity, string data_tag, path::RootSchemaNode & root_schema);
string get_netconf_payload(path::DataNode* input, string data_value, string data_tag);
static void walk_children(Entity* entity, path::DataNode & rpc_input,
    path::RootSchemaNode & root_schema, std::string path);
static void create_from_entity_path(Entity* entity, path::DataNode & rpc_input, std::string path);
static void create_from_children(std::map<string, Entity*> & children, path::DataNode & rpc_input,
    path::RootSchemaNode & root_schema);
static std::string get_data_payload(Entity& entity, path::RootSchemaNode& root_schema);
shared_ptr<Entity> get_top_entity_from_filter(Entity & filter);

ExecutorService::ExecutorService()
{

}

shared_ptr<Entity> ExecutorService::execute_rpc(NetconfServiceProvider & provider, Entity & rpc_entity)
{
    // Get the operation - RPC Name
    // auto operation = rpc_entity.get_entity_path(rpc_entity.parent).path;
    auto const & operation = rpc_entity.get_segment_path();

    // Create RPC instance
    path::RootSchemaNode & root_schema = provider.get_root_schema();
    shared_ptr<path::Rpc> rpc = root_schema.rpc(operation);
    path::DataNode & rpc_input = rpc->input();

    // Handle input
    auto input = rpc_entity.get_child_by_name("input", "");
    walk_children(input, rpc_input, root_schema, "");

    // Execute
    auto result_datanode = (*rpc)(provider);

    // Handle output
    auto output = rpc_entity.get_child_by_name("output", "");
    if (output != nullptr && result_datanode != nullptr)
    {
        auto filter = output->get_children()[0];

        shared_ptr<Entity> top_entity = get_top_entity_from_filter(*filter);
        get_entity_from_data_node(result_datanode->children()[0].get(), top_entity.get());
        return top_entity;
    }
    else
        return nullptr;
}

static void walk_children(Entity* entity, path::DataNode & rpc_input, path::RootSchemaNode & root_schema,
    std::string path)
{
    if (entity != nullptr)
    {
        std::map<string, Entity*> & children = entity->get_children();
        auto entity_path = entity->get_entity_path(entity->parent);
        YLOG_TRACE("Children count for: {} : {}", entity_path.path, children.size());

        if (path != "")
            path = path + '/';

        if (entity_path.path != "input")
            path = path + entity_path.path;

        if (entity_path.value_paths.size() == 0)
        {
            for( auto const & child : children )
                walk_children(child.second, rpc_input, root_schema, path);
        }
        else
            create_from_entity_path(entity, rpc_input, path);

        create_from_children(children, rpc_input, root_schema);
    }
}

static void create_from_entity_path(Entity* entity, path::DataNode & rpc_input, std::string path)
{
    auto entity_path = entity->get_entity_path(entity->parent);

    for (std::pair<std::string, LeafData> child : entity_path.value_paths)
    {
        YLOG_TRACE("Creating leaf '{}' in {}", child.first, entity_path.path);

        std::string temp_path = "";
        if (path != "")
            temp_path = path + '/';
        temp_path = temp_path + child.first;
        rpc_input.create(temp_path, child.second.value);
    }
}

static void create_from_children(std::map<string, Entity*> & children, path::DataNode & rpc_input,
    path::RootSchemaNode & root_schema)
{
    for( auto const & child : children )
    {
        if ( child.second->get_children().size() == 0 )
        {
            YLOG_TRACE("Creating child '{}': {}",child.first, child.second->get_entity_path(child.second->parent).path);

            std::string payload = get_data_payload(*child.second, root_schema);
            rpc_input.create(child.first, payload);
        }
    }
}

static std::string get_data_payload(Entity & entity, path::RootSchemaNode & root_schema)
{
    path::DataNode& data_node = get_data_node_from_entity(entity, root_schema);
    path::CodecService codec{};
    return codec.encode(data_node, ydk::EncodingFormat::XML, true);
}

shared_ptr<Entity> get_top_entity_from_filter(Entity & filter)
{
    if(filter.parent == nullptr)
        return filter.clone_ptr();

    return get_top_entity_from_filter(*(filter.parent));
}

}
