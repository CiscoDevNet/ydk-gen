<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Getting Started](#getting-started)
  - [Overview](#overview)
  - [System Requirements](#system-requirements)
  - [Install Tips](#install-tips)
- [Example Usage](#example-usage)
  - [Service Providers](#service-providers)
  - [Using the model APIs](#using-the-model-apis)
  - [Invoking the CRUD Service](#invoking-the-crud-service)
  - [Logging](#logging)
- [Release Notes](#release-notes)
- [Documentation and Support](#documentation-and-support)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

<a href="https://github.com/CiscoDevNet/ydk-gen"><img src="https://cloud.githubusercontent.com/assets/17089095/14834057/2e1fe270-0bb7-11e6-9e94-73dd7d71e87d.png" height="240" width="240" ></a>


Getting Started
===============

Overview
--------

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.  Each module bundle is generated using a [`bundle profile`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles) and the [`ydk-gen`](https://github.com/CiscoDevNet/ydk-gen) tool.

System Requirements
-------------------
**Linux**

  Ubuntu (Debian-based) - The following packages must be present in your system before installing YDK-Cpp
 
```
    $ sudo apt-get install libboost-all-dev libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev libtool-bin cmake
```

  Centos (Fedora-based) - The following packages must be present in your system before installing YDK-Cpp

```
    $ sudo yum install epel-release
    $ sudo yum install libxml2-devel libxslt-devel libssh-devel boost-devel libtool gcc-c++ pcre-devel cmake
```

**Mac**

  It is recommended to install homebrew (http://brew.sh) and Xcode command line tools on your system before installing YDK-Cpp
  
```
    $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    $ brew install boost curl libssh pcre xml2 cmake
    $ xcode-select --install
```

**Windows**
    
   YDK-Cpp is not currently supported on Windows

Install Tips
------------
YDK uses ``cmake`` as the build system of choice. To install the ``core`` package
```
  ydk-cpp$ cd core
  core$ mkdir build && cd build
  build$ cmake .. && sudo make install
```
Once you have installed the ``core`` package, you can install one or more model bundles.  Note that some bundles have dependencies on other bundles.  Those dependencies are already captured in the bundle package. To install the IETF bundle, execute
```
  core$ cd ../ietf
  ietf$ mkdir build && cd build
  build$ cmake .. && sudo make install
```
To install the openconfig bundle, execute
```
  ietf$ cd ../openconfig
  openconfig$ mkdir build && cd build
  build$ cmake .. && sudo make install
```
To install the cisco-ios-xr bundle, execute
```
  openconfig$ cd ../cisco-ios-xr
  cisco-ios-xr$ mkdir build && cd build
  build$ cmake .. && sudo make install
  build$ cd ../..
```

Example Usage
=============

In this example, we set some BGP configuration using the OpenConfig model, the CRUD (Create/Read/Update/Delete) service and the NETCONF service provider. The example in this document is a simplified version of the more complete sample that is available in `core/samples/bgp_create.cpp`. Assuming you have performed the above core and bundle installations first, that more complete sample can be run with the below steps

```
    ydk-py$ cd core/samples
    samples$ mkdir build && cd build
    build$ cmake .. && make
    build$ ./bgp_create ssh://<username>:<password>@<ip-address-of-netconf-server>:<port> [-v]
```

In our example YDK application, first, let us include the necessary header files
```c++
 #include <iostream>
 #include <boost/log/trivial.hpp>
 #include <boost/log/expressions.hpp>
 
 #include "ydk/crud_service.hpp"
 #include "ydk/netconf_provider.hpp"
 
 #include "ydk_openconfig/openconfig_bgp.hpp"
 
 using namespace std;
 using namespace ydk; 
```

Service Providers
-----------------
The first step in any application is to create a service provider instance. In this case, the NETCONF service provider (defined in `ydk/netconf_provider.hpp`) is responsible for mapping between the CRUD service API and the underlying manageability protocol (NETCONF RPCs).

We instantiate an instance of the service provider that creates a NETCONF session to the machine with address 10.0.0.1

```c++ 
 NetconfServiceProvider provider{"10.0.0.1", "test", "test", 830};
```

Using the model APIs
------------------------
After establishing the connection, we instantiate the entities and set some data. Now, create a BGP configuration object and set the attributes
```c++
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
```

Invoking the CRUD Service
--------------------------
The CRUD service provides methods to create, read, update and delete entities on a device making use of the session provided by a service provider (NETCONF in this case).  In order to use the CRUD service, we need to instantiate the `CrudService` class
```c++
 CrudService crud_service{};
```
Finally, we invoke the create method of the `CRUDService` class passing in the service provider instance and our entity, `bgp`
```c++
 try
 {
   crud_service.create(provider, *bgp);
 }    
 catch(YDKException & e)
 {
   cerr << "Error details: " << boost::diagnostic_information(e) << endl;
 }
```
Note if there were any errors the above API will raise an exception with the base type `YDKException`

Logging
-------
YDK uses the `boost::log` logging library. The logging verbosity can be set using the `set_filter` method

```c++
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
```

Release Notes
===============
The current YDK release version is 0.1.0 (alpha). YDK-Cpp is licensed under the Apache 2.0 License.

Documentation and Support
===============
- Samples can be found under the `core/samples` directory
- API documentation can be found at http://ydk.cisco.com/cpp/docs
- Additional samples can be found at https://github.com/CiscoDevNet/ydk-cpp-samples
- For queries related to usage of the API, please join the YDK community at https://communities.cisco.com/community/developer/ydk
