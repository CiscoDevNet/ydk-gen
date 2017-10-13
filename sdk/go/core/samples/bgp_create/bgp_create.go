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
	"github.com/CiscoDevNet/ydk-go/ydk"
	oc_bgp "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp"
	oc_bgp_types "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp_types"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	//"github.com/CiscoDevNet/ydk-go/ydk/types"
)

func config_bgp(bgp *oc_bgp.Bgp) {
	bgp.Global.Config.As = 65172 // yfilter.Delete
	bgp.Global.Config.RouterId = "1.2.3.4"

	ipv6_afisafi := oc_bgp.Bgp_Global_AfiSafis_AfiSafi{}
	ipv6_afisafi.AfiSafiName = &oc_bgp_types.IPV6UNICAST{}
	ipv6_afisafi.Config.AfiSafiName = &oc_bgp_types.IPV6UNICAST{}
	ipv6_afisafi.Config.Enabled = true
	bgp.Global.AfiSafis.AfiSafi = append(bgp.Global.AfiSafis.AfiSafi, ipv6_afisafi)

	ipv4_afisafi := oc_bgp.Bgp_Global_AfiSafis_AfiSafi{}
	ipv4_afisafi.AfiSafiName = &oc_bgp_types.IPV4UNICAST{}
	ipv4_afisafi.Config.AfiSafiName = &oc_bgp_types.IPV4UNICAST{}
	ipv4_afisafi.Config.Enabled = true
	bgp.Global.AfiSafis.AfiSafi = append(bgp.Global.AfiSafis.AfiSafi, ipv4_afisafi)
}

func main() {
	defer func() {
		if r := recover(); r != nil {
			fmt.Println("\nError occured!!\n ", r)
		}
	}()

	vPtr := flag.Bool("v", false, "Enable verbose")
	flag.Parse()

	if *vPtr {
		ydk.EnableLogging(ydk.Debug)
	}

	var provider providers.NetconfServiceProvider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}

	provider.Connect()

	crud := services.CrudService{}

	bgp := oc_bgp.Bgp{}
	config_bgp(&bgp)

	result := crud.Create(&provider, &bgp)

	provider.Disconnect()

	if result == true {
		fmt.Println("\nOperation succeeded!\n")
	} else {
		fmt.Println("\nOperation failed!\n")
	}

}
