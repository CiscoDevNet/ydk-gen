Introduction
=============
.. contents:: Table of Contents

YDK consists of two main components: core library, which consists of services and providers, and C++ model API, which are APIs generated based on YANG models and packaged as bundles.

Core library consists of the below:

 * **Service:** Provides simple API interface to be used with the bindings and providers
 * **ServiceProvider:** Provides concrete implementation that abstracts underlying protocol details (e.g. :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`, which is based on the NETCONF protocol) 

Applications can be written using the C++ model API in conjunction with a service and a provider.

Writing an app
---------------

In this example, we set some BGP configuration using the OpenConfig model, the CRUD (Create/Read/Update/Delete) service and the NETCONF service provider. The example in this document is a simplified version of the more complete sample that is available in ``core/samples/bgp_create.cpp``. Assuming you have performed the core and bundle installations first, that more complete sample can be run with the below steps::

  ydk-cpp$ cd core/samples
  samples$ mkdir build && cd build
  build$ cmake .. && make
  build$ ./bgp_create ssh://<username>:<password>@<host-address>:<port> [-v]

What happens underneath
~~~~~~~~~~~~~~~~~~~~~~~~
YDK performs the below actions when running this application:

 1. Establish a session with the device and, optionally, fetch any data models which are advertised by the device
 2. Encode C++ data objects to the protocol format (e.g. netconf XML payload)
 3. Perform transport operation with the device and collect the response (e.g. netconf reply)
 4. Decode response as C++ object and return the result to app
 5. Raise C++ exceptions for any errors that occurred

Header includes
----------------------
In our example YDK application, first, let us include the necessary header files

.. code-block:: c++
 :linenos:

 #include <iostream>
 #include <spdlog/spdlog.h>
  
 #include "ydk/crud_service.hpp"
 #include "ydk/netconf_provider.hpp"
 
 #include "ydk_openconfig/openconfig_bgp.hpp"
 
 using namespace std;
 using namespace ydk; 

Service Providers
----------------------
The first step in any application is to create a service provider instance. In this case, the NETCONF service provider (defined in ``ydk/netconf_provider.hpp``) is responsible for mapping between the CRUD service API and the underlying manageability protocol (NETCONF RPCs).

We instantiate an instance of the service provider that creates a NETCONF session to the machine with address 10.0.0.1

.. code-block:: c++

 NetconfServiceProvider provider{"10.0.0.1", "test", "test", 830};


Using the model APIs
----------------------
After establishing the connection, we instantiate the entities and set some data. Now, create an :cpp:class:`openconfig BGP<ydk::openconfig_bgp::Bgp>` configuration object and set the attributes

.. code-block:: c++
 :linenos:

 // Create BGP object
 auto bgp = make_unique<openconfig_bgp::Bgp>();

 // Set the Global AS
 bgp->global->config->as = 65001;
 bgp->global->config->router_id = "1.2.3.4";

 // Create a neighbor
 auto neighbor = make_unique<openconfig_bgp::Bgp::Neighbors::Neighbor>();
 neighbor->neighbor_address = "6.7.8.9";
 neighbor->config->neighbor_address = "6.7.8.9";
 neighbor->config->peer_as = 65001;
 neighbor->config->local_as = 65001;
 neighbor->config->peer_group = "IBGP";
 
 // Set the parent container of the neighbor
 neighbor->parent = bgp->neighbors.get();
 
 // Add the neighbor config to the BGP neighbors list
 bgp->neighbors->neighbor.push_back(move(neighbor));


Invoking the CRUD Service
---------------------------
The CRUD service provides methods to create, read, update and delete entities on a device making use of the session provided by a service provider (NETCONF in this case).  In order to use the CRUD service, we need to instantiate the :cpp:class:`CrudService<ydk::CrudService>` class

.. code-block:: c++

 CrudService crud_service{};

Finally, we invoke the create method of the :cpp:class:`CrudService<ydk::CrudService>` class passing in the service provider instance and our entity, ``bgp``

.. code-block:: c++
 :linenos:

 try
 {
   crud_service.create(provider, *bgp);
 }    
 catch(YCPPError & e)
 {
   cerr << "Error details: " << e.what() << endl;
 }

Note if there were any errors the above API will raise an exception with the base type :cpp:class:`YCPPError<ydk::YCPPError>`

Logging
----------------------
YDK uses the `spdlog` logging library. The logging can be enabled as follows by creating a logger called "ydk". For other options like logging the "ydk" log to a file, see the `spdlog reference <https://github.com/gabime/spdlog#usage-example>`_.

.. code-block:: c++
 :linenos:

 if(verbose)
 {
   auto console = spdlog::stdout_color_mt("ydk");
 }

