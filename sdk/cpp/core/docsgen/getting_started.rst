Getting Started
===============
.. contents:: Table of Contents

Overview
---------

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.  Each module bundle is generated using a `bundle profile <https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles>`_ and the `ydk-gen <https://github.com/CiscoDevNet/ydk-gen>`_ tool.

System Requirements
--------------------
Linux

  Ubuntu (Debian-based) - The following packages must be present in your system before installing YDK-Cpp::
 

    $ sudo apt-get install libboost-all-dev libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev libtool-bin cmake


  Centos (Fedora-based) - The following packages must be present in your system before installing YDK-Cpp::


    $ sudo yum install epel-release
    $ sudo yum install libxml2-devel libxslt-devel libssh-devel boost-devel libtool gcc-c++ pcre-devel cmake


Mac

  It is recommended to install homebrew (http://brew.sh) and Xcode command line tools on your system before installing YDK-Cpp::
  

    $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    $ brew install boost curl libssh pcre xml2 cmake
    $ xcode-select --install


Windows
    
   YDK-Cpp is not currently supported on Windows

Install Tips
-------------
YDK uses ``cmake`` as the build system of choice. To install the ``core`` package::

  ydk-cpp$ cd core/ydk
  ydk$ mkdir build && cd build
  build$ cmake .. && sudo make install

Once you have installed the ``core`` package, you can install one or more model bundles.  Note that some bundles have dependencies on other bundles.  Those dependencies are already captured in the bundle package. To install the IETF bundle, execute::

  build$ cd ../../../ietf
  ietf$ mkdir build && cd build
  build$ cmake .. && sudo make install

To install the openconfig bundle, execute::

  build$ cd ../../openconfig
  openconfig$ mkdir build && cd build
  build$ cmake .. && sudo make install

To install the cisco-ios-xr bundle, execute::

  build$ cd ../../cisco-ios-xr
  cisco-ios-xr$ mkdir build && cd build
  build$ cmake .. && sudo make install
  build$ cd ../..


Samples
-------------------

To get started with using the YDK API, there are sample apps available under the ``core/samples`` directory. For example, to run the ``core/samples/bgp_create.cpp`` sample, execute::

  ydk-cpp$ cd core/samples
  samples$ mkdir build && cd build
  build$ cmake .. && make
  build$ ./bgp_create ssh://<username>:<password>@<host-address>:<port> [-v]


Release Notes
-------------------
The current YDK release version is 0.5.2 (alpha). YDK-Cpp is licensed under the Apache 2.0 License.

Documentation and Support
--------------------------
- Samples can be found under the `core/samples <core/samples>`_ directory
- API documentation can be found at http://ydk.cisco.com/cpp/docs
- Additional samples can be found at https://github.com/CiscoDevNet/ydk-cpp-samples
- For queries related to usage of the API, please join the YDK community at https://communities.cisco.com/community/developer/ydk
