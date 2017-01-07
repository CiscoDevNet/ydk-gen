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
#include <boost/log/trivial.hpp>

#include <libyang/libyang.h>

#include "entity_data_node_walker.hpp"
#include "errors.hpp"
#include "ietf_parser.hpp"
#include "restconf_client.hpp"
#include "restconf_provider.hpp"
#include "types.hpp"

using namespace std;

namespace ydk
{
static path::DataNode* handle_read_reply(const string & reply, path::RootSchemaNode & root_schema, EncodingFormat encoding);
static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode & root_schema, const string & operation);
static string get_encoding_string(EncodingFormat encoding);

RestconfServiceProvider::RestconfServiceProvider(path::Repository & repo, const string & address, const string & username,
		const string & password, int port, EncodingFormat encoding, const string & config_url_root, const string & state_url_root)
    : client(make_unique<RestconfClient>(address, username, password, port, get_encoding_string(encoding))),
	  encoding(encoding), config_url_root(config_url_root), state_url_root(state_url_root)
{
	initialize(repo);
}

RestconfServiceProvider::RestconfServiceProvider(std::unique_ptr<RestconfClient> client,
		std::unique_ptr<ydk::path::RootSchemaNode> root_schema, const std::string & edit_method,
		const std::string & config_url_root, const std::string & state_url_root, EncodingFormat encoding)
	: client(move(client)), root_schema(move(root_schema)), encoding(encoding), edit_method(edit_method),
	  config_url_root(config_url_root), state_url_root(state_url_root)
{
}

void RestconfServiceProvider::initialize(path::Repository & repo)
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
	capabilities = capabilities_parser.parse(server_capabilities);

	root_schema = unique_ptr<ydk::path::RootSchemaNode>(
									repo.create_root_schema
										(
										capabilities
										)
									);
}

RestconfServiceProvider::~RestconfServiceProvider()
{
	BOOST_LOG_TRIVIAL(debug) << "Disconnected from device";
}

EncodingFormat RestconfServiceProvider::get_encoding() const
{
	return encoding;
}

path::RootSchemaNode* RestconfServiceProvider::get_root_schema() const
{
    return root_schema.get();
}

path::DataNode* RestconfServiceProvider::invoke(path::Rpc* rpc) const
{
	path::SchemaNode* create_schema = get_schema_for_operation(*root_schema, "ydk:create");
	path::SchemaNode* read_schema = get_schema_for_operation(*root_schema, "ydk:read");
	path::SchemaNode* update_schema = get_schema_for_operation(*root_schema, "ydk:update");
	path::SchemaNode* delete_schema = get_schema_for_operation(*root_schema, "ydk:delete");

    //sanity check of rpc
    if(rpc == nullptr)
    {
        BOOST_LOG_TRIVIAL(error) << "rpc is nullptr";
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"rpc is null!"});
    }

    path::SchemaNode* rpc_schema = rpc->schema();
    path::DataNode* datanode = nullptr;

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
        BOOST_LOG_TRIVIAL(error) << "rpc is not supported";
        BOOST_THROW_EXCEPTION(YCPPOperationNotSupportedError{"rpc is not supported!"});
    }

    return datanode;
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
	if(!rpc.input()->find("only-config").empty())
	{
		return true;
	}
	return false;
}

path::DataNode* RestconfServiceProvider::handle_read(path::Rpc* rpc) const
{
    path::CodecService codec_service{};

    auto filter = rpc->input()->find("filter");
	if(filter.empty()){
		BOOST_LOG_TRIVIAL(error) << "Failed to get entity node.";
		BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Failed to get entity node"});
	}

	path::DataNode* filter_node = filter[0];
	string filter_instance = filter_node->get();

    path::DataNode* datanode = codec_service.decode(root_schema.get(), filter_instance, encoding);

	string url;
	if(is_config(*rpc))
	{
		url = config_url_root + get_module_url_path(datanode->children()[0]->schema()->path());
	}
	else
	{
		url = state_url_root + get_module_url_path(datanode->children()[0]->schema()->path());
	}

    BOOST_LOG_TRIVIAL(debug) << "Performing GET on URL " << url;
    return handle_read_reply( client->execute("GET", url, ""), *root_schema, encoding);
}

path::DataNode* RestconfServiceProvider::handle_edit(path::Rpc* rpc, const string & operation) const
{
	path::CodecService codec_service{};
    auto entity = rpc->input()->find("entity");
	if(entity.empty()){
		BOOST_LOG_TRIVIAL(error) << "Failed to get entity node";
		BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Failed to get entity node"});
	}

	path::DataNode* entity_node = entity[0];
	string header_data = entity_node->get();

    path::DataNode* datanode = codec_service.decode(root_schema.get(), header_data, encoding);
	string url = config_url_root + get_module_url_path(datanode->children()[0]->schema()->path());

    BOOST_LOG_TRIVIAL(debug) << "Performing "<< operation <<" on URL " << url << ". Payload: " <<header_data;
    client->execute(operation, url, header_data);

    return nullptr;
}

static path::DataNode* handle_read_reply(const string & reply, path::RootSchemaNode & root_schema, EncodingFormat encoding)
{
	path::CodecService codec_service{};

	auto datanode = codec_service.decode(&root_schema, reply, encoding);

	if(!datanode){
		BOOST_LOG_TRIVIAL(debug) << "Codec service failed to decode datanode";
		BOOST_THROW_EXCEPTION(YCPPError{"Problems deserializing output"});
	}
	return datanode;
}

static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode & root_schema, const string & operation)
{
	auto c = root_schema.find(operation);
	if(c.empty())
	{
		BOOST_LOG_TRIVIAL(error) <<operation << " rpc schema not found!";
		BOOST_THROW_EXCEPTION(YCPPIllegalStateError{operation + " rpc schema not found!"});
	}
	return c[0];
}

static string get_encoding_string(EncodingFormat encoding)
{
	return (encoding == EncodingFormat::XML)?
		("application/yang-data+xml"):
		("application/yang-data+json");
}

}
