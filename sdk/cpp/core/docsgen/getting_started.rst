===============
Getting Started
===============
.. contents:: Table of Contents

Overview
========

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.

How to Install
==============
You can install YDK-Cpp on macOS or Linux.  It is not currently supported on Windows.

Quick Install
-------------
**macOS**

You can install the latest model packages using `homebrew <http://brew.sh>`_.  This utility will manage the dependencies between YDK packages and all other sytem dependencies.  First, add the third-party repository (homebrew tap) for YDK:

.. code-block:: sh

  $ brew tap CiscoDevNet/ydk

You get a fully operational YDK environment by installing the ``cisco-ios-xr`` bundle which automatically installs all other YDK-related packages (``ydk``, ``cisco-ios-xr``, ``openconfig`` and ``ietf`` packages):

.. code-block:: sh

  $ brew install ydk-cisco-ios-xr

Alternatively, you can perform a partial installation.  If you only want to install the ``openconfig`` bundle and its dependencies (``ydk`` and ``ietf`` packages), execute:

.. code-block:: sh

  $ brew install ydk-openconfig

If you only want to install the ``ietf`` bundle and its dependencies (``ydk`` package), execute:

.. code-block:: sh

  $ brew install ydk-ietf

**Linux**

Debian and RPM packages are coming soon.  Currently, you have to install it from source (see below).

Installing from source
----------------------
System Requirements
~~~~~~~~~~~~~~~~~~~
**Note:** libssh 0.8.0 and later `does not support <http://api.libssh.org/master/libssh_tutor_threads.html>`_ separate threading library which is required for YDK. Please use libssh versions older than 0.8.0.

**Linux**

Ubuntu (Debian-based) - The following packages must be present in your system before installing YDK-Cpp:

.. code-block:: sh

  $ sudo apt-get install libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev libtool-bin cmake

Centos (Fedora-based) - The following packages must be present in your system before installing YDK-Cpp:

.. code-block:: sh

  $ sudo yum install epel-release
  $ sudo yum install libxml2-devel libxslt-devel libssh-devel libtool gcc-c++ pcre-devel cmake

**Mac**

It is recommended to install `homebrew <http://brew.sh>`_ and Xcode command line tools on your system before installing YDK-Cpp:

.. code-block:: sh

  $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  $ brew install curl libssh pcre xml2 cmake
  $ xcode-select --install

Building YDK
~~~~~~~~~~~~
YDK uses ``cmake`` as the build system of choice. To install the ``core`` package, execute:

.. code-block:: sh

  $ ydk-cpp$ cd core/ydk
  $ core$ mkdir build && cd build
  $ build$ cmake .. && make
  $ build$ sudo make install

Once you have installed the ``core`` package, you can install one or more model bundles.  Note that some bundles have dependencies on other bundles.  Those dependencies are captured in the bundle packages used for quick installation. To install the ``ietf`` bundle, execute:

.. code-block:: sh

  $ core$ cd ../../ietf
  $ ietf$ mkdir build && cd build
  $ build$ cmake .. && make
  $ build$ sudo make install

To install the ``openconfig`` bundle, execute:

.. code-block:: sh

  $ ietf$ cd ../openconfig
  $ openconfig$ mkdir build && cd build
  $ build$ cmake .. && make
  $ build$ sudo make install

To install the ``cisco-ios-xr`` bundle, execute:

.. code-block:: sh

  $ openconfig$ cd ../cisco-ios-xr
  $ cisco-ios-xr$ mkdir build && cd build
  $ build$ cmake .. && make
  $ build$ sudo make install
  $ build$ cd ../..

Samples
=======
To get started with using the YDK API, there are sample apps available in the `YDK-Cpp repository <https://github.com/CiscoDevNet/ydk-cp/tree/master/core/samples>`_. For example, to run the ``bgp_create.cpp`` sample, execute:

.. code-block:: sh

    ydk-cpp$ cd core/samples
    samples$ mkdir build && cd build
    build$ cmake .. && make
    build$ ./bgp_create ssh://<username>:<password>@<host-address>:<port> [-v]

Documentation and Support
=========================
- Numerous additional samples can be found in the `YDK-Cpp samples repository <https://github.com/CiscoDevNet/ydk-cpp-samples>`_
- Join the `YDK community <https://communities.cisco.com/community/developer/ydk>`_ to connect with other users and with the makers of YDK
