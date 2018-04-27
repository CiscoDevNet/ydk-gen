// Package types provides built-in types specified in
// YANG RFC 6020 and types used in YDK Go APIs.
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
package types

import (
	"fmt"
	"reflect"
	"sort"
	"strings"
	encoding "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
	"github.com/CiscoDevNet/ydk-go/ydk/errors"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
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

// YChild encapsulates the GoName of an entity as well as the entity itself
type YChild struct {
	GoName 	string
	Value 	Entity
}

// YLeaf encapsulates the GoName of a leaf as well as the leaf itself
type YLeaf struct {
	GoName 	string
	Value 	interface{}
}

// CommonEntityData encapsulate common data within an Entity
type CommonEntityData struct {
	// static data (internals)
	YangName 					string
	BundleName 					string
	ParentYangName				string
	YFilter 					yfilter.YFilter
	Children 					*OrderedMap
	Leafs 						*OrderedMap
	SegmentPath					string

	CapabilitiesTable			map[string]string
	NamespaceTable				map[string]string
	BundleYangModelsLocation	string

	// dynamic data (internals)
	Parent 					Entity
}

// Entity is a basic type that represents containers in YANG
type Entity interface {
	GetEntityData()		*CommonEntityData
}

func EntityToString (e Entity) string {
    return fmt.Sprintf("Type: %s, Path: %v", reflect.TypeOf(e), GetSegmentPath(e))
}

// Bits is a basic type that represents the YANG bits type
type Bits map[string]bool

// BitsList represent a list of Bits
type BitsList struct {
	Value []map[string]bool
}

// Ordered Map
//
type OrderedMap struct {
	keys   []string
	values map[string]interface{}
}

func NewOrderedMap() *OrderedMap {
	om := OrderedMap{}
	om.keys = []string{}
	om.values = map[string]interface{}{}
	return &om
}

func (om *OrderedMap) Len() int {
	return len(om.keys)
}

func (om *OrderedMap) Get(key string) (interface{}, bool) {
	val, exists := om.values[key]
	return val, exists
}

func (om *OrderedMap) Append(key string, value interface{}) {
	_, exists := om.values[key]
	if !exists {
		om.keys = append(om.keys, key)
	}
	om.values[key] = value
}

func (om *OrderedMap) Pop(key string) (interface{}, bool) {
	// check key is in use
	value, ok := om.values[key]
	if !ok {
		return nil, ok
	}
	// remove from keys
	for i, k := range om.keys {
		if k == key {
			om.keys = append(om.keys[:i], om.keys[i+1:]...)
			break
		}
	}
	// remove from values
	delete(om.values, key)
	return value, ok
}

func (om *OrderedMap) Keys() []string {
	return om.keys
}

func (om *OrderedMap) Values() []interface{} {
	values := make([]interface{}, len(om.keys))
	for i:=0; i<len(om.keys); i++ {
		values[i] = om.values[om.keys[i]]
	}
	return values
}

func (om *OrderedMap) Map() map[string]interface{} {
	m := map[string]interface{}{}
	for _, key := range om.keys {
		m[key] = om.values[key]
	}
	return m
}

// EntityCollection definition and methods
//
type EntityCollection struct {
	EcMap   *OrderedMap
}

// Implementing Entity interface
func (ec EntityCollection) GetEntityData() *CommonEntityData {
	if ec.Len() > 0 {
		entity := ec.GetItem(0)
		return entity.GetEntityData()
	}
	return nil
}

func EntityToCollection (e Entity) *EntityCollection {
	if e == nil {
		ec := NewEntityCollection()
		return &ec
	}
	
	ec, ok := e.(EntityCollection)
	if ok {
		return &ec
	} else {
		ecp, okp := e.(*EntityCollection)
		if okp {
			return ecp
		}
	}
	ec = NewEntityCollection(e)
	return &ec
}

func IsEntityCollection (e Entity) bool {
	if e == nil {
		return false
	}
	
	_, ok := e.(EntityCollection)
	if ok {
		return true
	} else {
		_, ok = e.(*EntityCollection)
		if ok {
			return true
		}
	} 
	return false
}

func NewEntityCollection(entities ... Entity) EntityCollection {
	ec := EntityCollection{}
	ec.EcMap = NewOrderedMap()
	if len(entities) > 0 {
		ec.Append(entities)
	}
	return ec
}

func (ec *EntityCollection) Add(entities ... Entity) {
	ec.Append(entities)
}

func (ec *EntityCollection) Append(entities []Entity) {
	for i:=0; i<len(entities); i++ {
		entity := entities[i]
		key := GetSegmentPath(entity)
		ec.EcMap.Append(key, entity)
	}
}

func (ec *EntityCollection) Len() int {
    return ec.EcMap.Len()
}

func (ec *EntityCollection) Get(key string) (Entity, bool) {
    elem, exists := ec.EcMap.Get(key)
    if exists {
        return elem.(Entity), exists
    }
    return nil, exists
}

func (ec *EntityCollection) GetItem(item int) Entity {
	if item < ec.Len() {
		key := ec.EcMap.keys[item]
		return ec.EcMap.values[key].(Entity)
	}
	return nil
}

func (ec *EntityCollection) HasKey(key string) bool {
    _, exists := ec.EcMap.Get(key)
    return exists
}

func (ec *EntityCollection) Pop(key string) (Entity, bool) {
    iEntity, exists := ec.EcMap.Pop(key)
    if !exists {
        return nil, exists
    }
    return iEntity.(Entity), exists
}

func (ec *EntityCollection) Clear() {
	ec.EcMap = NewOrderedMap()
}

func (ec *EntityCollection) Keys() []string {
    return ec.EcMap.Keys()
}

func (ec *EntityCollection) Entities() []Entity {
	entities := make([]Entity, ec.Len())
	iEntities := ec.EcMap.Values()
	for i:=0; i<ec.Len(); i++ {
		entities[i] = iEntities[i].(Entity)
	}
	return entities
}

func (ec *EntityCollection) String() string {
    if ec.Len() == 0 {
        return "EntityCollection is empty"
    }
    entities := ec.Entities()
    entity_str := make([]string, ec.Len())
    for i, entity := range entities {
        entity_str[i] = EntityToString(entity)
    }
    return fmt.Sprintf("EntityCollection [%s]", strings.Join(entity_str, "; "))
}

type Config = EntityCollection

func NewConfig(entities ... Entity) Config {
	ec := Config{}
	ec.EcMap = NewOrderedMap()
	if len(entities) > 0 {
		ec.Append(entities)
	}
	return ec
}

type Filter = EntityCollection

func NewFilter(entities ... Entity) Filter {
	ec := Filter{}
	ec.EcMap = NewOrderedMap()
	if len(entities) > 0 {
		ec.Append(entities)
	}
	return ec
}

/////////////////////////////////////
// Entity Utility Functions
/////////////////////////////////////

func GetYChild(entityData *CommonEntityData, name string) (YChild, bool) {
	iChild, ok := entityData.Children.Get(name)
	if !ok {
		return YChild{}, ok
	}
	return iChild.(YChild), ok
}

func GetYChildren(entityData *CommonEntityData) []YChild {
	iChildren := entityData.Children.Values()
	children := make([]YChild, len(iChildren))
	for i:=0; i<len(iChildren); i++ {
		children[i] = iChildren[i].(YChild)
	}
	return children
}

func GetYChildrenMap(entity Entity) map[string]YChild {
	children := entity.GetEntityData().Children
	m := map[string]YChild{}
	for _, key := range children.Keys() {
		m[key] = children.values[key].(YChild)
	}
	return m
}

func GetYLeafs(entityData *CommonEntityData) []YLeaf {
	iLeafs := entityData.Leafs.Values()
	leafs := make([]YLeaf, len(iLeafs))
	for i:=0; i<len(iLeafs); i++ {
		leafs[i] = iLeafs[i].(YLeaf)
	}
	return leafs
}

// GetSegmentPath returns the given entity's segment path
func GetSegmentPath(entity Entity) string {
	return entity.GetEntityData().SegmentPath
}

// GetParent returns the given entity's parent
func GetParent(entity Entity) Entity {
	return entity.GetEntityData().Parent
}

// SetParent sets the given entity's parent to the given parent entity
func SetParent(entity, parent Entity) {
	entity.GetEntityData().Parent = parent
}

// HasDataOrFilter returns a bool representing whether the entity
// or any of its children have their data/filter set
func HasDataOrFilter(entity Entity) bool {
	if entity == nil {
		return false
	}
	entityData := entity.GetEntityData()
	if (entityData.YFilter != yfilter.NotSet) {
		return true
	}

	// children
	children := GetYChildren(entityData)
	for _, child := range children {
		if child.Value != nil {
			yf := child.Value.GetEntityData().YFilter
			if (yf != yfilter.NotSet || HasDataOrFilter(child.Value)) {
				return true
			}
		}
	}

	v := reflect.ValueOf(entity).Elem()

	// checking leafs
	leafs := GetYLeafs(entityData)
	for _, leaf := range leafs {
		field := v.FieldByName(leaf.GoName)

		if field.Kind() != reflect.Slice {
			if leaf.Value != nil { return true }
		} else {
			for _, l := range field.Interface().([]interface{}) {
				if l != nil { return true }
			}
		}
	}

	return false
}

// GetEntityPath returns an EntityPath struct for the given entity
func GetEntityPath(entity Entity) EntityPath {
	entityData := entity.GetEntityData()
	entityPath := EntityPath{Path: entityData.SegmentPath}
	v := reflect.ValueOf(entity).Elem()

	// leafs
	var leafData LeafData
	leafs := entityData.Leafs.Map()
	for name, ileaf := range leafs {
		leaf := ileaf.(YLeaf)
		field := v.FieldByName(leaf.GoName)

		if leaf.Value != nil && field.Kind() != reflect.Slice {
			switch leaf.Value.(type) {
			case yfilter.YFilter:
				// yfilter
				leafData = LeafData{
					IsSet: true, Filter: leaf.Value.(yfilter.YFilter)}
			case map[string]bool:
				// bits
				var used_bits []string
				for bit, enabled := range(leaf.Value.(map[string]bool)) {
					if enabled {
						used_bits = append(used_bits, bit)
					}
				}
				v := strings.Join(used_bits, " ")
				leafData = LeafData{IsSet: true, Value: v}
			default:
				var v string
				if reflect.TypeOf(leaf.Value) != reflect.TypeOf(Empty{}) {
					v = fmt.Sprintf("%v", leaf.Value)
				}
				leafData = LeafData{
					IsSet: true, Value: v}
			}
			entityPath.ValuePaths = append(
				entityPath.ValuePaths,
				NameLeafData{Name: name, Data: leafData})
		}
	}

	return entityPath
}

// GetChildByName takes an Entity and returns the child Entity described
// by the given childYangName and segmentPath or nil if there is no match
func GetChildByName(
	entity Entity,
	childYangName string,
	segmentPath string) Entity {

	entityData := entity.GetEntityData()
	child, exists := GetYChild(entityData, childYangName)
	if !exists {
		return nil
	}

	goName := child.GoName
	s := reflect.ValueOf(entity).Elem()
	v := s.FieldByName(goName)

	if v.IsValid() {
		if v.Kind() == reflect.Slice {
			yChild, ok := GetYChild(entityData, segmentPath)
			if ok {
				return yChild.Value
			} else {
				sliceType := v.Type().Elem()
				childValue := reflect.New(sliceType).Elem()

				method := reflect.New(sliceType).MethodByName("GetEntityData")
				in := make([]reflect.Value, method.Type().NumIn())
				data := method.Call(in)[0].Elem().Interface().(CommonEntityData)
				v.Set(reflect.Append(v, childValue))

				entityData = entity.GetEntityData()
				yChild, ok = GetYChild(entityData, data.SegmentPath)
				return yChild.Value
			}
		} else {
			return child.Value
		}
	}
	return nil
}

// SetValue sets the leaf value for given entity, valuePath, and value args
func SetValue(entity Entity, valuePath string, value interface{}) {
	leafs := entity.GetEntityData().Leafs
	ileaf, ok := leafs.Get(valuePath)
	if !ok { return }

	leaf := ileaf.(YLeaf)
	s := reflect.ValueOf(entity).Elem()
	v := s.FieldByName(leaf.GoName)
	if v.IsValid() {
		if v.Type() == reflect.TypeOf(make(map[string]bool)) {
			bits := v.Interface().(map[string]bool)
			bits[value.(string)] = true

			v.Set(reflect.ValueOf(bits))
		} else if v.Type() == reflect.TypeOf(BitsList{}) {
			bitsValue := make(map[string]bool)
			bitsValue[value.(string)] = true
			
			bitslist := v.Interface().(BitsList)
			bitslist.Value = append(bitslist.Value, bitsValue)

			v.Set(reflect.ValueOf(bitslist))
		} else if v.Kind() == reflect.Slice {
			v.Set(reflect.Append(v, reflect.ValueOf(value)))
		} else {
			v.Set(reflect.ValueOf(value))
		}
	}
}


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

// ServiceProvider
type ServiceProvider interface {
	GetPrivate() interface{}
	Connect()
	Disconnect()
	GetState() *errors.State
}

// CodecServiceProvider
type CodecServiceProvider interface {
	Initialize(Entity)
	GetEncoding() encoding.EncodingFormat
	GetRootSchemaNode(Entity) RootSchemaNode
	GetState() *errors.State
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
	return s[i].GetEntityData().SegmentPath < s[j].GetEntityData().SegmentPath
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
	p := GetParent(current_node)
	parents := EntitySlice{}
	for p != nil && p != ancestor {
		//append(parents, p)
		p = GetParent(p)
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
		path_buffer += p1.GetEntityData().SegmentPath
	}
	if p != nil {
		path_buffer += "/"
	}
	path_buffer += current_node.GetEntityData().SegmentPath
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
	valuePath1 := GetEntityPath(e1).ValuePaths
	valuePath2 := GetEntityPath(e2).ValuePaths
	path1 := sortValuePaths(valuePath1)
	path2 := sortValuePaths(valuePath2)

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
	children1 := GetYChildrenMap(e1)
	children2 := GetYChildrenMap(e2)

	marker := make(map[string]bool)

	ret := true
	for k, c1 := range children1 {
		if c1.Value != nil {
			marker[k] = true
			if HasDataOrFilter(c1.Value) {
				c2, ok := children2[k]
				if ok && deepValueEqual(c1.Value, c2.Value) {
					ret = ret && nameValuesEqual(c1.Value, c2.Value)
				} else {
					ret = false
					break
				}
			}
		}
	}

	for k := range children2 {
		if children2[k].Value != nil{
			_, ok := marker[k]
			if !ok {
				ret = false
				break
			}
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
