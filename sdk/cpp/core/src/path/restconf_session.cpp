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
#include <sstream>
#include <memory>

#include <libyang/libyang.h>

#include "../entity_data_node_walker.hpp"
#include "../errors.hpp"
#include "../ietf_parser.hpp"
#include "../restconf_client.hpp"
#include "../types.hpp"
#include "../logger.hpp"


using namespace std;

namespace ydk
{

namespace path
{
static const std::string default_capabilities_url = "/ietf-restconf-monitoring:restconf-state/capabilities";

static std::shared_ptr<path::DataNode> handle_read_reply(const string & reply, path::RootSchemaNode & root_schema, EncodingFormat encoding);
static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode & root_schema, const string & operation);
static string get_encoding_string(EncodingFormat encoding);

static bool is_config(path::Rpc & rpc);
static string get_module_url_path(const string & path);

RestconfSession::RestconfSession(path::Repository & repo,
                                 const string & address,
                                 const string & username,
                                 const string & password,
                                 int port,
                                 EncodingFormat encoding,
                                 const string & config_url_root,
                                 const string & state_url_root)
    : client(make_shared<RestconfClient>(address,
                                         username,
                                         password,
                                         port,
                                         get_encoding_string(encoding))),
      encoding(encoding),
      config_url_root(config_url_root),
      state_url_root(state_url_root)
{
    edit_method = "PATCH";
    initialize(repo);
}

RestconfSession::RestconfSession(std::shared_ptr<RestconfClient> client,
                                 const std::shared_ptr<ydk::path::RootSchemaNode>& root_schema,
                                 const std::string & edit_method,
                                 EncodingFormat encoding,
                                 const std::string & config_url_root,
                                 const std::string & state_url_root)
    : client(move(client)),
      root_schema(root_schema),
      encoding(encoding),
      edit_method(edit_method),
      config_url_root(config_url_root),
      state_url_root(state_url_root)
{
}

void RestconfSession::initialize(path::Repository & repo)
{
    vector<path::Capability> capabilities;
    IetfCapabilitiesParser capabilities_parser{};
    IetfCapabilitiesXmlParser capabilities_xml_parser{};
    edit_method = "PATCH";
    server_capabilities = capabilities_xml_parser.parse
                    (
                    client->get_capabilities
                                (
                                state_url_root + default_capabilities_url, get_encoding_string(EncodingFormat::XML)
                                )
                    );

    auto lookup_table = capabilities_parser.get_lookup_table(server_capabilities);
    capabilities = capabilities_parser.parse(server_capabilities);

    root_schema = repo.create_root_schema(lookup_table, capabilities);
}

RestconfSession::~RestconfSession()
{
    YLOG_INFO("Disconnected from device");
}

path::RootSchemaNode& RestconfSession::get_root_schema() const
{
    return *root_schema;
}

std::shared_ptr<path::DataNode> RestconfSession::invoke(
    path::Rpc& rpc) const
{
    path::SchemaNode* create_schema = get_schema_for_operation(*root_schema, "ydk:create");
    path::SchemaNode* read_schema = get_schema_for_operation(*root_schema, "ydk:read");
    path::SchemaNode* update_schema = get_schema_for_operation(*root_schema, "ydk:update");
    path::SchemaNode* delete_schema = get_schema_for_operation(*root_schema, "ydk:delete");

    path::SchemaNode* rpc_schema = &(rpc.get_schema_node());
    std::shared_ptr<path::DataNode> datanode = nullptr;

    if(rpc_schema == create_schema || rpc_schema == update_schema)
    {
        return handle_edit(rpc, edit_method);
    }
    else if(rpc_schema == read_schema)
    {
        return handle_read(rpc);
    }
    else if(rpc_schema == delete_schema)
    {
       return handle_edit(rpc, "DELETE");
    }
    else
    {
        YLOG_ERROR("rpc is not supported");
        throw(YCPPOperationNotSupportedError{"rpc is not supported!"});
    }

    return datanode;
}

std::shared_ptr<path::DataNode> RestconfSession::handle_read(path::Rpc& rpc) const
{
    path::Codec codec_service{};

    auto filter = rpc.get_input_node().find("filter");
    if(filter.empty()){
        YLOG_ERROR("Failed to get entity node.");
        throw(YCPPInvalidArgumentError{"Failed to get entity node"});
    }

    path::DataNode* filter_node = filter[0].get();
    string filter_instance = filter_node->get_value();

    auto datanode = codec_service.decode(*root_schema, filter_instance, encoding);

    string url;
    if(is_config(rpc))
    {
        url = config_url_root + get_module_url_path(datanode->get_children()[0]->get_schema_node().get_path());
    }
    else
    {
        url = state_url_root + get_module_url_path(datanode->get_children()[0]->get_schema_node().get_path());
    }

    YLOG_INFO("Performing GET on URL {}", url);
    return handle_read_reply( client->execute("GET", url, ""), *root_schema, encoding);
}

std::shared_ptr<path::DataNode> RestconfSession::handle_edit(path::Rpc& rpc, const string & operation) const
{
    path::Codec codec_service{};
    auto entity = rpc.get_input_node().find("entity");
    if(entity.empty()){
        YLOG_ERROR("Failed to get entity node");
        throw(YCPPInvalidArgumentError{"Failed to get entity node"});
    }

    path::DataNode* entity_node = entity[0].get();
    string header_data = entity_node->get_value();

    auto datanode = codec_service.decode(*root_schema, header_data, encoding);
    string url = config_url_root + get_module_url_path(datanode->get_children()[0]->get_schema_node().get_path());

    YLOG_INFO("Performing {} on URL {}. Payload: {}", operation, url, header_data);
    client->execute(operation, url, header_data);

    return nullptr;
}

static std::shared_ptr<path::DataNode> handle_read_reply(const string & reply, path::RootSchemaNode & root_schema, EncodingFormat encoding)
{
    path::Codec codec_service{};

    auto datanode = std::shared_ptr<path::DataNode>(codec_service.decode(root_schema, reply, encoding));

    if(!datanode){
        YLOG_INFO("Codec service failed to decode datanode");
        throw(YCPPError{"Problems deserializing output"});
    }
    return datanode;
}

static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode & root_schema, const string & operation)
{
    auto c = root_schema.find(operation);
    if(c.empty())
    {
        YLOG_ERROR("{} rpc schema not found!", operation);
        throw(YCPPIllegalStateError{operation + " rpc schema not found!"});
    }
    return c[0];
}

static string get_encoding_string(EncodingFormat encoding)
{
    return (encoding == EncodingFormat::XML)?
        ("application/yang-data+xml"):
        ("application/yang-data+json");
}

static string get_module_url_path(const string & path)
{
    auto top = path.find_last_of("/");
    auto t = path.substr(top+1, path.size()-top);

    if(t.find(":") != string::npos)
        return "/"+t;

    auto begin = path.find(":");
    auto mod = path.substr(0,begin);

    return mod+string(":")+t;
}

static bool is_config(path::Rpc & rpc)
{
    if(!rpc.get_input_node().find("only-config").empty())
    {
        return true;
    }
    return false;
}

} //namespace path

} //namespace ydk
