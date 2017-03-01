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

#include <libyang/libyang.h>

#include "codec_provider.hpp"
#include "entity_lookup.hpp"
#include "logger.hpp"

namespace ydk
{

#define USER_PROVIDED_REPO "ydk-user-provider-repo"

CodecServiceProvider::CodecServiceProvider(path::Repository & repo, EncodingFormat encoding)
    : m_encoding{encoding}, user_provided_repo(true), capabilities_augmented(false), m_repo(repo)
{
}

CodecServiceProvider::CodecServiceProvider(EncodingFormat encoding)
    : m_encoding{encoding}, user_provided_repo(false), capabilities_augmented(false)
{
}

CodecServiceProvider::~CodecServiceProvider()
{
}

void CodecServiceProvider::initialize(const std::string & bundle_name,
        const std::string & models_path, augment_capabilities_function augment_caps_function)
{
    if(!capabilities_augmented)
    {
        YLOG_TRACE("Augmenting global YDK capabilities for bundle {}", bundle_name);
        augment_caps_function();
    }

    if(user_provided_repo)
    {
        YLOG_TRACE("Using user provided repo");
        initialize_root_schema(USER_PROVIDED_REPO, m_repo);
        return;
    }

    if (m_root_schema_table.find(bundle_name) != m_root_schema_table.end())
    {
        return;
    }

    augment_caps_function();
    capabilities_augmented = true;
    YLOG_TRACE("Creating repo in path {}", models_path);
    path::Repository repo{models_path};
    initialize_root_schema(bundle_name, repo);
}

path::RootSchemaNode& CodecServiceProvider::get_root_schema(const std::string & bundle_name)
{
    if(user_provided_repo)
    {
        auto const & val = m_root_schema_table[USER_PROVIDED_REPO];
        return *(val);
    }

    if(m_root_schema_table.find(bundle_name) == m_root_schema_table.end())
    {
        YLOG_ERROR("Root schema not created");
        throw(YCPPServiceProviderError("Root schema not created"));
    }

    auto const & val = m_root_schema_table[bundle_name];
    return *(val);
}

void CodecServiceProvider::initialize_root_schema(const std::string & bundle_name, path::Repository & repo)
{
    YLOG_TRACE("Initializing root schema for {}", bundle_name);
    ly_verb(LY_LLSILENT); //turn off libyang logging at the beginning
    auto root_schema = std::unique_ptr<ydk::path::RootSchemaNode>(repo.create_root_schema(get_global_capabilities()));
    std::string s{bundle_name};
    std::pair<std::string, std::unique_ptr<path::RootSchemaNode>> entry {s, std::move(root_schema)};
    m_root_schema_table.insert(std::move(entry));
    ly_verb(LY_LLVRB); // enable libyang logging after payload has been created
}

#undef USER_PROVIDED_REPO
}
