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
	"flag"
	"fmt"
	"strconv"
	"strings"
	"github.com/CiscoDevNet/ydk-go/ydk"
	oc_bgp "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"runtime"
	"path/filepath"
)

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
	_, callerFile, _, _ := runtime.Caller(0)
	executablePath := filepath.Dir(callerFile)
	repo_path := executablePath + "/../../../../cpp/core/tests/models"
	repo := types.Repository{Path: repo_path}
	provider := providers.NetconfServiceProvider{
		Repo: repo,
		Address: device["address"],
		Username: device["username"],
		Password: device["password"],
		Port: port,
		Protocol: device["protocol"],
		OnDemand: true}
	provider.Connect()

	crud := services.CrudService{}

	bgp := oc_bgp.Bgp{}
	bgp.Global.Config.RouterId = "1.2.3.4"
	bgp.Global.Config.As = 65172

	data := crud.Read(&provider, &bgp)

	b := data.(*oc_bgp.Bgp)
	b.Global.Config.As = 65001

	if data != nil {
		fmt.Println("\nData read successfully!\n")
	}

	result := crud.Create(&provider, b)

	if result == true {
		fmt.Println("\nOperation succeeded!\n")
	} else {
		fmt.Println("\nOperation failed!\n")
	}

	provider.Disconnect()

}
