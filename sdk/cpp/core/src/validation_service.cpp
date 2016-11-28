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

#include "service.hpp"
#include "validation_service.hpp"
#include <map>
#include <boost/log/trivial.hpp>

using namespace std;

namespace ydk {

static void
validate_missing_keys(const EntityPath& entity_path,
                      const ydk::path::SchemaNode& schema_node,
                      EntityDiagnostic& diagnostic)
{
    auto keys = schema_node.keys();
    //create a map of the keys
    std::map<std::string, LeafData> name_value_map{};
    for(auto value_path : entity_path.value_paths) {
        name_value_map.insert(value_path);
    }

    //we need a key
    for(auto key : keys) {
        //check if the value list has the value's
        if(name_value_map.find(key.arg) == name_value_map.end()) {
            ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError> attr{};
            attr.source = key.arg;
            attr.errors.push_back(ydk::path::ValidationError::MISSELEM);
            diagnostic.attrs.push_back(std::move(attr));
        }
    }

}

static void
validate_attributes(const EntityPath& entity_path, const ydk::path::SchemaNode& schema_node,
                    EntityDiagnostic& diagnostic)
{
    //now validate the values in the value_path that have some values in them
    for(auto value_path : entity_path.value_paths) {
        if(value_path.second.value.empty())
            continue;
        auto leaf_schema_node_list = schema_node.find(value_path.first);
        if(leaf_schema_node_list.empty())
        {
            //the leaf is invalid or not present in the schema
            ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError> attr{};
            attr.source = value_path.first;
            attr.errors.push_back(ydk::path::ValidationError::SCHEMA_NOT_FOUND);
            diagnostic.attrs.push_back(std::move(attr));
        }
        else
        {
            ydk::path::SchemaNode* leaf_schema_node = leaf_schema_node_list[0];
            //now test to see if the value is correct
            ydk::path::SchemaValueType & type = leaf_schema_node->type();
	    auto attr = type.validate(value_path.second.value);
	    if(attr.has_errors())
            {
		attr.source = value_path.first;
		diagnostic.attrs.push_back(std::move(attr));
	    }
        }
    }

}

static bool
keyword_is_leaf(std::string & keyword)
{
    if(keyword == "leaf" || keyword == "leaf-list" || keyword == "anyxml")
        return true;

    return false;
}

static EntityDiagnostic
validate(const ydk::path::ServiceProvider& sp, ydk::Entity& entity, ydk::Entity* parent,
             ydk::ValidationService::Option option)
{

    EntityPath entity_path = entity.get_entity_path(parent);

    //validation checking
    //first check if the schema node that represents this path
    //actually exists and then throw an error

    auto root_schema_node = sp.get_root_schema();
    auto schema_node_list = root_schema_node->find(entity_path.path);

    EntityDiagnostic diagnostic{};
    diagnostic.source = &entity;

    if(schema_node_list.empty()) {
        diagnostic.errors.push_back(ydk::path::ValidationError::SCHEMA_NOT_FOUND);
        //no point processing children
        return diagnostic;

    }

    //schema node cannot be leaf, leaf-list or anyxml
    ydk::path::SchemaNode* schema_node = schema_node_list[0];
    auto stmt = schema_node->statement();
    if(keyword_is_leaf(stmt.keyword)) {
        diagnostic.errors.push_back(ydk::path::ValidationError::INVALID_USE_OF_SCHEMA);

    } else {
        // there is no error with the schema for this node
        // first check if is a list

        if(option == ValidationService::Option::EDIT_CONFIG || option == ValidationService::Option::DATASTORE) {

            if(stmt.keyword == "list") {
                auto keys = schema_node->keys();
                if(!keys.empty()) {
                    validate_missing_keys(entity_path, *schema_node, diagnostic);
                }
            }
            ydk::validate_attributes(entity_path, *schema_node, diagnostic);

        }
    }

    for(auto const & child_entity : entity.get_children()){
        EntityDiagnostic child_diagnostic = validate(sp, *(child_entity.second), &entity, option);
        diagnostic.children.push_back(child_diagnostic);
    }

    return diagnostic;

}

EntityDiagnostic
ValidationService::validate(const path::ServiceProvider& sp, Entity& entity,
                            ValidationService::Option option)
{

    return ydk::validate(sp, entity, entity.parent, option);

}

}
