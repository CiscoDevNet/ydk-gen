//////////////////////////////////////////////////////////////////
// @file ydk.h
//
// YANG Development Kit
// Copyright 2017 Cisco Systems. All rights reserved
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

#ifndef _YDK_H_
#define _YDK_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* DataNode;
typedef void* Rpc;
typedef void* SchemaNode;
typedef void* RootSchemaNode;
typedef void* RootSchemaWrapper;
typedef void* Codec;
typedef void* ServiceProvider;
typedef void* OpenDaylightServiceProvider;
typedef void* Capability;
typedef void* Repository;
typedef void* YDKStatePtr;

typedef void* Session;

typedef struct DataNodeChildren
{
    DataNode* datanodes;
    int count;
} DataNodeChildren;

typedef int boolean;

typedef enum EncodingFormat
{
    XML   = 0,
    JSON
} EncodingFormat;

typedef enum LogLevel
{
    OFF    = 0,
    DEBUG,
    INFO,
    WARNING,
    ERROR
} LogLevel;

typedef enum Protocol
{
    Netconf = 0,
    Restconf
} Protocol;

typedef enum YDKErrorType {
    YDK_ERROR_NONE = 0,
    YDK_ERROR,
    YDK_CORE_ERROR,
    YDK_CODEC_ERROR,
    YDK_CLIENT_ERROR,
    YDK_SERVICE_PROVIDER_ERROR,
    YDK_SERVICE_ERROR,
    YDK_ILLEGAL_STATE_ERROR,
    YDK_INVALID_ARGUMENT_ERROR,
    YDK_OPERATION_NOTSUPPORTED_ERROR,
    YDK_MODEL_ERROR
} YDKErrorType;

typedef struct YDKState {
    boolean error_occurred;
    char* error_message;
    YDKErrorType error_type;
} YDKState;

YDKStatePtr YDKStateCreate(void);
const char * YDKStateGetErrorMessage(YDKStatePtr);
YDKErrorType YDKStateGetErrorType(YDKStatePtr);
boolean YDKStateErrorOccurred(YDKStatePtr);
void YDKStateClear(YDKStatePtr ptr);
void YDKStateFree(YDKStatePtr);

void handle_error_message(YDKState* state, const char * message);
void handle_error(YDKState* state);

Capability CapabilityCreate(YDKStatePtr state, const char* mod, const char* rev);
void CapabilityFree(Capability);

Repository RepositoryInitWithPath(YDKStatePtr, const char*);
Repository RepositoryInit(void);
RootSchemaWrapper RepositoryCreateRootSchemaWrapper(YDKStatePtr state, Repository, const char* keys[], const Capability values[], int size);
void RepositoryFree(Repository);

ServiceProvider NetconfServiceProviderInit(YDKStatePtr state, const char * address, const char * username, const char * password, int port, const char * protocol);
ServiceProvider NetconfServiceProviderInitWithOnDemand(YDKStatePtr state, const char * address, const char * username, const char * password, int port, const char * protocol, boolean on_demand, boolean common_cache);
ServiceProvider NetconfServiceProviderInitWithRepo(YDKStatePtr state, Repository repo, const char * address, const char * username, const char * password, int port, const char * protocol);
ServiceProvider NetconfServiceProviderInitWithOnDemandRepo(YDKStatePtr state, Repository repo, const char * address, const char * username, const char * password, int port, const char * protocol, boolean on_demand);
RootSchemaNode ServiceProviderGetRootSchema(YDKStatePtr, ServiceProvider);
RootSchemaWrapper ServiceProviderGetRootSchemaNode(YDKStatePtr, ServiceProvider);
EncodingFormat ServiceProviderGetEncoding(ServiceProvider);
Session ServiceProviderGetSession(ServiceProvider);
RootSchemaNode SessionGetRootSchema(YDKStatePtr, Session);
void NetconfServiceProviderFree(ServiceProvider);
int NetconfServiceProviderGetNumCapabilities(ServiceProvider);
const char* NetconfServiceProviderGetCapabilityByIndex(ServiceProvider, int);

ServiceProvider RestconfServiceProviderInitWithRepo(YDKStatePtr state, Repository repo, const char * address, const char * username, const char * password, int port, EncodingFormat encoding, const char* config_url_root, const char* state_url_root);
void RestconfServiceProviderFree(ServiceProvider);

OpenDaylightServiceProvider OpenDaylightServiceProviderInitWithRepo(YDKStatePtr state, Repository repo, const char * address, const char * username, const char * password, int port, EncodingFormat encoding, Protocol protocol);
void OpenDaylightServiceProviderFree(OpenDaylightServiceProvider);
ServiceProvider OpenDaylightServiceProviderGetNodeProvider(YDKStatePtr state, OpenDaylightServiceProvider provider, const char * node_id);
const char* OpenDaylightServiceProviderGetNodeIDByIndex(YDKStatePtr state, OpenDaylightServiceProvider provider, int idx);

const char * ServiceProviderType(ServiceProvider);

Codec CodecInit();
void CodecFree(Codec);
const char* CodecEncode(YDKStatePtr state, Codec, DataNode, EncodingFormat, boolean);
DataNode CodecDecode(YDKStatePtr state, Codec, RootSchemaNode, const char*, EncodingFormat);

DataNode RootSchemaNodeCreate(YDKStatePtr, RootSchemaNode, const char*);
Rpc RootSchemaNodeRpc(YDKStatePtr state, RootSchemaNode, const char*);
RootSchemaNode RootSchemaWrapperUnwrap(RootSchemaWrapper);

DataNode RpcInput(YDKStatePtr, Rpc);
DataNode RpcExecute(YDKStatePtr, Rpc, ServiceProvider);

DataNode DataNodeCreate(YDKStatePtr, DataNode, const char*, const char*);
const char* DataNodeGetArgument(DataNode);
const char* DataNodeGetModuleName(DataNode);
const char* DataNodeGetKeyword(DataNode);
const char* DataNodeGetPath(DataNode);
const char* DataNodeGetValue(DataNode);
DataNode DataNodeGetParent(DataNode);
DataNode DataNodeGetTopDataNode(DataNode);
void DataNodeAddAnnotation(DataNode, const char*);
DataNodeChildren DataNodeGetChildren(DataNode);
const char* DataNodeGetSegmentPath(DataNode);

// what if duplicates? where to initialize?
void EnableLogging(LogLevel);
// void EnableLogging(YDKState*, LogLevel);
LogLevel GetLoggingLevel(void);
// LogLevel GetLoggingLevel(YDKState*, void);

void YLogInfo(const char*);
void YLogDebug(const char*);
void YLogWarn(const char*);
void YLogError(const char*);

#ifdef __cplusplus
}
#endif

#endif /* _YDK_H_ */
