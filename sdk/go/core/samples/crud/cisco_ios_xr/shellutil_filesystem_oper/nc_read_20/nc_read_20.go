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
 *   http:www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *----------------------------------------------
 */
package main

import (
	"bytes"
	"flag"
	"fmt"
	"strconv"
	"strings"
	fileSys "github.com/CiscoDevNet/ydk-go/ydk/models/cisco_ios_xr/shellutil_filesystem_oper"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk"
)

// processFileSystem processes data in file_system object.
// inspired by format in 'show filesystem'.
func processFileSystem(fileSystem fileSys.FileSystem) string {
	var showFileSystem bytes.Buffer
	// iterate over all nodes
	for _, node := range fileSystem.Node {
		// file system header
		fmt.Fprintln(&showFileSystem, fmt.Sprintf("Node: RP/%v", node.NodeName))
		fmt.Fprintln(&showFileSystem, "File Systems:")
		fmt.Fprintf(&showFileSystem, "    Size(b)")
		fmt.Fprintf(&showFileSystem, "     Free(b)")
		fmt.Fprintf(&showFileSystem, "        Type")
		fmt.Fprintf(&showFileSystem, "  Flags")
		fmt.Fprintln(&showFileSystem, "  Prefixes")

		// iterate over all file systems
		for _, fsys := range node.FileSystem {
			// file system row
			fmt.Fprintf(&showFileSystem, fmt.Sprintf(fsys.Size.(string)))
			fmt.Fprintf(&showFileSystem,  fmt.Sprintf(" %v", fsys.Free.(string)))
			fmt.Fprintf(&showFileSystem,  fmt.Sprintf(" %v", fsys.Type.(string)))
			fmt.Fprintf(&showFileSystem,  fmt.Sprintf(" %v", fsys.Flags.(string)))
			fmt.Fprintln(&showFileSystem,  fmt.Sprintf("  %v", fsys.Prefixes.(string)))
		}
	}
	
	return showFileSystem.String()
}

// main execute main program.
func main() {
	defer func() {
		if r := recover(); r != nil {
			fmt.Println("\nError occured!!\n ", r)
		}
	}()

	// args
	vPtr := flag.Bool("v", false, "Enable verbose")
	devicePtr := flag.String(
		"device",
		"",
		"NETCONF device (ssh://user:password@host:port)")
	flag.Parse()

	// log debug messages if verbose argument specified 
	if *vPtr {
		ydk.EnableLogging(ydk.Info)
	}

	if (*devicePtr == "") {
		panic("Missing device arg see --help for details")
	}

	ydk.YLogDebug(*devicePtr)
	
	denominators := []string{"://", ":", "@", ":"}
	keys := []string {"protocol", "username", "password", "address", "port"}
	device := make(map[string]string)
	
	var split []string
	unprocessed := *devicePtr
	for i := 0; i < 4; i++ {
		if (!strings.Contains(unprocessed, denominators[i])) {
			panic(fmt.Sprintln("Device arg: device must be entered in",
				"ssh://user:password@host:port format"))
		}
		split = strings.SplitN(unprocessed, denominators[i], 2)

		device[keys[i]] = split[0]
		unprocessed = split[1]
	}
	device[keys[4]] = unprocessed

	port, err := strconv.Atoi(device["port"])
	if (err != nil) {
		panic("Device arg: port must be an int")
	}

	// create NETCONF provider
	provider := providers.NetconfServiceProvider{
		Address: device["address"],
		Username: device["username"],
		Password: device["password"],
		Port: port,
		Protocol: device["protocol"]}
	provider.Connect()

	// create CRUD service
	service := services.CrudService{}

	// read data from NETCONF device
	fileSystem := fileSys.FileSystem{}
	service.Read(&provider, &fileSystem)

	fmt.Println(processFileSystem(fileSystem))
}
