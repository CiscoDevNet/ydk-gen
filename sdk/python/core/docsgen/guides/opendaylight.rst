How do I use OpenDaylight with YDK?
===================================
.. contents::

YDK makes it easy to interact with OpenDaylight programmatically using the YANG model APIs.

Applications can be written using the Python model API in conjunction with a service and a provider.

Writing the app
---------------

In this example, we set some BGP configuration using the Cisco IOS XR model, the :py:class:`CRUD (Create/Read/Update/Delete) service<ydk.services.CRUDService>` and the :py:class:`OpenDaylightServiceProvider<ydk.providers.OpendaylightServiceProvider>`. The example in this document is a simplified version of the more complete sample that is available in ``core/samples/bgp_xr_opendaylight.py``. Assuming you have performed the ``core`` and ``cisco-ios-xr`` bundle installations first(see :ref:`howto-install`), the more complete sample can be run with the below steps:

.. code-block:: sh

  ydk-py$ cd core/samples
  samples$ ./bgp_xr_opendaylight.py http://<username>:<password>@<host-address>:<port>

What happens underneath
-----------------------
YDK performs the below actions when running this application:

 1. Establish a session with the OpenDaylight instance and fetch the details of the nodes mounted on the southbound
 2. Encode Python data objects to the protocol format (e.g. restconf JSON payload)
 3. For a chosen node on the southbound, perform transport operation with the device and collect the response (e.g. restconf reply)
 4. Decode response as Python object and return the result to app
 5. Raise Python exceptions for any errors that occurred

Import libraries
----------------
In our example YDK application, first, let us import the necessary libraries

.. code-block:: python
    :linenos:

    import os
    import sys
    from argparse import ArgumentParser
    if sys.version_info > (3,):
        from urllib.parse import urlparse
    else:
        from urlparse import urlparse

    from ydk.types import Empty
    from ydk.services import CRUDService
    from ydk.providers import OpenDaylightServiceProvider
    from ydk.errors import YError
    from ydk.types import EncodingFormat
    from ydk.path import Repository
    from ydk.models.cisco_ios_xr import Cisco_IOS_XR_ipv4_bgp_cfg as xr_bgp
    from ydk.models.cisco_ios_xr import Cisco_IOS_XR_ipv4_bgp_datatypes as xr_bgp_types

OpenDaylight service provider
-----------------------------
The first step in any application is to create a service provider instance. In this case, the OpenDaylight service provider is responsible for mapping between the CRUD service API and the underlying manageability protocol (Restconf).

We first instantiate a :py:class:`Repository<ydk.path.Repository>` using the location of the schema cache of the OpenDaylight instance. We instantiate an instance of the service provider that can communicate using Restconf with an OpenDaylight instance running at host address: ``127.0.0.1`` and port: ``8181``

.. code-block:: python
    :linenos:
    :lineno-start: 17

    repo = Repository("/Users/home/distribution-karaf-0.5.2-Boron-SR2/cache/schema") # In this case, we have a ODL boron instance with this schema cache location
    odl_provider = OpenDaylightServiceProvider(repo, "127.0.0.1", "admin", "admin", 8181, EncodingFormat.XML)


Using the model APIs
--------------------
After establishing the connection, let's instantiate the entities and set some data. Now, create a Cisco IOS XR :py:class:`Bgp<ydk.models.cisco_ios_xr.Cisco_IOS_XR_ipv4_bgp_cfg.Bgp>` configuration object and set the attributes

.. code-block:: python
    :linenos:
    :lineno-start: 19

    # Create BGP object
    bgp = xr_bgp.Bgp()

    # BGP instance
    instance = bgp.Instance()
    instance.instance_name = "test"
    instance_as = instance.InstanceAs()
    instance_as.as_ = 65001;
    four_byte_as = instance_as.FourByteAs()
    four_byte_as.as_ = 65001;
    four_byte_as.bgp_running = Empty();

    # global address family
    global_af = four_byte_as.DefaultVrf.Global_.GlobalAfs.GlobalAf()
    global_af.af_name = xr_bgp_types.BgpAddressFamilyEnum.ipv4_unicast;
    global_af.enable = Empty();
    four_byte_as.default_vrf.global_.global_afs.global_af.append(global_af)

    # add the instance to the parent BGP object
    instance_as.four_byte_as.append(four_byte_as)
    instance.instance_as.append(instance_as)
    bgp.instance.append(instance)


Invoking the CRUD Service
-------------------------
The CRUD service provides methods to create, read, update and delete entities on a device making use of the session provided by a service provider.  In order to use the CRUD service, we need to instantiate the :py:class:`CRUDService<ydk.services.CRUDService>` class

.. code-block:: python
    :linenos:
    :lineno-start: 41

    crud_service = CRUDService()

At this point we can explore the southbound device node-IDs using the function call: :py:meth:`get_node_ids<ydk.providers.OpendaylightServiceProvider.get_node_ids>`. Let us assume there is a XR device mounted with the node ID ``xr``. We can obtain the :py:class:`ServiceProvider<ydk.path.ServiceProvider>` instance corresponding to this node using the : :py:meth:`get_node_provider<ydk.providers.OpendaylightServiceProvider.get_node_provider>`.

Finally, we invoke the create method of the :py:class:`CRUDService<ydk.services.CRUDService>` class passing in the service provider instance and our entity, ``bgp``

.. code-block:: python
    :linenos:
    :lineno-start: 42

    provider = odl_provider.get_node_provider('xr')
    crud_service.create(provider, bgp)


Note if there were any errors the above API will raise an exception with the base type :py:class:`YError<ydk.errors.YError>`

Logging
-------
YDK uses common Python logging. See :ref:`howto-logging`.
