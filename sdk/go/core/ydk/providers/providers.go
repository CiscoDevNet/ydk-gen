// Package providers implements support for Go service providers.
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
package providers

import (
	"fmt"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/errors"
	"github.com/CiscoDevNet/ydk-go/ydk/path"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	encoding "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
	"github.com/CiscoDevNet/ydk-go/ydk/types/protocol"
)

// OpenDaylightServiceProvider A service provider to be used to communicate with an OpenDaylight instance: https://www.opendaylight.org
type OpenDaylightServiceProvider struct {
	Path           string
	Address        string
	Username       string
	Password       string
	Port           int
	EncodingFormat encoding.EncodingFormat
	Protocol       protocol.Protocol

	Private types.COpenDaylightServiceProvider
	// keep alive
	ProvidersHolder []types.ServiceProvider
	State           errors.State
}

// NetconfServiceProvider Implementation of ServiceProvider for the NETCONF protocol: https://tools.ietf.org/html/rfc6241
type NetconfServiceProvider struct {
	Repo     	types.Repository
	Address  	string
	Username 	string
	Password 	string
	Port     	int
	Protocol 	string
	OnDemand 	bool
	CommonCache	bool

	Private types.CServiceProvider
	State   errors.State
}

// RestconfServiceProvider Implementation of ServiceProvider for the RESTCONF protocol: https://tools.ietf.org/html/draft-ietf-netconf-restconf-18
type RestconfServiceProvider struct {
	Path          string
	Address       string
	Username      string
	Password      string
	Port          int
	Encoding      encoding.EncodingFormat
	StateURLRoot  string
	ConfigURLRoot string

	Private types.CServiceProvider
	State   errors.State
}

// GetPrivate returns private pointer for OpenDaylightServiceProvider
func (provider *OpenDaylightServiceProvider) GetPrivate() interface{} {
	return provider.Private
}

// Connect to OpenDaylightServiceProvider using Path/Address/Username/Password/Port
func (provider *OpenDaylightServiceProvider) Connect() {
	provider.Private = path.ConnectToOpenDaylightProvider(&provider.State, provider.Path, provider.Address, provider.Username, provider.Password, provider.Port, provider.EncodingFormat, provider.Protocol)
}

// GetNodeIDs returns OpenDaylightServiceProvider Node IDs
func (provider *OpenDaylightServiceProvider) GetNodeIDs() []string {

	return path.OpenDaylightServiceProviderGetNodeIDs(&provider.State, provider.Private)
}

// GetNodeProvider returns Node provider by ID
func (provider *OpenDaylightServiceProvider) GetNodeProvider(nodeID string) types.ServiceProvider {
	p := path.OpenDaylightServiceProviderGetNodeProvider(&provider.State, provider.Private, nodeID)
	if provider.Protocol == protocol.Restconf {

		nodeProvider := RestconfServiceProvider{Path: provider.Path, Address: provider.Address, Password: provider.Password, Username: provider.Username, Port: provider.Port}
		path.AddCState(&nodeProvider.State)
		nodeProvider.Private = p
		provider.ProvidersHolder = append(provider.ProvidersHolder, &nodeProvider)

		return &nodeProvider
	}
	repo := types.Repository{}
	repo.Path = provider.Path
	nodeProvider := NetconfServiceProvider{Repo: repo, Address: provider.Address, Password: provider.Password, Username: provider.Username, Port: provider.Port}
	path.AddCState(&nodeProvider.State)
	nodeProvider.Private = p
	provider.ProvidersHolder = append(provider.ProvidersHolder, &nodeProvider)
	return &nodeProvider
}

// GetState returns error state from OpenDaylightServiceProvider
func (provider *OpenDaylightServiceProvider) GetState() *errors.State {
	return &provider.State
}

// Disconnect from OpenDaylightServiceProvider
func (provider *OpenDaylightServiceProvider) Disconnect() {
	if provider.Private.Private == nil {
		return
	}
	path.DisconnectFromOpenDaylightProvider(provider.Private)
	path.CleanUpErrorState(&provider.State)
}

// GetPrivate returns private pointer for NetconfServiceProvider
func (provider *NetconfServiceProvider) GetPrivate() interface{} {
	return provider.Private
}

// GetState returns error state from NetconfServiceProvider
func (provider *NetconfServiceProvider) GetState() *errors.State {
	return &provider.State
}

// GetCapabilities returns the capabilities supported by NetconfServiceProvider
func (provider *NetconfServiceProvider) GetCapabilities() []string {
	return path.GetCapabilitesFromNetconfProvider(provider.Private)
}

// Connect to NetconfServiceProvider using Repo/Address/Username/Password/Port
func (provider *NetconfServiceProvider) Connect() {
	if len(provider.Protocol) == 0 {
		provider.Protocol = "ssh"
	}
	provider.Private = path.ConnectToNetconfProvider(
		&provider.State,
		provider.Repo,
		provider.Address,
		provider.Username,
		provider.Password,
		provider.Port,
		provider.Protocol,
		provider.OnDemand,
		provider.CommonCache)
}

// Disconnect from NetconfServiceProvider
func (provider *NetconfServiceProvider) Disconnect() {
	if provider.Private.Private == nil {
		return
	}
	path.DisconnectFromNetconfProvider(provider.Private)
	path.CleanUpErrorState(&provider.State)
}

func getRpcTag(operation string) string {
    var rpc string
    if operation == "create" {
        rpc = "ydk:create"
    } else if operation == "update" {
    	rpc = "ydk:update"
    } else if operation == "delete" {
    	rpc = "ydk:delete"
    } else if operation == "read" {
    	rpc = "ydk:read"
    } else {
        ydk.YLogError(fmt.Sprintf("getRpcTag: Operation '{}' is not supported", operation));
        panic(1)
    }
    return rpc;
}

func executeNetconfRpc(provider types.ServiceProvider, operation string, entity types.Entity, params map[string]string) types.DataNode {
	rpcTag := getRpcTag(operation)
	dataTag := "entity"
	if operation == "read" {
		dataTag = "filter"
		types.SetNontopEntityFilter(entity, yfilter.Read)
	} else {
		if operation == "delete" {
			types.SetNontopEntityFilter(entity, yfilter.Delete)
		}
	}
	data := make(map[string]interface{})
	data[dataTag] = entity

	setConfigFlag := false
	mode, ok := params["mode"]
	if ok && mode == "config" {
		setConfigFlag = true
	}
	dn := path.ExecuteRPC(provider, rpcTag, data, setConfigFlag)
	types.SetNontopEntityFilter(entity, yfilter.NotSet)
	return dn
}

func (provider *NetconfServiceProvider) ExecuteRpc(operation string, entity types.Entity, params map[string]string) types.DataNode {
	return executeNetconfRpc(provider, operation, entity, params)
}

func (provider *RestconfServiceProvider) ExecuteRpc(operation string, entity types.Entity, params map[string]string) types.DataNode {
	return executeNetconfRpc(provider, operation, entity, params)
}

// GetPrivate returns private pointer for RestconfServiceProvider
func (provider *RestconfServiceProvider) GetPrivate() interface{} {
	return provider.Private
}

// Connect to RestconfServiceProvider using Path/Address/Username/Password/Port
func (provider *RestconfServiceProvider) Connect() {
	if len(provider.StateURLRoot) == 0 {
		provider.StateURLRoot = "/data"
	}
	if len(provider.ConfigURLRoot) == 0 {
		provider.ConfigURLRoot = "/data"
	}
	provider.Private = path.ConnectToRestconfProvider(&provider.State, provider.Path, provider.Address, provider.Username, provider.Password, provider.Port, provider.Encoding, provider.StateURLRoot, provider.ConfigURLRoot)
}

// GetState returns error state from RestconfServiceProvider
func (provider *RestconfServiceProvider) GetState() *errors.State {
	return &provider.State
}

// Disconnect from RestconfServiceProvider
func (provider *RestconfServiceProvider) Disconnect() {
	if provider.Private.Private == nil {
		return
	}
	path.DisconnectFromRestconfProvider(provider.Private)
	path.CleanUpErrorState(&provider.State)
}

// CodecServiceProvider Encode and decode to XML/JSON format
type CodecServiceProvider struct {
	Repo     types.Repository
	Encoding encoding.EncodingFormat

	RootSchemaTable map[string]types.RootSchemaNode
	State           errors.State
}

// Initialize CodecServiceProvider
func (provider *CodecServiceProvider) Initialize(entity types.Entity) {
	if provider.State.Private == nil {
		path.AddCState(&provider.State)
	}

	bundleName :=  entity.GetEntityData().BundleName
	if len(provider.RootSchemaTable) == 0 {
		provider.RootSchemaTable = make(map[string]types.RootSchemaNode)
	}
	_, ok := provider.RootSchemaTable[bundleName]
	if !ok {
		ydk.YLogDebug(fmt.Sprintf(
			"CodecServiceProvider initialized with %v bundle", bundleName))
		rootSchemaNode := path.InitCodecServiceProvider(&provider.State, entity, provider.Repo)
		provider.RootSchemaTable[bundleName] = rootSchemaNode
	}
}

// GetEncoding returns encoding format for CodecServiceProvider
func (provider *CodecServiceProvider) GetEncoding() encoding.EncodingFormat {
	return provider.Encoding
}

// GetState returns error state from CodecServiceProvider
func (provider *CodecServiceProvider) GetState() *errors.State {
	return &provider.State
}

// GetRootSchemaNode returns root schema node for entity
func (provider *CodecServiceProvider) GetRootSchemaNode(entity types.Entity) types.RootSchemaNode {
	rootSchemaNode, ok := provider.RootSchemaTable[entity.GetEntityData().BundleName]
	if !ok {
		panic("Root schema node not found in provider!")
	}
	return rootSchemaNode
}
