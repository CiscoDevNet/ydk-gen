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
static void add_annotation_to_datanode(const pair<string, LeafData> & name_value, path::DataNode & data_node);
static path::Annotation get_annotation(YFilter yfilter);


//////////////////////////////////////////////////////////////////////////
// DataNode* from Entity
//////////////////////////////////////////////////////////////////////////
path::DataNode& get_data_node_from_entity(Entity & entity, path::RootSchemaNode & root_schema)
{
    EntityPath root_path = get_entity_path(entity, nullptr);
    auto & root_data_node = root_schema.create_datanode(root_path.path);
    if(is_set(entity.yfilter))
    {
        add_annotation_to_datanode(entity, root_data_node);
    }

    YLOG_DEBUG("Root entity: {}", root_path.path);
    populate_name_values(root_data_node, root_path);
    walk_children(entity, root_data_node);
    return root_data_node;
}

static void walk_children(Entity & entity, path::DataNode & data_node)
{
    map<string, shared_ptr<Entity>> children = entity.get_children();
    vector<string> order = entity.get_order_of_children();
    YLOG_DEBUG("Children count for: {} : {}",get_entity_path(entity, entity.parent).path, children.size());
    YLOG_DEBUG("Children order count : {}",order.size());
    if(order.size()>0)
    {
        for(auto child_seg : order)
        {
            YLOG_DEBUG("Inserting in order for child segpath path: {}", child_seg);
            auto child = children[child_seg];
            if(child == nullptr)
            {
                YLOG_DEBUG("Child '{}' is null", child_seg);
                continue;
            }
            YLOG_DEBUG("==================");
            YLOG_DEBUG("Looking at child '{}': {}", child_seg, get_entity_path(*(child), child->parent).path);
            if(child->has_operation() || child->has_data() || child->is_presence_container)
                populate_data_node(*(child), data_node);
            else
                YLOG_DEBUG("Child has no data and no operations");

        }
    }
    else
    {
        for(auto const& child : children)
        {
            if(child.second == nullptr)
                continue;
            YLOG_DEBUG("==================");
            YLOG_DEBUG("Looking at child '{}': {}",child.first, get_entity_path(*(child.second), child.second->parent).path);
            if(child.second->has_operation() || child.second->has_data() || child.second->is_presence_container)
                populate_data_node(*(child.second), data_node);
            else
                YLOG_DEBUG("Child has no data and no operations");
        }
    }
}

static void populate_data_node(Entity & entity, path::DataNode & parent_data_node)
{
    EntityPath path = get_entity_path(entity, entity.parent);
    path::DataNode* data_node = &parent_data_node.create_datanode(path.path);

    if(is_set(entity.yfilter))
    {
        add_annotation_to_datanode(entity, *data_node);
    }

    populate_name_values(*data_node, path);
    walk_children(entity, *data_node);
}

static void populate_name_values(path::DataNode & data_node, EntityPath & path)
{
    YLOG_DEBUG("Leaf count: {}", path.value_paths.size());
    for(const pair<string, LeafData> & name_value : path.value_paths)
    {
        LeafData leaf_data = name_value.second;
        YLOG_DEBUG("Creating leaf {} of {} with value: '{}', is_set: {}", name_value.first, data_node.get_path(),
                leaf_data.value, leaf_data.is_set);

        if(leaf_data.is_set)
        {
            YLOG_DEBUG("Creating datanode");
            auto & result = data_node.create_datanode(name_value.first, leaf_data.value);
            YLOG_DEBUG("Created datanode");

            if(is_set(leaf_data.yfilter))
            {
                add_annotation_to_datanode(name_value, result);
            }
        }
    }
}

static void add_annotation_to_datanode(const Entity & entity, path::DataNode & data_node)
{
    YLOG_DEBUG("Got yfilter '{}' for {}", to_string(entity.yfilter), entity.yang_name);
    if (entity.yfilter != YFilter::read)
    {
        data_node.add_annotation(
                                 get_annotation(entity.yfilter)
                                 );
        YLOG_DEBUG("Set yfilter '{}' for {}", to_string(entity.yfilter), entity.yang_name);

    }
}

static void add_annotation_to_datanode(const pair<string, LeafData> & name_value, path::DataNode & data_node)
{
    YLOG_DEBUG("Got yfilter '{}' for {}", to_string(name_value.second.yfilter), name_value.first);
    if (name_value.second.yfilter != YFilter::read)
    {
        data_node.add_annotation(
                                 get_annotation(name_value.second.yfilter)
                                 );
        YLOG_DEBUG("Set yfilter '{}' for {}", to_string(name_value.second.yfilter), name_value.first);
    }
}

static path::Annotation get_annotation(YFilter yfilter)
{
    if(yfilter == YFilter::not_set)
        throw(YInvalidArgumentError{"Invalid operation"});
    return {IETF_NETCONF_MODULE_NAME, "operation", to_string(yfilter)};
}

//////////////////////////////////////////////////////////////////////////
// Entity from DataNode*
//////////////////////////////////////////////////////////////////////////
void get_entity_from_data_node(path::DataNode * node, shared_ptr<Entity> entity)
{
    if (entity == nullptr || node == nullptr)
        return;

    YLOG_DEBUG("Looking at parent entity {}", entity->yang_name);
    string module_name = node->get_schema_node().get_statement().module_name;
    for(auto & child_data_node:node->get_children())
    {
        string child_name = child_data_node->get_schema_node().get_statement().arg;
        string child_module_name = child_data_node->get_schema_node().get_statement().module_name;
        if(module_name != child_module_name)
        {
            child_name = child_module_name + ":" + child_name;
        }

        YLOG_DEBUG("Looking at child {} '{}'", child_data_node->get_schema_node().get_statement().keyword, child_data_node->get_path());
        
        if(data_node_is_leaf(*child_data_node))
        {
            YLOG_DEBUG("Creating entity leaf {} of value '{}' in parent {}", child_name,
                    child_data_node->get_value(), node->get_path());
            entity->set_value(child_name, child_data_node->get_value());
            YLOG_DEBUG("Created leaf");
        }
        else
        {
            YLOG_DEBUG("Going into child {} in parent {}", child_name, node->get_path());
            shared_ptr<Entity> child_entity;
            if(data_node_is_list(*child_data_node))
            {
                child_entity = entity->get_child_by_name(child_name, get_segment_path(child_data_node->get_path()));
            }
            else
            {
                child_entity = entity->get_child_by_name(child_name);
            }

            if(child_entity == nullptr)
            {
                YLOG_ERROR("Couldn't fetch child entity {} in parent {}!", child_name, node->get_path());
                throw(path::YCoreError{"Couldn't fetch child entity '" + child_name + "' in parent " + node->get_path()});
            }
            child_entity->parent = entity.get();
            get_entity_from_data_node(child_data_node.get(), child_entity);
        }
    }
}

static bool data_node_is_leaf(path::DataNode & data_node)
{
    return (data_node.get_schema_node().get_statement().keyword == "leaf"
            || data_node.get_schema_node().get_statement().keyword == "leaf-list");
}

static bool data_node_is_list(path::DataNode & data_node)
{
    return (data_node.get_schema_node().get_statement().keyword == "list");
}

static string get_segment_path(const string & path)
{
    vector<string> segments = path::segmentalize(path);
    return segments.back();
}

}
