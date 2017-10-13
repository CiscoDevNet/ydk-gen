/*  ----------------------------------------------------------------
 Copyright 2016 Cisco Systems

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
#include <iostream>
#include <spdlog/spdlog.h>
#include "ydk/netconf_provider.hpp"
#include "ydk/crud_service.hpp"
#include "ydk_openconfig/openconfig_bgp.hpp"
#include "ydk_openconfig/openconfig_bgp_policy.hpp"
#include "ydk_openconfig/openconfig_bgp_types.hpp"
#include "ydk_openconfig/openconfig_mpls.hpp"
#include "ydk_openconfig/openconfig_routing_policy.hpp"
#include "ydk_openconfig/openconfig_policy_types.hpp"

#include "args_parser.h"

using namespace ydk;
using namespace std;
using namespace openconfig;

static std::shared_ptr<Entity> s()
{/*
//    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_aspathlength = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::AsPathLength>();
//    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchprefixset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchPrefixSet>();
    auto routingpolicy_policydefinitions_policydefinition = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition>();
    auto routingpolicy = std::make_shared<openconfig_routing_policy::RoutingPolicy>();
//    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_communitycount = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::CommunityCount>();
    auto routingpolicy_policydefinitions_policydefinition_statements_statement = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement>();
//    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchtagset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchTagSet>();
//    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchaspathset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchAsPathSet>();
//    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchcommunityset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchCommunitySet>();
//    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchneighborset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchNeighborSet>();
//    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchextcommunityset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchExtCommunitySet>();
    routingpolicy_policydefinitions_policydefinition->parent = routingpolicy->policy_definitions.get();
    routingpolicy->policy_definitions->policy_definition.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition));
    routingpolicy->policy_definitions->policy_definition[0]->name = R"(7)";
    routingpolicy_policydefinitions_policydefinition_statements_statement->parent = routingpolicy->policy_definitions->policy_definition[0]->statements.get();
    routingpolicy->policy_definitions->policy_definition[0]->statements->statement.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition_statements_statement));
//    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->as_path_length = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_aspathlength);
//    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->community_count = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_communitycount);
    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->community_count->config->operator_ = openconfig_policy_types::ATTRIBUTEGE();
    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->community_count->config->value_ = 19;
//    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_as_path_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchaspathset);
//    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_community_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchcommunityset);
//    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_ext_community_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchextcommunityset);
//    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_neighbor_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchneighborset);
//    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_prefix_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchprefixset);
//    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_tag_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchtagset);
    routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->name = R"(6)";
    return move(routingpolicy);*/
    
//        auto routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setaspathprepend = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Actions::BgpActions::SetAsPathPrepend>();
//        auto routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setcommunity = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Actions::BgpActions::SetCommunity>();
  /*      auto routingpolicy_policydefinitions_policydefinition = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition>();
        auto routingpolicy = std::make_shared<openconfig_routing_policy::RoutingPolicy>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement>();
//        auto routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setextcommunity = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Actions::BgpActions::SetExtCommunity>();
        auto routingpolicy_definedsets_bgpdefinedsets_communitysets_communityset = std::make_shared<openconfig_routing_policy::RoutingPolicy::DefinedSets::BgpDefinedSets::CommunitySets::CommunitySet>();
        routingpolicy_definedsets_bgpdefinedsets_communitysets_communityset->parent = routingpolicy->defined_sets->bgp_defined_sets->community_sets.get();
        routingpolicy->defined_sets->bgp_defined_sets->community_sets->community_set.emplace_back(std::move(routingpolicy_definedsets_bgpdefinedsets_communitysets_communityset));
        routingpolicy->defined_sets->bgp_defined_sets->community_sets->community_set[0]->community_set_name = R"(p)";
        routingpolicy_policydefinitions_policydefinition->parent = routingpolicy->policy_definitions.get();
        routingpolicy->policy_definitions->policy_definition.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition));
        routingpolicy->policy_definitions->policy_definition[0]->name = R"(z)";
        routingpolicy_policydefinitions_policydefinition_statements_statement->parent = routingpolicy->policy_definitions->policy_definition[0]->statements.get();
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition_statements_statement));
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_as_path_prepend = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setaspathprepend);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_community = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setcommunity);
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_community->config->options = openconfig_bgp_policy::BgpSetCommunityOptionType::REMOVE;
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_ext_community = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setextcommunity);
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->name = R"(5)";
//        auto routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setcommunity_communities = 4;
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_community->reference->config->community_set_ref = routingpolicy->defined_sets->bgp_defined_sets->community_sets->community_set[0]->community_set_name.get();
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_community->inline_->config->communities.append("4:5");
        return routingpolicy;*/
      /*
//        auto routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setaspathprepend = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Actions::BgpActions::SetAsPathPrepend>();
//        auto routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setcommunity = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Actions::BgpActions::SetCommunity>();
        auto routingpolicy_policydefinitions_policydefinition = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition>();
        auto routingpolicy = std::make_shared<openconfig_routing_policy::RoutingPolicy>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement>();
//        auto routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setextcommunity = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Actions::BgpActions::SetExtCommunity>();
        auto routingpolicy_definedsets_bgpdefinedsets_extcommunitysets_extcommunityset = std::make_shared<openconfig_routing_policy::RoutingPolicy::DefinedSets::BgpDefinedSets::ExtCommunitySets::ExtCommunitySet>();
        routingpolicy_definedsets_bgpdefinedsets_extcommunitysets_extcommunityset->parent = routingpolicy->defined_sets->bgp_defined_sets->ext_community_sets.get();
        routingpolicy->defined_sets->bgp_defined_sets->ext_community_sets->ext_community_set.emplace_back(std::move(routingpolicy_definedsets_bgpdefinedsets_extcommunitysets_extcommunityset));
        routingpolicy->defined_sets->bgp_defined_sets->ext_community_sets->ext_community_set[0]->ext_community_set_name = R"(9)";
        routingpolicy_policydefinitions_policydefinition->parent = routingpolicy->policy_definitions.get();
        routingpolicy->policy_definitions->policy_definition.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition));
        routingpolicy->policy_definitions->policy_definition[0]->name = R"(s)";
        routingpolicy_policydefinitions_policydefinition_statements_statement->parent = routingpolicy->policy_definitions->policy_definition[0]->statements.get();
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition_statements_statement));
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_as_path_prepend = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setaspathprepend);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_community = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setcommunity);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_ext_community = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_actions_bgpactions_setextcommunity);
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_ext_community->config->options = openconfig_bgp_policy::BgpSetCommunityOptionType::REPLACE;
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->name = R"(9)";
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_ext_community->reference->config->ext_community_set_ref = routingpolicy->defined_sets->bgp_defined_sets->ext_community_sets->ext_community_set[0]->ext_community_set_name.get();
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->actions->bgp_actions->set_ext_community->inline_->config->communities.append("8.252.233.5:17486");
        return routingpolicy;*/

    /*    auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_aspathlength = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::AsPathLength>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchprefixset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchPrefixSet>();
        auto routingpolicy_policydefinitions_policydefinition = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition>();
        auto routingpolicy = std::make_shared<openconfig_routing_policy::RoutingPolicy>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_communitycount = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::CommunityCount>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchtagset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchTagSet>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchaspathset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchAsPathSet>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchcommunityset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchCommunitySet>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchneighborset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchNeighborSet>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchextcommunityset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchExtCommunitySet>();
        routingpolicy_policydefinitions_policydefinition->parent = routingpolicy->policy_definitions.get();
        routingpolicy->policy_definitions->policy_definition.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition));
        routingpolicy->policy_definitions->policy_definition[0]->name = R"(q)";
        routingpolicy_policydefinitions_policydefinition_statements_statement->parent = routingpolicy->policy_definitions->policy_definition[0]->statements.get();
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition_statements_statement));
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->as_path_length = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_aspathlength);
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->as_path_length->config->operator_ = openconfig_policy_types::ATTRIBUTELE();
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->as_path_length->config->value_ = 24;
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->community_count = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_communitycount);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_as_path_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchaspathset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_community_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchcommunityset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_ext_community_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchextcommunityset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_neighbor_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchneighborset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_prefix_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchprefixset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_tag_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchtagset);
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->name = R"(4)";
        return routingpolicy;*/
        
     /*          auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_aspathlength = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::AsPathLength>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchprefixset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchPrefixSet>();
        auto routingpolicy_policydefinitions_policydefinition = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition>();
        auto routingpolicy = std::make_shared<openconfig_routing_policy::RoutingPolicy>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_communitycount = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::CommunityCount>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchtagset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchTagSet>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchaspathset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchAsPathSet>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchcommunityset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchCommunitySet>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchneighborset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::MatchNeighborSet>();
        auto routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchextcommunityset = std::make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition::Statements::Statement::Conditions::BgpConditions::MatchExtCommunitySet>();
        routingpolicy_policydefinitions_policydefinition->parent = routingpolicy->policy_definitions.get();
        routingpolicy->policy_definitions->policy_definition.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition));
        routingpolicy->policy_definitions->policy_definition[0]->name = R"(7)";
        routingpolicy_policydefinitions_policydefinition_statements_statement->parent = routingpolicy->policy_definitions->policy_definition[0]->statements.get();
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement.emplace_back(std::move(routingpolicy_policydefinitions_policydefinition_statements_statement));
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->as_path_length = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_aspathlength);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->community_count = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_communitycount);
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->community_count->config->operator_ = openconfig_policy_types::ATTRIBUTEGE();
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->community_count->config->value_ = 19;
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_as_path_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchaspathset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_community_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchcommunityset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->bgp_conditions->match_ext_community_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_bgpconditions_matchextcommunityset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_neighbor_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchneighborset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_prefix_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchprefixset);
//        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->conditions->match_tag_set = std::move(routingpolicy_policydefinitions_policydefinition_statements_statement_conditions_matchtagset);
        routingpolicy->policy_definitions->policy_definition[0]->statements->statement[0]->name = R"(6)";
        return routingpolicy;*/

    auto mpls = std::make_shared<openconfig_mpls::Mpls>();
    mpls->lsps->unconstrained_path->path_setup_protocol->ldp = std::make_shared<openconfig_mpls::Mpls::Lsps::UnconstrainedPath::PathSetupProtocol::Ldp>();
    mpls->lsps->unconstrained_path->path_setup_protocol->ldp->tunnel->p2p_lsp->fec_address.append("10.0.0.1/32");
    return mpls;
}

void config_bgp(openconfig_bgp::Bgp* bgp)
{
    // Set the Global AS
    bgp->global->config->as = 65001;
    bgp->global->config->router_id = "1.2.3.4";

    auto afi_safi = make_shared<openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
    afi_safi->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
    afi_safi->config->afi_safi_name = openconfig_bgp_types::L3VPNIPV4UNICAST();
    afi_safi->config->enabled = false;
    afi_safi->parent = bgp->global->afi_safis.get();
    bgp->global->afi_safis->afi_safi.push_back(move(afi_safi));

    auto neighbor = make_shared<openconfig_bgp::Bgp::Neighbors::Neighbor>();
    neighbor->neighbor_address = "6.7.8.9";
    neighbor->config->neighbor_address = "6.7.8.9";
    neighbor->config->peer_as = 65001;
    neighbor->config->local_as = 65001;
    neighbor->config->peer_group = "IBGP";
    neighbor->config->peer_type = "INTERNAL";
//  neighbor->config->remove_private_as =
//  openconfig_bgp_types::PRIVATEASREMOVEALL();
    neighbor->parent = bgp->neighbors.get();
    bgp->neighbors->neighbor.push_back(move(neighbor));

    auto peer_group = make_shared<openconfig_bgp::Bgp::PeerGroups::PeerGroup>();
    peer_group->peer_group_name = "IBGP";
    peer_group->config->peer_group_name = "IBGP";
    peer_group->config->auth_password = "password";
    peer_group->config->description = "test description";
    peer_group->config->peer_as = 65001;
    peer_group->config->local_as = 65001;
    peer_group->config->peer_type = "INTERNAL";
    //peer_group->config->remove_private_as = openconfig_bgp_types::REMOVEPRIVATEASOPTION();
    peer_group->parent = bgp->peer_groups.get();
    bgp->peer_groups->peer_group.push_back(move(peer_group));
}

void config_routing_policy(openconfig_routing_policy::RoutingPolicy* routing_policy)
{
    auto policy_definition = make_shared<openconfig_routing_policy::RoutingPolicy::PolicyDefinitions::PolicyDefinition>();
    policy_definition->name = "PASS-ALL";
    policy_definition->parent = routing_policy->policy_definitions.get();
    routing_policy->policy_definitions->policy_definition.push_back(move(policy_definition));
}

int main(int argc, char* argv[])
{
    vector<string> args = parse_args(argc, argv);
    if(args.empty()) return 1;
    string host, username, password;
    int port;
    username = args[0]; password = args[1]; host = args[2]; port = stoi(args[3]);
    bool verbose=(args[4]=="--verbose");
    if(verbose)
    {
        auto logger = spdlog::stdout_color_mt("ydk");
        logger->set_level(spdlog::level::info);
    }
    NetconfServiceProvider provider{host, username, password, port};
    CrudService crud{};

//    auto bgp = make_shared<openconfig_bgp::Bgp>();
//    auto routing_policy = make_shared<openconfig_routing_policy::RoutingPolicy>();
//    config_bgp(bgp.get());
//    config_routing_policy(routing_policy.get());
//
//    bool reply = crud.create(provider, *bgp );
//
//    if(reply) cout << "Create yfilter success" << endl << endl; else cout << "Operation failed" << endl << endl;

    auto t = s();
    crud.create(provider, *t);

}
