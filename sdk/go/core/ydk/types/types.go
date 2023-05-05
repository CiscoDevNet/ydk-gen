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
	YangName			string
	BundleName			string
	ParentYangName			string
	YFilter				yfilter.YFilter
	Children			*OrderedMap
	Leafs				*OrderedMap
	SegmentPath			string
	AbsolutePath			string

	CapabilitiesTable		map[string]string
	NamespaceTable			map[string]string
	BundleYangModelsLocation	string

	YListKeys			[]string

	// dynamic data (internals)
	Parent 				Entity
}

// Entity is a basic type that represents containers in YANG
type Entity interface {
	GetEntityData()		*CommonEntityData
}

func EntityToString (e Entity) string {
    return fmt.Sprintf("Type: %s, Path: %v", reflect.TypeOf(e), GetSegmentPath(e))
}

func GetEntityFilter(ent Entity) yfilter.YFilter {
	filter := yfilter.NotSet
	entData := ent.GetEntityData()
	if entData != nil {
		filter = entData.YFilter
	}
	return filter
}

func SetEntityFilter(ent Entity, filter yfilter.YFilter) {
	s := reflect.ValueOf(ent).Elem()
	v := s.FieldByName("YFilter")
	if v.IsValid() {
		v.Set(reflect.ValueOf(filter))
	}
}

func IsTopLevelEntity(entity Entity) bool {
	entData := entity.GetEntityData()
	if entData != nil && entData.AbsolutePath == entData.SegmentPath {
		return true
	}
	return false
}

func SetNontopEntityFilter(entity Entity, filter yfilter.YFilter) {
	if IsEntityCollection(entity) {
		entCollection := EntityToCollection(entity)
	        for _, ent := range entCollection.Entities() {
	        	SetNontopEntityFilter(ent, filter)
	        }
	} else {
	        if !IsTopLevelEntity(entity) {
	        	SetEntityFilter(entity, filter)
	        }
	}
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

func (ec *EntityCollection) SetFilter(filter yfilter.YFilter) {
	iEntities := ec.EcMap.Values()
	for i:=0; i<ec.Len(); i++ {
		ent := iEntities[i].(Entity)
		SetEntityFilter(ent, filter)
	}
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

// Set Parent values in all children recursively
func SetAllParents(entity Entity) {
	children := GetYChildren(entity.GetEntityData())
	for _, child := range children {
		childEntity := child.Value
		if childEntity == nil || (IsPresenceContainer(childEntity) && !GetPresenceFlag(childEntity)) {
			continue
		}
		SetParent(childEntity, entity)
		SetAllParents(childEntity)
	}
}

// HasDataOrFilter returns a bool representing whether the entity
// or any of its children have their data/filter set
func HasDataOrFilter(entity Entity) bool {
	if entity == nil { return false }
	if GetPresenceFlag(entity) { return true }

	entityData := entity.GetEntityData()
	if (entityData.YFilter != yfilter.NotSet) { return true }

	// checking leaves
	leafs := GetYLeafs(entityData)
	v := reflect.ValueOf(entity).Elem()
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

	// checking children
	children := GetYChildren(entityData)
	for _, child := range children {
		if child.Value != nil {
			if HasDataOrFilter(child.Value) {
				return true
			}
		}
	}
	return false
}

// HasData returns a bool representing whether the entity
// or any of its children have data set
func HasData(entity Entity) bool {
	if entity == nil { return false }
	if GetPresenceFlag(entity) { return true }

	entityData := entity.GetEntityData()

	// checking leafs
	leafs := GetYLeafs(entityData)
	v := reflect.ValueOf(entity).Elem()
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

	// children
	children := GetYChildren(entityData)
	for _, child := range children {
		if HasData(child.Value) {
			return true
		}
	}
	return false
}

func getLeafValue(value interface{}) LeafData {
	var leafData LeafData
	switch value.(type) {
	case yfilter.YFilter:
		leafData = LeafData{IsSet: false, Filter: value.(yfilter.YFilter)}
	case LeafData:
	    leafData = value.(LeafData)
	    leafData.IsSet = true
	case map[string]bool:
		// bits
		var used_bits []string
		for bit, enabled := range(value.(map[string]bool)) {
			if enabled {
				used_bits = append(used_bits, bit)
			}
		}
		v := strings.Join(used_bits, " ")
		leafData = LeafData{IsSet: true, Value: v}
	default:
		var v string
		if reflect.TypeOf(value) != reflect.TypeOf(Empty{}) {
			v = fmt.Sprintf("%v", value)
		}
		leafData = LeafData{IsSet: true, Value: v}
	}
	return leafData
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
	    if !field.IsValid() || leaf.Value == nil  {
			continue
		}
		if field.Kind() != reflect.Slice {
			leafData = getLeafValue(leaf.Value)
            //fmt.Printf("Adding leaf: name: %s, data: %v\n", name, leafData)
			entityPath.ValuePaths = append(
				entityPath.ValuePaths,
				NameLeafData{Name: name, Data: leafData})
		} else {
		    // leaf-list
		    sliceInt := leaf.Value.([]interface{})
			for i := range sliceInt {
				leafData = getLeafValue(sliceInt[i])
				path := name
				if len(leafData.Value) > 0 {
				    path = fmt.Sprintf("%s[.=\"%v\"]", name, leafData.Value)
				    leafData.Value = ""
				}
				//fmt.Printf("Adding leaf-list: path: %s, data: %v\n", path, leafData)
				entityPath.ValuePaths = append(
					entityPath.ValuePaths,
					NameLeafData{Name: path, Data: leafData})
			}
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
				sliceType := v.Type().Elem().Elem()
				childValue := reflect.New(sliceType).Elem()
				v.Set(reflect.Append(v, childValue.Addr()))

				cv := childValue.FieldByName("YListKey")
				if cv.IsValid() {
					key := fmt.Sprintf("%d", v.Len())
					cv.Set(reflect.ValueOf(key))
				}

				method := childValue.Addr().MethodByName("GetEntityData")
				in := make([]reflect.Value, method.Type().NumIn())
				data := method.Call(in)[0].Elem().Interface().(CommonEntityData)

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

// IsPresenceContainer returns if the given entity is a presence container
func IsPresenceContainer(entity Entity) bool {
	v := reflect.ValueOf(entity).Elem()
	field := v.FieldByName("YPresence")
	return field.IsValid()
}

// GetPresenceFlag returns whether the presence flag of the given entity
// is set or not if it is a presence container
func GetPresenceFlag(entity Entity) bool {
	if !IsPresenceContainer(entity) { return false }
	v := reflect.ValueOf(entity).Elem()
	field := v.FieldByName("YPresence")
	return field.Interface().(bool)
}

// SetPresenceFlag sets the presence flag of the given entity if it is a presence container
func SetPresenceFlag(entity Entity) {
	if !IsPresenceContainer(entity) { return }
	v := reflect.ValueOf(entity).Elem()
	field := v.FieldByName("YPresence")
	field.Set(reflect.ValueOf(true))
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
	ExecuteRpc(string, Entity, map[string]string) DataNode
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

type Session struct {
	Private		interface{}
}

type Rpc struct {
	Input   DataNode
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

// IsFilterSet returns whether the given filter is set or not
func IsFilterSet(Filter yfilter.YFilter) bool {
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
		return true
	}
	if e1 == nil || e2 == nil {
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

func AddKeyToken(attr interface{}, attrName string) string {
    attrStr := fmt.Sprintf("%v", attr)
    var token string
    if strings.Index(attrStr, "'") >= 0 {
        token = fmt.Sprintf("[%s=\"%s\"]", attrName, attrStr)
    } else {
        token = fmt.Sprintf("[%s='%s']", attrName, attrStr)
    }
    return token
}

func AddNoKeyToken(ent Entity) string {
	var token string
	s := reflect.ValueOf(ent).Elem()
	v := s.FieldByName("YListKey")
	if v.IsValid() {
		key := v.Interface().(string)
		if len(key) > 0 {
			token = fmt.Sprintf("[%s]", key)
		}
	}
	return token
}

func SetYListKey(ent Entity, index int) {
	key := fmt.Sprintf("%d", index+1)
	s := reflect.ValueOf(ent).Elem()
	v := s.FieldByName("YListKey")
	if v.IsValid() {
		v.Set(reflect.ValueOf(key))
	}
}

func GetAbsolutePath(entity Entity) string {
	entityData := entity.GetEntityData()
	path := entityData.SegmentPath
	parent := entityData.Parent
	if parent != nil {
		path = fmt.Sprintf("%s/%s", GetAbsolutePath(parent), path)
	} else {
		if !IsTopLevelEntity(entity) {
			// This is the best available approximation
			path = entityData.AbsolutePath
		}
	}
	return path
}

func EntityToDict(entity Entity) map[string]string {
	edict := make(map[string]string)
	absPath := GetAbsolutePath(entity)
	if IsPresenceContainer(entity) || strings.LastIndex(absPath, "]") == len(absPath)-1 {
		edict[absPath] = ""
	}

	entityPath := GetEntityPath(entity)
	for _, leafData := range entityPath.ValuePaths {
		if leafData.Data.IsSet {
			leafName := leafData.Name
			leafValue := leafData.Data.Value
			keyPath := fmt.Sprintf("[%s=", leafName)
			if strings.Index(absPath, keyPath) == -1 {
				path := fmt.Sprintf("%s/%s", absPath, leafName)
				edict[path] = leafValue
			}
		}
	}
	children := GetYChildren(entity.GetEntityData())
	for _, child := range children {
		if child.Value == nil || (IsPresenceContainer(child.Value) && !GetPresenceFlag(child.Value)) {
			continue
		}
		childDict := EntityToDict(child.Value);
		for k, v := range childDict {
			edict[k] = v
		}
	}
	return edict
}

func PathToEntity(entity Entity, absPath string) Entity {
	topAbsPath := GetAbsolutePath(entity)
	if topAbsPath == absPath {
		return entity
	}

	if strings.Index(absPath, topAbsPath) == 0 {
		entityPath := GetEntityPath(entity)
		for _, leafData := range entityPath.ValuePaths {
			if leafData.Data.IsSet {
				leafName := leafData.Name
				keyPath := fmt.Sprintf("[%s=", leafName)
				if strings.Index(absPath, keyPath) == -1 {
					path := fmt.Sprintf("%s/%s", topAbsPath, leafName)
					if path == absPath {
						return entity
					}
				}
			}
		}
		children := GetYChildren(entity.GetEntityData())
		for _, child := range children {
			if child.Value == nil || (IsPresenceContainer(child.Value) && !GetPresenceFlag(child.Value)) {
				continue
			}
			childAbsPath := GetAbsolutePath(child.Value)
			if childAbsPath == absPath {
				return child.Value
			}
			if strings.Index(absPath, childAbsPath) != 0 {
				continue
			}
			matchingEntity := PathToEntity(child.Value, absPath)
			if matchingEntity != nil {
				return matchingEntity
			}
		}
	}
	return nil
}

func keyInSlice(k string, v []string) bool {
	for _, e := range v {
		if e == k {
			return true
		}
	}
	return false
}

func removeKeyFromSlice(k string, v []string) []string {
	for i, key := range v {
		if (k == key) {
			return append(v[:i], v[i+1:]...)
		}
	}
	return v
}

func getMapKeys(m map[string]string) []string {
	keys := make([]string, len(m))
	i := 0
	for k := range m {
		keys[i] = k
		i++
	}
	sort.Strings(keys)
	return keys
}

type StringPair struct {
	Left	string
	Right   string
}

func EntityDiff(ent1 Entity, ent2 Entity) map[string]StringPair {
	diffs := make(map[string]StringPair)
	if reflect.TypeOf(ent1) != reflect.TypeOf(ent2) {
		panic("EntityDiff: Incompatible arguments provided.")
	}

	entDict1 := EntityToDict(ent1)
	entDict2 := EntityToDict(ent2)
	entKeys1 := getMapKeys(entDict1)
	entKeys2 := getMapKeys(entDict2)
	var skipKeys1 []string
	for _, key := range entKeys1 {
		if keyInSlice(key, skipKeys1) {
			continue
		}
		if keyInSlice(key, entKeys2) {
			if entDict1[key] != entDict2[key] {
				diffs[key] = StringPair{Left: entDict1[key], Right: entDict2[key]}
			}
			entKeys2 = removeKeyFromSlice(key, entKeys2)
		} else {
			diffs[key] = StringPair{Left: entDict1[key], Right: "None"}
			for _, dupkey := range entKeys1 {
				if strings.Index(dupkey, key) >= 0 {
					skipKeys1 = append(skipKeys1, dupkey)
				}
			}
		}
	}
	var skipKeys2 []string
	for _, key := range entKeys2 {
		if keyInSlice(key, skipKeys2) {
			continue
		}
		diffs[key] = StringPair{Left: "None", Right: entDict2[key]}
		for _, dupkey := range entKeys2 {
			if (strings.Index(dupkey, key) >= 0) {
				skipKeys2 = append(skipKeys2, dupkey)
			}
		}
	}
	return diffs
}

func copyLeaves(originalEntity, clonedEntity Entity) {
	entityPath := GetEntityPath(originalEntity)
	for _, leafData := range entityPath.ValuePaths {
		if leafData.Data.IsSet {
			leafName := leafData.Name
			leafValue := leafData.Data.Value
			// fmt.Printf("Setting leaf '%s' value '%s'\n", leafName, leafValue)
			bracketPos := strings.Index(leafName, "[.=")
			if bracketPos != -1 {
				// Here we have leaf-list
				leafValue = leafName[bracketPos+4 : len(leafName)-2]
				leafName = leafName[0 : bracketPos]
			}
			SetValue(clonedEntity, leafName, leafValue)
		}
	}
}

func copyChildren(originalEntity, clonedEntity Entity) {
	children := GetYChildren(originalEntity.GetEntityData())
	for _, child := range children {
		childEntity := child.Value
		if HasData(childEntity) {
			childEntityData := childEntity.GetEntityData()
			childYangName := childEntityData.YangName
			// fmt.Printf("Cloning child '%s' with path '%s'\n",
			// 		childYangName, childEntityData.SegmentPath)
			clonedChild := GetChildByName(clonedEntity, childYangName, childEntityData.SegmentPath)
			if clonedChild == nil {
				panic("copyChildren: Failed to get entity child by YANG name")
			}
			SetParent(clonedChild, clonedEntity)
			if IsPresenceContainer(clonedChild) {
				SetPresenceFlag(clonedChild)
			}
			copyLeaves(childEntity, clonedChild);
			copyChildren(childEntity, clonedChild);
			clonedChild.GetEntityData()
		}
	}
}

// NewEntityOfType: Function to create new instance of given Entity
func NewEntityOfType(ent Entity) Entity {
	entType := reflect.TypeOf(ent).Elem()
	entPtr := reflect.New(entType)
	entInt := entPtr.Interface()
	entClone := entInt.(Entity)
	return entClone
}

// EntityClone - Function to clone an Entity
func EntityClone(ent Entity) Entity {
    entClone := NewEntityOfType(ent)
	copyLeaves(ent, entClone);
	copyChildren(ent, entClone);
	entClone.GetEntityData()
	return entClone
}
