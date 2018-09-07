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
package ylist

import (
	"fmt"
	"reflect"
	"github.com/CiscoDevNet/ydk-go/ydk/errors"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
)

// YList utilities to access list elements by key
func buildKeyList(entity types.Entity) []interface{} {
	keySlice := entity.GetEntityData().YListKeys
	key_list := []interface{}{}
	s := reflect.ValueOf(entity).Elem()

	for _, key := range keySlice {
		keyValueField := s.FieldByName(key)
		if keyValueField.IsValid() {
			f := keyValueField.Interface()
			val := reflect.ValueOf(f)
			switch val.Kind() {
			case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64:
				intVal := val.Int()
				key_list = append(key_list, intVal)
			case reflect.String:
				strVal := val.String()
				key_list = append(key_list, strVal)
			default:
				sVal := fmt.Sprintf("%v", val)
				key_list = append(key_list, sVal)
			}
		}
	}
	return key_list
}

func keysToStr(key_list []interface{}) string {
	var key_str string
	for _, key := range key_list {
		if len(key_str) > 0 {
			key_str += ","
		}
		key_str += fmt.Sprintf("%v", key)
	}
	return key_str
}

func toEntitySlice(slice interface{}) []types.Entity {
	s := reflect.ValueOf(slice)
	if s.Kind() != reflect.Slice {
		err := errors.YError{Msg: "ToEntitySlice() given a non-slice parameter type"}
		panic(err.Error())
	}
	ret := make([]types.Entity, s.Len())
	for i := 0; i < s.Len(); i++ {
		ent, ok := s.Index(i).Interface().(types.Entity)
		if !ok {
			err := errors.YError{Msg: "Slice element is not of type Entity. Cannot convert."}
			panic(err.Error())
		}
		ret[i] = ent
	}
	return ret
}

func Get(iSlice interface{}, keys ... interface{}) (int, types.Entity) {
	eSlice := toEntitySlice(iSlice)
	if len(keys) == 0 {
		return -1, nil
	}
	var keyToCmp string
	if reflect.ValueOf(keys[0]).Kind() == reflect.Slice {
		keyToCmp = keysToStr(keys[0].([]interface{}))
	} else {
		keyToCmp = keysToStr(keys)
	}
	for i, elem := range eSlice {
		keyList := buildKeyList(elem)
		if len(keyList) > 0 {
			key := keysToStr(keyList)
			if key == keyToCmp {
				return i, elem
			}
		}
	}
	return -1, nil
}

func Keys(iSlice interface{}) []interface{} {
	eSlice := toEntitySlice(iSlice)
	keyList := []interface{}{}
	for _, ient := range eSlice {
		ent := ient.(types.Entity)
		key := buildKeyList(ent)
		if len(key) == 1 {
			keyList = append(keyList, key[0])
			continue
		}
		if len(key) > 1 {
			keyList = append(keyList, key)
		}
	}
	return keyList
}

