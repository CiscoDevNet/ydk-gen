/*
 * ------------------------------------------------------------------
 * YANG Development Kit
 * Copyright 2017 Cisco Systems. All rights reserved
 *
 *----------------------------------------------
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *----------------------------------------------
 */
package path

// #cgo CXXFLAGS: -g -std=c++11
// #cgo darwin LDFLAGS:  -fprofile-arcs -ftest-coverage -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lpython -lc++
// #cgo linux LDFLAGS:  -fprofile-arcs -ftest-coverage --coverage -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lstdc++ -lpython2.7 -ldl
// #include <ydk/ydk.h>
// #include <stdlib.h>
import "C"

import (
	"fmt"
	"github.com/CiscoDevNet/ydk-go/ydk"
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
	state *types.State,
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

	ydkRPC := C.RootSchemaNodeRpc(
		*cstate, rootSchema, C.CString(rpcEntity.GetSegmentPath()))
	panicOnCStateError(cstate)

	if rootSchema == nil {
        ydk.YLogError("root schema is nil!")
		panic(1)
	}

	rpcInput := C.RpcInput(*cstate, ydkRPC)
	panicOnCStateError(cstate)

	child := rpcEntity.GetChildByName("input", "")
	if (child != nil && child.HasDataOrFilter()) {
		walkRPCChildren(state, child, rpcInput, "")
	}

	readDataNode := types.DataNode{C.RpcExecute(*cstate, ydkRPC, realProvider)}
	panicOnCStateError(cstate)

	output := rpcEntity.GetChildByName("output", "")

	if (output == nil || readDataNode.Private == nil) {
		return nil
	}
	return ReadDatanode(topEntity, readDataNode)
}

func walkRPCChildren(
	state *types.State, rpcEntity types.Entity, rpcInput C.DataNode, path string) {

	ydk.YLogDebug("Walking Rpc Children...")
	if(rpcEntity != nil) {
		children := rpcEntity.GetChildren()
		entityPath := rpcEntity.GetEntityPath(rpcEntity.GetParent())
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
			if (children[childName] != nil &&
				children[childName].HasDataOrFilter()) {

				ydk.YLogDebug(fmt.Sprintf("Looking at entity child '%s'",
					children[childName].GetSegmentPath()))
				walkRPCChildren(state, children[childName], rpcInput, path)
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
	state *types.State, rpcEntity types.Entity, rpcInput C.DataNode, path string) {

	entityPath := rpcEntity.GetEntityPath(rpcEntity.GetParent())
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
	state *types.State, children map[string]types.Entity, rpcInput C.DataNode) {

	for childName, child := range children {
		if(child.HasDataOrFilter()) {
			ydk.YLogDebug(fmt.Sprintf("Creating child '%s' : %s",
				childName, child.GetEntityPath(child.GetParent()).Path))
			C.DataNodeCreate(*getCState(state), rpcInput, C.CString(childName), C.CString(""))
		}
	}
}

func getTopEntityFromFilter(filter types.Entity) types.Entity {
	if filter.GetParent() == nil {
		return filter
	}

	return getTopEntityFromFilter(filter.GetParent())
}

// ReadDatanode populates entity by reading the top level entity from a given data node.
// Returns the top entity (types.Entity) from readDataNode.
func ReadDatanode(filter types.Entity, readDataNode types.DataNode) types.Entity {
	if readDataNode.Private == nil {
		return nil
	}

	topEntity := getTopEntityFromFilter(filter)
	ydk.YLogDebug(
		fmt.Sprintf("Reading top entity: '%s'", topEntity.GetSegmentPath()))

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
	state *types.State,
	repo types.Repository,
	Address, Username, Password string,
	port int,
	Protocol string) types.CServiceProvider {

	var address *C.char = C.CString(Address)
	defer C.free(unsafe.Pointer(address))
	var username *C.char = C.CString(Username)
	defer C.free(unsafe.Pointer(username))
	var password *C.char = C.CString(Password)
	defer C.free(unsafe.Pointer(password))
	var cport C.int = C.int(port)

	var cprotocol *C.char = C.CString(Protocol)
	defer C.free(unsafe.Pointer(cprotocol))

	AddCState(state)
	cstate := getCState(state)

	var p C.ServiceProvider

	if len(repo.Path) > 0 {
		var path *C.char = C.CString(repo.Path)
		repo := C.RepositoryInitWithPath(*cstate, path)
		panicOnCStateError(cstate)
		p = C.NetconfServiceProviderInitWithRepo(
			*cstate, repo, address, username, password, cport, cprotocol)
		panicOnCStateError(cstate)
	} else {
		p = C.NetconfServiceProviderInit(
			*cstate, address, username, password, cport, cprotocol)
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
func CleanUpErrorState(state *types.State) {
	realState := getCState(state)
	C.YDKStateFree(*realState)
}

// ConnectToRestconfProvider connects to the RESTCONF device by creating
// a connection to the provider using given path, address, username, password,
// and port.
// Returns the connected service provider (types.CServiceProvider).
func ConnectToRestconfProvider(
	state *types.State,
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
	state *types.State,
	entity types.Entity,
	repo types.Repository) types.RootSchemaNode {

	caps := entity.GetAugmentCapabilitiesFunction()()

	var repoPath *C.char
	defer C.free(unsafe.Pointer(repoPath))

	if len(repo.Path) > 0 {
		ydk.YLogDebug(fmt.Sprintf(
			"CodecServiceProvider using YANG models in %v", repo.Path))
		repoPath = C.CString(repo.Path)
	} else {
		yangPath := entity.GetBundleYangModelsLocation()
		ydk.YLogDebug(fmt.Sprintf(
			"CodecServiceProvider using YANG models in %v", yangPath))
		repoPath = C.CString(yangPath)
	}

	realCaps := make([]C.Capability, 0)
	var realCap C.Capability
	for mod, rev := range caps {
		realCap = C.CapabilityCreate(
			*getCState(state), C.CString(mod), C.CString(rev))
		panicOnCStateError(getCState(state))
		defer C.CapabilityFree(realCap)
		realCaps = append(realCaps, realCap)
	}

	realRepo := C.RepositoryInitWithPath(*getCState(state), repoPath)
	panicOnCStateError(getCState(state))

	repo.Private = realRepo
	rootSchemaWrapper := C.RepositoryCreateRootSchemaWrapper(
		*getCState(state), realRepo, &realCaps[0], C.int(len(realCaps)))
	panicOnCStateError(getCState(state))

	rootSchemaNode := types.RootSchemaNode{Private: rootSchemaWrapper}
	return rootSchemaNode
}

// CodecServiceEncode encodes entity to XML/JSON payloads based on
// encoding format passed in.
// Returns the resulting payload (string).
func CodecServiceEncode(
	state *types.State,
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
	state *types.State,
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
	state *types.State,
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
	state *types.State, provider types.COpenDaylightServiceProvider) []string {

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
	state *types.State,
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
	state *types.State,
	entity types.Entity,
	rootSchema C.RootSchemaNode) C.DataNode {

	if entity == nil {
		return nil
	}
	for parent := entity.GetParent(); parent != nil; parent = parent.GetParent() {
		entity = parent
	}

	rootPath := entity.GetEntityPath(nil)
	path := C.CString(rootPath.Path)
	defer C.free(unsafe.Pointer(path))

	rootDataNode := C.RootSchemaNodeCreate(*getCState(state), rootSchema, path)
	panicOnCStateError(getCState(state))

	addDataNodeFilterAnnotation(&rootDataNode, entity.GetFilter())

	populateNameValues(state, rootDataNode, rootPath)
	walkChildren(state, entity, rootDataNode)
	return rootDataNode
}

func walkChildren(
	state *types.State, entity types.Entity, dataNode C.DataNode) {
	children := entity.GetChildren()

	ydk.YLogDebug(fmt.Sprintf("Got %d entity children", len(children)))

	for childName := range children {

		ydk.YLogDebug(fmt.Sprintf(
			"Looking at entity child '%s'", children[childName].GetSegmentPath()))

		if children[childName].HasDataOrFilter() {
			populateDataNode(state, children[childName], dataNode)
		}
	}
	ydk.YLogDebug("")
}

func populateDataNode(
	state *types.State, entity types.Entity, parentDataNode C.DataNode) {

	path := entity.GetEntityPath(entity.GetParent())
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

	addDataNodeFilterAnnotation(&dataNode, entity.GetFilter())

	populateNameValues(state, dataNode, path)
	walkChildren(state, entity, dataNode)
}

func populateNameValues(
	state *types.State, dataNode C.DataNode, path types.EntityPath) {

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
			entity.SetValue(childName, value)
		} else {

			var childEntity types.Entity
			if dataNodeIsList(childDataNode) {
				segmentPath := C.GoString(C.DataNodeGetSegmentPath(childDataNode))
				ydk.YLogDebug(fmt.Sprintf("Creating child list instance '%s'", segmentPath))
				childEntity = entity.GetChildByName(childName, segmentPath)
			} else {
				ydk.YLogDebug(fmt.Sprintf("Creating child node '%s'", childName))
				childEntity = entity.GetChildByName(childName, "")
			}
			if childEntity == nil {
				ydk.YLogError(fmt.Sprintf("Could not create child entity '%s'!", childName))
				panic("Could not create child entity!")
			}
			childEntity.SetParent(entity)
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

// AddCState creates and adds cstate to *types.State.
func AddCState(state *types.State) {
	cstate := C.YDKStateCreate()
	state.Private = types.CState{Private: cstate}
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
		return &types.YGOClientError{Msg: errMsg}
	case C.YDK_SERVICE_PROVIDER_ERROR:
		return &types.YGOServiceProviderError{Msg: errMsg}
	case C.YDK_SERVICE_ERROR:
		return &types.YGOServiceError{Msg: errMsg}
	case C.YDK_ILLEGAL_STATE_ERROR:
		return &types.YGOIllegalStateError{Msg: errMsg}
	case C.YDK_INVALID_ARGUMENT_ERROR:
		return &types.YGOInvalidArgumentError{Msg: errMsg}
	case C.YDK_OPERATION_NOTSUPPORTED_ERROR:
		return &types.YGOOperationNotSupportedError{Msg: errMsg}
	case C.YDK_MODEL_ERROR:
		return &types.YGOModelError{Msg: errMsg}
	case C.YDK_CORE_ERROR:
		return &types.YGOCoreError{Msg: errMsg}
	case C.YDK_CODEC_ERROR:
		return &types.YGOCodecError{Msg: errMsg}
	default:
		return &types.YGOError{Msg: errMsg}
	}
}

func getCProtocol(proto protocol.Protocol) C.Protocol {
	if proto == protocol.Netconf {
		return C.Netconf
	} else {
		return C.Restconf
	}
}

func getCState(state *types.State) *C.YDKStatePtr {
	statePtr := state.Private.(types.CState).Private.(C.YDKStatePtr)
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
