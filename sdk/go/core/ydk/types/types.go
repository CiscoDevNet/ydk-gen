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
 * "License") you may not use this file except in compliance
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
package types

import (
	"reflect"
	"sort"
	encoding "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	"github.com/CiscoDevNet/ydk-go/ydk/types/ytype"
)

// Empty represents a YANG built-in Empty type
type Empty struct {
}

// String returns the representation of the Empty type as a string (an empty string)
func (e *Empty) String() string {
	return ""
}

// LeafData represents the data contained in a YANG leaf
type LeafData struct {
	Value  string
	Filter yfilter.YFilter
	IsSet  bool
}

// NameLeafData represents a YANG leaf to which a name and data can be assigned
type NameLeafData struct {
	Name string
	Data LeafData
}

// nameLeafDataList represents a YANG leaf-list
type nameLeafDataList []NameLeafData

// Len returns the length (int) of a given nameLeafDataList
func (p nameLeafDataList) Len() int {
	return len(p)
}

// Swap swaps the NameLeafData at indices i and j of the given nameLeafDataList
func (p nameLeafDataList) Swap(i, j int) {
	p[i], p[j] = p[j], p[i]
}

// Less returns whether the name of the NameLeafData at index i is less than the one at index j of a given nameLeafDataList
func (p nameLeafDataList) Less(i, j int) bool {
	return p[i].Name < p[j].Name
}

// EntityPath
type EntityPath struct {
	Path       string
	ValuePaths []NameLeafData
}

// AugmentCapabilitiesFunction
type AugmentCapabilitiesFunction func() map[string]string

// Entity is a basic type that represents containers in YANG
type Entity interface {
	GetEntityPath(Entity) EntityPath
	GetSegmentPath() string

	HasDataOrFilter() bool

	SetValue(string, string)
	GetChildByName(string, string) Entity

	GetChildren() map[string]Entity

	SetParent(Entity)
	GetParent() Entity

	GetAugmentCapabilitiesFunction() AugmentCapabilitiesFunction
	GetBundleYangModelsLocation() string
	GetBundleName() string

	GetYangName() string
	GetParentYangName() string

	GetFilter() yfilter.YFilter
}

// Entity is a basic type that represents containers in YANG
type Bits map[string]bool

// Decimal64 represents a YANG built-in Decimal64 type
type Decimal64 struct {
	value string
}

// EnumYLeaf represents variable data
type EnumYLeaf struct {
	value int
	name  string
}

// Enum represents a YANG built-in enum type, a base type for all YDK enums.
type Enum struct {
	EnumYLeaf
}

// YLeaf represents a YANG leaf to which data can be assigned.
type YLeaf struct {
	name string

	leaf_type  ytype.YType
	bits_value Bits

	Value  string
	IsSet  bool
	Filter yfilter.YFilter
}

// GetNameLeafdata instantiates and returns NameLeafData type for this leaf
func (y *YLeaf) GetNameLeafdata() NameLeafData {
	return NameLeafData{y.name, LeafData{y.Value, y.Filter, y.IsSet}}
}

// YLeafList represents a YANG leaf-list to which multiple instances of data can be appended
type YLeafList struct {
	name   string
	values []YLeaf

	Filter    yfilter.YFilter
	leaf_type ytype.YType
}

// GetYLeafs is a getter function for YLeafList values
func (y *YLeafList) GetYLeafs() []YLeaf {
	return y.values
}

// GetNameLeafdata instantiates and returns name NameLeafData for this YLeafList
func (y *YLeafList) GetNameLeafdata() [](NameLeafData) {
	result := make([]NameLeafData, len(y.values))
	for i := 0; i < len(y.values); i++ {
		result = append(result, NameLeafData{y.values[i].name, LeafData{y.values[i].Value, y.values[i].Filter, y.values[i].IsSet}})
	}
	return result
}

// ServiceProvider
type ServiceProvider interface {
	GetPrivate() interface{}
	Connect()
	Disconnect()
	GetState() *State
}

// CodecServiceProvider
type CodecServiceProvider interface {
	Initialize(Entity)
	GetEncoding() encoding.EncodingFormat
	GetRootSchemaNode(Entity) RootSchemaNode
	GetState() *State
}

// DataNode represents a containment hierarchy
type DataNode struct {
	Private interface{}
}

// RootSchemaNode represents the root of the SchemaTree. 
// It can be used to instantiate a DataNode tree or an Rpc object. 
// The children of the RootSchemaNode represent the top level SchemaNode in the YANG module submodules.
type RootSchemaNode struct {
	Private interface{}
}

// CServiceProvider
type CServiceProvider struct {
	Private interface{}
}

// COpenDaylightServiceProvider is a service provider to be used to communicate with an OpenDaylight instance.
type COpenDaylightServiceProvider struct {
	Private interface{}
}

// Repository represents the Repository of YANG models.
// A instance of the Repository will be used to create a RootSchemaNode given a set of ©pabilities.
// Behind the scenes the repository is responsible for loading and parsing the YANG modules and creating the SchemaNode tree. 
type Repository struct {
	Path    string
	Private interface{}
}

//////////////////////////////////////////////////////////////////////////
// Errors
//////////////////////////////////////////////////////////////////////////

// Represents YDK Go error types
type YGO_ERROR_TYPE int

const (
	YGO_ERROR_TYPE_NONE YGO_ERROR_TYPE = iota
	YGO_ERROR_TYPE_ERROR
	YGO_ERROR_TYPE_CLIENT_ERROR
	YGO_ERROR_TYPE_SERVICE_PROVIDER_ERROR
	YGO_ERROR_TYPE_SERVICE_ERROR
	YGO_ERROR_TYPE_ILLEGAL_STATE_ERROR
	YGO_ERROR_TYPE_INVALID_ARGUMENT_ERROR
	YGO_ERROR_TYPE_OPERATION_NOTSUPPORTED_ERROR
	YGO_ERROR_TYPE_MODEL_ERROR
)

// State represents the error state
type State struct {
	Private interface{}
}

// CState represents the error state
type CState struct {
	Private interface{}
}

type CError interface {
	Error() string
}

// YGOError is the basic error type in Go
type YGOError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOError) Error() string {
	return "YGOError:" + e.Msg
}

// YGOClientError is the error for client.
type YGOClientError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOClientError) Error() string {
	return "YGOClientError:" + e.Msg
}

// YGOServiceProviderError is the error for service provider.
type YGOServiceProviderError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOServiceProviderError) Error() string {
	return "YGOServiceProviderError:" + e.Msg
}

// YGOServiceError is the error for service.
type YGOServiceError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOServiceError) Error() string {
	return "YGOServiceError:" + e.Msg
}

// YGOIllegalStateError is raised when an operation/service is invoked on an object that is not in the right state.
type YGOIllegalStateError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOIllegalStateError) Error() string {
	return "YGOIllegalStateError:" + e.Msg
}

// YGOInvalidArgumentError is raised when there is an invalid argument.
type YGOInvalidArgumentError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOInvalidArgumentError) Error() string {
	return "YGOInvalidArgumentError:" + e.Msg
}

// YGOOperationNotSupportedError is raised for an unsupported operation.
type YGOOperationNotSupportedError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOOperationNotSupportedError) Error() string {
	return "YGOOperationNotSupportedError:" + e.Msg
}

// YGOModelError is raised when a model constraint is violated.
type YGOModelError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOModelError) Error() string {
	return "YGOModelError:" + e.Msg
}

// YGOModelError is the error for core.
type YGOCoreError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOCoreError) Error() string {
	return "YGOCoreError:" + e.Msg
}

// YGOCodecError encapsualtes the validation errors for codec service.
type YGOCodecError struct {
	Msg string
}

// Error satisfies the error interface
// Returns the error message (string)
func (e *YGOCodecError) Error() string {
	return "YGOCodecError" + e.Msg
}

//////////////////////////////////////////////////////////////////////////
// Exported utility functions
//////////////////////////////////////////////////////////////////////////

// EntitySlice is a slice of entities
type EntitySlice []Entity

// Len returns the length of given EntitySlice
func (s EntitySlice) Len() int {
	return len(s)
}

// Less returns whether the Entity at index i is less than the one at index j of the given EntitySlice
func (s EntitySlice) Less(i, j int) bool {
	return s[i].GetSegmentPath() < s[j].GetSegmentPath()
}

// Swap swaps the Entities at indices i and j of the given EntitySlice
func (s EntitySlice) Swap(i, j int) {
	s[i], s[j] = s[j], s[i]
}

// GetRelativeEntityPath returns the relative entity path (string)
func GetRelativeEntityPath(current_node Entity, ancestor Entity, path string) string {
	path_buffer := path

	if ancestor == nil {
		return ""
	}
	p := current_node.GetParent()
	parents := EntitySlice{}
	for p != nil && p != ancestor {
		//append(parents, p)
		p = p.GetParent()
	}

	if p == nil {
		return ""
	}

	parents = sort.Reverse(parents).(EntitySlice)

	p = nil
	for _, p1 := range parents {
		if p != nil {
			path_buffer += "/"
		} else {
			p = p1
		}
		path_buffer += p1.GetSegmentPath()
	}
	if p != nil {
		path_buffer += "/"
	}
	path_buffer += current_node.GetSegmentPath()
	return path_buffer

}

// IsSet returns whether the given filter is set or not
func IsSet(Filter yfilter.YFilter) bool {
	return Filter != yfilter.NotSet
}

func sortValuePaths(v []NameLeafData) []NameLeafData {
	ret := make([]NameLeafData, 0)
	for _, v := range v {
		ret = append(ret, v)
	}
	sort.Sort(nameLeafDataList(ret))
	return ret
}

func nameValuesEqual(e1, e2 Entity) bool {
	value_path1 := e1.GetEntityPath(e1.GetParent()).ValuePaths
	value_path2 := e2.GetEntityPath(e2.GetParent()).ValuePaths
	path1 := sortValuePaths(value_path1)
	path2 := sortValuePaths(value_path2)

	if len(path1) != len(path2) {
		return false
	}

	ret := true
	for k := range path1 {
		name1 := path1[k].Name
		value1 := path1[k].Data
		name2 := path2[k].Name
		value2 := path2[k].Data

		if name1 != name2 || !reflect.DeepEqual(value1, value2) {
			ret = false
			break
		}
	}
	return ret
}

func deepValueEqual(e1, e2 Entity) bool {
	if e1 == nil && e2 == nil {
		return false
	}
	children1 := e1.GetChildren()
	children2 := e2.GetChildren()

	marker := make(map[string]bool)

	ret := true
	for k, c1 := range children1 {
		marker[k] = true
		if c1.HasDataOrFilter() {
			c2, ok := children2[k]
			if ok && deepValueEqual(c1, c2) {
				ret = ret && nameValuesEqual(c1, c2)
			} else {
				ret = false
				break
			}
		}
	}

	for k := range children2 {
		_, ok := marker[k]
		if !ok {
			ret = false
			break
		}
	}

	return ret
}

// EntityEqual returns whether the entities x and y and their children are equal in value
func EntityEqual(x, y Entity) bool {
	if x == nil && y == nil {
		return x == y
	}
	return deepValueEqual(x, y)
}
