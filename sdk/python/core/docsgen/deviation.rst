Deviation
=========

Overview
---------

Not all devices faithfully support features defined in the standard yang module. For a particular device, it could support only part of features or the feature it supported varies from the standard module. In YANG, we use `deviation statement <https://tools.ietf.org/html/rfc6020#section-7.18.3>`_ to specify it.
For example, in cisco-xr-bgp-deviations.yang,

.. code-block:: c

  deviation /bgp:bgp/bgp:global/bgp:afi-safis/bgp:afi-safi/bgp:apply-policy {
    deviate not-supported;
  }

apply-policy is not supported.


How to use deviation with YDK
------------------------------

When using YDK to program a device which has some unsupported features, YDK will raise a validation error before sending payload to device. In the example below, the device has published a deviation ``cisco-xr-bgp-deviations.yang`` on a standard bgp module.

YDK will raise an error if an app tries to assign a value to this feature:

.. code-block:: python

        from ydk.models.bgp import bgp
        from ydk.models.routing.routing_policy import DefaultPolicyTypeEnum, RoutingPolicy

        bgp_cfg = bgp.Bgp()
        ipv4_afsf = bgp_cfg.global_.afi_safis.AfiSafi()
        ipv4_afsf.afi_safi_name = 'ipv4-unicast'
        ipv4_afsf.config.afi_safi_name = 'ipv4-unicast'
        ipv4_afsf.config.enabled = True
        ipv4_afsf.apply_policy.config.default_export_policy = \
            DefaultPolicyTypeEnum.ACCEPT_ROUTE
        bgp_cfg.global_.afi_safis.afi_safi.append(ipv4_afsf)

YDK will raise YPYDataValidationError when processing the above python object.


Behind the Scenes
------------------

YDK use pyang to compile yang module to intermediate tree structured python objects(pyang statements), and feed those objects to YDK’s API module to generate python objects(YDK packages) suitable for language binding.
If, compiled with deviation module, pyang will automatically trim the unsupported subtrees, and change the deviated feature. With subtree being trimmed, the original API will not being generated. However, we also need the deviation meta information at runtime. 

Pyang’s infrastructure provide way to insert additional phases between basic phases. So what YDK does is to capture deviation information before trim for deviation happens, and restore the information after that.

With those deviation messages being captured in pyang statements object, YDK’s API module could use that information to print original module along with a central point for deviation.

Before send payload to the device, YDK will get active deviation from the device it is talking to, from ncclient, and use this information to trim/validate YDK-py object accordingly.


