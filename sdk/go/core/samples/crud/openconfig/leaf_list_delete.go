/*  ----------------------------------------------------------------
 YDK - YANG Development Kit
 Copyright 2020 Yan Gorelik, YDK Solutions. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ------------------------------------------------------------------*/

package main

import (
	"fmt"
// 	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	"github.com/CiscoDevNet/ydk-go/ydk/types/ylist"
	ysanity_bgp "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp"
	ysanity_bgp_types "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp_types"
	ysanity_rp "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_routing_policy"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
)

func configBgp(bgp *ysanity_bgp.Bgp) {
	bgp.Global.Config.As = 65001

	ipv6_afisafi := ysanity_bgp.Bgp_Global_AfiSafis_AfiSafi{}
	ipv6_afisafi.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	ipv6_afisafi.Config.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	ipv6_afisafi.Config.Enabled = true
	bgp.Global.AfiSafis.AfiSafi = append(bgp.Global.AfiSafis.AfiSafi, &ipv6_afisafi)

	peer_group := ysanity_bgp.Bgp_PeerGroups_PeerGroup{}
	peer_group.PeerGroupName = "EBGP"
	peer_group.Config.PeerGroupName = "EBGP"
	peer_group.Config.PeerAs = 65002
	peer_group.Transport.Config.LocalAddress = "Lookpback0"

	peer_group_afisafi := ysanity_bgp.Bgp_PeerGroups_PeerGroup_AfiSafis_AfiSafi{}
	peer_group_afisafi.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	peer_group_afisafi.Config.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	peer_group_afisafi.Config.Enabled = true

	// Add import policies to the leaf-list
	peer_group_afisafi.ApplyPolicy.Config.ImportPolicy = append(peer_group_afisafi.ApplyPolicy.Config.ImportPolicy, "POLICY1")
	peer_group_afisafi.ApplyPolicy.Config.ImportPolicy = append(peer_group_afisafi.ApplyPolicy.Config.ImportPolicy, "POLICY3")

	peer_group.AfiSafis.AfiSafi = append(peer_group.AfiSafis.AfiSafi, &peer_group_afisafi)
	bgp.PeerGroups.PeerGroup = append(bgp.PeerGroups.PeerGroup, &peer_group)
}

func configDeletePolicy(bgp *ysanity_bgp.Bgp, policy string) {
	peer_group := ysanity_bgp.Bgp_PeerGroups_PeerGroup{}
	peer_group.PeerGroupName = "EBGP"

	peer_group_afisafi := ysanity_bgp.Bgp_PeerGroups_PeerGroup_AfiSafis_AfiSafi{}
	peer_group_afisafi.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}

	// Delete import policy in the leaf-list
	peer_group_afisafi.ApplyPolicy.Config.ImportPolicy =
		append(peer_group_afisafi.ApplyPolicy.Config.ImportPolicy,
		       types.LeafData{Value: policy, Filter: yfilter.Delete})

	peer_group.AfiSafis.AfiSafi = append(peer_group.AfiSafis.AfiSafi, &peer_group_afisafi)
	bgp.PeerGroups.PeerGroup = append(bgp.PeerGroups.PeerGroup, &peer_group)
	fmt.Printf("Removing routing import policy '%s'\n", policy)
}

func configRoutingPolicies(routingPolicy *ysanity_rp.RoutingPolicy) {
	policy_def1 := ysanity_rp.RoutingPolicy_PolicyDefinitions_PolicyDefinition{Name: "POLICY1"}
	policy_def3 := ysanity_rp.RoutingPolicy_PolicyDefinitions_PolicyDefinition{Name: "POLICY3"}
	policy_def1.Config.Name = "POLICY1"
	policy_def3.Config.Name = "POLICY3"
	routingPolicy.PolicyDefinitions.PolicyDefinition =
		append(routingPolicy.PolicyDefinitions.PolicyDefinition, &policy_def1)
	routingPolicy.PolicyDefinitions.PolicyDefinition =
		append(routingPolicy.PolicyDefinitions.PolicyDefinition, &policy_def3)
}

func readRoutingPolicies(provider *providers.NetconfServiceProvider) {
	bgp := ysanity_bgp.Bgp{}
	bgp.PeerGroups.YFilter = yfilter.Read

	crud := services.CrudService{}
	bgpEntity := crud.ReadConfig(provider, &bgp)
	bgpRead := bgpEntity.(*ysanity_bgp.Bgp)
	_, peerGroupEntity := ylist.Get(bgpRead.PeerGroups.PeerGroup, "EBGP")
	if peerGroupEntity != nil {
		peerGroup := peerGroupEntity.(*ysanity_bgp.Bgp_PeerGroups_PeerGroup)
		// fmt.Printf("AfiSafi keys: %v\n", ylist.Keys(peerGroup.AfiSafis.AfiSafi))
		_, afisafiEntity := ylist.Get(peerGroup.AfiSafis.AfiSafi, "openconfig-bgp-types:IPV6_UNICAST")
		if afisafiEntity != nil {
			afisafi := afisafiEntity.(*ysanity_bgp.Bgp_PeerGroups_PeerGroup_AfiSafis_AfiSafi)
			policyConfig := afisafi.ApplyPolicy.Config
			fmt.Printf("Currently configured import policies (%d):\n", len(policyConfig.ImportPolicy))
			for _, policy := range policyConfig.ImportPolicy {
				fmt.Printf("    %s\n", policy.(string))
			}
		}
	}
}

func main() {
// 	ydk.EnableLogging(ydk.Info)

	// Connect to the device
	var provider = providers.NetconfServiceProvider{
				Address:  "127.0.0.1",
				Username: "admin",
				Password: "admin",
				Port:     12022}
	provider.Connect()
	var CRUD = services.CrudService{}

	// Build routing policies list
	routingPolicy := ysanity_rp.RoutingPolicy{}
	configRoutingPolicies(&routingPolicy)
	CRUD.Create(&provider, &routingPolicy)

	// Build BGP configuration
	bgp := ysanity_bgp.Bgp{}
	configBgp(&bgp)
	CRUD.Create(&provider, &bgp)
	readRoutingPolicies(&provider)

	// Delete POLICY1 from import policies leaf-list
	bgpDelete := ysanity_bgp.Bgp{}
	configDeletePolicy(&bgpDelete, "POLICY1")
	CRUD.Update(&provider, &bgpDelete)
	readRoutingPolicies(&provider)

	// Delete POLICY3 from import policies leaf-list. The last must be removed.
	bgpDelete = ysanity_bgp.Bgp{}
	configDeletePolicy(&bgpDelete, "POLICY3")
	CRUD.Update(&provider, &bgpDelete)
	readRoutingPolicies(&provider)

	// Delete BGP configuration
	CRUD.Delete(&provider, &ysanity_bgp.Bgp{})

	provider.Disconnect()
}
