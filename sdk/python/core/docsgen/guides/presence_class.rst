.. _presence-class:

Presence Classes
==================
According to `RFC 6020 <https://tools.ietf.org/html/rfc6020#section-7.5.1>`_, YANG supports two styles of containers, one for organizing hierarchy, another for representing configuration data. For instance the existence of presence container ``ssh`` indicates the capability to log in to the device using ssh. Let's consider a presence node ``match-community-set`` in `openconfig-bgp-policy.yang <https://github.com/YangModels/yang/blob/96883adbf612605f02271523d7eaa731ded46b61/vendor/cisco/xr/621/openconfig-bgp-policy.yang#L126>`_. This node is generated as YDK class shown below:

.. code-block:: python

    class BgpConditions(Entity):
        """
        ...

        .. attribute:: match_community_set

        Match a referenced community\-set according to the logic defined in the match\-set\-options leaf
        **type**\:   :py:class:`MatchCommunitySet <ydk.models.openconfig.openconfig_routing_policy.RoutingPolicy.PolicyDefinitions.PolicyDefinition.Statements.Statement.Conditions.BgpConditions.MatchCommunitySet>`

        **presence node**\: True

        ...

        """

        ...

        def __init__(self):
            super(RoutingPolicy.PolicyDefinitions.PolicyDefinition.Statements.Statement.Conditions.BgpConditions, self).__init__()

            self.yang_name = "bgp-conditions"
            self.yang_parent_name = "conditions"

            ...

            self.match_community_set = None

            ...

Since the existence of container ``match-community-set`` itself represents configuration data, YDK does not instantiate an instance of class :py:class:`MatchCommunitySet<ydk.models.openconfig.openconfig_routing_policy.RoutingPolicy.PolicyDefinitions.PolicyDefinition.Statements.Statement.Conditions.BgpConditions.MatchCommunitySet>` and assign it to ``self.match_community_set``. The user need to manually assign it.
