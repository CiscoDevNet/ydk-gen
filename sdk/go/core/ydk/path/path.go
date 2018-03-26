// Package path provides a new interface in the form of Path API,
// which can be used to write apps using a generic API,
// using xpath-like path expressions to create and access YANG data nodes.
// The nodes created using the YDK model API are converted to Path API data nodes
// for validation and encoding to respective protocol payloads.
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
package path

// #cgo CXXFLAGS: -g -std=c++11
// #cgo darwin LDFLAGS:  -fprofile-arcs -ftest-coverage -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lpython -lc++
// #cgo linux LDFLAGS:  -fprofile-arcs -ftest-coverage --coverage -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lstdc++ -lpython2.7 -lm -ldl
// #include <ydk/ydk.h>
// #include <stdlib.h>
import "C"

import (
	"fmt"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/errors"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	encodingFormat "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
	"github.com/CiscoDevNet/ydk-go/ydk/types/protocol"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	"strings"
	"unsafe"
)


// ExecuteRPC executes payload converted from entity for CRUD/Netconf services.
// Returns a data node (types.DataNode) representing the result of the executed rpc.
func ExecuteRPC(
	provider types.ServiceProvider,
	Filter string,
	data map[string]interface{},
	setConfigFlag bool) types.DataNode {

	state := provider.GetState()
	cstate := getCState(state)
	wrappedProvider := provider.GetPrivate().(types.CServiceProvider)
	realProvider := wrappedProvider.Private.(C.ServiceProvider)
	rootSchema := C.ServiceProviderGetRootSchema(*cstate, realProvider)
	panicOnCStateError(cstate)

	ydkRPC := C.RootSchemaNodeRpc(*cstate, rootSchema, C.CString(Filter))
	panicOnCStateError(cstate)

	if rootSchema == nil {
		ydk.YLogError("root schema is nil!")
		panic(1)
	}

	input := C.RpcInput(*cstate, ydkRPC)
	panicOnCStateError(cstate)

	if setConfigFlag {
		C.DataNodeCreate(*cstate, input, C.CString("only-config"), C.CString(""))
		panicOnCStateError(cstate)
	}

	var dataTag string = ""
	var value interface{} = nil
	for dataTag, value = range data {
		dataValue := C.CString("")
		switch v := value.(type){
		case string:
			dataValue = C.CString(value.(string))
		default:
			_ = v
			dataValue = getDataPayload(state, value.(types.Entity), rootSchema, provider)
			defer C.free(unsafe.Pointer(dataValue))
		}
		C.DataNodeCreate(*cstate, input, C.CString(dataTag), dataValue)
	}

	dataNode := types.DataNode{C.RpcExecute(*cstate, ydkRPC, realProvider)}
	panicOnCStateError(cstate)

	return dataNode
}

func getDataPayload(
	state *errors.State,
	entity types.Entity,
	rootSchema C.RootSchemaNode,
	provider types.ServiceProvider) *C.char {

	datanode := getDataNodeFromEntity(state, entity, rootSchema)

	if datanode == nil {
		return nil
	}

	//for datanode != nil && C.DataNodeGetParent(datanode) != nil {
	//	datanode = C.DataNodeGetParent(datanode)
	//}
	cprovider := provider.GetPrivate().(
		types.CServiceProvider).Private.(C.ServiceProvider)
	cencoding := C.ServiceProviderGetEncoding(cprovider)

	codec := C.CodecInit()
	defer C.CodecFree(codec)
	var data *C.char = C.CodecEncode(*getCState(state), codec, datanode, cencoding, 1)
	panicOnCStateError(getCState(state))

	return (data)
}

// ExecuteRPCEntity provides the functionality to execute RPCs with
// executor service using an RPC class defined under a ydk.models subpackage
// Optional args: topEntity to be passed in only when expecting return data
// Returns nil or an instance of types.Entity when expecting return data
func ExecuteRPCEntity(
	provider types.ServiceProvider,
	rpcEntity, topEntity types.Entity) types.Entity {

	state := provider.GetState()
	cstate := getCState(state)
	wrappedProvider := provider.GetPrivate().(types.CServiceProvider)
	realProvider := wrappedProvider.Private.(C.ServiceProvider)
	rootSchema := C.ServiceProviderGetRootSchema(*cstate, realProvider)
	panicOnCStateError(cstate)

	segmentPath := rpcEntity.GetEntityData().SegmentPath
	ydkRPC := C.RootSchemaNodeRpc(
		*cstate, rootSchema, C.CString(segmentPath))
	panicOnCStateError(cstate)

	if rootSchema == nil {
        ydk.YLogError("root schema is nil!")
		panic(1)
	}

	rpcInput := C.RpcInput(*cstate, ydkRPC)
	panicOnCStateError(cstate)

	child := types.GetChildByName(rpcEntity, "input", "")
	if (child != nil && types.HasDataOrFilter(child)) {
		walkRPCChildren(state, child, rpcInput, "")
	}

	readDataNode := types.DataNode{C.RpcExecute(*cstate, ydkRPC, realProvider)}
	panicOnCStateError(cstate)

	output := types.GetChildByName(rpcEntity, "output", "")

	if (output == nil || readDataNode.Private == nil) {
		return nil
	}
	return ReadDatanode(topEntity, readDataNode)
}

func walkRPCChildren(
	state *errors.State, rpcEntity types.Entity, rpcInput C.DataNode, path string) {

	ydk.YLogDebug("Walking Rpc Children...")
	if(rpcEntity != nil) {
		children := rpcEntity.GetEntityData().Children
		entityPath := types.GetEntityPath(rpcEntity)
		ydk.YLogDebug(fmt.Sprintf(
			"Got %d entity children in '%s'", len(children), entityPath.Path))
		ydk.YLogDebug(fmt.Sprintf(
			"Got %d leafs in '%s'", len(entityPath.ValuePaths), entityPath.Path))

		if (path != "") {
			path = fmt.Sprintf("%s/", path)
		}

		if (entityPath.Path != "input") {
			path = fmt.Sprintf("%s%s", path, entityPath.Path)
		}

		if (path != "") {
			ydk.YLogDebug(fmt.Sprintf("Path: %s", path))
		}

		for childName, _ := range children {
			if (children[childName].Value != nil &&
				types.HasDataOrFilter(children[childName].Value)) {

				segmentPath := children[childName].Value.GetEntityData().SegmentPath
				ydk.YLogDebug(fmt.Sprintf("Looking at entity child '%s'", segmentPath))
				walkRPCChildren(state, children[childName].Value, rpcInput, path)
			}
		}

		// if there are leafs, create from entity path
		if (len(entityPath.ValuePaths) != 0) {
			createFromEntityPath(state, rpcEntity, rpcInput, path)
		}

		createFromChildren(state, children, rpcInput)
	}
}

func createFromEntityPath(
	state *errors.State, rpcEntity types.Entity, rpcInput C.DataNode, path string) {

	entityPath := types.GetEntityPath(rpcEntity)
	for _, nameValue := range entityPath.ValuePaths {
		ydk.YLogDebug(fmt.Sprintf("Creating leaf '%s' with value '%s' in '%s'",
			nameValue.Name, nameValue.Data.Value, entityPath.Path))

		tempPath := ""
		if (path != "") {
			tempPath = fmt.Sprintf("%s/", path)
		}
		tempPath = fmt.Sprintf("%s%s", tempPath, nameValue.Name)
		C.DataNodeCreate(
			*getCState(state),
			rpcInput,
			C.CString(tempPath),
			C.CString(nameValue.Data.Value))
	}
}

func createFromChildren(
	state *errors.State, children map[string]types.YChild, rpcInput C.DataNode) {

	for childName, child := range children {
		if child.Value != nil && types.HasDataOrFilter(child.Value) {
			ydk.YLogDebug(fmt.Sprintf("Creating child '%s' : %s",
				childName, types.GetEntityPath(child.Value).Path))
			C.DataNodeCreate(*getCState(state), rpcInput, C.CString(childName), C.CString(""))
		}
	}
}

func getTopEntityFromFilter(filter types.Entity) types.Entity {
	parent := types.GetParent(filter)
	if parent == nil {
		return filter
	}

	return getTopEntityFromFilter(parent)
}

// ReadDatanode populates entity by reading the top level entity from a given data node.
// Returns the top entity (types.Entity) from readDataNode.
func ReadDatanode(filter types.Entity, readDataNode types.DataNode) types.Entity {
	if readDataNode.Private == nil {
		return nil
	}

	topEntity := getTopEntityFromFilter(filter)
	segmentPath := topEntity.GetEntityData().SegmentPath
	ydk.YLogDebug(
		fmt.Sprintf("Reading top entity: '%s'", segmentPath))

	cchildren := C.DataNodeGetChildren(readDataNode.Private.(C.DataNode))

	if cchildren.count == C.int(0) {
		return topEntity
	}

	children := (*[1 << 30]C.DataNode)(
		unsafe.Pointer(cchildren.datanodes))[:cchildren.count:cchildren.count]
	getEntityFromDataNode(children[0], topEntity)
	return topEntity
}

// ConnectToNetconfProvider connects to NETCONF service provider by creating a
// connection to the provider using given address, username, password, and port.
// Returns the connected service provider (types.CServiceProvider).
func ConnectToNetconfProvider(
	state *errors.State,
	repo types.Repository,
	address, username, password string,
	port int,
	protocol string,
	onDemand, commonCache bool) types.CServiceProvider {

	var caddress *C.char = C.CString(address)
	defer C.free(unsafe.Pointer(caddress))
	var cusername *C.char = C.CString(username)
	defer C.free(unsafe.Pointer(cusername))
	var cpassword *C.char = C.CString(password)
	defer C.free(unsafe.Pointer(cpassword))
	var cport C.int = C.int(port)

	var cprotocol *C.char = C.CString(protocol)
	defer C.free(unsafe.Pointer(cprotocol))

	var cOnDemand C.boolean = 1
	if onDemand { cOnDemand = 0 }
	var cCommonCache C.boolean = 0
	if commonCache { cCommonCache = 1 }

	AddCState(state)
	cstate := getCState(state)

	var p C.ServiceProvider

	if len(repo.Path) > 0 {
		var path *C.char = C.CString(repo.Path)
		repo := C.RepositoryInitWithPath(*cstate, path)
		panicOnCStateError(cstate)
		p = C.NetconfServiceProviderInitWithOnDemandRepo(
			*cstate, repo, caddress, cusername, cpassword, cport, cprotocol, cOnDemand)
		panicOnCStateError(cstate)
	} else {
		p = C.NetconfServiceProviderInitWithOnDemand(
			*cstate, caddress, cusername, cpassword, cport, cprotocol, cOnDemand, cCommonCache)
		panicOnCStateError(cstate)
	}

	cprovider := types.CServiceProvider{Private: p}
	return cprovider
}

// DisconnectFromNetconfProvider disconnects from NETCONF device and frees
// the given service provider.
func DisconnectFromNetconfProvider(provider types.CServiceProvider) {
	realProvider := provider.Private.(C.ServiceProvider)
	C.NetconfServiceProviderFree(realProvider)
}

// CleanUpErrorState cleans up memory for CState.
func CleanUpErrorState(state *errors.State) {
	realState := getCState(state)
	C.YDKStateFree(*realState)
}

// ConnectToRestconfProvider connects to the RESTCONF device by creating
// a connection to the provider using given path, address, username, password,
// and port.
// Returns the connected service provider (types.CServiceProvider).
func ConnectToRestconfProvider(
	state *errors.State,
	Path, Address, Username, Password string,
	port int, encoding encodingFormat.EncodingFormat,
	stateURLRoot, configURLRoot string) types.CServiceProvider {

	var path *C.char = C.CString(Path)
	defer C.free(unsafe.Pointer(path))
	var address *C.char = C.CString(Address)
	defer C.free(unsafe.Pointer(address))
	var username *C.char = C.CString(Username)
	defer C.free(unsafe.Pointer(username))
	var password *C.char = C.CString(Password)
	defer C.free(unsafe.Pointer(password))
	var cport C.int = C.int(port)

	cencoding := getCEncoding(encoding)

	var cstateURLRoot *C.char = C.CString(stateURLRoot)
	defer C.free(unsafe.Pointer(cstateURLRoot))

	var cconfigURLRoot *C.char = C.CString(configURLRoot)
	defer C.free(unsafe.Pointer(cconfigURLRoot))

	AddCState(state)
	cstate := getCState(state)

	var p C.ServiceProvider

	crepo := C.RepositoryInitWithPath(*getCState(state), path)
	panicOnCStateError(cstate)
	p = C.RestconfServiceProviderInitWithRepo(
		*cstate, crepo,
		address, username, password,
		cport, cencoding, cstateURLRoot,
		cconfigURLRoot)
	panicOnCStateError(cstate)

	cprovider := types.CServiceProvider{Private: p}
	return cprovider
}

// DisconnectFromRestconfProvider disconnects from RESTCONF device and frees
// the given service provider.
func DisconnectFromRestconfProvider(provider types.CServiceProvider) {
	realProvider := provider.Private.(C.ServiceProvider)
	C.RestconfServiceProviderFree(realProvider)
}

// InitCodecServiceProvider initializes CodecServiceProvider.
// Returns root schema node (types.RootSchemaNode) parsed from repository.
func InitCodecServiceProvider(
	state *errors.State,
	entity types.Entity,
	repo types.Repository) types.RootSchemaNode {

	var repoPath *C.char
	defer C.free(unsafe.Pointer(repoPath))

	data := entity.GetEntityData()
	if len(repo.Path) > 0 {
		ydk.YLogDebug(fmt.Sprintf(
			"CodecServiceProvider using YANG models in %v", repo.Path))
		repoPath = C.CString(repo.Path)
	} else {
		yangPath := data.BundleYangModelsLocation
		ydk.YLogDebug(fmt.Sprintf(
			"CodecServiceProvider using YANG models in %v", yangPath))
		repoPath = C.CString(yangPath)
	}

	capabilities := data.CapabilitiesTable
	namespaces := data.NamespaceTable
	lookupTableKeys := make([]*C.char, 0)
	lookupTableValues := make([]C.Capability, 0)
	for name, revision := range capabilities {
		var cname *C.char = C.CString(name)
		lookupTableKeys = append(lookupTableKeys, cname)

		capability := C.CapabilityCreate(
			*getCState(state), C.CString(name), C.CString(revision))
		defer C.CapabilityFree(capability)
		lookupTableValues = append(lookupTableValues, capability)

		namespace, ok := namespaces[name]
		if ok {
			var cnamespace *C.char = C.CString(namespace)
			lookupTableKeys = append(lookupTableKeys, cnamespace)
			lookupTableValues = append(lookupTableValues, capability)
		}
	}

	realRepo := C.RepositoryInitWithPath(*getCState(state), repoPath)
	panicOnCStateError(getCState(state))

	repo.Private = realRepo
	rootSchemaWrapper := C.RepositoryCreateRootSchemaWrapper(
		*getCState(state),
		realRepo,
		&lookupTableKeys[0],
		&lookupTableValues[0],
		C.int(len(lookupTableKeys)))
	panicOnCStateError(getCState(state))

	rootSchemaNode := types.RootSchemaNode{Private: rootSchemaWrapper}
	return rootSchemaNode
}

// CodecServiceEncode encodes entity to XML/JSON payloads based on
// encoding format passed in.
// Returns the resulting payload (string).
func CodecServiceEncode(
	state *errors.State,
	entity types.Entity,
	rootSchema types.RootSchemaNode,
	encoding encodingFormat.EncodingFormat) string {

	rootSchemaWrapper := rootSchema.Private.(C.RootSchemaWrapper)
	realRootSchema := C.RootSchemaWrapperUnwrap(rootSchemaWrapper)

	dataNode := getDataNodeFromEntity(state, entity, realRootSchema)

	if dataNode == nil {
		return ""
	}

	codec := C.CodecInit()
	defer C.CodecFree(codec)

	var payload *C.char
	defer C.free(unsafe.Pointer(payload))

	switch encoding {
	case encodingFormat.XML:
		payload = C.CodecEncode(*getCState(state), codec, dataNode, C.XML, 1)
		panicOnCStateError(getCState(state))
	case encodingFormat.JSON:
		payload = C.CodecEncode(*getCState(state), codec, dataNode, C.JSON, 1)
		panicOnCStateError(getCState(state))
	}

	return C.GoString(payload)
}

// CodecServiceDecode decodes XML/JSON payloads passed in to entity.
// Returns the top level entity (types.Entity) from resulting data node.
func CodecServiceDecode(
	state *errors.State,
	rootSchema types.RootSchemaNode,
	payload string,
	encoding encodingFormat.EncodingFormat,
	topEntity types.Entity) types.Entity {

	rootSchemaWrapper := rootSchema.Private.(C.RootSchemaWrapper)
	realRootSchema := C.RootSchemaWrapperUnwrap(rootSchemaWrapper)

	codec := C.CodecInit()
	defer C.CodecFree(codec)

	var realPayload = C.CString(payload)
	defer C.free(unsafe.Pointer(realPayload))
	var realDataNode C.DataNode

	switch encoding {
	case encodingFormat.XML:
		realDataNode = C.CodecDecode(
			*getCState(state), codec, realRootSchema, realPayload, C.XML)
		panicOnCStateError(getCState(state))
	case encodingFormat.JSON:
		realDataNode = C.CodecDecode(
			*getCState(state), codec, realRootSchema, realPayload, C.JSON)
		panicOnCStateError(getCState(state))
	}

	var dataNode = types.DataNode{Private: realDataNode}

	return ReadDatanode(topEntity, dataNode)
}

// ConnectToOpenDaylightProvider connects to OpenDaylight device.
// Returns the connected service provider (types.COpenDaylightServiceProvier).
func ConnectToOpenDaylightProvider(
	state *errors.State,
	Path, Address, Username, Password string,
	port int,
	encoding encodingFormat.EncodingFormat,
	proto protocol.Protocol) types.COpenDaylightServiceProvider {

	var path *C.char = C.CString(Path)
	defer C.free(unsafe.Pointer(path))
	var address *C.char = C.CString(Address)
	defer C.free(unsafe.Pointer(address))
	var username *C.char = C.CString(Username)
	defer C.free(unsafe.Pointer(username))
	var password *C.char = C.CString(Password)
	defer C.free(unsafe.Pointer(password))
	var cport C.int = C.int(port)

	AddCState(state)
	cstate := getCState(state)

	var p C.OpenDaylightServiceProvider
	crepo := C.RepositoryInitWithPath(*getCState(state), path)
	panicOnCStateError(getCState(state))

	cencoding := getCEncoding(encoding)

	cprotocol := getCProtocol(proto)

	p = C.OpenDaylightServiceProviderInitWithRepo(
		*cstate, crepo, address, username, password, cport, cencoding, cprotocol)
	panicOnCStateError(cstate)

	cprovider := types.COpenDaylightServiceProvider{Private: p}
	return cprovider
}

// DisconnectFromOpenDaylightProvider disconnects from OpenDaylight device and
// frees allocated memory.
func DisconnectFromOpenDaylightProvider(
	provider types.COpenDaylightServiceProvider) {

	realProvider := provider.Private.(C.OpenDaylightServiceProvider)
	C.OpenDaylightServiceProviderFree(realProvider)
}

// OpenDaylightServiceProviderGetNodeIDs is a getter function for the node ids
// given the opendaylight service provider.
// Returns node ids available ([]string).
func OpenDaylightServiceProviderGetNodeIDs(
	state *errors.State, provider types.COpenDaylightServiceProvider) []string {

	cprovider := provider.Private.(C.OpenDaylightServiceProvider)
	var ids []string
	id := 0
	for {
		cid := C.int(id)
		nodeID := C.OpenDaylightServiceProviderGetNodeIDByIndex(*getCState(state), cprovider, cid)
		panicOnCStateError(getCState(state))
		defer C.free(unsafe.Pointer(nodeID))
		if nodeID != nil {
			ids = append(ids, C.GoString(nodeID))
			id++
		} else {
			break
		}
	}
	return ids
}

// OpenDaylightServiceProviderGetNodeProvider is a getter function for the
// node provider given the opendaylight service provider and node id.
// Returns service provider (types.CServiceProvider) based on given node id.
func OpenDaylightServiceProviderGetNodeProvider(
	state *errors.State,
	provider types.COpenDaylightServiceProvider,
	nodeID string) types.CServiceProvider {

	realProvider := provider.Private.(C.OpenDaylightServiceProvider)
	cnodeID := C.CString(nodeID)
	defer C.free(unsafe.Pointer(cnodeID))
	var nodeProvider C.ServiceProvider
	nodeProvider = C.OpenDaylightServiceProviderGetNodeProvider(
		*getCState(state), realProvider, cnodeID)
	panicOnCStateError(getCState(state))

	cnodeProvider := types.CServiceProvider{Private: nodeProvider}
	return cnodeProvider
}

//////////////////////////////////////////////////////////////////////////
// DataNode from Entity
//////////////////////////////////////////////////////////////////////////
func getDataNodeFromEntity(
	state *errors.State,
	entity types.Entity,
	rootSchema C.RootSchemaNode) C.DataNode {

	if entity == nil {
		return nil
	}

	parent := types.GetParent(entity)
	for parent != nil {
		entity = parent
		parent = types.GetParent(entity)
	}

	rootPath := types.GetEntityPath(entity)
	path := C.CString(rootPath.Path)
	defer C.free(unsafe.Pointer(path))

	rootDataNode := C.RootSchemaNodeCreate(*getCState(state), rootSchema, path)
	panicOnCStateError(getCState(state))

	addDataNodeFilterAnnotation(&rootDataNode, entity.GetEntityData().YFilter)

	populateNameValues(state, rootDataNode, rootPath)
	walkChildren(state, entity, rootDataNode)
	return rootDataNode
}

func walkChildren(
	state *errors.State, entity types.Entity, dataNode C.DataNode) {
	children := entity.GetEntityData().Children

	ydk.YLogDebug(fmt.Sprintf("Got %d entity children", len(children)))

	for childName := range children {
		if children[childName].Value != nil {
			segmentPath := children[childName].Value.GetEntityData().SegmentPath
			ydk.YLogDebug(fmt.Sprintf(
				"Looking at entity child '%s'", segmentPath))

			if types.HasDataOrFilter(children[childName].Value) {
				populateDataNode(state, children[childName].Value, dataNode)
			}
		}
	}
	ydk.YLogDebug("")
}

func populateDataNode(
	state *errors.State, entity types.Entity, parentDataNode C.DataNode) {

	path := types.GetEntityPath(entity)
	p := C.CString(path.Path)
	defer C.free(unsafe.Pointer(p))
	ep := C.CString("")
	defer C.free(unsafe.Pointer(ep))

	dataNode := C.DataNodeCreate(*getCState(state), parentDataNode, p, ep)
	panicOnCStateError(getCState(state))

	if dataNode == nil {
        ydk.YLogError(fmt.Sprintf("Datanode could not be created for: %v", path.Path))
		panic("Datanode could not be created for: " + path.Path)
	}

	addDataNodeFilterAnnotation(&dataNode, entity.GetEntityData().YFilter)

	populateNameValues(state, dataNode, path)
	walkChildren(state, entity, dataNode)
}

func populateNameValues(
	state *errors.State, dataNode C.DataNode, path types.EntityPath) {

	for _, nameValue := range path.ValuePaths {
		var result C.DataNode
		leafData := nameValue.Data
		p := C.CString(nameValue.Name)
		ydk.YLogDebug(fmt.Sprintf(
			"got leaf {%s: %s}", nameValue.Name, nameValue.Data.Value))

		if leafData.IsSet {
			p1 := C.CString(leafData.Value)
			result = C.DataNodeCreate(*getCState(state), dataNode, p, p1)
			panicOnCStateError(getCState(state))
			C.free(unsafe.Pointer(p1))
		}

		addDataNodeFilterAnnotation(&result, leafData.Filter)
		C.free(unsafe.Pointer(p))
	}
}

//////////////////////////////////////////////////////////////////////////
// Entity from DataNode
//////////////////////////////////////////////////////////////////////////
func getEntityFromDataNode(node C.DataNode, entity types.Entity) {
	if entity == nil || node == nil {
		return
	}

	cchildren := C.DataNodeGetChildren(node)
	children := (*[1 << 30]C.DataNode)(
		unsafe.Pointer(cchildren.datanodes))[:cchildren.count:cchildren.count]
    nodeName := C.GoString(C.DataNodeGetArgument(node))
    ydk.YLogDebug(fmt.Sprintf("Got %d datanode children for %v", cchildren.count, nodeName))
    moduleName := C.GoString(C.DataNodeGetModuleName(node))

	for _, childDataNode := range children {
		childName := C.GoString(C.DataNodeGetArgument(childDataNode))
        childModuleName := C.GoString(C.DataNodeGetModuleName(childDataNode))
		ydk.YLogDebug(fmt.Sprintf("Looking at child datanode: '%s'", childName))
        if(moduleName != childModuleName) {
            childName = childModuleName + ":" + childName;
        }

		if dataNodeIsLeaf(childDataNode) {

			value := C.GoString(C.DataNodeGetValue(childDataNode))
			ydk.YLogDebug(fmt.Sprintf(
				"Creating leaf '%s' with value '%s'", childName, value))
			types.SetValue(entity, childName, value)
		} else {

			var childEntity types.Entity
			if dataNodeIsList(childDataNode) {
				segmentPath := C.GoString(C.DataNodeGetSegmentPath(childDataNode))
				ydk.YLogDebug(fmt.Sprintf("Creating child list instance '%s' '%s'", childName, segmentPath))
				childEntity = types.GetChildByName(entity, childName, segmentPath)
			} else {
				ydk.YLogDebug(fmt.Sprintf("Creating child node '%s'", childName))
				childEntity = types.GetChildByName(entity, childName, "")
			}
			if childEntity == nil {
				ydk.YLogError(fmt.Sprintf("Could not create child entity '%s'!", childName))
				panic("Could not create child entity!")
			}
			types.SetParent(childEntity, entity)
			getEntityFromDataNode(childDataNode, childEntity)
		}
	}
}

func dataNodeIsLeaf(dataNode C.DataNode) bool {
	return C.GoString(C.DataNodeGetKeyword(dataNode)) == "leaf" ||
		C.GoString(C.DataNodeGetKeyword(dataNode)) == "leaf-list"
}

func dataNodeIsList(dataNode C.DataNode) bool {
	return C.GoString(C.DataNodeGetKeyword(dataNode)) == "list"
}

func addDataNodeFilterAnnotation(dataNode *C.DataNode, yf yfilter.YFilter) {
	if types.IsSet(yf) && yf != yfilter.Read {
		p := C.CString(fmt.Sprintf("%s", yf))
		defer C.free(unsafe.Pointer(p))
		C.DataNodeAddAnnotation(*dataNode, p)
	}
}

//////////////////////////////////////////////////////////////////////////
// Error Handling
//////////////////////////////////////////////////////////////////////////

// AddCState creates and adds cstate to *errors.State.
func AddCState(state *errors.State) {
	cstate := C.YDKStateCreate()
	state.Private = errors.CState{Private: cstate}
}

func checkState(cstate *C.YDKStatePtr) error {
	var cerrorOccurred C.boolean
	cerrorOccurred = C.YDKStateErrorOccurred(*cstate)
	if cerrorOccurred == 0 {
		return nil
	}

	rawErrMsg := C.GoString(C.YDKStateGetErrorMessage(*cstate))
	i := strings.Index(rawErrMsg, ":")
	errMsg := rawErrMsg[i+1:]

	var cerrorType C.YDKErrorType
	cerrorType = C.YDKStateGetErrorType(*cstate)

	switch cerrorType {
	case C.YDK_CLIENT_ERROR:
		return &errors.YClientError{Msg: errMsg}
	case C.YDK_SERVICE_PROVIDER_ERROR:
		return &errors.YServiceProviderError{Msg: errMsg}
	case C.YDK_SERVICE_ERROR:
		return &errors.YServiceError{Msg: errMsg}
	case C.YDK_ILLEGAL_STATE_ERROR:
		return &errors.YIllegalStateError{Msg: errMsg}
	case C.YDK_INVALID_ARGUMENT_ERROR:
		return &errors.YInvalidArgumentError{Msg: errMsg}
	case C.YDK_OPERATION_NOTSUPPORTED_ERROR:
		return &errors.YOperationNotSupportedError{Msg: errMsg}
	case C.YDK_MODEL_ERROR:
		return &errors.YModelError{Msg: errMsg}
	case C.YDK_CORE_ERROR:
		return &errors.YCoreError{Msg: errMsg}
	case C.YDK_CODEC_ERROR:
		return &errors.YCodecError{Msg: errMsg}
	default:
		return &errors.YError{Msg: errMsg}
	}
}

func getCProtocol(proto protocol.Protocol) C.Protocol {
	if proto == protocol.Netconf {
		return C.Netconf
	} else {
		return C.Restconf
	}
}

func getCState(state *errors.State) *C.YDKStatePtr {
	statePtr := state.Private.(errors.CState).Private.(C.YDKStatePtr)
	return &statePtr
}

func getCEncoding(encoding encodingFormat.EncodingFormat) C.EncodingFormat {
	if encoding == encodingFormat.XML {
		return C.XML
	} else {
		return C.JSON
	}
}

func panicOnCStateError(cstate *C.YDKStatePtr) {
	err := checkState(cstate)
	if err != nil {
		C.YDKStateClear(*cstate)
		panic(err.Error())
	}
}
