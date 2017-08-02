.. _compatibility:

Backward Compatibility Notes
=============================

.. contents:: Table of Contents

When installing and using the ``0.6.0`` and newer releases of ``YDK-Py``, please note the below issues with backward compatibility.

Installation
------------

When installing ``YDK-Py``, there is a new system requirement which needs to be installed. This is the ``libydk`` library, which is available on the DevHub website for various OS platforms. Please refer to the `system requirements <http://ydk.cisco.com/py/docs/getting_started.html#system-requirements>`_ for details.

Windows Support
---------------

From release ``0.6.0`` onwards, YDK no longer is supported on the Windows platform. We hope to add back support for this platform in the future.

API Changes
-----------

1. :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` no longer has the ``close()`` method. There is no need to explicitly close the provider as it will be automatically cleaned up when the object goes out of scope.
2. :py:class:`YFilter<ydk.filters.YFilter>` has replaced the functionality of the ``READ`` and ``DELETE`` objects
3. When using YDK's :ref:`howto-logging`, the suggested level to be used is ``INFO``
4. The type names of ``enumerations`` and ``identities`` no longer have ``Enum`` or ``Identity`` in their names. For example, the ``InterfaceTypeIdentity`` identity in ``ydk.models.ietf.ietf_interfaces`` is now renamed to just ``InterfaceType``.
