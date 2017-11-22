package yfilter
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

// YFilters represent edit operation for YDK objects as specified in NETCONF RFC 6241,
// defaults to not_set, and read operation providing functionality to read a singal leaf.
// Operations as defined under netconf edit-config operation attribute in RFC 6241
// and for filtering read operations by leaf to be used with various Services and entities.
type YFilter int

// YFilter constants.
const (
	NotSet YFilter = iota
	Read
	Merge
	Create
	Remove
	Delete
	Replace
)

// String returns the name of the given YFilter (string)
func (e YFilter) String() string {
	switch e {
	case Read:
		return "read"
	case Replace:
		return "replace"
	case Delete:
		return "delete"
	case Merge:
		return "merge"
	case Create:
		return "create"
	case Remove:
		return "remove"
	case NotSet:
		return ""
	}
	return ""
}