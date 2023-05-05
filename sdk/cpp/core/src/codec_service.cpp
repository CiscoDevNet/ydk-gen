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

#include "codec_provider.hpp"
#include "codec_service.hpp"
#include "entity_data_node_walker.hpp"
#include "path_api.hpp"
#include "types.hpp"
#include "logger.hpp"
#include "xml_subtree_codec.hpp"

namespace ydk
{

const char * PAYLOAD_ERROR_MSG ="Codec service only supports one entity per payload, please split payload";

static augment_capabilities_function get_augment_capabilities_function(Entity & entity);
static std::string get_bundle_yang_models_location(Entity & entity);
static std::string get_bundle_name(Entity & entity);
static path::RootSchemaNode & get_root_schema(CodecServiceProvider & provider, Entity & entity);


CodecService::CodecService()
{
}

CodecService::~CodecService()
{
}

std::string
CodecService::encode(CodecServiceProvider & provider, Entity & entity, bool pretty, bool subtree)
{
    path::RootSchemaNode& root_schema = get_root_schema(provider, entity);
    if(subtree)
    {
        if(provider.m_encoding != EncodingFormat::XML)
        {
            YLOG_ERROR("Subtree option can only be used with XML encoding");
            throw(YServiceProviderError("Subtree option can only be used with XML encoding"));
        }
        XmlSubtreeCodec codec{};
        return codec.encode(entity, root_schema);
    }

    path::DataNode& datanode = get_data_node_from_entity(entity, root_schema);
    const path::DataNode* dn = &datanode;
    while(dn!= nullptr && dn->get_parent()!=nullptr)
        dn = dn->get_parent();
    path::Codec core_codec_service{};
    std::string result = core_codec_service.encode(*dn, provider.m_encoding, pretty);
    YLOG_INFO("Performing encode operation, resulting in {}", result);
    return result;
}

std::map<std::string, std::string>
CodecService::encode(CodecServiceProvider & provider, std::map<std::string, std::unique_ptr<Entity>> & entity_map, bool pretty)
{
    std::map<std::string, std::string> payload_map;
    for (auto it = entity_map.begin(); it != entity_map.end(); ++it)
    {
        std::string payload = encode(provider, *(it->second), pretty);
        payload_map[it->first] = payload;
    }
    return payload_map;
}

std::shared_ptr<Entity>
CodecService::decode(CodecServiceProvider & provider, const std::string & payload, std::shared_ptr<Entity> entity, bool subtree)
{
    if(subtree)
    {
        if(provider.m_encoding != EncodingFormat::XML)
        {
            YLOG_ERROR("Subtree option can only be used with XML encoding");
            throw(YServiceProviderError("Subtree option can only be used with XML encoding"));
        }
        XmlSubtreeCodec codec{};
        return codec.decode(payload, entity);
    }

    path::RootSchemaNode& root_schema = get_root_schema(provider, *entity);

    YLOG_INFO("Performing decode operation on {}", payload);

    path::Codec core_codec_service{};
    auto root_data_node = core_codec_service.decode(root_schema, payload, provider.m_encoding);

    if (root_data_node->get_children().size() != 1)
    {
        YLOG_ERROR(PAYLOAD_ERROR_MSG);
        throw(YServiceProviderError(PAYLOAD_ERROR_MSG));
    }
    else
    {
        for (auto data_node: root_data_node->get_children())
        {
            get_entity_from_data_node(data_node.get(), entity);
            // Required for validation of decoded entity
            get_data_node_from_entity(*entity, root_schema);
        }
    }
    return entity;
}

std::map<std::string, std::shared_ptr<Entity>>
CodecService::decode(CodecServiceProvider & provider, std::map<std::string, std::string> & payload_map,
        std::map<std::string, std::shared_ptr<Entity>> entity_map)
{
    for (auto it: payload_map)
    {
        std::shared_ptr<Entity> entity = decode(provider, it.second, entity_map[it.first]);
        entity_map[it.first] = entity;
    }
    return entity_map;
}

static augment_capabilities_function get_augment_capabilities_function(Entity & entity)
{
    if(entity.parent == nullptr)
        return entity.get_augment_capabilities_function();

    return get_augment_capabilities_function(*(entity.parent));
}

static std::string get_bundle_yang_models_location(Entity & entity)
{
    if(entity.parent == nullptr)
        return entity.get_bundle_yang_models_location();

    return get_bundle_yang_models_location(*(entity.parent));
}

static std::string get_bundle_name(Entity & entity)
{
    if(entity.parent == nullptr)
        return entity.get_bundle_name();

    return get_bundle_name(*(entity.parent));
}

static path::RootSchemaNode & get_root_schema(CodecServiceProvider & provider, Entity & entity)
{
    auto const & bundle_name = get_bundle_name(entity);
    provider.initialize(bundle_name, get_bundle_yang_models_location(entity), get_augment_capabilities_function(entity));

    path::RootSchemaNode& root_schema = provider.get_root_schema_for_bundle(bundle_name);
    return root_schema;
}
}
