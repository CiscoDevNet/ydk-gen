//
// @file types.hpp
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

#include <assert.h>
#include <iostream>

#include "entity_data_node_walker.hpp"
#include "entity_util.hpp"
#include "path_api.hpp"
#include "path/path_private.hpp"
#include "types.hpp"
#include "ydk_yang.hpp"
#include "logger.hpp"

using namespace std;

namespace ydk
{
static void populate_data_node(Entity & entity, path::DataNode & data_node);
static void walk_children(Entity & entity, path::DataNode & data_node);
static void populate_name_values(path::DataNode & parent_data_node, EntityPath & path);
static bool data_node_is_leaf(path::DataNode & data_node);
static bool data_node_is_list(path::DataNode & data_node);
static string get_segment_path(const string & path);
static void add_annotation_to_datanode(const Entity & entity, path::DataNode & data_node);
static void add_annotation_to_datanode(const std::pair<std::string, LeafData> & name_value, path::DataNode & data_node);
static path::Annotation get_annotation(EditOperation operation);


//////////////////////////////////////////////////////////////////////////
// DataNode* from Entity
//////////////////////////////////////////////////////////////////////////
path::DataNode& get_data_node_from_entity(Entity & entity, ydk::path::RootSchemaNode & root_schema)
{
    EntityPath root_path = entity.get_entity_path(nullptr);
    auto & root_data_node = root_schema.create(root_path.path);
    if(is_set(entity.operation))
    {
        add_annotation_to_datanode(entity, root_data_node);
    }

    YLOG_DEBUG("Root entity: {}", root_path.path);
    populate_name_values(root_data_node, root_path);
    walk_children(entity, root_data_node)
;
    return root_data_node;
}

static void walk_children(Entity & entity, path::DataNode & data_node)
{
	std::map<string, shared_ptr<Entity>> children = entity.get_children();
	YLOG_DEBUG("Children count for: {} : {}",entity.get_entity_path(entity.parent).path, children.size());
	for(auto const& child : children)
	{
		YLOG_DEBUG("==================");
		YLOG_DEBUG("Looking at child '{}': {}",child.first, child.second->get_entity_path(child.second->parent).path);
		if(child.second->has_operation() || child.second->has_data())
			populate_data_node(*(child.second), data_node);
		else
			YLOG_DEBUG("Child has no data and no operations");
	}
}

static void populate_data_node(Entity & entity, path::DataNode & parent_data_node)
{
    EntityPath path = entity.get_entity_path(entity.parent);
    path::DataNode* data_node = nullptr;

    data_node = &parent_data_node.create(path.path);

    if(is_set(entity.operation))
    {
        add_annotation_to_datanode(entity, *data_node);
    }

    populate_name_values(*data_node, path);
    walk_children(entity, *data_node);
}

static void populate_name_values(path::DataNode & data_node, EntityPath & path)
{
	YLOG_DEBUG("Leaf count: {}", path.value_paths.size());
	for(const std::pair<std::string, LeafData> & name_value : path.value_paths)
	{
		path::DataNode* result = nullptr;
		LeafData leaf_data = name_value.second;
		YLOG_DEBUG("Creating child {} of {} with value: '{}', is_set: {}", name_value.first, data_node.path(),
				leaf_data.value, leaf_data.is_set);

        if(leaf_data.is_set)
        {
            result = &data_node.create(name_value.first, leaf_data.value);
        }

        if(is_set(leaf_data.operation))
        {
            add_annotation_to_datanode(name_value, *result);
        }

        YLOG_DEBUG("Result: {}", (result?"success":"failure"));
        }
}

static void add_annotation_to_datanode(const Entity & entity, path::DataNode & data_node)
{
	YLOG_DEBUG("Got operation '{}' for {}", to_string(entity.operation), entity.yang_name);
	data_node.add_annotation(
							 get_annotation(entity.operation)
							 );
}

static void add_annotation_to_datanode(const std::pair<std::string, LeafData> & name_value, path::DataNode & data_node)
{
	YLOG_DEBUG("Got operation '{}' for {}", to_string(name_value.second.operation), name_value.first);
	data_node.add_annotation(
							 get_annotation(name_value.second.operation)
							 );
}

static path::Annotation get_annotation(EditOperation operation)
{
	if(operation == EditOperation::not_set)
		throw(YCPPInvalidArgumentError{"Invalid operation"});
	return {IETF_NETCONF_MODULE_NAME, "operation", to_string(operation)};
}

//////////////////////////////////////////////////////////////////////////
// Entity from DataNode*
//////////////////////////////////////////////////////////////////////////
void get_entity_from_data_node(path::DataNode * node, std::shared_ptr<Entity> entity)
{
	if (entity == nullptr || node == nullptr)
		return;

	for(auto & child_data_node:node->children())
	{
		std::string child_name = child_data_node->schema().statement().arg;
		if(data_node_is_leaf(*child_data_node))
		{
			YLOG_DEBUG("Creating leaf {} of value '{}' in parent {}", child_name,
					child_data_node->get(), node->path());
			entity->set_value(child_name, child_data_node->get());
		}
		else
		{
			YLOG_DEBUG("Going into child {} in parent {}", child_name, node->path());
			std::shared_ptr<Entity> child_entity;
			if(data_node_is_list(*child_data_node))
			{
				child_entity = entity->get_child_by_name(child_name, get_segment_path(child_data_node->path()));
			}
			else
			{
				child_entity = entity->get_child_by_name(child_name);
			}

			if(child_entity == nullptr)
			{
				YLOG_ERROR("Couldn't fetch child entity {} in parent {}!", child_name, node->path());
			}
			get_entity_from_data_node(child_data_node.get(), child_entity);
		}
	}
}

static bool data_node_is_leaf(path::DataNode & data_node)
{
    return (data_node.schema().statement().keyword == "leaf"
            || data_node.schema().statement().keyword == "leaf-list");
}

static bool data_node_is_list(path::DataNode & data_node)
{
    return (data_node.schema().statement().keyword == "list");
}

static string get_segment_path(const string & path)
{
    std::vector<std::string> segments = path::segmentalize(path);
    return segments.back();
}

}
