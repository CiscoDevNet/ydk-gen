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

namespace ydk
{
CodecServiceProvider::CodecServiceProvider(path::Repository & repo, EncodingFormat encoding)
    : m_encoding{encoding}, m_repo{repo}
{
    augment_lookup_tables();
    ly_verb(LY_LLSILENT); //turn off libyang logging at the beginning
    m_root_schema = std::unique_ptr<ydk::path::RootSchemaNode>(m_repo.create_root_schema(get_global_capabilities()));
    ly_verb(LY_LLVRB); // enable libyang logging after payload has been created
}

CodecServiceProvider::~CodecServiceProvider()
{
}

path::RootSchemaNode&
CodecServiceProvider::get_root_schema()
{
    return *m_root_schema;
}


}
