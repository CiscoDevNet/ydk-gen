Introduction
=================
.. contents:: Table of Contents

YDK consists of two main components: core library, which consists of services and providers, and python model API, which are APIs generated based on YANG models and packaged as bundles.

Core library consists of the below:

 * **Service:** Provides simple API interface to be used with the bindings and providers
 * **ServiceProvider:** Provides concrete implementation that abstracts underlying protocol details (e.g. :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>`, which is based on the NETCONF protocol) 

Applications can be written using the python model API in conjunction with a service and a provider.

Writing an app
---------------

In this example, we set some BGP configuration using the OpenConfig model, the CRUD (Create/Read/Update/Delete) service and the NETCONF service provider. The example in this document is a simplified version of the more complete sample that is available in ``samples/bgp.py``. That more complete sample can be run with the below steps::

    (ydk-py)ydk-py$ cd core/samples
    (ydk-py)samples$ ./bgp.py -h
    Usage: bgp.py [-h | --help] [options]

    Options:
    -h, --help            show this help message and exit
    -v VERSION, --version=VERSION
                        force NETCONF version 1.0 or 1.1
    -u USERNAME, --user=USERNAME
    -p PASSWORD, --password=PASSWORD
                        password
    --proto=PROTO         Which transport protocol to use, one of ssh or tcp
    --host=HOST           NETCONF agent hostname
    --port=PORT           NETCONF agent SSH port

    (ydk-py)samples$ ./bgp.py --host <ip-address-of-netconf-server> -u <username> -p <password> --port <port-number>

What happens underneath
~~~~~~~~~~~~~~~~~~~~~~~~
YDK performs the below actions when running this application:

 1. Establish a session with the device 
 2. Encode python data objects to the protocol format (e.g. netconf XML payload)
 3. Perform transport operation with the device and collect the response (e.g. netconf reply)
 4. Decode response as python object and return the result to app
 5. Raise python exceptions for any errors that occurred


Service Providers
-----------------
The first step in any application is to create a service provider instance. In this case, the NETCONF service provider (defined in ``ydk.providers.NetconfServiceProvider``) is responsible for mapping between the CRUD service API and the underlying manageability protocol (NETCONF RPCs).

We instantiate an instance of the service provider that creates a NETCONF session to the machine with address 10.0.0.1 ::

 from ydk.providers import NetconfServiceProvider

 sp_instance = NetconfServiceProvider(address='10.0.0.1',
                                      port=830,
                                      username='test',
                                      password='test',
                                      protocol='ssh')

Using the model APIs
---------------------
After establishing the connection, we instantiate the entities and set some data. First, we import the types from the OpenConfig BGP module::

 from ydk.models.openconfig import openconfig_bgp
 from ydk.models.openconfig import openconfig_bgp_types

Next, create a :py:class:`BGP<ydk.openconfig_bgp.Bgp>` configuration object and set the attributes::

 # create BGP object
 bgp_cfg = openconfig_bgp.Bgp()

 # set the Global AS
 bgp_cfg.global_.config.as_ = 65001

 # Create an AFI SAFI config
 ipv4_afsf = bgp_cfg.global_.afi_safis.AfiSafi()
 ipv4_afsf.afi_safi_name = openconfig_bgp_types.Ipv4Unicast()
 ipv4_afsf.config.afi_safi_name = openconfig_bgp_types.Ipv4Unicast()
 ipv4_afsf.config.enabled = True

 # Add the AFI SAFI config to the global AFI SAFI list
 bgp_cfg.global_.afi_safis.afi_safi.append(ipv4_afsf)

Invoking the CRUD Service
--------------------------
The CRUD service provides methods to create, read, update and delete entities on a device making use of the session provided by a service provider (NETCONF in this case).  In order to use the CRUD service, we need to import the :py:class:`CRUDService<ydk.services.CRUDService>` class::

 from ydk.services import CRUDService

Next, we instantiate the CRUD service::

 crud_service = CRUDService()

Finally, we invoke the create method of the ``CRUDService`` class passing in the
service provider instance and our entity (bgp_cfg)::

 try:
     crud_service.create(sp_instance, bgp_cfg)
 except YPYError:

Note if there were any errors the above API will raise a YPYError exception.

Logging
---------------
YDK uses common Python logging.  All modules are based on the "ydk" log::

 import logging
 log = logging.getLogger('ydk')
 log.setLevel(logging.DEBUG)
 ch = logging.StreamHandler()
 log.addHandler(ch)
