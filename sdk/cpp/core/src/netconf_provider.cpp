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

#include "entity_data_node_walker.hpp"
#include "errors.hpp"
#include "ietf_parser.hpp"
#include "netconf_client.hpp"
#include "netconf_provider.hpp"
#include "netconf_model_provider.hpp"
#include "logger.hpp"
#include "types.hpp"
#include "ydk_yang.hpp"

using namespace std;
using namespace ydk;

namespace ydk
{
static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode& root_schema, string operation);

static shared_ptr<path::Rpc> create_rpc_instance(path::RootSchemaNode & root_schema, string rpc_name);
static path::DataNode& create_rpc_input(path::Rpc & netconf_rpc);

static bool is_candidate_supported(vector<string> capbilities);
static void create_input_target(path::DataNode & input, bool candidate_supported);
static void create_input_source(path::DataNode & input, bool config);
static void create_input_error_option(path::DataNode & input);
static string get_annotated_config_payload(path::RootSchemaNode & root_schema, path::Rpc & rpc, path::Annotation & annotation);
static string get_commit_rpc_payload();
static std::shared_ptr<path::DataNode> handle_edit_reply(string reply, NetconfClient & client, bool candidate_supported);

static string get_read_rpc_name(bool config);
static bool is_config(path::Rpc & rpc);
static string get_filter_payload(path::Rpc & ydk_rpc);
static string get_netconf_payload(path::DataNode & input, string data_tag, string data_value);
static std::shared_ptr<path::DataNode> handle_read_reply(string reply, path::RootSchemaNode & root_schema);

const char* CANDIDATE = "urn:ietf:params:netconf:capability:candidate:1.0";

NetconfServiceProvider::NetconfServiceProvider(string address, string username, string password, int port)
    : client(make_unique<NetconfClient>(username, password, address, port)),
	  model_provider(make_unique<NetconfModelProvider>(*client))
{
	path::Repository repo;
    initialize(repo);
    YLOG_DEBUG("Connected to {} on port {} using ssh", address, port);
}

NetconfServiceProvider::NetconfServiceProvider(path::Repository & repo, string address, string username, string password, int port)
    : client(make_unique<NetconfClient>(username, password, address, port)),
	  model_provider(make_unique<NetconfModelProvider>(*client))
{
    initialize(repo);
    YLOG_DEBUG("Connected to {} on port {} using ssh", address, port);
}

void NetconfServiceProvider::initialize(path::Repository & repo)
{
    IetfCapabilitiesParser capabilities_parser{};
    client->connect();
    server_capabilities = client->get_capabilities();

	for(std::string c : server_capabilities )
	{
		if(c.find("ietf-netconf-monitoring") != std::string::npos)
		{
			repo.add_model_provider(model_provider.get());
		}
	}

	root_schema = std::unique_ptr<ydk::path::RootSchemaNode>(
								repo.create_root_schema
									(
									capabilities_parser.parse(server_capabilities)
									)
								);

	if(root_schema.get() == nullptr)
	{
		YLOG_ERROR("Root schema cannot be obtained");
		throw(YCPPIllegalStateError{"Root schema cannot be obtained"});
	}
}

NetconfServiceProvider::~NetconfServiceProvider()
{
	YLOG_DEBUG("Disconnected from device");
}

EncodingFormat NetconfServiceProvider::get_encoding() const
{
	return EncodingFormat::XML;
}

path::RootSchemaNode& NetconfServiceProvider::get_root_schema() const
{
    return *root_schema;
}

std::shared_ptr<path::DataNode> NetconfServiceProvider::handle_read(path::Rpc& ydk_rpc) const
{
    //for now we only support crud rpc's
    bool config = is_config(ydk_rpc);
    auto netconf_rpc = create_rpc_instance(*root_schema, get_read_rpc_name(config));
    auto & input = create_rpc_input(*netconf_rpc);
    create_input_source(input, config);
    std::string filter_value = get_filter_payload(ydk_rpc);

    string netconf_payload = get_netconf_payload(input, "filter", filter_value);
    return handle_read_reply(execute_payload(netconf_payload), *root_schema	);
}

std::shared_ptr<path::DataNode> NetconfServiceProvider::handle_edit(path::Rpc& ydk_rpc, path::Annotation annotation) const
{
    //for now we only support crud rpc's
    bool candidate_supported = is_candidate_supported(server_capabilities);

    auto netconf_rpc = create_rpc_instance(*root_schema, "ietf-netconf:edit-config");
    auto & input = create_rpc_input(*netconf_rpc);
    create_input_target(input, candidate_supported);
    create_input_error_option(input);
    string config_payload = get_annotated_config_payload(*root_schema, ydk_rpc, annotation);

    ly_verb(LY_LLSILENT); //turn off libyang logging at the beginning
    string netconf_payload = get_netconf_payload(input, "config", config_payload);
    ly_verb(LY_LLVRB); // enable libyang logging after payload has been created

    return handle_edit_reply(execute_payload(netconf_payload), *client, candidate_supported);
}

std::shared_ptr<path::DataNode> NetconfServiceProvider::handle_netconf_operation(path::Rpc& ydk_rpc) const
{
    path::CodecService codec_service{};
    auto netconf_payload = codec_service.encode(ydk_rpc.input(), EncodingFormat::XML, true);
    std::string payload{"<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"};
    netconf_payload = payload + netconf_payload + "</rpc>";

    YLOG_DEBUG("=============Generating payload to send to device=============");
    YLOG_DEBUG(netconf_payload.c_str());
    YLOG_DEBUG("\n");

    std::string reply = execute_payload(netconf_payload);
    if (ydk_rpc.schema().path().find("get") != string::npos or ydk_rpc.schema().path().find("get-config") != string::npos)
    {
        return handle_read_reply(reply, *root_schema);
    }
    if(reply.find("<ok/>") == std::string::npos)
    {
        YLOG_ERROR("No ok in reply ");
        throw(YCPPServiceProviderError{reply});
    }
    return nullptr;

}

std::shared_ptr<path::DataNode> NetconfServiceProvider::invoke(path::Rpc& rpc) const
{
    path::SchemaNode* create_schema = get_schema_for_operation(*root_schema, "ydk:create");
    path::SchemaNode* read_schema = get_schema_for_operation(*root_schema, "ydk:read");
    path::SchemaNode* update_schema = get_schema_for_operation(*root_schema, "ydk:update");
    path::SchemaNode* delete_schema = get_schema_for_operation(*root_schema, "ydk:delete");

    //for now we only support crud rpc's
    path::SchemaNode* rpc_schema = &(rpc.schema());
	std::shared_ptr<path::DataNode> datanode = nullptr;

    if(rpc_schema == create_schema || rpc_schema == delete_schema || rpc_schema == update_schema)
    {
        //for each child node in datanode add the nc:operation attribute
        path::Annotation an{IETF_NETCONF_MODULE_NAME, "operation", rpc_schema == delete_schema ? "delete" : "merge"};
        return handle_edit(rpc, an);
    }
    else if(rpc_schema == read_schema)
    {
        return handle_read(rpc);
    }
    else if(rpc_schema->path().find("ietf-netconf:")!= string::npos)
    {
       return handle_netconf_operation(rpc);
    }
    else
    {
        YLOG_ERROR("rpc is not supported");
        throw(YCPPOperationNotSupportedError{"rpc is not supported!"});
    }

    return datanode;
}

std::string NetconfServiceProvider::execute_payload(const std::string & payload) const
{
    std::string reply = client->execute_payload(payload);
    YLOG_DEBUG("=============Reply payload received from device=============");
    YLOG_DEBUG(reply.c_str());
    YLOG_DEBUG("\n");
    return reply;
}

static shared_ptr<path::Rpc> create_rpc_instance(path::RootSchemaNode & root_schema, string rpc_name)
{
    auto rpc = shared_ptr<path::Rpc>(root_schema.rpc(rpc_name));
    if(rpc == nullptr){
        YLOG_ERROR("Cannot create payload for RPC: {}", rpc_name);
        throw(YCPPIllegalStateError{"Cannot create payload for RPC: "+ rpc_name});
    }
    return rpc;
}

static path::DataNode& create_rpc_input(path::Rpc & netconf_rpc)
{
    return netconf_rpc.input();
}

static string get_commit_rpc_payload()
{
    return "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
           "<commit/>"
           "</rpc>";
}

static bool is_candidate_supported(vector<string> capabilities)
{
    if(std::find(capabilities.begin(), capabilities.end(), CANDIDATE) != capabilities.end()){
        //candidate is supported
        return true;
    }
    return false;
}

static void create_input_target(path::DataNode & input, bool candidate_supported)
{
    if(candidate_supported){
        input.create("target/candidate", "");
    }
    else {
        input.create("target/running", "");
    }
}

static void create_input_error_option(path::DataNode & input)
{
	input.create("error-option", "rollback-on-error");
}

static void create_input_source(path::DataNode & input, bool config)
{
    if(config)
    {
        input.create("source/running");
    }
}

static string get_annotated_config_payload(path::RootSchemaNode & root_schema,
        path::Rpc & rpc, path::Annotation & annotation)
{
    path::CodecService codec_service{};
    auto entity = rpc.input().find("entity");
    if(entity.empty()){
        YLOG_ERROR("Failed to get entity node");
        throw(YCPPInvalidArgumentError{"Failed to get entity node"});
    }

    path::DataNode* entity_node = entity[0].get();
    std::string entity_value = entity_node->get();

    //deserialize the entity_value
    auto datanode = codec_service.decode(root_schema, entity_value, EncodingFormat::XML);

    if(!datanode){
        YLOG_ERROR("Failed to decode entity node");
        throw(YCPPInvalidArgumentError{"Failed to decode entity node"});
    }

    std::string config_payload {};

    for(auto const & child : datanode->children())
    {
    	if((child->annotations()).size()==0)
    	{
    		child->add_annotation(annotation);
    	}
        config_payload += codec_service.encode(*child, EncodingFormat::XML, true);
    }
    return config_payload;
}

static string get_filter_payload(path::Rpc & ydk_rpc)
{
    auto entity = ydk_rpc.input().find("filter");
    if(entity.empty()){
        YLOG_ERROR("Failed to get entity node.");
        throw(YCPPInvalidArgumentError{"Failed to get entity node"});
    }

    auto datanode = entity[0];
    return datanode->get();
}

static string get_netconf_payload(path::DataNode & input, string data_tag, string data_value)
{
    path::CodecService codec_service{};
    input.create(data_tag, data_value);

    std::string payload{"<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"};
    payload+=codec_service.encode(input, EncodingFormat::XML, true);
    payload+="</rpc>";
    YLOG_DEBUG("=============Generating payload to send to device=============");
    YLOG_DEBUG(payload.c_str());
    YLOG_DEBUG("\n");
    return payload;
}

static std::shared_ptr<path::DataNode> handle_edit_reply(string reply, NetconfClient & client, bool candidate_supported)
{
    if(reply.find("<ok/>") == std::string::npos)
    {
        YLOG_ERROR("No ok in reply received from device");
        throw(YCPPServiceProviderError{reply});
    }

	if(candidate_supported)
	{
		//need to send the commit request
		string commit_payload = get_commit_rpc_payload();

        YLOG_DEBUG( "Executing commit RPC: {}", commit_payload);
        reply = client.execute_payload(commit_payload);

        YLOG_DEBUG("=============Reply payload received from device=============");
        YLOG_DEBUG(reply.c_str());
        YLOG_DEBUG("\n");
        if(reply.find("<ok/>") == std::string::npos)
        {
            YLOG_ERROR("RPC error occurred: {}", reply);
            throw(YCPPServiceProviderError{reply});
        }
    }

    //no error no output for edit-config
    return nullptr;
}

static std::shared_ptr<path::DataNode> handle_read_reply(string reply, path::RootSchemaNode & root_schema)
{
    path::CodecService codec_service{};
    auto empty_data = reply.find("<data/>");
    if(empty_data != std::string::npos)
    {
        YLOG_DEBUG( "Found empty data tag");
        return nullptr;
    }

    auto data_start = reply.find("<data>");
    if(data_start == std::string::npos)
    {
        YLOG_ERROR( "Can't find data tag in reply sent by device {}", reply);
        throw(YCPPServiceProviderError{reply});
    }
    data_start+= sizeof("<data>") - 1;
    auto data_end = reply.find("</data>", data_start);
    if(data_end == std::string::npos)
    {
        YLOG_ERROR( "No end data tag found in reply sent by device {}", reply);
        throw(YCPPError{"No end data tag found"});
    }

    string data = reply.substr(data_start, data_end-data_start);

	auto datanode = std::shared_ptr<path::DataNode>(codec_service.decode(root_schema, data, EncodingFormat::XML));

    if(!datanode){
        YLOG_ERROR( "Codec service failed to decode datanode");
        throw(YCPPError{"Problems deserializing output"});
    }
    return datanode;
}

static string get_read_rpc_name(bool config)
{
    if(config)
    {
        return "ietf-netconf:get-config";
    }
    return "ietf-netconf:get";
}

static bool is_config(path::Rpc & rpc)
{
    if(!rpc.input().find("only-config").empty())
    {
        return true;
    }
    return false;
}

static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode & root_schema, string operation)
{
	auto c = root_schema.find(operation);
	if(c.empty())
	{
		YLOG_ERROR("CRUD create rpc schema not found!");
		throw(YCPPIllegalStateError{"CRUD create rpc schema not found!"});
	}
	return c[0];
}

}
