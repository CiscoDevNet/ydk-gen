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


#include "crud_service.hpp"
#include "types.hpp"
#include "path_api.hpp"
#include "entity_data_node_walker.hpp"
#include "validation_service.hpp"
#include "logger.hpp"
#include <sstream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

using namespace std;

namespace ydk {

static string get_data_payload(Entity & entity, path::ServiceProvider & provider);
static std::shared_ptr<path::DataNode> execute_rpc(path::ServiceProvider & provider, Entity & entity,
		const string & operation, const string & data_tag, bool set_config_flag);
static shared_ptr<Entity> get_top_entity_from_filter(Entity & filter);
static bool operation_succeeded(shared_ptr<path::DataNode> node);

CrudService::CrudService()
{
}

bool CrudService::create(path::ServiceProvider & provider, Entity & entity)
{
	YLOG_INFO("Executing CRUD create operation");
	return operation_succeeded(
			execute_rpc(provider, entity, "ydk:create", "entity", false)
			);
}

bool CrudService::update(path::ServiceProvider & provider, Entity & entity)
{
	YLOG_INFO("Executing CRUD update operation");
	return operation_succeeded(
			execute_rpc(provider, entity, "ydk:update", "entity", false)
			);
}

bool CrudService::delete_(path::ServiceProvider & provider, Entity & entity)
{
	YLOG_INFO("Executing CRUD delete operation");
	return operation_succeeded(
			execute_rpc(provider, entity, "ydk:delete", "entity", false)
			);
}

shared_ptr<Entity> CrudService::read(path::ServiceProvider & provider, Entity & filter)
{
	YLOG_INFO("Executing CRUD read operation");
	return read_datanode(filter, execute_rpc(provider, filter, "ydk:read", "filter", false));
}

shared_ptr<Entity> CrudService::read_config(path::ServiceProvider & provider, Entity & filter)
{
	YLOG_INFO("Executing CRUD config read operation");
	return read_datanode(filter, execute_rpc(provider, filter, "ydk:read", "filter", true));
}

shared_ptr<Entity> CrudService::read_datanode(Entity & filter, shared_ptr<path::DataNode> read_data_node)
{
    if (read_data_node == nullptr)
        return {};
    shared_ptr<Entity> top_entity = get_top_entity_from_filter(filter);
    get_entity_from_data_node(read_data_node->children()[0].get(), top_entity);
    return top_entity;
}

static bool operation_succeeded(shared_ptr<path::DataNode> node)
{
	YLOG_INFO("Operation {}", ((node == nullptr)?"succeeded":"failed"));
	return node == nullptr;
}

static shared_ptr<Entity> get_top_entity_from_filter(Entity & filter)
{
    if(filter.parent == nullptr)
        return filter.clone_ptr();

    return get_top_entity_from_filter(*(filter.parent));
}

static shared_ptr<path::DataNode> execute_rpc(path::ServiceProvider & provider, Entity & entity,
        const string & operation, const string & data_tag, bool set_config_flag)
{
//    if(data_tag == "entity")
//    {
//        ValidationService validation{}; //TODO
//        validation.validate(provider, entity, ValidationService::Option::DATASTORE);
//    }
    path::RootSchemaNode& root_schema = provider.get_root_schema();
    shared_ptr<ydk::path::Rpc> ydk_rpc { root_schema.rpc(operation) };
    string data = get_data_payload(entity, provider);

    if(set_config_flag)
    {
        ydk_rpc->input().create("only-config");
    }
    ydk_rpc->input().create(data_tag, data);
    return (*ydk_rpc)(provider);
}

static void modify_xml_tree(xmlNodePtr root, const string & data_path,
        const string & leaf_name, YFilter operation)
{
    int i;
    xmlNodePtr node = NULL;

    for (node = root, i= 0; node; node = node->next, i=i+1)
    {
        ostringstream os{};
        xmlNodePtr n = node;
        os<<leaf_name<<"/";
        while(n && n->parent)
        {
            os<<n->name<<"/";
            n = n->parent;
        }
        if(os.str() == data_path)
        {
            YLOG_INFO("Setting leaf {} to operation {}", leaf_name, to_string(operation));
            xmlNodePtr child = xmlNewChild(node, NULL, (const unsigned char *)leaf_name.c_str(), NULL);
            if(operation!=YFilter::read)
            {
                xmlNewProp(child, (const unsigned char *)"operation", (const unsigned char *)(to_string(operation)).c_str());
            }
        }
        modify_xml_tree(node->children, data_path, leaf_name, operation);
    }
}

static string get_data_payload(Entity & entity, path::ServiceProvider & provider)
{
    map<string, pair<string, YFilter>> leaf_operations;
	const ydk::path::DataNode& datanode = get_data_node_from_entity(entity, provider.get_root_schema(), leaf_operations);

    const path::DataNode* dn = &datanode;
    while(dn!= nullptr && dn->parent()!=nullptr)
        dn = dn->parent();
    path::CodecService codec{};
	string payload = codec.encode(*dn, provider.get_encoding(), false);
	if(provider.get_encoding() == EncodingFormat::XML && leaf_operations.size()>0)
	{
        xmlDocPtr doc = xmlParseDoc((const unsigned char *)payload.c_str());
        xmlNodePtr root = xmlDocGetRootElement(doc);
        for(auto & leaf_operation : leaf_operations)
        {
            modify_xml_tree(root, leaf_operation.first, leaf_operation.second.first, leaf_operation.second.second);
        }
        xmlBufferPtr buf = xmlBufferCreate();
        if (buf != NULL)
        {
            xmlNodeDump(buf, doc, root, 0, 1);
            payload.clear();
            string path{reinterpret_cast<char*>(buf->content)};
            payload = path;
            xmlBufferFree(buf);
        }
        else
        {
            YLOG_ERROR("Error creating the xml buffer");
        }
	}
	return payload;
}

}
