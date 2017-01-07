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

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/xpressive/xpressive.hpp>
#include "codec_provider.hpp"
#include "entity_lookup.hpp"

namespace re = boost::xpressive;

namespace ydk
{
static std::string get_xml_lookup_key(std::string & payload);
static std::string get_json_lookup_key(std::string & payload);

re::sregex XML_LOOKUP_KEY = re::sregex::compile("<(?P<entity>[^ ]+) xmlns=\"(?P<ns>[^\"]+)\"");
re::sregex JSON_LOOKUP_KEY = re::sregex::compile("{[^\"]+\"(?P<key>[^\"]+)\"");
std::string ERROR_MSG{"Failed to find namespace from %1% payload,"
                      " please make sure payload format is consistent with encoding format."};

CodecServiceProvider::CodecServiceProvider(path::Repository & repo, EncodingFormat encoding)
    : m_encoding{encoding}, m_repo{repo}
{
    augment_lookup_tables();
    m_root_schema = std::unique_ptr<ydk::path::RootSchemaNode>(m_repo.create_root_schema(get_global_capabilities()));
}

CodecServiceProvider::~CodecServiceProvider()
{
}

path::RootSchemaNode*
CodecServiceProvider::get_root_schema()
{
    return m_root_schema.get();
}

std::unique_ptr<Entity>
CodecServiceProvider::get_top_entity(std::string & payload)
{
    if (m_encoding == EncodingFormat::XML)
    {
        return lookup_top_entity(get_xml_lookup_key(payload));
    }
    else
    {
        return lookup_top_entity(get_json_lookup_key(payload));
    }
}

static std::string get_xml_lookup_key(std::string & payload)
{
    std::string lookup_key;
    re::smatch what;
    if (re::regex_search(payload, what, XML_LOOKUP_KEY))
    {
        lookup_key += what["ns"];
        lookup_key += ":";
        lookup_key += what["entity"];
    }
    else
    {
        std::string error_msg{boost::str(boost::format(ERROR_MSG) % "XML")};
        BOOST_LOG_TRIVIAL(error) << error_msg;
        BOOST_THROW_EXCEPTION(YCPPServiceProviderError(error_msg));
    }
    return lookup_key;
}

static std::string get_json_lookup_key(std::string & payload)
{
    std::string lookup_key;
    re::smatch what;
    if (re::regex_search(payload, what, JSON_LOOKUP_KEY))
    {
        lookup_key += "/";
        lookup_key += what["key"];
    }
    else
    {
        std::string error_msg{boost::str(boost::format(ERROR_MSG) % "JSON")};
        BOOST_LOG_TRIVIAL(error) << error_msg;
        BOOST_THROW_EXCEPTION(YCPPServiceProviderError(error_msg));
    }
    return lookup_key;
}


}
