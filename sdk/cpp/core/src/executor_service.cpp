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

#include "entity_data_node_walker.hpp"
#include "entity_util.hpp"
#include "executor_service.hpp"
#include "logger.hpp"
#include "service_provider.hpp"
#include "types.hpp"


using namespace std;

namespace ydk{

static void walk_children(std::shared_ptr<Entity> entity, path::DataNode & rpc_input, std::string path);
static void create_from_entity_path(std::shared_ptr<Entity> entity, path::DataNode & rpc_input, const std::string & path);
static void create_from_children(std::map<string, std::shared_ptr<Entity>> & children, path::DataNode & rpc_input);

ExecutorService::ExecutorService()
{
}


ExecutorService::~ExecutorService()
{
}

shared_ptr<Entity> ExecutorService::execute_rpc(ServiceProvider& provider,
    Entity & rpc_entity, std::shared_ptr<Entity> top_entity)
{
    // Get the operation - RPC Name
    auto const & operation = rpc_entity.get_segment_path();

    // Create RPC instance
    path::RootSchemaNode & root_schema = provider.get_session().get_root_schema();
    shared_ptr<path::Rpc> rpc = root_schema.create_rpc(operation);
    path::DataNode & rpc_input = rpc->get_input_node();

    // Handle input
    auto input = rpc_entity.get_child_by_name("input", "");
    if(input != nullptr && (input->has_operation() || input->has_data() || input->is_presence_container))
        walk_children(input, rpc_input, "");

    // Execute
    auto result_datanode = (*rpc)(provider.get_session());

    // Handle output
    auto output = rpc_entity.get_child_by_name("output", "");
    if (output != nullptr && result_datanode != nullptr)
    {
        auto filter = result_datanode->get_children()[0].get();

        get_entity_from_data_node(filter, top_entity);
        return top_entity;
    }
    else
        return nullptr;
}

static void walk_children(std::shared_ptr<Entity> entity, path::DataNode & rpc_input, std::string path)
{
    if (entity != nullptr)
    {
        std::map<string, std::shared_ptr<Entity>> children = entity->get_children();
        auto entity_path = get_entity_path(*entity, entity->parent);
        YLOG_DEBUG("Children count for: {} : {}", entity_path.path, children.size());

        if (path != "")
            path = path + '/';

        if (entity_path.path != "input")
            path = path + entity_path.path;

        YLOG_DEBUG("Path: {}", path);

        for( auto const & child : children )
        {
            if(child.second->has_operation() || child.second->has_data() || child.second->is_presence_container)
                walk_children(child.second, rpc_input, path);
        }

        // if there are leafs, create from entity path
        if (entity_path.value_paths.size() != 0)
            create_from_entity_path(entity, rpc_input, path);

        create_from_children(children, rpc_input);
    }
}

static void create_from_entity_path(std::shared_ptr<Entity> entity, path::DataNode & rpc_input, const std::string & path)
{
    auto entity_path = get_entity_path(*entity, entity->parent);

    for (std::pair<std::string, LeafData> child : entity_path.value_paths)
    {
        YLOG_DEBUG("Creating leaf '{}' in {}", child.first, entity_path.path);

        std::string temp_path = "";
        if (path != "")
            temp_path = path + '/';
        temp_path = temp_path + child.first;
        rpc_input.create_datanode(temp_path, child.second.value);
    }
}

static void create_from_children(std::map<string, std::shared_ptr<Entity>> & children, path::DataNode & rpc_input)
{
    for( auto const & child : children )
    {
        if(child.second->has_operation() || child.second->has_data() || child.second->is_presence_container)
        {
            YLOG_DEBUG("Creating child '{}': {}",child.first,
                get_entity_path(*(child.second), child.second->parent).path);

            rpc_input.create_datanode(child.first);
        }
    }
}

}
