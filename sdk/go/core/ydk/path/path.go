/*  ----------------------------------------------------------------
 YDK - YANG Development Kit
 Copyright 2016 Cisco Systems. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 -------------------------------------------------------------------
 This file has been modified by Yan Gorelik, YDK Solutions.
 All modifications in original under CiscoDevNet domain
 introduced since October 2019 are copyrighted.
 All rights reserved under Apache License, Version 2.0.
 ------------------------------------------------------------------*/

package path

// #cgo CXXFLAGS: -g -std=c++11
// #cgo darwin LDFLAGS: -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lc++
// #cgo linux LDFLAGS:  -fprofile-arcs -ftest-coverage --coverage -lydk -lxml2 -lxslt -lpcre -lssh -lssh_threads -lcurl -lstdc++ -lm -ldl
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
	"reflect"
)

// ExecuteRPC executes payload converted from entity for CRUD/Netconf services.
// Returns a data node (types.DataNode) representing the result of the executed rpc.
func ExecuteRPC(
	provider types.ServiceProvider,
	rpcTag string,
	data map[string]interface{},
	setConfigFlag bool) types.DataNode {

	state := provider.GetState()
	cstate := GetCState(state)
	wrappedProvider := provider.GetPrivate().(types.CServiceProvider)
	realProvider := wrappedProvider.Private.(C.ServiceProvider)
	rootSchema := C.ServiceProviderGetRootSchema(*cstate, realProvider)
	PanicOnCStateError(cstate)

	ydkRPC := C.RootSchemaNodeRpc(*cstate, rootSchema, C.CString(rpcTag))
	PanicOnCStateError(cstate)

	if rootSchema == nil {
		ydk.YLogError("root schema is nil!")
		panic(1)
	}

	input := C.RpcInput(*cstate, ydkRPC)
	PanicOnCStateError(cstate)

	if setConfigFlag {
		C.DataNodeCreate(*cstate, input, C.CString("only-config"), C.CString(""))
		PanicOnCStateError(cstate)
	}

	var dataTag string = ""
	var value interface{} = nil
	for dataTag, value = range data {
		dataValue := C.CString("")
		switch v := value.(type) {
		case string:
			dataValue = C.CString(value.(string))
		default:
			_ = v
			dataValue = GetDataPayload(state, value.(types.Entity), rootSchema, provider)
			defer C.free(unsafe.Pointer(dataValue))
		}
		if !(dataTag == "filter" && len(C.GoString(dataValue)) == 0) {
			C.DataNodeCreate(*cstate, input, C.CString(dataTag), dataValue)
		}
	}

	dataNode := types.DataNode{C.RpcExecute(*cstate, ydkRPC, realProvider)}
	PanicOnCStateError(cstate)

	return dataNode
}

func GetDataPayload(
	state *errors.State,
	entity types.Entity,
	rootSchema C.RootSchemaNode,
	provider types.ServiceProvider) *C.char {

	codec := C.CodecInit()
	defer C.CodecFree(codec)
	cprovider := provider.GetPrivate().(
		types.CServiceProvider).Private.(C.ServiceProvider)
	cencoding := C.ServiceProviderGetEncoding(cprovider)

	retString := ""
	config := types.EntityToCollection(entity)
	for _, ent := range config.Entities() {

		datanode := getDataNodeFromEntity(state, ent, rootSchema)
		if datanode == nil {
			return nil
		}

		topDatanode := C.DataNodeGetTopDataNode(datanode)
		if topDatanode == nil {
			dnPath := C.GoString(C.DataNodeGetPath(datanode))
			panic(fmt.Sprintf("path.GetDataPayload: Failed to get top level datanode from datanode with path '%s'", dnPath))
		}

		var data *C.char = C.CodecEncode(*GetCState(state), codec, topDatanode, cencoding, 0)
		PanicOnCStateError(GetCState(state))

		if cencoding == C.JSON {
			// YG: So far there is no support for multiple JSON encoded entities
			return data
		}
		retString += C.GoString(data)
		C.free(unsafe.Pointer(data))
	}
	return C.CString(retString)
}

// ExecuteRPCEntity provides the functionality to execute RPCs with
// executor service using an RPC class defined under a ydk.models subpackage
// Optional args: topEntity to be passed in only when expecting return data
// Returns nil or an instance of types.Entity when expecting return data
func ExecuteRPCEntity(
	provider types.ServiceProvider,
	rpcEntity, topEntity types.Entity) types.Entity {

	state := provider.GetState()
	cstate := GetCState(state)
	wrappedProvider := provider.GetPrivate().(types.CServiceProvider)
	realProvider := wrappedProvider.Private.(C.ServiceProvider)
	rootSchema := C.ServiceProviderGetRootSchema(*cstate, realProvider)
	PanicOnCStateError(cstate)

	segmentPath := rpcEntity.GetEntityData().SegmentPath
	ydkRPC := C.RootSchemaNodeRpc(
		*cstate, rootSchema, C.CString(segmentPath))
	PanicOnCStateError(cstate)

	if rootSchema == nil {
        ydk.YLogError("root schema is nil!")
		panic(1)
	}

	rpcInput := C.RpcInput(*cstate, ydkRPC)
	PanicOnCStateError(cstate)

	ydk.YLogDebug(fmt.Sprintf("Calling GetChildByName for Entity: %s: childYangName: %s, segmentPath: %s", types.EntityToString(rpcEntity), "input", ""))

	child := types.GetChildByName(rpcEntity, "input", "")
	if (child != nil && types.HasDataOrFilter(child)) {
		ydk.YLogDebug("Calling walkRPCChildren")
		walkRPCChildren(state, child, rpcInput, "")
	}

	readDataNode := types.DataNode{C.RpcExecute(*cstate, ydkRPC, realProvider)}
	PanicOnCStateError(cstate)

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
		children := types.GetYChildrenMap(rpcEntity)
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

		for _, child := range children {
			if (child.Value != nil &&
				types.HasDataOrFilter(child.Value)) {

				segmentPath := child.Value.GetEntityData().SegmentPath
				ydk.YLogDebug(fmt.Sprintf("Looking at entity child '%s'", segmentPath))
				walkRPCChildren(state, child.Value, rpcInput, path)
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
			*GetCState(state),
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
			C.DataNodeCreate(*GetCState(state), rpcInput, C.CString(childName), C.CString(""))
		}
	}
}

// GetTopEntity traverses the entity hierarchy up to the top-level entity.
// Develops error if Parent for non-top-level entity is not set.
func GetTopEntity(entity types.Entity) types.Entity {
	if types.IsEntityCollection(entity) {
		entCollection := types.EntityToCollection(entity)
		topEntityCollection := types.NewEntityCollection()
	        for _, ent := range entCollection.Entities() {
	        	topEntity := GetTopEntity(ent)
	        	if topEntity != nil {
		        	topEntityCollection.Add(topEntity)
	        	}
	        }
	        return topEntityCollection
	} else {
		parent := types.GetParent(entity)
		if parent == nil {
			if types.IsTopLevelEntity(entity) {
				return entity
			} else {
				entData := entity.GetEntityData()
				msg := fmt.Sprintf("Parent is not set for non-top entity '%s'", entData.SegmentPath)
				ydk.YLogError(msg)
				return nil
			}
		}
		return GetTopEntity(parent)
	}
}

func findEntityInChildren(parentEntity types.Entity, filterAbsPath string) types.Entity {
	parentAbsPath := types.GetAbsolutePath(parentEntity)
	if (filterAbsPath == parentAbsPath) {
		ydk.YLogDebug("path.findEntityInChildren: Filter matches with parent entity, returning")
		return parentEntity
	}

	// Traverse parentEntity tree for search of matching filter entity
	ydk.YLogDebug(fmt.Sprintf("path.findEntityInChildren: Searching for filter entity '%s' under parent entity '%s'", filterAbsPath, parentAbsPath))
	ychildren := types.GetYChildren(parentEntity.GetEntityData())
	for _, ychild := range(ychildren) {
		child := ychild.Value
		if child == nil {
			continue
	    	}
	    	childAbsPath := types.GetAbsolutePath(child)
	        if childAbsPath == filterAbsPath {
	            return child
	        }
	        if strings.Index(filterAbsPath, childAbsPath) == 0 {
	        	ch := findEntityInChildren(child, filterAbsPath)
	        	if ch != nil {
				return ch
			}
	        }
	}
        return nil
}

func findChildEntity(topEntity types.Entity, filterAbsPath string) types.Entity {

        ydk.YLogDebug(fmt.Sprintf("path.findChildEntity: Searching for child entity matching non-top level filter '%s'", filterAbsPath))
        childEntity := findEntityInChildren(topEntity, filterAbsPath)
        if childEntity != nil {
        	ydk.YLogDebug("Found matching child entity!!")

		// Set paren of found entity to nil
		s := reflect.ValueOf(childEntity).Elem()
		v := s.FieldByName("Parent")
		if v.IsValid() {
			v.Set(reflect.ValueOf(nil))
		}
        } else {
        	ydk.YLogDebug("Matching child entity was not found")
        }
	return childEntity
}

// ReadDatanode populates entity by reading the top level entity from a given data node.
// Returns the top entity (types.Entity) from readDataNode.
func ReadDatanode(filter types.Entity, readDataNode types.DataNode) types.Entity {

	ec := types.NewEntityCollection()

	isFilterEC := types.IsEntityCollection(filter)
	cdn := readDataNode.Private.(C.DataNode)
	if cdn == nil {
		if isFilterEC {
			ydk.YLogDebug("path.ReadDatanode: The readDataNode is nil; returning empty collection")
			return ec
		} else {
			ydk.YLogDebug("path.ReadDatanode: The readDataNode is nil, returning nil")
			return nil
		}
	}

	cchildren := C.DataNodeGetChildren(cdn)
	if cchildren.count == C.int(0) {
		return filter
	}
	children := (*[1 << 30]C.DataNode)(
		unsafe.Pointer(cchildren.datanodes))[:cchildren.count:cchildren.count]

	// Need keep order of filters.
	filterEC := types.EntityToCollection(filter)
	ydk.YLogDebug(fmt.Sprintf("path.ReadDatanode: Number of entities in the filter: %v", filterEC.Len()))
	if filterEC.Len() > 0 {
		// Follow filter order
		for _, key := range filterEC.Keys() {
			filterEntity, _ := filterEC.Get(key)
			filterAbsPath := types.GetAbsolutePath(filterEntity)
			// Make fresh copy of entity to drop all data and filters
			topEntity := types.NewEntityOfType(filterEntity)
			for _, dn := range children {
				path := C.GoString(C.DataNodeGetPath(dn))[1:]
				bracketPos := strings.Index(path, "[")
				if bracketPos > 0 {
					path = path[0:bracketPos-1]
				}
				if strings.Index(filterAbsPath, path) == 0 {
					if dn != nil {
						if types.IsTopLevelEntity(filterEntity) {
							// Top level Entity
							getEntityFromDataNode(dn, topEntity)
						} else {
							// Non-top level Entity
							topEntity = DatanodeToEntity(dn)
							if topEntity == nil {
								continue
							}
							topEntity = findChildEntity(topEntity, filterAbsPath)
						}
					}
					ec.Add(topEntity)
				}
			}
		}
	} else {
		// Itterate over Datanodes
		for _, dn := range children {
			entity := DatanodeToEntity(dn)
			if entity != nil {
				ec.Add(entity)
			}
		}
	}
	ydk.YLogDebug(fmt.Sprintf("path.ReadDatanode: Number of entities returning: %v, filter is EC: %v", ec.Len(), isFilterEC))
	if ec.Len() > 1 || isFilterEC {
		return ec
	}
	if ec.Len() == 1 {
		return ec.GetItem(0)
	} else {
		return nil
	}
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
	cstate := GetCState(state)

	var p C.ServiceProvider

	if len(repo.Path) > 0 {
		var path *C.char = C.CString(repo.Path)
		repo := C.RepositoryInitWithPath(*cstate, path)
		PanicOnCStateError(cstate)
		p = C.NetconfServiceProviderInitWithOnDemandRepo(
			*cstate, repo, caddress, cusername, cpassword, cport, cprotocol, cOnDemand)
		PanicOnCStateError(cstate)
	} else {
		p = C.NetconfServiceProviderInitWithOnDemand(
			*cstate, caddress, cusername, cpassword, cport, cprotocol, cOnDemand, cCommonCache)
		PanicOnCStateError(cstate)
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

// GetCapabilitesFromNetconfProvider gets the capabilities supported by the provider.
// Returns the list of capabilities.
func GetCapabilitesFromNetconfProvider(provider types.CServiceProvider) []string {
	realProvider := provider.Private.(C.ServiceProvider)
	size := C.NetconfServiceProviderGetNumCapabilities(realProvider)
	capabilities := make([]string, size)
	for i := range capabilities {
		ccapability := C.NetconfServiceProviderGetCapabilityByIndex(realProvider, C.int(i))
		capabilities[i] = C.GoString(ccapability)
	}
	return capabilities
}

// CleanUpErrorState cleans up memory for CState.
func CleanUpErrorState(state *errors.State) {
	realState := GetCState(state)
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
	cstate := GetCState(state)

	var p C.ServiceProvider

	crepo := C.RepositoryInitWithPath(*GetCState(state), path)
	PanicOnCStateError(cstate)
	p = C.RestconfServiceProviderInitWithRepo(
		*cstate, crepo,
		address, username, password,
		cport, cencoding, cstateURLRoot,
		cconfigURLRoot)
	PanicOnCStateError(cstate)

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
			*GetCState(state), C.CString(name), C.CString(revision))
		defer C.CapabilityFree(capability)
		lookupTableValues = append(lookupTableValues, capability)

		namespace, ok := namespaces[name]
		if ok {
			var cnamespace *C.char = C.CString(namespace)
			lookupTableKeys = append(lookupTableKeys, cnamespace)
			lookupTableValues = append(lookupTableValues, capability)
		}
	}

	realRepo := C.RepositoryInitWithPath(*GetCState(state), repoPath)
	PanicOnCStateError(GetCState(state))

	repo.Private = realRepo
	rootSchemaWrapper := C.RepositoryCreateRootSchemaWrapper(
		*GetCState(state),
		realRepo,
		&lookupTableKeys[0],
		&lookupTableValues[0],
		C.int(len(lookupTableKeys)))
	PanicOnCStateError(GetCState(state))

	rootSchemaNode := types.RootSchemaNode{Private: rootSchemaWrapper}
	return rootSchemaNode
}

// CodecEncode encodes datanode to XML or JSON payload
// Returns the resulting payload (string).
func CodecEncode( datanode types.DataNode, encoding encodingFormat.EncodingFormat, pretty bool) string {

	cdn := datanode.Private.(C.DataNode)

	codec := C.CodecInit()
	defer C.CodecFree(codec)

	var state errors.State
	AddCState(&state)
	cstate := GetCState(&state)

	var cPretty C.boolean = 0
	if  pretty {
		cPretty = 1
	}

	var payload *C.char
	defer C.free(unsafe.Pointer(payload))
	switch encoding {
	case encodingFormat.XML:
		payload = C.CodecEncode(*cstate, codec, cdn, C.XML, cPretty)
		PanicOnCStateError(cstate)
	case encodingFormat.JSON:
		payload = C.CodecEncode(*cstate, codec, cdn, C.JSON, cPretty)
		PanicOnCStateError(cstate)
	}
	retString := C.GoString(payload)
	return retString
}

// CodecDecode decodes XML or JSON encoded payload
// Returns DanaNode.
func CodecDecode( rootSchema types.RootSchemaNode,
                  payload string,
                  encoding encodingFormat.EncodingFormat) types.DataNode {

	rootSchemaWrapper := rootSchema.Private.(C.RootSchemaWrapper)
	realRootSchema := C.RootSchemaWrapperUnwrap(rootSchemaWrapper)

	codec := C.CodecInit()
	defer C.CodecFree(codec)

	var state errors.State
	AddCState(&state)
	cstate := GetCState(&state)
	defer C.free(unsafe.Pointer(cstate))

	var realPayload = C.CString(payload)
	defer C.free(unsafe.Pointer(realPayload))

	var realDataNode C.DataNode

	switch encoding {
	case encodingFormat.XML:
		realDataNode = C.CodecDecode(
			*cstate, codec, realRootSchema, realPayload, C.XML)
	case encodingFormat.JSON:
		realDataNode = C.CodecDecode(
			*cstate, codec, realRootSchema, realPayload, C.JSON)
	}
	PanicOnCStateError(cstate)

	dataNode := types.DataNode{Private: realDataNode}
	return dataNode
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

	codec := C.CodecInit()
	defer C.CodecFree(codec)

	retString := ""
	config := types.EntityToCollection(entity)
	for _, ent := range config.Entities() {

		dataNode := getDataNodeFromEntity(state, ent, realRootSchema)
		if dataNode == nil {
			return ""
		}

		var payload *C.char
		defer C.free(unsafe.Pointer(payload))

		switch encoding {
		case encodingFormat.XML:
			payload = C.CodecEncode(*GetCState(state), codec, dataNode, C.XML, 1)
			PanicOnCStateError(GetCState(state))
			retString += C.GoString(payload)
		case encodingFormat.JSON:
			payload = C.CodecEncode(*GetCState(state), codec, dataNode, C.JSON, 1)
			PanicOnCStateError(GetCState(state))

			// YG: So far there is no support for multiple entities encoded with JSON format
			return C.GoString(payload)
		}
	}
	return retString
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
			*GetCState(state), codec, realRootSchema, realPayload, C.XML)
	case encodingFormat.JSON:
		realDataNode = C.CodecDecode(
			*GetCState(state), codec, realRootSchema, realPayload, C.JSON)
	}
	PanicOnCStateError(GetCState(state))

	var dataNode = types.DataNode{Private: realDataNode}
	cchildren := C.DataNodeGetChildren(dataNode.Private.(C.DataNode))
	ydk.YLogDebug(fmt.Sprintf("path.CodecServiceDecode: Top entity path: '%s'; Number of children in datanode: '%v'",
			topEntity.GetEntityData().SegmentPath, cchildren.count))
	if cchildren.count == C.int(0) {
		ydk.YLogDebug("path.CodecServiceDecode: Returning top entity")
		return topEntity
	}
	if cchildren.count == C.int(1) {
		ydk.YLogDebug("path.CodecServiceDecode: Getting entity from single datanode")
		return ReadDatanode(topEntity, dataNode)
	}
	// Payload contains multiple containers and or listleafs
	ydk.YLogDebug("path.CodecServiceDecode: Getting collection of entities")
	emptyCollection := types.NewFilter()
	return ReadDatanode(emptyCollection, dataNode)
}

func DatanodeToEntity(node C.DataNode) types.Entity {

	nodeName := C.GoString(C.DataNodeGetArgument(node))
    moduleName := C.GoString(C.DataNodeGetModuleName(node))
    path := fmt.Sprintf("%s:%s", moduleName, nodeName)
    ydk.YLogDebug(fmt.Sprintf("path.DatanodeToEntity: Got datanode with path: '%s'", path))

    topEntity, ok := ydk.GetTopEntity(path)
    if ok {
	    getEntityFromDataNode(node, topEntity)
    }
    return topEntity
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
	cstate := GetCState(state)

	var p C.OpenDaylightServiceProvider
	crepo := C.RepositoryInitWithPath(*GetCState(state), path)
	PanicOnCStateError(GetCState(state))

	cencoding := getCEncoding(encoding)

	cprotocol := getCProtocol(proto)

	p = C.OpenDaylightServiceProviderInitWithRepo(
		*cstate, crepo, address, username, password, cport, cencoding, cprotocol)
	PanicOnCStateError(cstate)

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
		nodeID := C.OpenDaylightServiceProviderGetNodeIDByIndex(*GetCState(state), cprovider, cid)
		PanicOnCStateError(GetCState(state))
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
		*GetCState(state), realProvider, cnodeID)
	PanicOnCStateError(GetCState(state))

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

	entPath := types.GetEntityPath(entity)
	entAbsPath := types.GetAbsolutePath(entity)
	path := C.CString(entAbsPath)
	defer C.free(unsafe.Pointer(path))

	cdn := C.RootSchemaNodeCreate(*GetCState(state), rootSchema, path)
	PanicOnCStateError(GetCState(state))
	dnPath := C.GoString(C.DataNodeGetPath(cdn))[1:]
	ydk.YLogDebug(fmt.Sprintf("path.getDataNodeFromEntity: Created datanode with path '%s'", dnPath))

	addDataNodeFilterAnnotation(&cdn, entity.GetEntityData().YFilter)

	populateNameValues(state, cdn, entPath)
	walkChildren(state, entity, cdn)
	return cdn
}

func walkChildren(
	state *errors.State, entity types.Entity, dataNode C.DataNode) {
	children := types.GetYChildren(entity.GetEntityData())

	ydk.YLogDebug(fmt.Sprintf("Got %d entity children", len(children)))

	for _, child := range children {
		if child.Value != nil {
			segmentPath := child.Value.GetEntityData().SegmentPath
			ydk.YLogDebug(fmt.Sprintf(
				"Looking at entity child '%s'", segmentPath))

			if types.HasDataOrFilter(child.Value) {
				populateDataNode(state, child.Value, dataNode)
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

	dataNode := C.DataNodeCreate(*GetCState(state), parentDataNode, p, ep)
	PanicOnCStateError(GetCState(state))

	if dataNode == nil {
        ydk.YLogError(fmt.Sprintf("Datanode could not be created for: %v", path.Path))
		panic("Datanode could not be created for: " + path.Path)
	}

	addDataNodeFilterAnnotation(&dataNode, entity.GetEntityData().YFilter)

	dnPath := C.GoString(C.DataNodeGetPath(dataNode))[1:]
	ydk.YLogDebug(fmt.Sprintf("path.populateDataNode: Populating leafs in datanode with path '%s'", dnPath))
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
			"path.populateNameValues: Got leaf: %v", nameValue))

		if leafData.IsSet || types.IsFilterSet(leafData.Filter) {
			p1 := C.CString("")
			if leafData.IsSet {
				p1 = C.CString(leafData.Value)
			}
			result = C.DataNodeCreate(*GetCState(state), dataNode, p, p1)
			PanicOnCStateError(GetCState(state))
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
	ydk.YLogDebug(fmt.Sprintf("path.getEntityFromDataNode: Got %d children in datanode '%s'", cchildren.count, nodeName))
	moduleName := C.GoString(C.DataNodeGetModuleName(node))

	for _, childDataNode := range children {
		childName := C.GoString(C.DataNodeGetArgument(childDataNode))
	        childModuleName := C.GoString(C.DataNodeGetModuleName(childDataNode))
		ydk.YLogDebug(fmt.Sprintf("Looking at child datanode: '%s'", childName))
	        if(moduleName != childModuleName) {
	            childName = childModuleName + ":" + childName
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
				ydk.YLogDebug(fmt.Sprintf("Creating child list instance '%s' with path '%s'", childName, segmentPath))
				childEntity = types.GetChildByName(entity, childName, segmentPath)
			} else {
				ydk.YLogDebug(fmt.Sprintf("Creating child entity '%s'", childName))
				childEntity = types.GetChildByName(entity, childName, "")
			}
			if childEntity == nil {
				ydk.YLogError(fmt.Sprintf("Could not create child entity '%s'!", childName))
				panic("Could not create child entity!")
			}
			types.SetPresenceFlag(childEntity)
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
	if types.IsFilterSet(yf) && yf != yfilter.Read {
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

func checkState(cstate C.YDKStatePtr) error {
	var cerrorOccurred C.boolean
	cerrorOccurred = C.YDKStateErrorOccurred(cstate)
	if cerrorOccurred == 0 {
		return nil
	}

	rawErrMsg := C.GoString(C.YDKStateGetErrorMessage(cstate))
	i := strings.Index(rawErrMsg, ":")
	errMsg := rawErrMsg[i+1:]

	var cerrorType C.YDKErrorType
	cerrorType = C.YDKStateGetErrorType(cstate)

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

func GetCState(state *errors.State) *C.YDKStatePtr {
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

func PanicOnCStateError(cstate *C.YDKStatePtr) {
	err := checkState(*cstate)
	if err != nil {
		C.YDKStateClear(*cstate)
		panic(err.Error())
	}
}

func CreateDataNode(dn types.DataNode, path string, value interface{}) types.DataNode {
	var state errors.State
	AddCState(&state)
	cstate := GetCState(&state)
	var cpath *C.char = C.CString(path)
	defer C.free(unsafe.Pointer(cpath))

	var svalue string = ""
	if value != nil {
		switch value.(type) {
		case string:
			svalue = value.(string)
		default:
			s := reflect.ValueOf(value)
			svalue = fmt.Sprintf("%v", s)
		}
	}
	cvalue := C.CString(svalue)
	defer C.free(unsafe.Pointer(cvalue))

	cdn := dn.Private.(C.DataNode)
	cdatanode := C.DataNodeCreate(*cstate, cdn, cpath, cvalue)
	PanicOnCStateError(cstate)

	datanode := types.DataNode{Private: cdatanode}
	return datanode
}

func CreateRootDataNode(rsn types.RootSchemaNode, path string) types.DataNode {
	var state errors.State
	AddCState(&state)
	cstate := GetCState(&state)

	var cpath *C.char = C.CString(path)
	defer C.free(unsafe.Pointer(cpath))

	rootSchemaWrapper := rsn.Private.(C.RootSchemaWrapper)
	realRootSchema := C.RootSchemaWrapperUnwrap(rootSchemaWrapper)

	cdatanode := C.RootSchemaNodeCreate(*cstate, realRootSchema, cpath)
	PanicOnCStateError(cstate)

	datanode := types.DataNode{Private: cdatanode}
	return datanode
}

func CreateRpc(rsn types.RootSchemaNode, yangpath string) types.Rpc {
	var state errors.State
	AddCState(&state)
	cstate := GetCState(&state)
	var cpath *C.char = C.CString(yangpath)
	defer C.free(unsafe.Pointer(cpath))

	rootSchemaWrapper := rsn.Private.(C.RootSchemaWrapper)
	realRootSchema := C.RootSchemaWrapperUnwrap(rootSchemaWrapper)

	crpc := C.RootSchemaNodeRpc(*cstate, realRootSchema, cpath)
	PanicOnCStateError(cstate)

	cinput := C.RpcInput(*cstate, crpc)
	PanicOnCStateError(cstate)

	inputdn := types.DataNode{Private: cinput}
	rpc := types.Rpc{Input: inputdn, Private: crpc}
	return rpc
}

func GetRootSchemaNode(provider types.CServiceProvider) types.RootSchemaNode {
	var state errors.State
	AddCState(&state)
	cstate := GetCState(&state)

	realProvider := provider.Private.(C.ServiceProvider)

	rootSchema := C.ServiceProviderGetRootSchemaNode(*cstate, realProvider)
	PanicOnCStateError(cstate)
	if rootSchema == nil {
	        ydk.YLogError("Root schema is nil!")
		panic(1)
	}

	rsn := types.RootSchemaNode{Private: rootSchema}
    return rsn
}

func ServiceProviderGetSession(provider types.CServiceProvider) types.Session {

	realProvider := provider.Private.(C.ServiceProvider)

	realSession := C.ServiceProviderGetSession(realProvider)

	session := types.Session{Private: realSession}
    return session
}
