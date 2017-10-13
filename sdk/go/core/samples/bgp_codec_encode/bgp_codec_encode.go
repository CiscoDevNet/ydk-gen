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
	ysanity_bgp_types "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp_types"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	encoding "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
)

func config(bgp *ysanity_bgp.Bgp) {
	bgp.Global.Config.As = 65001 // yfilter.Delete

	ipv6_afisafi := ysanity_bgp.Bgp_Global_AfiSafis_AfiSafi{}
	ipv6_afisafi.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	ipv6_afisafi.Config.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	ipv6_afisafi.Config.Enabled = true
	bgp.Global.AfiSafis.AfiSafi = append(bgp.Global.AfiSafis.AfiSafi, ipv6_afisafi)

	peer_group := ysanity_bgp.Bgp_PeerGroups_PeerGroup{}
	peer_group.PeerGroupName = "EBGP"
	peer_group.Config.PeerGroupName = "EBGP"
	peer_group.Config.PeerAs = 65002
	peer_group.Transport.Config.LocalAddress = "Lookpback0"

	peer_group_afisafi := ysanity_bgp.Bgp_PeerGroups_PeerGroup_AfiSafis_AfiSafi{}
	peer_group_afisafi.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	peer_group_afisafi.Config.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	peer_group_afisafi.Config.Enabled = true
	// TODO
	// peer_group_afisafi.ApplyPolicy.Config.ImportPolicy = append(peer_group_afisafi.ApplyPolicy.Config.ImportPolicy, "POLICY3")
	// peer_group_afisafi.ApplyPolicy.Config.ExportPolicy = append(peer_group_afisafi.ApplyPolicy.Config.ExportPolicy, "POLICY1")

	peer_group.AfiSafis.AfiSafi = append(peer_group.AfiSafis.AfiSafi, peer_group_afisafi)
	bgp.PeerGroups.PeerGroup = append(bgp.PeerGroups.PeerGroup, peer_group)

	neighbor := ysanity_bgp.Bgp_Neighbors_Neighbor{}
	neighbor.NeighborAddress = "2001:db8:e:1::1"
	neighbor.Config.NeighborAddress = "2001:db8:e:1::1"
	neighbor.Config.PeerGroup = "EBGP"
	bgp.Neighbors.Neighbor = append(bgp.Neighbors.Neighbor, neighbor)
}

func main() {
	vPtr := flag.Bool("v", false, "Enable verbose")

	var encoding_fmt string
	flag.StringVar(&encoding_fmt, "e", "xml", "Encoding format")

	flag.Parse()

	if *vPtr {
		ydk.EnableLogging(ydk.Debug)
	}
	var ef types.EncodingFormat

	switch encoding_fmt {
	case "xml":
		ef = encoding.XML
	case "json":
		ef = encoding.JSON
	default:
		panic("Encoding format not supported!")
	}

	bgp := ysanity_bgp.Bgp{}
	config(&bgp)

	var provider = providers.CodecServiceProvider{}
	provider.Encoding = ef

	var service = services.CodecService{}
	payload := service.Encode(&provider, &bgp)

	fmt.Printf("\n========================= Encoded Payload =========================\n")
	fmt.Println(payload)
	fmt.Printf("\n========================= Encoded Payload =========================\n")
}
