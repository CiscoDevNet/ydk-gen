Using OpenDaylight with YDK
============================
.. contents::

YDK makes it easy to interact with OpenDaylight programmatically using the YANG model APIs.

Applications can be written using the C++ model API in conjunction with a service and a provider.

Writing the app
----------------

In this example, we set some BGP configuration using the Cisco IOS XR model, the :cpp:class:`CRUD (Create/Read/Update/Delete) service<ydk::CrudService>` and the :cpp:class:`OpenDaylight service provider<ydk::OpenDaylightServiceProvider>`. The example in this document is a simplified version of the more complete sample that is available in ``core/samples/bgp_xr_opendaylight.cpp``. Assuming you have performed the core and cisco-ios-xr bundle installations first, that more complete sample can be run with the below steps::

  ydk-cpp$ cd core/samples
  samples$ mkdir build && cd build
  build$ cmake .. && make
  build$ ./bgp_xr_opendaylight http://<username>:<password>@<host-address>:<port> [-v]

What happens underneath
~~~~~~~~~~~~~~~~~~~~~~~~
YDK performs the below actions when running this application:

 1. Establish a session with the OpenDaylight instance and fetch the details of the nodes mounted on the southbound
 2. Encode C++ data objects to the protocol format (e.g. restconf JSON payload)
 3. For a chosen node on the southbound, perform transport operation with the device and collect the response (e.g. restconf reply)
 4. Decode response as C++ object and return the result to app
 5. Raise C++ exceptions for any errors that occurred

Header includes
----------------------
In our example YDK application, first, let us include the necessary header files

.. code-block:: c++
 :linenos:

 #include <iostream>
 #include <boost/log/trivial.hpp>
 #include <boost/log/expressions.hpp>
 
 #include <ydk/crud_service.hpp>
 #include <ydk/path_api.hpp>
 #include <ydk/opendaylight_provider.hpp>
 #include <ydk/types.hpp>

 #include <ydk_cisco_ios_xr/Cisco_IOS_XR_ipv4_bgp_cfg.hpp>
 #include <ydk_cisco_ios_xr/Cisco_IOS_XR_ipv4_bgp_datatypes.hpp>
 
  // indicate the namespaces being used (optional)
  using namespace std;
  using namespace ydk; 
  using namespace Cisco_IOS_XR_ipv4_bgp_cfg;
  using namespace Cisco_IOS_XR_ipv4_bgp_datatypes;

OpenDaylight service provider
------------------------------
The first step in any application is to create a service provider instance. In this case, the OpenDaylight service provider is responsible for mapping between the CRUD service API and the underlying manageability protocol (Restconf).

We first instantiate a :cpp:class:`Repository<ydk::path::Repository>` using the location of the schema cache of the OpenDaylight instance. We instantiate an instance of the service provider that can communicate using Restconf with an OpenDaylight instance running at host address: ``127.0.0.1`` and port: ``8181``

.. code-block:: c++

 path::Repository repo{"/Users/home/distribution-karaf-0.5.2-Boron-SR2/cache/schema"}; // In this case, we have a ODL boron instance with this schema cache location
 OpenDaylightServiceProvider odl_provider{repo, "127.0.0.1", "admin", "admin", 8181};


Using the model APIs
----------------------
After establishing the connection, we instantiate the entities and set some data. Now, create an :cpp:class:`Cisco IOS XR BGP<ydk::Cisco_IOS_XR_ipv4_bgp_cfg::Bgp>` configuration object and set the attributes

.. code-block:: c++
 :linenos:

 // Create BGP object
 auto bgp = make_unique<Bgp>();

 // BGP instance
 auto instance = make_unique<Bgp::Instance>();
 instance->instance_name = "test";
 auto instance_as = make_unique<Bgp::Instance::InstanceAs>();
 instance_as->as = 65001;
 auto four_byte_as = make_unique<Bgp::Instance::InstanceAs::FourByteAs>();
 four_byte_as->as = 65001;
 four_byte_as->bgp_running = Empty();

 // global address family
 auto global_af = make_unique<Bgp::Instance::InstanceAs::FourByteAs::DefaultVrf::Global::GlobalAfs::GlobalAf>();
 global_af->af_name = BgpAddressFamilyEnum::ipv4_unicast;
 global_af->enable = Empty();
 global_af->parent = four_byte_as->default_vrf->global->global_afs.get();
 four_byte_as->default_vrf->global->global_afs->global_af.push_back(move(global_af));

 // add the instance to the parent BGP object
 four_byte_as->parent = instance_as.get();
 instance_as->parent = instance.get();
 instance->parent = bgp.get();
 instance_as->four_byte_as.push_back(move(four_byte_as));
 instance->instance_as.push_back(move(instance_as));
 bgp->instance.push_back(move(instance));


Invoking the CRUD Service
---------------------------
The CRUD service provides methods to create, read, update and delete entities on a device making use of the session provided by a service provider.  In order to use the CRUD service, we need to instantiate the :cpp:class:`CrudService<ydk::CrudService>` class

.. code-block:: c++

 CrudService crud_service{};

At this point we can explore the southbound device node-IDs using the function call: ``provider.get_node_ids()``. Let us assume there is a XR device mounted with the node ID "xr". We can obtain the :cpp:class:`ServiceProvider<ydk::path::ServiceProvider>` instance corresponding to this node using the function call: ``odl_provider.get_node_provider("xr")``.

Finally, we invoke the create method of the :cpp:class:`CrudService<ydk::CrudService>` class passing in the service provider instance and our entity, ``bgp``

.. code-block:: c++
 :linenos:

 try
 {
   auto & provider = odl_provider.get_node_provider("xr");
   crud_service.create(provider, *bgp);
 }    
 catch(YCPPError & e)
 {
   cerr << "Error details: " << boost::diagnostic_information(e) << endl;
 }

Note if there were any errors the above API will raise an exception with the base type :cpp:class:`YCPPError<ydk::YCPPError>`

Logging
----------------------
YDK uses the `boost::log` logging library. The logging verbosity can be set using the ``set_filter`` method of ``boost::log``

.. code-block:: c++
 :linenos:

 if(verbose)
 {
   boost::log::core::get()->set_filter(
                                      boost::log::trivial::severity >= boost::log::trivial::debug
                                      );
 }
 else
 {
   boost::log::core::get()->set_filter(
                                      boost::log::trivial::severity >= boost::log::trivial::debug
                                      );
 }

