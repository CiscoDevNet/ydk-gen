Using Types
***********
This document will explain and give examples to using the ydk types.
Types explained will include:

- Empty.
- Decimal64.
- FixedBitsDict.
- YList.

Example use of Empty type
=========================

- The leaf being configured (accept_route) under the module ydk.models.openconfig.openconfig_routing_policy:

**accept_route:** accepts the route into the routing table.
**type:** Empty

.. code-block:: python

    from ydk.models.openconfig.openconfig_routing_policy import RoutingPolicy

    # configure policy definition
    routing_policy = RoutingPolicy()
    policy_definition = routing_policy.policy_definitions.PolicyDefinition()
    policy_definition.name = "POLICY2"
    # community-set statement
    statement = policy_definition.statements.Statement()
    statement.actions.accept_route = Empty() # accept_route is of Empty type

Example use of Decimal64 type
=============================

- The leaf being configured (restart_timer) under the ydk.models.openconfig_bgp.bgp module:
**restart_timer:** Time interval in seconds after which the BGP session is re-established after being torn down due to exceeding the max-prefix limit.
**type:** Decimal64

.. code-block:: python

    from ydk.models.openconfig.openconfig_bgp import Bgp

    config = Bgp.Neighbors.Neighbor.AfiSafis.AfiSafi.Ipv4LabelledUnicast.PrefixLimit.Config()
    config.restart_timer = Decimal64('3.343') # restart_timer is of Decimal64 type

Example use of FixedBitsDict type
=================================

- The leaf being configured (restart_timer) under the ydk.models.ietf.ietf_netconf_acm module:
**access_operations:** Access operations associated with this rule.  This leaf matches if it has the value '*' or if the bit corresponding to the requested operation is set.
**type:** str

.. code-block:: python

    from ydk.models.ietf.ietf_netconf_acm import Nacm

    rule_list   = Nacm.RuleList()
    rule        = rule_list.Rule()
    rule.parent = rule_list
    rule.rule_list.rule.access_operations['read'] = True # access_operations is of bits type

Example use of YList type
=========================

- The node being configured is afi_safi under the ydk.models.openconfig_bgp.bgp module:

.. code-block:: python

    from ydk.models.openconfig.openconfig_bgp import Bgp

    bgp = Bgp()
    afi_safi = bgp.global_.afi_safis.AfiSafi() # afi_safi is of YList type
    afi_safi.afi_safi_name = oc_bgp_types.Ipv4UnicastIdentity()
    afi_safi.config.afi_safi_name = oc_bgp_types.Ipv4UnicastIdentity()
    afi_safi.config.enabled = True
    bgp.global_.afi_safis.afi_safi.append(afi_safi)

Example use of YLeafList type
=============================

- The leaf being configured (ipv4_dscp) under the ydk.models.cisco_ios_xr.Cisco_IOS_XR_asr9k_policymgr_cfg module:
**ipv4_dscp:** An leaflist of Match IPv4 DSCP.
**type:** YLeafList

.. code-block:: python

    from ydk.models.asr9k.Cisco_IOS_XR_asr9k_policymgr_cfg import PolicyManager

    match = PolicyManager.ClassMaps.ClassMap.Match()
    match.ipv4_dscp.extend(['15', '16', '17', '18', '19'])
    even_elements = match.ipv4_dscp[::2]

    # Note: YLeafList is associative array, attempt to add duplicated element will raise Exception.
    match.ipv4_dscp.append('15')
    # YPYDataValidationError will be raised.
