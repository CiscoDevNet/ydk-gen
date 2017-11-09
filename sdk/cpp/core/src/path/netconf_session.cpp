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

#include <memory>

#include <libyang/libyang.h>

#include "../entity_data_node_walker.hpp"
#include "../errors.hpp"
#include "../ietf_parser.hpp"
#include "../netconf_ssh_client.hpp"
#include "../netconf_tcp_client.hpp"
#include "../netconf_provider.hpp"
#include "../types.hpp"
#include "../ydk_yang.hpp"
#include "../logger.hpp"
#include "netconf_model_provider.hpp"


using namespace std;
using namespace ydk;

namespace ydk
{

namespace path
{

static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode& root_schema, string yfilter);

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
static string get_netconf_payload(path::DataNode & input, const string& data_tag, const string& data_value);
static std::shared_ptr<path::DataNode> handle_rpc_output(const string & reply, path::RootSchemaNode & root_schema, path::Rpc & rpc);

const char* CANDIDATE = "urn:ietf:params:netconf:capability:candidate:1.0";
const string PROTOCOL_SSH = "ssh";
const string PROTOCOL_TCP = "tcp";

static bool is_netconf_get_rpc(path::Rpc & rpc);
static std::shared_ptr<path::DataNode> handle_netconf_get_output(const string & reply, path::RootSchemaNode & root_schema);

// static std::shared_ptr<path::DataNode> handle_read_reply(string reply, path::RootSchemaNode & root_schema);

NetconfSession::NetconfSession(path::Repository & repo,
                               const string& address,
                               const string& username,
                               const string& password,
                               int port,
                               const string& protocol,
                               bool on_demand,
                               int timeout)
{
    initialize_client(address, username, password, port, protocol, timeout);
    initialize_repo(repo, on_demand);
    YLOG_INFO("Connected to {} on port {} using {} with timeout of {}", address, port, protocol, timeout);
}

NetconfSession::NetconfSession(const string& address,
                               const string& username,
                               const string& password,
                               int port,
                               const string& protocol,
                               bool on_demand,
                               bool common_cache,
                               int timeout)
{
    initialize_client(address, username, password, port, protocol, timeout);
    auto caching_option = common_cache ? path::ModelCachingOption::COMMON : path::ModelCachingOption::PER_DEVICE;
    path::Repository repo(caching_option);
    initialize_repo(repo, on_demand);
    YLOG_INFO("Connected to {} on port {} using {} with timeout of {}", address, port, protocol, timeout);
}

// todo: decide whether or not to add TWO more signatures for 
//       constructor to match above or just ONE more signature (go style)

NetconfSession::NetconfSession(path::Repository& repo,
                               const string& address,
                               const string& username,
                               const string& private_key_path,
                               const string& public_key_path,
                               int port,
                               bool on_demand,
                               int timeout)
{
    initialize_client_with_key(address, username, private_key_path, public_key_path,
        port, timeout);
    initialize_repo(repo, on_demand);
   YLOG_INFO("Connected to {} on port {} using SSH with timeout of {}",
       address, port, timeout);
}

NetconfSession::NetconfSession(const string& address,
                               const string& username,
                               const string& private_key_path,
                               const string& public_key_path,
                               int port,
                               bool on_demand,
                               bool common_cache,
                               int timeout)
{
    initialize_client_with_key(address, username, private_key_path, public_key_path,
        port, timeout);
    auto caching_option = common_cache ? path::ModelCachingOption::COMMON : path::ModelCachingOption::PER_DEVICE;
    path::Repository repo(caching_option);
    initialize_repo(repo, on_demand);
   YLOG_INFO("Connected to {} on port {} using SSH with timeout of {}",
       address, port, timeout);
}

void NetconfSession::initialize_client_with_key(const string& address,
                                       const string& username,
                                       const string& private_key_path,
                                       const string& public_key_path,
                                       int port,
                                       int timeout)
{
    client = make_unique<NetconfSSHClient>(
        username, private_key_path, public_key_path, address, port, timeout);
    model_provider = make_unique<NetconfModelProvider>(*client);
}

void NetconfSession::initialize_client(const string& address,
                                       const string& username,
                                       const string& password,
                                       int port,
                                       const string& protocol,
                                       int timeout)
{
    if (protocol.compare(PROTOCOL_SSH) == 0)
    {
        client = make_unique<NetconfSSHClient>(username, password, address, port, timeout);
    }
    else if (protocol.compare(PROTOCOL_TCP) == 0)
    {
        client = make_unique<NetconfTCPClient>(username, password, address, port);
    }
    else
    {
        YLOG_ERROR("Protocol {} not supported.", protocol);
        throw(YCPPOperationNotSupportedError{"Protocol is not supported!"});
    }
    model_provider = make_unique<NetconfModelProvider>(*client);
}

void NetconfSession::initialize_repo(path::Repository & repo, bool on_demand)
{
    IetfCapabilitiesParser capabilities_parser{};
    client->connect();
    server_capabilities = client->get_capabilities();

    for(std::string &c : server_capabilities )
    {
        if(c.find("ietf-netconf-monitoring") != std::string::npos)
        {
            repo.add_model_provider(model_provider.get());
        }
    }

    auto lookup_table = capabilities_parser.get_lookup_table(server_capabilities);

    std::vector<path::Capability> yang_caps;
    std::vector<std::string> empty_caps;

    if (on_demand)
        yang_caps = capabilities_parser.parse(empty_caps);
    else
        yang_caps = capabilities_parser.parse(server_capabilities);

    root_schema = repo.create_root_schema(lookup_table, yang_caps);

    if(root_schema.get() == nullptr)
    {
        YLOG_ERROR("Root schema cannot be obtained");
        throw(YCPPIllegalStateError{"Root schema cannot be obtained"});
    }
}

NetconfSession::~NetconfSession()
{
    YLOG_INFO("Disconnected from device");
}

std::vector<std::string> NetconfSession::get_capabilities() const
{
    return server_capabilities;
}

path::RootSchemaNode& NetconfSession::get_root_schema() const
{
    return *root_schema;
}

std::shared_ptr<path::DataNode> NetconfSession::handle_read(path::Rpc& ydk_rpc) const
{
    //for now we only support crud rpc's
    bool config = is_config(ydk_rpc);
    auto netconf_rpc = create_rpc_instance(*root_schema, get_read_rpc_name(config));
    auto & input = create_rpc_input(*netconf_rpc);
    create_input_source(input, config);
    std::string filter_value = get_filter_payload(ydk_rpc);

    string netconf_payload = get_netconf_payload(input, "filter", filter_value);
    return handle_rpc_output(execute_payload(netconf_payload), *root_schema, *netconf_rpc );
}

std::shared_ptr<path::DataNode> NetconfSession::handle_edit(path::Rpc& ydk_rpc, path::Annotation annotation) const
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

std::shared_ptr<path::DataNode> NetconfSession::handle_netconf_operation(path::Rpc& ydk_rpc) const
{
    path::Codec codec_service{};
    auto netconf_payload = codec_service.encode(ydk_rpc.get_input_node(), EncodingFormat::XML, true);
    std::string payload{"<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"};
    netconf_payload = payload + netconf_payload + "</rpc>";

    YLOG_INFO("=============Generating payload to send to device=============");
    YLOG_INFO("{}", netconf_payload);
    YLOG_INFO("\n");

    std::string reply = execute_payload(netconf_payload);
    if (ydk_rpc.has_output_node())
    {
        return handle_rpc_output(reply, *root_schema, ydk_rpc);
    }
    if(reply.find("<ok/>") == std::string::npos)
    {
        YLOG_ERROR("Did not receive OK reply from the device");
        throw(YCPPServiceProviderError{reply});
    }
    return nullptr;
}

std::shared_ptr<path::DataNode> NetconfSession::invoke(path::Rpc& rpc) const
{
    path::SchemaNode* create_schema = get_schema_for_operation(*root_schema, "ydk:create");
    path::SchemaNode* read_schema = get_schema_for_operation(*root_schema, "ydk:read");
    path::SchemaNode* update_schema = get_schema_for_operation(*root_schema, "ydk:update");
    path::SchemaNode* delete_schema = get_schema_for_operation(*root_schema, "ydk:delete");

    //for now we only support crud rpc's
    path::SchemaNode* rpc_schema = &(rpc.get_schema_node());
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
    else
    {
       return handle_netconf_operation(rpc);
    }

    return datanode;
}

std::string NetconfSession::execute_payload(const std::string & payload) const
{
    std::string reply = client->execute_payload(payload);
    YLOG_INFO("=============Reply payload received from device=============");
    YLOG_INFO("{}", reply);
    YLOG_INFO("\n");
    return reply;
}

static shared_ptr<path::Rpc> create_rpc_instance(path::RootSchemaNode & root_schema, string rpc_name)
{
    auto rpc = shared_ptr<path::Rpc>(root_schema.create_rpc(rpc_name));
    if(rpc == nullptr){
        YLOG_ERROR("Cannot create payload for RPC: {}", rpc_name);
        throw(YCPPIllegalStateError{"Cannot create payload for RPC: "+ rpc_name});
    }
    return rpc;
}

static path::DataNode& create_rpc_input(path::Rpc & netconf_rpc)
{
    return netconf_rpc.get_input_node();
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


//////////////////////////////////
static void create_input_target(path::DataNode & input, bool candidate_supported)
{
    if(candidate_supported){
        input.create_datanode("target/candidate", "");
    }
    else {
        input.create_datanode("target/running", "");
    }
}

static void create_input_error_option(path::DataNode & input)
{
    input.create_datanode("error-option", "rollback-on-error");
}

static void create_input_source(path::DataNode & input, bool config)
{
    if(config)
    {
        input.create_datanode("source/running");
    }
}

static string get_annotated_config_payload(path::RootSchemaNode & root_schema,
        path::Rpc & rpc, path::Annotation & annotation)
{
    path::Codec codec_service{};
    auto entity = rpc.get_input_node().find("entity");
    if(entity.empty()){
        YLOG_ERROR("Failed to get entity node");
        throw(YCPPInvalidArgumentError{"Failed to get entity node"});
    }

    path::DataNode* entity_node = entity[0].get();
    std::string entity_value = entity_node->get_value();

    //deserialize the entity_value
    auto datanode = codec_service.decode(root_schema, entity_value, EncodingFormat::XML);

    if(!datanode){
        YLOG_ERROR("Failed to decode entity node");
        throw(YCPPInvalidArgumentError{"Failed to decode entity node"});
    }

    std::string config_payload {};

    for(auto const & child : datanode->get_children())
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
    auto entity = ydk_rpc.get_input_node().find("filter");
    if(entity.empty()){
        YLOG_ERROR("Failed to get entity node.");
        throw(YCPPInvalidArgumentError{"Failed to get entity node"});
    }

    auto datanode = entity[0];
    return datanode->get_value();
}

static string get_netconf_payload(path::DataNode & input, const string &  data_tag, const string &  data_value)
{
    path::Codec codec_service{};
    input.create_datanode(data_tag, data_value);

    std::string payload{"<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"};
    payload+=codec_service.encode(input, EncodingFormat::XML, true);
    payload+="</rpc>";
    YLOG_INFO("=============Generating payload to send to device=============");
    YLOG_INFO(payload.c_str());
    YLOG_INFO("\n");
    return payload;
}

static std::shared_ptr<path::DataNode> handle_edit_reply(string reply, NetconfClient & client, bool candidate_supported)
{
    if(reply.find("<ok/>") == std::string::npos)
    {
        YLOG_ERROR("Did not receive OK reply from the device");
        throw(YCPPServiceProviderError{reply});
    }

    if(candidate_supported)
    {
        //need to send the commit request
        string commit_payload = get_commit_rpc_payload();

        YLOG_INFO( "Executing commit RPC: {}", commit_payload);
        reply = client.execute_payload(commit_payload);

        YLOG_INFO("=============Reply payload received from device=============");
        YLOG_INFO(reply.c_str());
        YLOG_INFO("\n");
        if(reply.find("<ok/>") == std::string::npos)
        {
            YLOG_ERROR("RPC error occurred: {}", reply);
            throw(YCPPServiceProviderError{reply});
        }
    }

    //no error no output for edit-config
    return nullptr;
}

static bool is_netconf_get_rpc(path::Rpc & rpc)
{
    return (rpc.get_schema_node().get_path() == "/ietf-netconf:get"
            or rpc.get_schema_node().get_path() == "/ietf-netconf:get-config");
}

static std::shared_ptr<path::DataNode> handle_netconf_get_output(const string & reply, path::RootSchemaNode & root_schema)
{
    path::Codec codec_service{};
    auto empty_data = reply.find("<data/>");
    if(empty_data != std::string::npos)
    {
        YLOG_INFO( "Found empty data tag");
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

static std::shared_ptr<path::DataNode> handle_rpc_output(const string & reply, path::RootSchemaNode & root_schema, path::Rpc & rpc)
{
    if (is_netconf_get_rpc(rpc))
    {
        return handle_netconf_get_output(reply, root_schema);
    }

    path::Codec codec_service{};

    auto data_start = reply.find("<rpc-reply ");
    auto data_end = reply.find("</rpc-reply>", data_start);
    //need to find the end of the "<rpc-reply start tag
    auto data_start_end = reply.find(">", data_start);
    data_start = data_start_end + 1;
    data_end -= 1;
    string data = reply.substr(data_start, data_end-data_start+1);

    std::shared_ptr<path::DataNode> datanode = codec_service.decode_rpc_output(
                                                    root_schema,
                                                    data,
                                                    rpc.get_schema_node().get_path(),
                                                    EncodingFormat::XML);
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
    if(!rpc.get_input_node().find("only-config").empty())
    {
        return true;
    }
    return false;
}

static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode & root_schema, string yfilter)
{
    auto c = root_schema.find(yfilter);
    if(c.empty())
    {
        YLOG_ERROR("CRUD create rpc schema not found!");
        throw(YCPPIllegalStateError{"CRUD create rpc schema not found!"});
    }
    return c[0];
}

} //namespace path

} //namespace ydk