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
	ysanity_bgp "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	encoding "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
)

const (
	json_payload = `{
	"openconfig-bgp:bgp": {
		"global": {
			"afi-safis": {
				"afi-safi": [
					{
						"afi-safi-name": "openconfig-bgp-types:IPV6_UNICAST",
						"config": {
							"afi-safi-name": "openconfig-bgp-types:IPV6_UNICAST",
							"enabled": true
						}
					}
				]
			},
			"config": {
				"as": 65001
			}
		},
		"neighbors": {
			"neighbor": [
				{
					"neighbor-address": "2001:db8:e:1::1",
					"config": {
						"neighbor-address": "2001:db8:e:1::1",
						"peer-group": "EBGP"
					}
				}
			]
		},
		"peer-groups": {
			"peer-group": [
				{
					"peer-group-name": "EBGP",
					"afi-safis": {
						"afi-safi": [
							{
								"afi-safi-name": "openconfig-bgp-types:IPV6_UNICAST",
								"config": {
									"afi-safi-name": "openconfig-bgp-types:IPV6_UNICAST",
									"enabled": true
								},
								"apply-policy": {
									"config": {
									}
								}
							}
						]
					},
					"config": {
						"peer-as": 65002,
						"peer-group-name": "EBGP"
					},
					"transport": {
						"config": {
							"local-address": "Lookpback0"
						}
					}
				}
			]
		}
	}
}
`
)

func main() {

	vPtr := flag.Bool("v", false, "Enable verbose")
	flag.Parse()

	if *vPtr {
		ydk.EnableLogging(ydk.Info)
	}

	var provider = providers.CodecServiceProvider{}
	var service = services.CodecService{}

	fmt.Printf("\n========================= Payload for Decode =========================\n")
	fmt.Println(json_payload)
	fmt.Printf("\n========================= Encoded for Decode =========================\n")

	provider.Encoding = encoding.JSON
	entity := service.Decode(&provider, json_payload)
	bgp := entity.(*ysanity_bgp.Bgp)
	provider.Encoding = encoding.XML
	payload := service.Encode(&provider, bgp)

	fmt.Printf("\n========================= Encoded Payload =========================\n")
	fmt.Println(payload)
	fmt.Printf("\n========================= Encoded Payload =========================\n")
}
