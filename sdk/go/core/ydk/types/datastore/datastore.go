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
package datastore

// DataStore is a complete set of configuration data that is required to get a
// device from its initial default state into a desired operational state
type DataStore int

const (
	NotSet DataStore = iota
	Candidate
	Running
	Startup
	Url
)

// String returns the name of the given YFilter (string)
func (ds DataStore) String() string {
	switch ds {
	case Candidate:
		return "candidate"
	case Running:
		return "running"
	case Startup:
		return "startup"
	case Url:
		return "url"
	case NotSet:
		return ""
	}
	return ""
}
