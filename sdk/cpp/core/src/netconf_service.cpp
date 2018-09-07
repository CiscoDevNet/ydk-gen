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

#include "netconf_service.hpp"
#include "entity_data_node_walker.hpp"
#include "errors.hpp"
#include "path_api.hpp"
#include "validation_service.hpp"
#include "xml_subtree_codec.hpp"
#include "logger.hpp"
#include "common_utilities.hpp"

using namespace std;

namespace ydk {

static shared_ptr<path::Rpc> get_rpc_instance(NetconfServiceProvider& provider, const char* path);
static void create_input_leaf(path::DataNode & input_datanode, DataStore datastore, string && datastore_string, string & url);
static void create_input_leaf(path::DataNode & input_datanode, DataStore datastore, string && datastore_string);
static string datastore_to_string(DataStore datastore);

NetconfService::NetconfService()
{
}

NetconfService::~NetconfService()
{
}

//cancel_commit
bool NetconfService::cancel_commit(NetconfServiceProvider& provider, int persist_id)
{
    YLOG_INFO("Executing 'cancel-commit' RPC, persist-id={}", persist_id);

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:cancel-commit");

    if (persist_id > -1)
    {
        rpc->get_input_node().create_datanode("persist-id", std::to_string(persist_id));
    }

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

//close_session
bool NetconfService::close_session(NetconfServiceProvider& provider)
{
    YLOG_INFO("Executing 'close-session' RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:close-session");

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

//commit
bool NetconfService::commit(NetconfServiceProvider& provider, bool confirmed,
    int confirm_timeout, int persist, int persist_id)
{
    YLOG_INFO("Executing 'commit' RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:commit");

    if (confirmed)
    {
        rpc->get_input_node().create_datanode("confirmed");
    }

    if (confirm_timeout > -1)
    {
        rpc->get_input_node().create_datanode("confirm-timeout", std::to_string(confirm_timeout));
    }

    if (persist > -1)
    {
        rpc->get_input_node().create_datanode("persist", std::to_string(persist));
    }

    if (persist_id > -1)
    {
        rpc->get_input_node().create_datanode("persist-id", std::to_string(persist_id));
    }

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

//copy_config
bool NetconfService::copy_config(NetconfServiceProvider& provider, DataStore target, DataStore source, string url)
{
    YLOG_INFO("Executing 'copy-config' RPC from {} to {}, URL={}", datastore_to_string(source), datastore_to_string(target), url);

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:copy-config");

    // target options: candidate | running | startup | url
    create_input_leaf(rpc->get_input_node(), target, "target", url);
    create_input_leaf(rpc->get_input_node(), source, "source", url);

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

static bool copy_payload(NetconfServiceProvider& provider, DataStore target, string payload)
{
    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:copy-config");

    // target options: candidate | running | startup | url
    create_input_leaf(rpc->get_input_node(), target, "target");

    // source
    rpc->get_input_node().create_datanode("source/config", payload);

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

bool NetconfService::copy_config(NetconfServiceProvider& provider, DataStore target, Entity& source)
{
    YLOG_INFO("Executing 'copy-config' RPC from [{}] to {}", source.get_segment_path(), datastore_to_string(target));

    std::string entity_string = get_data_payload(source, provider);

    return copy_payload(provider, target, entity_string);
}

bool NetconfService::copy_config(NetconfServiceProvider& provider, DataStore target, vector<Entity*>& source_list)
{
    YLOG_INFO("Executing 'copy-config' RPC from {} to {}", entity_vector_to_string(source_list), datastore_to_string(target));

    string payload = "";
    for (auto entity : source_list) {
        payload += get_data_payload(*entity, provider);
    }

    return copy_payload(provider, target, payload);
}

//delete_config
bool NetconfService::delete_config(NetconfServiceProvider& provider, DataStore target, std::string url)
{
    YLOG_INFO("Executing 'delete-config' RPC, target={}", datastore_to_string(target));

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:delete-config");

    // target options: startup | url
    create_input_leaf(rpc->get_input_node(), target, "target", url);

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

//discard_changes
bool NetconfService::discard_changes(NetconfServiceProvider& provider)
{
    YLOG_INFO("Executing 'discard-changes' RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:discard-changes");

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

//edit_config
static bool
edit_payload(NetconfServiceProvider& provider, DataStore target,
		string& payload, string& default_operation, string& test_option, string& error_option)
{
    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:edit-config");

    // target options: candidate | running
    create_input_leaf(rpc->get_input_node(), target, "target");

    //config
    rpc->get_input_node().create_datanode("config", payload);

    if (default_operation.size() > 0)
    {
        rpc->get_input_node().create_datanode("default-operation", default_operation);
    }

    if (test_option.size() > 0)
    {
        rpc->get_input_node().create_datanode("test-option", test_option);
    }

    if (error_option.size() > 0)
    {
        rpc->get_input_node().create_datanode("error-option", error_option);
    }

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

bool NetconfService::edit_config(NetconfServiceProvider& provider, DataStore target,
    Entity& config, string default_operation, string test_option, string error_option)
{
    YLOG_INFO("Executing 'edit-config' RPC on [{}]", config.get_segment_path());

    std::string payload = get_data_payload(config, provider);

    return edit_payload(provider, target, payload, default_operation, test_option, error_option);
}

bool NetconfService::edit_config(NetconfServiceProvider& provider, DataStore target,
	vector<Entity*>& config_list, string default_operation, string test_option, string error_option)
{
    YLOG_INFO("Executing 'edit-config' RPC on {}", entity_vector_to_string(config_list));

    string payload = "";
    for (auto entity : config_list) {
        payload += get_xml_subtree_filter_payload(*entity, provider);
    }

    return edit_payload(provider, target, payload, default_operation, test_option, error_option);
}

static vector<shared_ptr<Entity>>
get_from_list(NetconfServiceProvider& provider, DataStore source, vector<Entity*> & filter_list, const char* path)
{
    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, path);

    // Build filter
    string filter_string = "";
    for (auto entity : filter_list) {
        filter_string += get_xml_subtree_filter_payload(*entity, provider);
    }

    // Source options: candidate | running | startup
    if (source != DataStore::na)
        create_input_leaf(rpc->get_input_node(), source, "source");
    rpc->get_input_node().create_datanode("filter", filter_string);

    // Get root data node
    vector<shared_ptr<Entity>> result_list{};
    shared_ptr<path::DataNode> result_datanode = (*rpc)(provider.get_session());
    if (result_datanode == nullptr)
        return result_list;

    // Build mapping of DataNode path to DataNode pointer.
    // Use this mapping to retain order of filter list in results.
    map<string,shared_ptr<path::DataNode>> path_to_datanode{};
    for (auto dn : result_datanode->get_children()) {
        string internal_key = dn->get_path().substr(1);
        path_to_datanode[internal_key] = dn;
    }

    // Build resulting list of entities
    for (Entity* entity : filter_list) {
        string internal_key = entity->get_segment_path();
        shared_ptr<path::DataNode> datanode = path_to_datanode[internal_key];
        if (!datanode) {
            YLOG_DEBUG("Searching for datanode using entity yang name '{}'", entity->yang_name);
            path_to_datanode.erase(internal_key);
            for (auto dn_entry : path_to_datanode) {
                if (dn_entry.first.find(entity->yang_name) != string::npos && dn_entry.second) {
                    datanode = dn_entry.second;
                    break;
                }
            }
        }
        if (datanode) {
            result_list.push_back( read_datanode(*entity, datanode));
        }
        else {
            YLOG_DEBUG("Netconf Service 'get' operation did not return data node on entity '{}'; returning nullptr.", internal_key);
            result_list.push_back(nullptr);
        }
    }

    return result_list;
}

vector<shared_ptr<Entity>>
NetconfService::get(NetconfServiceProvider& provider, vector<Entity*> & filter_list)
{
    YLOG_INFO("Executing 'get' RPC on {}", entity_vector_to_string(filter_list));
    return get_from_list(provider, DataStore::na, filter_list, "ietf-netconf:get");
}

vector<shared_ptr<Entity>>
NetconfService::get_config(NetconfServiceProvider& provider, DataStore source, vector<Entity*> & filter_list)
{
    YLOG_INFO("Executing 'get-config' RPC on {} from {}", entity_vector_to_string(filter_list), datastore_to_string(source));
    return get_from_list(provider, source, filter_list, "ietf-netconf:get-config");
}

static shared_ptr<Entity>
get_entity(NetconfServiceProvider& provider, DataStore source, Entity& filter, const char* path)
{
    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, path);
    if (source != DataStore::na) {
        create_input_leaf(rpc->get_input_node(), source, "source");
    }

    // filter
    std::string filter_string  = get_xml_subtree_filter_payload(filter, provider);
    rpc->get_input_node().create_datanode("filter", filter_string);

    shared_ptr<path::DataNode> result_datanode = (*rpc)(provider.get_session());
    if (result_datanode == nullptr)
        return {};

    vector<shared_ptr<path::DataNode>> data_nodes = result_datanode->get_children();
    if (data_nodes.size() == 0)
        return nullptr;

    return read_datanode( filter, data_nodes[0]);
}

shared_ptr<Entity> NetconfService::get(NetconfServiceProvider& provider, Entity& filter)
{
    YLOG_INFO("Executing 'get' RPC on [{}]", filter.get_segment_path());
    return get_entity(provider, DataStore::na, filter, "ietf-netconf:get");
}

shared_ptr<Entity> NetconfService::get_config(NetconfServiceProvider& provider, DataStore source, Entity& filter)
{
    YLOG_INFO("Executing 'get' RPC on [{}] from {}", filter.get_segment_path(), datastore_to_string(source));
    return get_entity(provider, source, filter, "ietf-netconf:get-config");
}

//kill_session
bool NetconfService::kill_session(NetconfServiceProvider& provider, unsigned int session_id)
{
    YLOG_INFO("Executing 'kill-session' RPC");

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:kill-session");

    std::string sid_string = std::to_string(session_id);
    rpc->get_input_node().create_datanode("session-id", sid_string);

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

//lock
bool NetconfService::lock(NetconfServiceProvider& provider, DataStore target)
{
    YLOG_INFO("Executing 'lock' RPC on {}", datastore_to_string(target));

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:lock");

    // target options: candidate | running | startup
    create_input_leaf(rpc->get_input_node(), target, "target");

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

//unlock
bool NetconfService::unlock(NetconfServiceProvider& provider, DataStore target)
{
    YLOG_INFO("Executing 'unlock' RPC on {}", datastore_to_string(target));

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:unlock");

    // target options: candidate | running | startup
    create_input_leaf(rpc->get_input_node(), target, "target");

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

//validate
bool NetconfService::validate(NetconfServiceProvider& provider, DataStore source, string url)
{
    YLOG_INFO("Executing 'validate' RPC on {}, URL={}", datastore_to_string(source), url);

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:validate");

    // source options: candidate | running | startup | url
    create_input_leaf(rpc->get_input_node(), source, "source", url);

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

bool NetconfService::validate(NetconfServiceProvider& provider, Entity& source)
{
    YLOG_INFO("Executing 'validate' RPC on [{}]", source.get_segment_path());

    // Get the root schema node
    shared_ptr<path::Rpc> rpc = get_rpc_instance(provider, "ietf-netconf:validate");

    // source
    std::string entity_string = get_data_payload(source, provider);
    rpc->get_input_node().create_datanode("source/config", entity_string);

    auto read_datanode = (*rpc)(provider.get_session());
    return read_datanode == nullptr;
}

static shared_ptr<path::Rpc> get_rpc_instance(NetconfServiceProvider& provider, const char* operation)
{
    path::RootSchemaNode & root_schema = provider.get_session().get_root_schema();
    auto rpc =  root_schema.create_rpc(operation);
    if (rpc == nullptr)
        throw(YError{"Unable to create rpc"});

    return rpc;
}

static void create_input_leaf(path::DataNode & input_datanode, DataStore datastore, string && datastore_string, string & url)
{
    ostringstream os;
    os << datastore_string;

    if(datastore == DataStore::url)
    {
        if(url.size() == 0)
        {
            throw(YServiceError{"URL needs to be specified"});
        }
        os << "/" << datastore_to_string(datastore);

        input_datanode.create_datanode(os.str(), url);
    }
    else
    {
        create_input_leaf(input_datanode, datastore, move(datastore_string));
    }
}

static void create_input_leaf(path::DataNode & input_datanode, DataStore datastore, string && datastore_string)
{
    ostringstream os;
    os << datastore_string << "/" << datastore_to_string(datastore);
    input_datanode.create_datanode(os.str());
}

static string datastore_to_string(DataStore datastore)
{
    switch(datastore)
    {
        case DataStore::candidate:
            return "candidate";
        case DataStore::running:
            return "running";
        case DataStore::startup:
            return "startup";
        case DataStore::url:
            return "url";
        case DataStore::na:
        default:
            return "na";
    }
}

}
