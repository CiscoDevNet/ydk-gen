What are deviations?
====================

.. contents:: Table of Contents

Overview
---------

Not all devices faithfully support features defined in standard yang models. For a particular device, it could support only part of features or the feature it supported varies from the standard module. The YANG language defines the `deviation statement <https://tools.ietf.org/html/rfc6020#section-7.18.3>`_ to indicate such a thing.
For example, using a deviation statement in `cisco-xr-openconfig-telemetry-deviations.yang <https://github.com/YangModels/yang/blob/master/vendor/cisco/xr/631/cisco-xr-openconfig-telemetry-deviations.yang#L73>`_, the netconf server can indicate that the :py:class:`openconfig_telemetry.TelemetrySystem.Subscriptions.Dynamic.Subscription<ydk.models.openconfig.openconfig_telemetry.TelemetrySystem.Subscriptions.Dynamic.Subscription>` container of the ``openconfig_telemetry`` model is not supported.

.. code-block:: c

  deviation /oc-telemetry:telemetry-system/oc-telemetry:subscriptions/oc-telemetry:dynamic/oc-telemetry:subscription {
    deviate not-supported;
  }


How to use deviation with YDK
------------------------------

When using YDK to program a device which has some unsupported features, YDK will raise error **before** sending payload to device. For instance, if the device advertises via a deviation that it does not support the ``subscription`` node as shown above, an error will be raised.

We can try to configure a dynamic subscription with the below app.

.. code-block:: python
  :linenos:

  from ydk.models.openconfig.openconfig_telemetry import TelemetrySystem

  telemetry = TelemetrySystem()
  s = telemetry.subscriptions.dynamic.Subscription()
  s.subscription_id = 123
  s.state.subscription_id = 123
  telemetry.subscriptions.dynamic.subscription.append(s)

  # Call the CRUD create on the top-level telemetry object
  # (assuming you have already instantiated the service and provider)
  crud.create(provider, telemetry)

The above app results in the below errors logged and exception being raised because of the deviation which is active on the ``subscription`` node.

.. code-block:: sh
  :linenos:

  2017-11-08 21:26:58,543 - ydk - ERROR - Data is invalid according to the yang model. Error details: Schema node not found. Path: 'subscription'
  2017-11-08 21:26:58,543 - ydk - ERROR - Invalid path: subscription[subscription-id='123']

    File "/Users/lib/python3.6/site-packages/ydk/errors/error_handler.py", line 112, in helper
      return func(self, provider, entity, *args, **kwargs)
    File "/Users/lib/python3.6/site-packages/ydk/services/crud_service.py", line 30, in create
      return self._crud.create(provider, entity)
    File "/Library/Frameworks/Python.framework/Versions/3.6/lib/python3.6/contextlib.py", line 99, in __exit__
      self.gen.throw(type, value, traceback)
    File "/Users/lib/python3.6/site-packages/ydk/errors/error_handler.py", line 82, in handle_runtime_error
      _raise(_exc)
    File "/Users/lib/python3.6/site-packages/ydk/errors/error_handler.py", line 54, in _raise
      exec("raise exc from None")
    File "<string>", line 1, in <module>
  ydk.errors.YModelError:  Invalid path: subscription[subscription-id='123'] : Schema node not found.. Path: subscription
