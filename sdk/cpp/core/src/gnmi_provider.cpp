
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
// with the License.  Yo::u may obtain a copy of the License at
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
#include "gnmi_provider.hpp"

using grpc::Channel;
using grpc::ChannelArguments;
using grpc::ChannelCredentials;
using grpc::SslCredentialsOptions;

using namespace std;
using namespace ydk;

namespace ydk
{
    static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode& root_schema, string operation);
    static shared_ptr<path::Rpc> create_rpc_instance(path::RootSchemaNode & root_schema, string rpc_name);
    static path::DataNode& create_rpc_input(path::Rpc & gnmi_rpc);
    static void create_input_source(path::DataNode & input, bool config);
    static string get_read_rpc_name(bool config);
    static bool is_config(path::Rpc & rpc);
    static string get_filter_payload(path::Rpc & ydk_rpc);
    static string get_gnmi_payload(path::DataNode & input, string data_tag, string data_value);
    static std::shared_ptr<path::DataNode> handle_read_reply(string reply, path::RootSchemaNode & root_schema);
    static gNMIServiceProvider::SecureChannelArguments get_channel_credentials();

    // Create a default SSL ChannelCredentials object.
    gNMIServiceProvider::SecureChannelArguments input_args = get_channel_credentials(); 

    gNMIServiceProvider::gNMIServiceProvider(string address)
        : client(make_unique<gNMIClient>(grpc::CreateCustomChannel(address, input_args.channel_creds, input_args.args)))
    {
        path::Repository repo;       
        initialize(repo, address);
        YLOG_INFO("Connected to {} using ssh", address);
    }

    gNMIServiceProvider::gNMIServiceProvider(path::Repository & repo, string address)
        : client(make_unique<gNMIClient>(grpc::CreateCustomChannel(address, input_args.channel_creds, input_args.args)))
    {
        initialize(repo, address);
        YLOG_INFO("Connected to {} using ssh", address);
    }

    gNMIServiceProvider::~gNMIServiceProvider() = default;

    void gNMIServiceProvider::initialize(path::Repository & repo, std::string address) {
        IetfCapabilitiesParser capabilities_parser{};
        client->connect(address);
        server_capabilities = client->get_capabilities();
        
        root_schema = repo.create_root_schema(capabilities_parser.parse(server_capabilities));

        if(root_schema.get() == nullptr)
        {
            YLOG_ERROR("Root schema cannot be obtained");
            throw(YCPPIllegalStateError{"Root schema cannot be obtained"});
        }
    }

    gNMIServiceProvider::SecureChannelArguments get_channel_credentials() 
    {
        std::string server_cert, client_key, client_cert;
        std::ifstream rf("ems.pem");

        server_cert.assign((std::istreambuf_iterator<char>(rf)),(std::istreambuf_iterator<char>()));

        grpc::SslCredentialsOptions ssl_opts;
        grpc::ChannelArguments      args;
        gNMIServiceProvider::SecureChannelArguments input_args;
        ssl_opts.pem_root_certs = server_cert;
        args.SetSslTargetNameOverride("ems.cisco.com");

        /* ToDo Authenticate client at server
        std::ifstream kf("client.key");
        std::ifstream cf("client.pem");
        client_key.assign((std::istreambuf_iterator<char>(kf)),(std::istreambuf_iterator<char>()));
        client_cert.assign((std::istreambuf_iterator<char>(cf)),(std::istreambuf_iterator<char>()));
        ssl_opts = {server_cert, client_key, client_cert};
        */
        auto channel_creds = grpc::SslCredentials(grpc::SslCredentialsOptions(ssl_opts));
        input_args.channel_creds = channel_creds;
        input_args.args = args;
        return input_args;
    }

    EncodingFormat gNMIServiceProvider::get_encoding() const
    {
        return EncodingFormat::JSON;
    }

    path::RootSchemaNode& gNMIServiceProvider::get_root_schema() const
    {
        return *root_schema;
    }

    std::shared_ptr<path::DataNode> gNMIServiceProvider::invoke(path::Rpc& rpc) const
    {
        path::SchemaNode* read_schema = get_schema_for_operation(*root_schema, "ydk:read");

        //for now we only support crud rpc's
        path::SchemaNode* rpc_schema = &(rpc.schema());

        std::shared_ptr<path::DataNode> datanode = nullptr;
        
        if(rpc_schema == read_schema)
        {
            return handle_read(rpc);
        }
        else
        {
            YLOG_ERROR("RPC is not supported");
            throw(YCPPOperationNotSupportedError{"RPC is not supported!"});
        }
        return datanode;
    }

    std::shared_ptr<path::DataNode> gNMIServiceProvider::handle_read(path::Rpc& ydk_rpc) const
    {
        //for now we only support crud rpc's
        bool config = is_config(ydk_rpc);
        auto gnmi_rpc = create_rpc_instance(*root_schema, get_read_rpc_name(config));
        auto & input = create_rpc_input(*gnmi_rpc);
        create_input_source(input, config);
        std::string filter_value = get_filter_payload(ydk_rpc);
        std::string gnmi_payload = get_gnmi_payload(input, "filter", filter_value);
        std::shared_ptr<path::DataNode> datanode = nullptr;
        return handle_read_reply(execute_payload(gnmi_payload), *root_schema);
    }

    
    static std::shared_ptr<path::DataNode> handle_read_reply(string reply, path::RootSchemaNode & root_schema)
    {
        path::CodecService codec_service{};
        auto empty_data = reply.find("data");
        if(empty_data == std::string::npos)
        {
            YLOG_INFO( "Found empty data tag");
            return nullptr;
        }

        auto data_start = reply.find("\"data\":");
        if(data_start == std::string::npos)
        {
            YLOG_ERROR( "Can't find data tag in reply sent by device {}", reply);
            throw(YCPPServiceProviderError{reply});
        }
        data_start+= sizeof("\"data\":") - 1;
        auto data_end = reply.find_last_of("}");
        if(data_end == std::string::npos)
        {
            YLOG_ERROR( "No end data tag found in reply sent by device {}", reply);
            throw(YCPPError{"No end data tag found"});
        }

        string data = reply.substr(data_start, data_end-data_start);

        auto datanode = std::shared_ptr<path::DataNode>(codec_service.decode(root_schema, data, EncodingFormat::JSON));
        if(!datanode)
        {
            YLOG_ERROR( "Codec service failed to decode datanode");
            throw(YCPPError{"Problems deserializing output"});
        }
        return datanode;
    }

    static bool is_config(path::Rpc & rpc)
    {
        if(!rpc.input().find("only-config").empty())
        {
            return true;
        }
        return false;
    }

    static shared_ptr<path::Rpc> create_rpc_instance(path::RootSchemaNode & root_schema, string rpc_name)
    {
        auto rpc = shared_ptr<path::Rpc>(root_schema.rpc(rpc_name));
        if(rpc == nullptr)
        {
            YLOG_ERROR("Cannot create payload for RPC: {}", rpc_name);
            throw(YCPPIllegalStateError{"Cannot create payload for RPC: "+ rpc_name});
        }
        return rpc;
    }

    static string get_read_rpc_name(bool config)
    {
        if(config)
        {
            return "ietf-netconf:get-config";
        }
        return "ietf-netconf:get";
    }

    static path::DataNode& create_rpc_input(path::Rpc & gnmi_rpc)
    {
        return gnmi_rpc.input();
    }

    static void create_input_source(path::DataNode & input, bool config)
    {
        if(config)
        {
            input.create("source/running");
        }
    }

    static string get_filter_payload(path::Rpc & ydk_rpc)
    {
        auto entity = ydk_rpc.input().find("filter");
        if(entity.empty())
        {
            YLOG_ERROR("Failed to get entity node.");
            throw(YCPPInvalidArgumentError{"Failed to get entity node"});
        }

        auto datanode = entity[0];
        return datanode->get();
    }

    static string get_gnmi_payload(path::DataNode & input, string data_tag, string data_value)
    {
        path::CodecService codec_service{};
        input.create(data_tag, data_value);
        string payload{"\"rpc\":"};
        payload+=codec_service.encode(input, EncodingFormat::JSON, true);
        YLOG_INFO("===========Generating Target Payload============");
        YLOG_INFO(payload.c_str());
        YLOG_INFO("\n");
        return payload;
    }

    std::string gNMIServiceProvider::execute_payload(const std::string & payload) const
    {
        std::string reply = client->execute_wrapper(payload);
        YLOG_INFO("=============Reply payload received from device=============");
        YLOG_INFO(reply.c_str());
        YLOG_INFO("\n");
        return reply;
    }

    static path::SchemaNode* get_schema_for_operation(path::RootSchemaNode & root_schema, string operation)
    {
        auto c = root_schema.find(operation);
        if(c.empty())
        {
            YLOG_ERROR("CRUD read rpc schema not found!");
            throw(YCPPIllegalStateError{"CRUD read rpc schema not found!"});
        }
        return c[0];
    }
}
