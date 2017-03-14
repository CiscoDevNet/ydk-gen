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

#include "entity_data_node_walker.hpp"
#include "errors.hpp"
#include "netconf_service.hpp"
#include "path_api.hpp"
#include "validation_service.hpp"
#include "logger.hpp"

using namespace std;

namespace ydk {

static std::string get_data_payload(Entity& entity, path::RootSchemaNode& root_schema);
static shared_ptr<Entity> get_top_entity_from_filter(Entity & filter);
static shared_ptr<path::Rpc> get_rpc_instance(NetconfServiceProvider & provider, string && operation);
static void create_input_leaf(path::DataNode & input_datanode, DataStore datastore, string && datastore_string, string & url);
static void create_input_leaf(path::DataNode & input_datanode, DataStore datastore, string && datastore_string);

NetconfService::NetconfService()
{
}

NetconfService::~NetconfService()
{
}

//cancel_commit
bool NetconfService::cancel_commit(NetconfServiceProvider & provider, int persist_id)
{
	YLOG_INFO("Executing cancel-commit RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:cancel-commit");

    if (persist_id > -1)
    {
        rpc->input().create("persist-id", std::to_string(persist_id));
    }

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//close_session
bool NetconfService::close_session(NetconfServiceProvider & provider)
{
	YLOG_INFO("Executing close-session RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:close-session");

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//commit
bool NetconfService::commit(NetconfServiceProvider & provider, bool confirmed,
    int confirm_timeout, int persist, int persist_id)
{
	YLOG_INFO("Executing commit RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:commit");

    if (confirmed)
    {
        rpc->input().create("confirmed");
    }

    if (confirm_timeout > -1)
    {
        rpc->input().create("confirm-timeout", std::to_string(confirm_timeout));
    }

    if (persist > -1)
    {
        rpc->input().create("persist", std::to_string(persist));
    }

    if (persist_id > -1)
    {
        rpc->input().create("persist", std::to_string(persist_id));
    }

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//copy_config
bool NetconfService::copy_config(NetconfServiceProvider & provider, DataStore target, DataStore source, string url)
{
	YLOG_INFO("Executing copy-config RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:copy-config");

    // target options: candidate | running | startup | url
    create_input_leaf(rpc->input(), target, "target", url);
    create_input_leaf(rpc->input(), source, "source", url);

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

bool NetconfService::copy_config(NetconfServiceProvider & provider, DataStore target, Entity& source)
{
	YLOG_INFO("Executing copy-config RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:copy-config");

    // target options: candidate | running | startup | url
    create_input_leaf(rpc->input(), target, "target");

    // source
    std::string entity_string = get_data_payload(source, provider.get_root_schema());
    rpc->input().create("source/config", entity_string);

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//delete_config
bool NetconfService::delete_config(NetconfServiceProvider & provider, DataStore target, std::string url)
{
	YLOG_INFO("Executing delete-config RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:delete-config");

    // target options: startup | url
    create_input_leaf(rpc->input(), target, "target", url);

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//discard_changes
bool NetconfService::discard_changes(NetconfServiceProvider & provider)
{
	YLOG_INFO("Executing discard-changes RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:discard-changes");

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//edit_config
bool NetconfService::edit_config(NetconfServiceProvider & provider, DataStore target,
    Entity& config, std::string default_operation, std::string test_option, std::string error_option)
{
	YLOG_INFO("Executing edit-config RPC");
	ValidationService validation{};
	validation.validate(provider, config, ValidationService::Option::DATASTORE);

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:edit-config");

    // target options: candidate | running
    create_input_leaf(rpc->input(), target, "target");

    //config
    std::string entity_string = get_data_payload(config, provider.get_root_schema());
    rpc->input().create("config", entity_string);

    if (default_operation.size() > 0)
    {
        rpc->input().create("default-operation", default_operation);
    }

    if (test_option.size() > 0)
    {
        rpc->input().create("test-option", test_option);
    }

    if (error_option.size() > 0)
    {
        rpc->input().create("error-option", error_option);
    }

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//get_config
shared_ptr<Entity> NetconfService::get_config(NetconfServiceProvider & provider, DataStore source, Entity& filter)
{
	YLOG_INFO("Executing get-config RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:get-config");

    // source options: candidate | running | startup
    create_input_leaf(rpc->input(), source, "source");

    // filter
    std::string entity_string = get_data_payload(filter, provider.get_root_schema());
    rpc->input().create("filter", entity_string);

    auto read_datanode = (*rpc)(provider);
    if (read_datanode == nullptr)
        return nullptr;

    shared_ptr<Entity> top_entity = get_top_entity_from_filter(filter);
    get_entity_from_data_node(read_datanode->children()[0].get(), top_entity);
    return top_entity;

}

//get
shared_ptr<Entity> NetconfService::get(NetconfServiceProvider & provider, Entity& filter)
{
	YLOG_INFO("Executing get RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:get");

    // filter
    std::string entity_string = get_data_payload(filter, provider.get_root_schema());
    rpc->input().create("filter", entity_string);

    auto result_datanode = (*rpc)(provider);
    if (result_datanode == nullptr)
        return {};
    shared_ptr<Entity> top_entity = get_top_entity_from_filter(filter);
    get_entity_from_data_node(result_datanode->children()[0].get(), top_entity);
    return top_entity;
}

//kill_session
bool NetconfService::kill_session(NetconfServiceProvider & provider, int session_id)
{
	YLOG_INFO("Executing kill-session RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:kill-session");

    std::string sid_string = std::to_string(session_id);
    rpc->input().create("session-id", sid_string);

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//lock
bool NetconfService::lock(NetconfServiceProvider & provider, DataStore target)
{
	YLOG_INFO("Executing lock RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:lock");

    // target options: candidate | running | startup
    create_input_leaf(rpc->input(), target, "target");

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//unlock
bool NetconfService::unlock(NetconfServiceProvider & provider, DataStore target)
{
	YLOG_INFO("Executing unlock RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:unlock");

    // target options: candidate | running | startup
    create_input_leaf(rpc->input(), target, "target");

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

//validate
bool NetconfService::validate(NetconfServiceProvider & provider, DataStore source, string url)
{
	YLOG_INFO("Executing validate RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:validate");

    // source options: candidate | running | startup | url
    create_input_leaf(rpc->input(), source, "source", url);

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

bool NetconfService::validate(NetconfServiceProvider & provider, Entity& source)
{
	YLOG_INFO("Executing validate RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:validate");

    // source
    std::string entity_string = get_data_payload(source, provider.get_root_schema());
    rpc->input().create("source/config", entity_string);

    auto read_datanode = (*rpc)(provider);
    return read_datanode == nullptr;
}

static shared_ptr<path::Rpc> get_rpc_instance(NetconfServiceProvider & provider, string && operation)
{
	path::RootSchemaNode & root_schema = provider.get_root_schema();
	auto rpc =  root_schema.rpc(operation);
	if (rpc == nullptr)
		throw(YCPPServiceProviderError{"Unable to create rpc"});

    return rpc;
}

static std::string get_data_payload(Entity & entity, path::RootSchemaNode & root_schema)
{
    path::DataNode& data_node = get_data_node_from_entity(entity, root_schema);
    path::CodecService codec{};
    return codec.encode(data_node, ydk::EncodingFormat::XML, true);
}

static shared_ptr<Entity> get_top_entity_from_filter(Entity & filter)
{
    if(filter.parent == nullptr)
        return filter.clone_ptr();

    return get_top_entity_from_filter(*(filter.parent));
}

static void create_input_leaf(path::DataNode & input_datanode, DataStore datastore, string && datastore_string, string & url)
{
    ostringstream os;
    os << datastore_string;

    if(datastore == DataStore::url)
    {
        if(url.size() == 0)
        {
            throw(YCPPServiceError{"URL needs to be specified"});
        }
        os << "/url";

        input_datanode.create(os.str(), url);
    }
    else
    {
        create_input_leaf(input_datanode, datastore, move(datastore_string));
    }
}

static void create_input_leaf(path::DataNode & input_datanode, DataStore datastore, string && datastore_string)
{
    ostringstream os;
    os << datastore_string;

    switch(datastore)
    {
        case DataStore::candidate:
            os << "/candidate";
            break;

        case DataStore::running:
            os << "/running";
            break;

        case DataStore::startup:
            os << "/startup";
            break;

        case DataStore::url:
            throw(YCPPServiceError{"URL needs to be specified"});
            break;
    }

    input_datanode.create(os.str());
}
}
