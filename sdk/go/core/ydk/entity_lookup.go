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
package ydk

import (
	"fmt"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"reflect"
)

var topEntityRegistry = make(map[string]reflect.Type)

func RegisterEntity(name string, entity_type reflect.Type) {
	topEntityRegistry[name] = entity_type
}

func GetTopEntity(name string) types.Entity {
	_, ok := topEntityRegistry[name]
	if !ok {
		panic(fmt.Sprintf("Top entity '%s' not registered!", name))
	}
	return reflect.New(topEntityRegistry[name]).Elem().Addr().Interface().(types.Entity)
}