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

#ifndef CODEC_PROVIDER_HPP
#define CODEC_PROVIDER_HPP


#include "path_api.hpp"
#include "types.hpp"

namespace ydk
{
class TopEntityLookUp;
typedef void (*augment_capabilities_function)();
#define USER_PROVIDED_REPO "ydk-user-provider-repo"

class CodecServiceProvider
{
  public:
    CodecServiceProvider(EncodingFormat encoding);
    CodecServiceProvider(path::Repository & repo, EncodingFormat encoding);
    ~CodecServiceProvider();

    void initialize(const std::string & bundle_name, const std::string & models_path, augment_capabilities_function get_caps_func);
    path::RootSchemaNode& get_root_schema_for_bundle(const std::string & bundle_name);

  private:
    void initialize_root_schema(const std::string & bundle_name, path::Repository & repo);

  public:
    EncodingFormat m_encoding;

  private:
    std::map<std::string, std::shared_ptr<path::RootSchemaNode>> m_root_schema_table;
    bool user_provided_repo;
    bool capabilities_augmented;
    path::Repository m_repo;
};
}

#endif /* CODEC_PROVIDER_HPP */
