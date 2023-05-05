/*  ----------------------------------------------------------------
 YDK - YANG Development Kit
 Copyright 2016 Cisco Systems. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 -------------------------------------------------------------------
 This file has been modified by Yan Gorelik, YDK Solutions.
 All modifications in original under CiscoDevNet domain
 introduced since October 2019 are copyrighted.
 All rights reserved under Apache License, Version 2.0.
 ------------------------------------------------------------------*/

#include <libyang/libyang.h>

#include "codec_provider.hpp"
#include "entity_lookup.hpp"
#include "ietf_parser.hpp"
#include "logger.hpp"

namespace ydk
{

CodecServiceProvider::CodecServiceProvider(path::Repository & repo, EncodingFormat encoding)
    : m_encoding{encoding}, user_provided_repo(true), capabilities_augmented(false), m_repo(repo)
{
  if (user_provided_repo)
  {
      YLOG_DEBUG("Initializing root schema in user provided repo");
      initialize_root_schema(USER_PROVIDED_REPO, m_repo);
      return;
  }
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
    if (!capabilities_augmented)
    {
        YLOG_DEBUG("Augmenting global YDK capabilities for bundle {}", bundle_name);
        augment_caps_function();
    }

    if (user_provided_repo)
    {
        YLOG_DEBUG("Using user provided repo");
        initialize_root_schema(USER_PROVIDED_REPO, m_repo);
        return;
    }

    if (m_root_schema_table.find(bundle_name) != m_root_schema_table.end())
    {
        return;
    }

    if (augment_caps_function)
    {
        augment_caps_function();
        capabilities_augmented = true;
    }
    YLOG_DEBUG("Creating repo in path {}", models_path);
    path::Repository repo{models_path};
    initialize_root_schema(bundle_name, repo);
}

path::RootSchemaNode& CodecServiceProvider::get_root_schema_for_bundle(const std::string & bundle_name)
{
    if(user_provided_repo)
    {
        auto const & val = m_root_schema_table[USER_PROVIDED_REPO];
        return *(val);
    }

    if(m_root_schema_table.find(bundle_name) == m_root_schema_table.end())
    {
        YLOG_ERROR("Root schema not created");
        throw(YServiceProviderError("Root schema not created"));
    }

    auto const & val = m_root_schema_table[bundle_name];
    return *(val);
}

void CodecServiceProvider::initialize_root_schema(const std::string & bundle_name, path::Repository & repo)
{
    YLOG_DEBUG("Initializing root schema for {}", bundle_name);
    ly_verb(LY_LLSILENT); //turn off libyang logging at the beginning

    IetfCapabilitiesParser capabilities_parser{};
    std::vector<std::string> empty_caps;
    auto yang_caps = capabilities_parser.parse(empty_caps);

    auto root_schema = repo.create_root_schema(get_global_capabilities_lookup_tables(), yang_caps);

    std::string s = (user_provided_repo) ? USER_PROVIDED_REPO : bundle_name;
    std::pair<std::string, std::shared_ptr<path::RootSchemaNode>> entry {s, root_schema};
    m_root_schema_table.insert(entry);
    ly_verb(LY_LLVRB); // enable libyang logging after payload has been created
}

#undef USER_PROVIDED_REPO
}
