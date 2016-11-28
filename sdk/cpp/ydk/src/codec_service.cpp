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
#include <boost/log/trivial.hpp>

#include "codec_provider.hpp"
#include "codec_service.hpp"
#include "entity_data_node_walker.hpp"
#include "path_api.hpp"
#include "types.hpp"

namespace ydk
{

std::string REPO_ERROR_MSG{"YANG models stored in repository is not consistent with"
                           " bundle capabilities, please make sure all YANG models used"
                           " to generate bundle is stored in this repository."};

std::string PAYLOAD_ERROR_MSG{"Codec Service only support one entity per payload, please split paylaod"};

CodecService::CodecService()
{
}

CodecService::~CodecService()
{
}

std::string
CodecService::encode(CodecServiceProvider & provider, Entity & entity, bool pretty)
{
    path::RootSchemaNode* root_schema = provider.get_root_schema();
    try
    {
        path::DataNode* data_node = get_data_node_from_entity(entity, *root_schema);
        path::CodecService core_codec_service{};
        return core_codec_service.encode(data_node, provider.m_encoding, pretty);
    }
    catch (const YDKInvalidArgumentException& e)
    {
        BOOST_LOG_TRIVIAL(error) << REPO_ERROR_MSG;
        BOOST_THROW_EXCEPTION(YDKServiceProviderException(REPO_ERROR_MSG));
    }
    return {};
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

std::unique_ptr<Entity>
CodecService::decode(CodecServiceProvider & provider, std::string & payload)
{
	BOOST_LOG_TRIVIAL(debug) << "Decoding " << payload;
    std::unique_ptr<Entity> entity = provider.get_top_entity(payload);
    path::RootSchemaNode* root_schema = provider.get_root_schema();

    path::CodecService core_codec_service{};
    path::DataNode* root_data_node = core_codec_service.decode(root_schema, payload, provider.m_encoding);

    if (root_data_node->children().size() != 1)
    {
        BOOST_LOG_TRIVIAL(error) << PAYLOAD_ERROR_MSG;
        BOOST_THROW_EXCEPTION(YDKServiceProviderException(PAYLOAD_ERROR_MSG));
    }
    else
    {
        for (auto data_node: root_data_node->children())
        {
            get_entity_from_data_node(data_node, entity.get());
        }
    }
    return entity;
}

std::map<std::string, std::unique_ptr<Entity>>
CodecService::decode(CodecServiceProvider & provider, std::map<std::string, std::string> & payload_map)
{
    std::map<std::string, std::unique_ptr<Entity>> entity_map;
    for (auto it: payload_map)
    {
    	BOOST_LOG_TRIVIAL(debug) << "Decoding " << it.second;
        std::unique_ptr<Entity> entity = decode(provider, it.second);
        entity_map[it.first] = std::move(entity);
    }
    return entity_map;
}


}
