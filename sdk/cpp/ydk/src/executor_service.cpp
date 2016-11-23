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

#include <boost/log/trivial.hpp>
#include <iostream>

#include "executor_service.hpp"
#include "types.hpp"
#include "path_api.hpp"
#include "entity_data_node_walker.hpp"

using namespace std;

namespace ydk{

// static string get_data_payload(Entity & entity, string data_tag, path::RootSchemaNode & root_schema);
string get_netconf_payload(path::DataNode* input, string data_value, string data_tag);

ExecutorService::ExecutorService()
{
	BOOST_LOG_TRIVIAL(debug) << "ydk.services.NetconfService";
}

bool ExecutorService::execute_rpc(NetconfServiceProvider & provider, Entity & entity)
{
	// Get the operation
	auto operation = entity.get_entity_path(entity.parent).path;

	// Set the data tag
	string data_tag = "entity";

	// Set the config flag
	bool set_config_flag = false;

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };
	// to do: implement way to walk through children to get what to create
	ydk_rpc->input()->create("source/candidate");	// -- hard coding
	
	// Create netconf payload
	path::CodecService codec_service{};
	std::string netconf_payload{"<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"};
    netconf_payload+=codec_service.encode(ydk_rpc->input(), path::CodecService::Format::XML, false);
    netconf_payload+="</rpc>";

    // Execute payload
	std::string reply = provider.execute_payload(netconf_payload);

	// Scan reply for OK
	return reply.find("<ok/>") != string::npos;
}

string get_netconf_payload(path::DataNode* input, string data_value, string data_tag)
{
    path::CodecService codec_service{};
    auto config_node = input->create(data_tag, data_value);
    if(!config_node)
    {
        BOOST_LOG_TRIVIAL(debug) << "Failed to create data tree";
        throw YDKIllegalStateException{"Failed to create data tree"};
    }

    std::string payload{"<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"};
    payload+=codec_service.encode(input, path::CodecService::Format::XML, false);
    payload+="</rpc>";
    BOOST_LOG_TRIVIAL(debug) <<"=============Generating payload=============";
    BOOST_LOG_TRIVIAL(debug) <<payload;
    BOOST_LOG_TRIVIAL(debug) <<"=========================="<<endl;
    return payload;
}

}
