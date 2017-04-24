Deviation
=========

.. contents:: Table of Contents

Overview
---------

Not all devices faithfully support features defined in the standard yang module. For a particular device, it could support only part of features or the feature it supported varies from the standard module. In YANG, we use `deviation statement <https://tools.ietf.org/html/rfc6020#section-7.18.3>`_ to specify it.
For example, in `cisco-xr-openconfig-bgp-deviations.yang <https://github.com/YangModels/yang/blob/74bf74f94ffe38eeafd68bd4d14eb6c4ae8f3ad4/vendor/cisco/xr/621/cisco-xr-openconfig-bgp-deviations.yang#L110>`_,

.. code-block:: c

  deviation /bgp:bgp/bgp:global/bgp:afi-safis/bgp:afi-safi/bgp:apply-policy {
    deviate not-supported;
  }

``apply-policy`` is not supported.


How to use deviation with YDK
------------------------------

When using YDK to program a device which has some unsupported features, YDK will raise error **before** sending payload to device. For instance, if the device has published a deviation model not support the ``apply-policy`` node shown above, error will be raised:

.. code-block:: python

    >>> from ydk.services import CRUDService
    >>> from ydk.providers import NetconfServiceProvider
    >>> from ydk.models.openconfig.openconfig_bgp import Bgp
    >>> from ydk.models.openconfig.openconfig_bgp_types import Ipv4Unicast
    >>> from ydk.models.openconfig.openconfig_routing_policy import DefaultPolicyTypeEnum, RoutingPolicy
    >>> provider = NetconfServiceProvider(address='127.0.0.1', username='admin', password='admin')
    >>> crud = CRUDService()
    >>> bgp_cfg = Bgp()
    >>> ipv4_afsf = bgp_cfg.global_.afi_safis.AfiSafi()
    >>> ipv4_afsf.afi_safi_name = Ipv4Unicast()
    >>> ipv4_afsf.config.afi_safi_name = Ipv4Unicast()
    >>> ipv4_afsf.config.enabled = True
    >>> ipv4_afsf.apply_policy.config.default_export_policy = DefaultPolicyTypeEnum.ACCEPT_ROUTE
    >>> bgp_cfg.global_.afi_safis.afi_safi.append(ipv4_afsf)
    >>> crud.create(provider, bgp_cfg)
    Traceback (most recent call last):
      File "<stdin>", line 1, in <module>
      File "/Users/home/.virtualenvs/36/lib/python3.6/site-packages/ydk/errors/error_handler.py", line 107, in helper
        return func(self, provider, entity, *args)
      File "/Users/home/.virtualenvs/36/lib/python3.6/site-packages/ydk/services/crud_service.py", line 32, in create
        return self._crud.create(provider, entity)
      File "/usr/local/Cellar/python3/3.6.0_1/Frameworks/Python.framework/Versions/3.6/lib/python3.6/contextlib.py", line 100, in __exit__
        self.gen.throw(type, value, traceback)
      File "/Users/home/.virtualenvs/36/lib/python3.6/site-packages/ydk/errors/error_handler.py", line 77, in handle_runtime_error
        _raise(_exc)
      File "/Users/home/.virtualenvs/36/lib/python3.6/site-packages/ydk/errors/error_handler.py", line 49, in _raise
        exec("raise exc from None")
      File "<string>", line 1, in <module>
    ydk.errors.YPYModelError:  Invalid path: apply-policy : Schema node not found.. Path: apply-policy
