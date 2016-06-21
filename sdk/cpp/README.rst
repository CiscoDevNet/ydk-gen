Getting Started
===============

Overview:
----------

YDK or YANG Development Kit is a Software Development Kit that provides API's that are modeled
in YANG. The main goal of YDK is to reduce the learning curve by expressing the model semantics
in API and abstracting protocol/encoding details. The API's are generated from YANG models found
in this profile file `https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/ydk/ydk_0_4_0.json` using the ydk-gen tool `https://github.com/CiscoDevNet/ydk-gen` .

System Requirements:
--------------------
Linux
  Ubuntu (Debian-based) - The following packages must be present in your system before installing YDK-Py::

    $ sudo apt-get install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev

Mac
  It is recommended to install homebrew (http://brew.sh) and Xcode command line tools on your system before installing YDK-Py::

    $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    $ xcode-select --install

Install Tips:
-------------
Create a shared library::

    $ cd <ydk_cpp_git_root>
    $ make

You can start by compiling and running the sample application::

    $ make bgp_sample
    $ ./bgp_sample

You can also write applications of your own and edit the Makefile to compile and run your application.

Notes:
------
- YANG Development Kit is a C++ SDK that provides an API to access/modify configuration and operational entities that are modeled using YANG
- The modules under the package ydk.models are derived from YANG models
- YDK provides a simple CRUD (Create/Read/Update/Delete) api that allows the developer to perform these operations on entities on a server that supports them


Example Usage
========================

In this example we are going to set some configuration on the openconfig bgp model.
The complete sample is available in samples/bgp.py. The sample can be run with the below steps.
::
    
    $ ./bgp_sample    


Service Providers
------------------------
The first step in any application is create a 'Service Provider Instance'. 'Service Providers'
are responsible for mapping between the CRUD service API and the underlying manageability
protocol. In the current version of YDK we have one service provider which is the
ydk.providers.NetconfServiceProvider . It maps the CRUD api's to netconf rpc.

In this example we instantiate an instance of the service provider that creates a netconf
session to the machine at ip 10.0.0.1 ::

 #include "ydk/providers.h"

 NetconfServiceProvider provider{{"10.0.0.1", "12022", "admin", "admin"}};

Using the model APIs
------------------------
After establishing the connection, it's time to instantiate the entities and set some data.

First include the header ::

 #include "ydk/models/bgp/bgp.h"

Next set the attributes ::

 // create BGP object
 auto bgp = make_unique<Bgp>();

 // set the Global AS
 bgp->global_->config->as_ = 65001;

 // create an AFI SAFI config
 auto ipv4_afsf = make_unique<Bgp::Global::AfiSafis::AfiSafi>();
 ipv4_afsf->afi_safi_name = "ipv4-unicast";
 ipv4_afsf->config->afi_safi_name = "ipv4-unicast";
 ipv4_afsf->config->enabled = true;

 // add the AFI SAFI config to the global AFI SAFI list
 bgp->global_->afi_safis->afi_safi.push_back(move(ipv4_afsf));

Invoking the CRUDService
--------------------------
First we need to include the header for the CRUDService::

 #include "ydk/services.h"

Next we instantiate the CRUDService::

 CRUDService crud{};

And finally we invoke the create method of the CRUDService class passing in the
service provider instance and our entity (bgp)::
 
 crud.create(provider, *bgp); 

Release Notes
--------------
The current release version is <TBD>. YDK-Cpp is licensed under the Apache 2.0 License.

Documentation and Support
--------------------------
- Samples can be found under the <git_root>/samples directory
- For queries related to usage of the API, please join the YDK community at https://communities.cisco.com/community/developer/ydk
