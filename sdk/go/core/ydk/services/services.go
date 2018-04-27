// Package services implements support for Go services.
//
// YANG Development Kit Copyright 2017 Cisco Systems. All rights reserved.
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
package services

// #cgo CXXFLAGS: -g -std=c++11
// #cgo darwin LDFLAGS:  -fprofile-arcs -ftest-coverage -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lpython -lc++
// #cgo linux LDFLAGS:  -fprofile-arcs -ftest-coverage --coverage -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lstdc++ -lpython2.7 -ldl
// #include <ydk/ydk.h>
// #include <stdlib.h>
import "C"

import (
	"encoding/json"
	"encoding/xml"
	"fmt"
	"strconv"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/errors"
	"github.com/CiscoDevNet/ydk-go/ydk/path"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/datastore"
	encodingFmt "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
)

// CrudService supports CRUD operations on entities.
type CrudService struct {
}

// Create the entity.
// Returns whether the operation was successful or not (bool)
func (c *CrudService) Create(
	provider types.ServiceProvider, entity types.Entity) bool {

	data := map[string]interface{} { "entity": entity }
	return operationSucceeded(path.ExecuteRPC(provider, "ydk:create", data, false))
}

// Update the entity.
// Returns whether the operation was successful or not (bool)
func (c *CrudService) Update(
	provider types.ServiceProvider, entity types.Entity) bool {

	data := map[string]interface{} { "entity": entity }
	return operationSucceeded(path.ExecuteRPC(provider, "ydk:update", data, false))
}

// Delete the entity.
// Returns whether the operation was successful or not (bool)
func (c *CrudService) Delete(
	provider types.ServiceProvider, entity types.Entity) bool {

	data := map[string]interface{} { "entity": entity }
	return operationSucceeded(path.ExecuteRPC(provider, "ydk:delete", data, false))
}

// Read the entity.
// Returns the entity as identified by the given filter (types.Entity)
func (c *CrudService) Read(
	provider types.ServiceProvider, filter types.Entity) types.Entity {

	data := map[string]interface{} { "filter": filter }
	return path.ReadDatanode(
		filter, path.ExecuteRPC(provider, "ydk:read", data, false))
}

// ReadConfig only reads config.
// Returns the entity as identified by the given filter (types.Entity)
func (c *CrudService) ReadConfig(
	provider types.ServiceProvider, filter types.Entity) types.Entity {

	data := map[string]interface{} { "filter": filter }
	return path.ReadDatanode(
		filter, path.ExecuteRPC(provider, "ydk:read", data, true))
}

// CodecService supports encoding and decoding Go model API objects of type
type CodecService struct {
}

// Encode converts entity object to XML/JSON payload
func (c *CodecService) Encode(
	provider types.CodecServiceProvider, entity types.Entity) string {

	// 1. initialize provider, set root schema node for entity
	provider.Initialize(entity)
	// 2. get data node from root schema
	rootSchemaNode := provider.GetRootSchemaNode(entity)
	// 3. encode and return payload
	return path.CodecServiceEncode(
		provider.GetState(), entity, rootSchemaNode, provider.GetEncoding())
}

// Decode converts XML/JSON object to entity object
func (c *CodecService) Decode(
	provider types.CodecServiceProvider, payload string) types.Entity {

	// 1. parse payload, get topEntity
	nmsp := getEntityLookupKey(provider, payload)
	topEntity, _ := ydk.GetTopEntity(nmsp)
	if topEntity == nil {
		err := errors.YModelError{Msg: fmt.Sprintf("Failed to find top entity for lookup key '%s'", nmsp)}
		panic(err.Error())
	}
	// 2. initialize repository, fetch rootSchema
	provider.Initialize(topEntity)
	rootSchema := provider.GetRootSchemaNode(topEntity)
	// 3. populate and return entity
	return path.CodecServiceDecode(
		provider.GetState(),
		rootSchema,
		payload,
		provider.GetEncoding(),
		topEntity)
}

// ExecutorService provides the functionality to execute RPCs
type ExecutorService struct {
}

// ExecuteRpc creates the entity represented by rpcEntity
// Any expected return data uses topEntity as a filter
func (es *ExecutorService) ExecuteRpc (
	provider types.ServiceProvider,
	rpcEntity, topEntity types.Entity) types.Entity {

	return path.ExecuteRPCEntity(provider, rpcEntity, topEntity)
}

// NetconfService implements the NETCONF Protocol Operations:
// https://tools.ietf.org/html/rfc6241
type NetconfService struct {
}

// CancelCommit cancels an ongoing confirmed commit.
// If persistID < 1, the operation MUST be issued on the same session
// that issued the confirmed commit.
// Returns whether the operation is successful or not
func (ns *NetconfService) CancelCommit(
	provider types.ServiceProvider, persistID int) bool {

	data := map[string]interface{} {}
	if (persistID > 1) {
		data["persist-id"] = strconv.Itoa(persistID)
	}
	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:cancel-commit", data, false)
	return operationSucceeded(readDataNode)
}

// CloseSession requests graceful termination of a NETCONF session
// Returns whether the operation is successful or not
func (ns *NetconfService) CloseSession(provider types.ServiceProvider) bool {
	data := map[string]interface{} {}
	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:close-session", data, false)
	return operationSucceeded(readDataNode)
}

// Commit instructs the device to implement the configuration data contained
// in the candidate configuration.
// Returns whether the operation is successful or not
func (ns *NetconfService) Commit(
	provider types.ServiceProvider,
	confirmed bool,
	confirmTimeOut, persist, persistID int) bool {

	data := map[string]interface{} {}
	if (confirmed) {
		data["confirmed"] = ""
	}
	if (confirmTimeOut > -1) {
		data["confirm-timeout"] = strconv.Itoa(confirmTimeOut)
	}
	if (persist > -1) {
		data["persist"] = strconv.Itoa(persist)
	}
	if (persistID > -1) {
		data["persist-id"] = strconv.Itoa(persistID)
	}

	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:commit", data, false)
	return operationSucceeded(readDataNode)
}

// CopyConfig creates or replaces an entire configuration DataStore with the
// contents of another complete configuration DataStore. If the target
// DataStore exists, it is overwritten. Otherwise, a new one is created,
// if allowed.
// Parameters:
// 		sourceEntity should be nil OR sourceDS should be nil, but not neither
//		or both.
// 		url is ignored unless target/sourceDS is set to Url.
// Returns whether the operation is successful or not
func (ns *NetconfService) CopyConfig(
	provider types.ServiceProvider,
	target, sourceDS datastore.DataStore,
	sourceEntity types.Entity,
	url string) bool {

	// target/source options: candidate | running | startup | url
	if (target == datastore.NotSet) {
		err := errors.YError{Msg: "You must select target"}
		panic(err.Error())
	}

	if ((target == datastore.URL ||
		sourceDS == datastore.URL) && len(url) == 0){
		err := errors.YError{Msg: "url must be specified"}
		panic(err.Error())
	}
	if ((sourceDS != datastore.NotSet && sourceEntity != nil) ||
		sourceDS == datastore.NotSet && sourceEntity == nil) {
		err := errors.YError{
			Msg: "sourceDS OR sourceEntity must be valid, not neither nor both"}
		panic(err.Error())
	}

	data := map[string]interface{} {}
	dsStr := target.String()
	dataValue := getDataValue(dsStr, url)
	data["target/" + dsStr] = dataValue

	if (sourceDS != datastore.NotSet){
		dsStr = sourceDS.String()
		dataValue = getDataValue(dsStr, url)
		data["source/" + dsStr] = dataValue
	} else {
		data["source/config"] = sourceEntity
	}

	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:copy-config", data, false)
	return operationSucceeded(readDataNode)
}

// DeleteConfig deletes a configuration DataStore. The CANDIDATE and RUNNING
// DataStores cannot be deleted.
// Returns whether the operation is successful or not
func (ns *NetconfService) DeleteConfig(
	provider types.ServiceProvider,
	target datastore.DataStore,
	url string) bool {

	// target options: startup | url
	dsStr := target.String()
	dataValue := getDataValue(dsStr, url)
	if (target == datastore.Candidate || target == datastore.Running ||
		target == datastore.URL && len(url) == 0){

		errMsg := "target: %v can only be Startup (url is ignored)"
		errMsg += " or Url (url must be specified)"
		err := errors.YError{Msg: fmt.Sprintf(errMsg, dsStr)}
		panic(err.Error())
	}

	data := map[string]interface{} {}
	data["target/" + dsStr] = dataValue

	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:delete-config", data, false)
	return operationSucceeded(readDataNode)
}

// DiscardChanges reverts the candidate configuration to the current
// running configuration.
// Returns whether the operation is successful or not
func (ns *NetconfService) DiscardChanges(provider types.ServiceProvider) bool {
	data := map[string]interface{} {}
	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:discard-changes", data, false)
	return operationSucceeded(readDataNode)
}

// EditConfig loads all or part of a specified configuration to the specified
// target configuration datastore. It allows the new configuration to be
// expressed using a local file, a remote file, or inline. If the target
// configuration datastore does not exist, it will be created.
// Returns whether the operation is successful or not
func (ns *NetconfService) EditConfig(
	provider types.ServiceProvider,
	target datastore.DataStore,
	config types.Entity,
	defaultOper, testOp, errorOp string) bool {

	// target options: candidate | running
	dsStr := target.String()
	dataValue := ""

	if (target == datastore.URL || target == datastore.Startup){
		errMsg := fmt.Sprintf(
			"target: %v can only be Candidate or Running", dsStr)
		err := errors.YError{Msg: errMsg}
		panic(err.Error())
	}

	data := map[string]interface{} {}
	data["target/" + dsStr] = dataValue

	// config
	data["config"] = config

	// default operation, test option, error option
	if (len(defaultOper) > 0) {
		data["default-operation"] = defaultOper
	}
	if (len(testOp) > 0) {
		data["test-option"] = testOp
	}
	if (len(errorOp) > 0) {
		data["error-option"] = errorOp
	}

	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:edit-config", data, false)
	return operationSucceeded(readDataNode)
}

// GetConfig retrieves all or part of a specified configuration datastore
// Returns the requested data in Entity instance
func (ns *NetconfService) GetConfig(
	provider types.ServiceProvider,
	source datastore.DataStore,
	filter types.Entity) types.Entity {

	// source options: candidate | running | startup
	dsStr := source.String()
	dataValue := ""

	if (source == datastore.URL){
		errMsg := fmt.Sprintf(
			"source: %v can only be Candidate, Running, or Startup", dsStr)
		err := errors.YError{Msg: errMsg}
		panic(err.Error())
	}

	data := map[string]interface{} {}
	data["source/" + dsStr] = dataValue

	// filter
	data["filter"] = filter

	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:get-config", data, false)
	return path.ReadDatanode(filter, readDataNode)
}

// Get the running configuration and device state information.
// Returns the requested data in Entity instance
func (ns *NetconfService) Get(
	provider types.ServiceProvider, filter types.Entity) types.Entity {

	data := map[string]interface{} {}
	data["filter"] = filter

	readDataNode := path.ExecuteRPC(provider, "ietf-netconf:get", data, false)
	return path.ReadDatanode(filter, readDataNode)
}

// KillSession forces the termination of a NETCONF session.
// Returns whether the operation is successful or not
func (ns *NetconfService) KillSession(
	provider types.ServiceProvider, sessionID int) bool {

	data := map[string]interface{} {}
	data["session-id"] = strconv.Itoa(sessionID)

	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:kill-session", data, false)
	return operationSucceeded(readDataNode)
}

// Lock allows the client to lock the entire configuration datastore system
// of a device.
// Returns whether the operation is successful or not
func (ns *NetconfService) Lock(
	provider types.ServiceProvider, target datastore.DataStore) bool {
	// target options: candidate | running | startup
	dsStr := target.String()
	dataValue := ""

	if (target == datastore.URL){
		err := errors.YError{Msg: "url must be specified"}
		panic(err.Error())
	}

	data := map[string]interface{} {}
	data["target/" + dsStr] = dataValue

	readDataNode := path.ExecuteRPC(provider, "ietf-netconf:lock", data, false)
	return operationSucceeded(readDataNode)
}

// Unlock a configuration lock, previously obtained with the LOCK operation.
// Returns whether the operation is successful or not
func (ns *NetconfService) Unlock(
	provider types.ServiceProvider, target datastore.DataStore) bool {

	// target options: candidate | running | startup
	dsStr := target.String()
	dataValue := ""

	if (target == datastore.URL){
		err := errors.YError{Msg: "url must be specified"}
		panic(err.Error())
	}

	data := map[string]interface{} {}
	data["target/" + dsStr] = dataValue

	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:unlock", data, false)
	return operationSucceeded(readDataNode)
}

// Validate the contents of the specified configuration
// Parameters:
// 		sourceEntity should be nil OR sourceDS should be nil, but not neither
//		or both.
// 		url is ignored unless target/sourceDS is set to Url.
// Returns whether the operation is successful or not
func (ns *NetconfService) Validate(
	provider types.ServiceProvider,
	sourceDS datastore.DataStore,
	sourceEntity types.Entity,
	url string) bool {

	if ((sourceDS != datastore.NotSet && sourceEntity != nil) ||
		sourceDS == datastore.NotSet && sourceEntity == nil) {
		err := errors.YError{
			Msg: "sourceDS OR sourceEntity must be valid, not neither nor both"}
		panic(err.Error())
	}
	// sourceDS options: candidate | running | startup | url
	if (sourceDS == datastore.URL && len(url) == 0){
		err := errors.YError{Msg: "url must be specified"}
		panic(err.Error())
	}
	data := map[string]interface{} {}
	if (sourceDS != datastore.NotSet){
		dsStr := sourceDS.String()
		dataValue := getDataValue(dsStr, url)
		data["source/" + dsStr] = dataValue
	} else {
		data["source/config"] = sourceEntity
	}

	readDataNode := path.ExecuteRPC(
		provider, "ietf-netconf:validate", data, false)
	return operationSucceeded(readDataNode)
}

func getDataValue(dataStoreStr, url string) string {
	if (dataStoreStr == "url") {
		return url
	}
	return ""
}

func operationSucceeded(node types.DataNode) bool {
	return node.Private != nil
}

func getEntityLookupKey(
	provider types.CodecServiceProvider, payload string) string {

	var nmsp string
	encoding := provider.GetEncoding()

	switch encoding {

	case encodingFmt.XML:
		ydk.YLogDebug("Using XML encoding...")

		type XMLObj struct {
			XMLName xml.Name
		}

		var xmlObj XMLObj
		err := xml.Unmarshal([]byte(payload), &xmlObj)
		if err != nil {
			ydk.YLogError(fmt.Sprintf("Error parsing XML: ", err))
			panic(err)
		}

		nmsp = fmt.Sprintf("%v", xmlObj.XMLName)

	case encodingFmt.JSON:
		ydk.YLogDebug("Using JSON encoding...")

		var jsonObj interface{}
		err := json.Unmarshal([]byte(payload), &jsonObj)
		if err != nil {
			ydk.YLogError(fmt.Sprintf("Error parsing JSON: ", err))
			panic(err)
		}

		items := jsonObj.(map[string]interface{})
		if len(items) != 1 {
			ydk.YLogError("List of payload not supported")
			panic("List of payload not supported")
		}

		for k := range items {
			nmsp = k
		}

	default:
		panic("Encoding not supported!")
	}

	return nmsp
}
