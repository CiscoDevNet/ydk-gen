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

#include "netconf_service.hpp"
#include "path_api.hpp"
#include "entity_data_node_walker.hpp"

using namespace std;

namespace ydk {

std::string get_data_payload(Entity& entity, path::RootSchemaNode& root_schema);

NetconfService::NetconfService()
{
	BOOST_LOG_TRIVIAL(debug) << "ydk.services.NetconfService";
}

//cancel_commit
bool NetconfService::cancel_commit(NetconfServiceProvider & provider, std::string persist_id)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing cancel-commit RPC";

	// Get the operation
	auto operation = "ietf-netconf:cancel-commit";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;
	if (persist_id.size() > 0)
	{
		auto result = ydk_rpc->input()->create("persist-id", persist_id);
		is_pass = is_pass and (result != nullptr);
	}

	return is_pass and (ydk_rpc != nullptr);
}

//close_session
bool NetconfService::close_session(NetconfServiceProvider & provider)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing close-session RPC";

	// Get the operation
	auto operation = "ietf-netconf:close-session";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	return ydk_rpc != nullptr;
}

//commit
bool NetconfService::commit(NetconfServiceProvider & provider, std::string confirmed, 
	std::string confirm_timeout, std::string persist, std::string persist_id)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing commit RPC";

	// Get the operation
	auto operation = "ietf-netconf:commit";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	if (confirmed.size() > 0)
	{
		auto result = ydk_rpc->input()->create("confirmed", confirmed);
		is_pass = is_pass and (result != nullptr);
	}
	
	if (confirm_timeout.size() > 0)
	{
		auto result = ydk_rpc->input()->create("confirm-timeout", confirm_timeout);
		is_pass = is_pass and (result != nullptr);
	}

	if (persist.size() > 0)
	{
		auto result = ydk_rpc->input()->create("persist", persist);
		is_pass = is_pass and (result != nullptr);
	}

	if (persist_id.size() > 0)
	{
		auto result = ydk_rpc->input()->create("persist-id", persist_id);
		is_pass = is_pass and (result != nullptr);
	}

	return is_pass and (ydk_rpc != nullptr);
}

//copy_config
bool NetconfService::copy_config(NetconfServiceProvider & provider, DataStore target, DataStore source)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing copy-config RPC";

	// Get the operation
	auto operation = "ietf-netconf:copy-config";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// target options: candidate | running | startup | url
	if (target == DataStore::candidate)
	{
		auto result = ydk_rpc->input()->create("target/candidate");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::running)
	{
		auto result = ydk_rpc->input()->create("target/running");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::startup)
	{
		auto result = ydk_rpc->input()->create("target/startup");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::url)
	{
		auto result = ydk_rpc->input()->create("target/url");
		is_pass = is_pass and (result != nullptr);
	}

	// source options: candidate | running | startup | url | config
	if (source == DataStore::candidate)
	{
		auto result = ydk_rpc->input()->create("source/candidate");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::running)
	{
		auto result = ydk_rpc->input()->create("source/running");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::startup)
	{
		auto result = ydk_rpc->input()->create("source/startup");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::url)
	{
		auto result = ydk_rpc->input()->create("source/url");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::config)
	{
		auto result = ydk_rpc->input()->create("source/config");
		is_pass = is_pass and (result != nullptr);
	}

	return is_pass and (ydk_rpc != nullptr);
}

bool NetconfService::copy_config(NetconfServiceProvider & provider, DataStore target, Entity& source)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing copy-config RPC";

	// Get the operation
	auto operation = "ietf-netconf:copy-config";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// target options: candidate | running | startup | url
	if (target == DataStore::candidate)
	{
		auto result = ydk_rpc->input()->create("target/candidate");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::running)
	{
		auto result = ydk_rpc->input()->create("target/running");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::startup)
	{
		auto result = ydk_rpc->input()->create("target/startup");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::url)
	{
		auto result = ydk_rpc->input()->create("target/url");
		is_pass = is_pass and (result != nullptr);
	}

	// source
	std::string entity_string = get_data_payload(source, *root_schema);
	auto result = ydk_rpc->input()->create("source/config", entity_string);
	is_pass = is_pass and (result != nullptr);

	return is_pass and (ydk_rpc != nullptr);
}

//delete_config -- url ??
bool NetconfService::delete_config(NetconfServiceProvider & provider, DataStore target)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing delete-config RPC";

	// Get the operation
	auto operation = "ietf-netconf:delete-config";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// target options: startup | url
	if (target == DataStore::startup)
	{
		auto result = ydk_rpc->input()->create("target/startup");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::url)
	{
		auto result = ydk_rpc->input()->create("target/url");
		is_pass = is_pass and (result != nullptr);
	}

	return is_pass and (ydk_rpc != nullptr);
}

//discard_changes
bool NetconfService::discard_changes(NetconfServiceProvider & provider)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing discard-changes RPC";

	// Get the operation
	auto operation = "ietf-netconf:discard-changes";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	return ydk_rpc != nullptr;
}

//edit_config -- choice edit-content config/url??
bool NetconfService::edit_config(NetconfServiceProvider & provider, DataStore target, 
	Entity& config, std::string default_operation, std::string test_option, std::string error_option)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing edit-config RPC";

	// Get the operation
	auto operation = "ietf-netconf:edit-config";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// target options: candidate | running
	if (target == DataStore::candidate)
	{
		auto result = ydk_rpc->input()->create("target/candidate");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::running)
	{
		auto result = ydk_rpc->input()->create("target/running");
		is_pass = is_pass and (result != nullptr);
	}

	//config
	std::string entity_string = get_data_payload(config, *root_schema);
	auto result = ydk_rpc->input()->create("config", entity_string);
	is_pass = is_pass and (result != nullptr);

	if (default_operation.size() > 0)
	{
		auto result = ydk_rpc->input()->create("default-operation");
		is_pass = is_pass and (result != nullptr);
	}

	if (test_option.size() > 0)
	{
		auto result = ydk_rpc->input()->create("test-option");
		is_pass = is_pass and (result != nullptr);
	}

	if (error_option.size() > 0)
	{
		auto result = ydk_rpc->input()->create("error-option");
		is_pass = is_pass and (result != nullptr);
	}

	return is_pass and (ydk_rpc != nullptr);
}

//get_config
bool NetconfService::get_config(NetconfServiceProvider & provider, DataStore source, Entity& filter)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing get-config RPC";

	// Get the operation
	auto operation = "ietf-netconf:get-config";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// source options: candidate | running | startup
	if (source == DataStore::candidate)
	{
		auto result = ydk_rpc->input()->create("source/candidate");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::running)
	{
		auto result = ydk_rpc->input()->create("source/running");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::startup)
	{
		auto result = ydk_rpc->input()->create("source/startup");
		is_pass = is_pass and (result != nullptr);
	}

	// filter
	std::string entity_string = get_data_payload(filter, *root_schema);
	auto result = ydk_rpc->input()->create("filter", entity_string);
	is_pass = is_pass and (result != nullptr);

	return is_pass and (ydk_rpc != nullptr);
}

//get
bool NetconfService::get(NetconfServiceProvider & provider, Entity& filter)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing get RPC";

	// Get the operation
	auto operation = "ietf-netconf:get";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = ydk_rpc != nullptr;

	// filter 
	std::string entity_string = get_data_payload(filter, *root_schema);
	auto result = ydk_rpc->input()->create("filter", entity_string);
	is_pass = is_pass and (result != nullptr);

	return is_pass;
}

//kill_session
bool NetconfService::kill_session(NetconfServiceProvider & provider, int session_id)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing kill-session RPC";

	// Get the operation
	auto operation = "ietf-netconf:kill-session";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	std::string sid_string = std::to_string(session_id);
	auto result = ydk_rpc->input()->create("session-id", sid_string);
	return (result != nullptr) and (ydk_rpc != nullptr);
}

//lock
bool NetconfService::lock(NetconfServiceProvider & provider, DataStore target)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing lock RPC";

	// Get the operation
	auto operation = "ietf-netconf:lock";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// target options: candidate | running | startup
	if (target == DataStore::candidate)
	{
		auto result = ydk_rpc->input()->create("target/candidate");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::running)
	{
		auto result = ydk_rpc->input()->create("target/running");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::startup)
	{
		auto result = ydk_rpc->input()->create("target/startup");
		is_pass = is_pass and (result != nullptr);
	}

	return is_pass and (ydk_rpc != nullptr);
}

//unlock
bool NetconfService::unlock(NetconfServiceProvider & provider, DataStore target)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing unlock RPC";

	// Get the operation
	auto operation = "ietf-netconf:unlock";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// target options: candidate | running | startup
	if (target == DataStore::candidate)
	{
		auto result = ydk_rpc->input()->create("target/candidate");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::running)
	{
		auto result = ydk_rpc->input()->create("target/running");
		is_pass = is_pass and (result != nullptr);
	}
	else if (target == DataStore::startup)
	{
		auto result = ydk_rpc->input()->create("target/startup");
		is_pass = is_pass and (result != nullptr);
	}

	return is_pass and (ydk_rpc != nullptr);
}

//validate -- url??
bool NetconfService::validate(NetconfServiceProvider & provider, DataStore source)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing validate RPC";

	// Get the operation
	auto operation = "ietf-netconf:validate";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// source options: candidate | running | startup | url
	if (source == DataStore::candidate)
	{
		auto result = ydk_rpc->input()->create("source/candidate");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::running)
	{
		auto result = ydk_rpc->input()->create("source/running");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::startup)
	{
		auto result = ydk_rpc->input()->create("source/startup");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::url)
	{
		auto result = ydk_rpc->input()->create("source/url");
		is_pass = is_pass and (result != nullptr);
	}
	else if (source == DataStore::config)
	{
		auto result = ydk_rpc->input()->create("source/config");
		is_pass = is_pass and (result != nullptr);
	}

	return is_pass and (ydk_rpc != nullptr);
}

bool NetconfService::validate(NetconfServiceProvider & provider, Entity& source)
{
	BOOST_LOG_TRIVIAL(debug) << "Executing validate RPC";

	// Get the operation
	auto operation = "ietf-netconf:validate";

	// Get the root schema node
	path::RootSchemaNode* root_schema = provider.get_root_schema();
	unique_ptr<ydk::path::Rpc> ydk_rpc { root_schema->rpc(operation) };

	bool is_pass = true;

	// source
	std::string entity_string = get_data_payload(source, *root_schema);
	auto result = ydk_rpc->input()->create("source/config", entity_string);
	is_pass = is_pass and (result != nullptr);

	return is_pass and (ydk_rpc != nullptr);
}

std::string get_data_payload(Entity & entity, path::RootSchemaNode & root_schema)
{
	const ydk::path::DataNode* data_node = get_data_node_from_entity(entity, root_schema);
	if (data_node==nullptr)
		return "";
	path::CodecService codec{};
	return codec.encode(data_node, ydk::path::CodecService::Format::XML, true);
}

}