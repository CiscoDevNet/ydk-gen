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

#ifndef CODEC_PROVIDER_HPP
#define CODEC_PROVIDER_HPP


#include "path_api.hpp"
#include "types.hpp"

namespace ydk
{
class TopEntityLookUp;

class CodecServiceProvider
{
public:
	CodecServiceProvider(path::Repository & repo, EncodingFormat encoding);
	~CodecServiceProvider();

	path::RootSchemaNode* get_root_schema();
	std::unique_ptr<Entity> get_top_entity(std::string & payload);

public:
	EncodingFormat m_encoding;

private:
	std::unique_ptr<path::RootSchemaNode> m_root_schema;
	path::Repository & m_repo;

};
}

#endif /* CODEC_PROVIDER_HPP */
