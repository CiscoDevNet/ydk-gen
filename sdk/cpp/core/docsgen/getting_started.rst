..
  #  Copyright 2016 Cisco Systems. All rights reserved
  # *************************************************************
  # Licensed to the Apache Software Foundation (ASF) under one
  # or more contributor license agreements.  See the NOTICE file
  # distributed with this work for additional information
  # regarding copyright ownership.  The ASF licenses this file
  # to you under the Apache License, Version 2.0 (the
  # "License"); you may not use this file except in compliance
  # with the License.  You may obtain a copy of the License at
  #
  #   http:#www.apache.org/licenses/LICENSE-2.0
  #
  #  Unless required by applicable law or agreed to in writing,
  # software distributed under the License is distributed on an
  # "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  # KIND, either express or implied.  See the License for the
  # specific language governing permissions and limitations
  # under the License.
  # *************************************************************
  # This file has been modified by Yan Gorelik, YDK Solutions.
  # All modifications in original under CiscoDevNet domain
  # introduced since October 2019 are copyrighted.
  # All rights reserved under Apache License, Version 2.0.
  # *************************************************************

===============
Getting Started
===============
.. contents:: Table of Contents

Overview
========

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.

How to Install
==============

You can install YDK-Cpp on Linux or MacOS.  It is currently not supported on Windows.

System Requirements
-------------------

Linux
~~~~~

**Ubuntu (Debian-based)**

The following packages must be present in your system before installing YDK-Cpp:

.. code-block:: sh

   # Linux dependencies
   sudo apt-get install libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev libtool-bin cmake
   sudo apt-get install python3-dev

   # gcc-5 and g++5 for modern c++
   sudo apt-get install gcc-5 g++-5 -y > /dev/null
   sudo ln -f -s /usr/bin/g++-5 /usr/bin/g++
   sudo ln -f -s /usr/bin/gcc-5 /usr/bin/gcc

**CentOS-7 (Fedora-based)**

The following packages must be present in your system before installing YDK-Cpp:

.. code-block:: sh

   # Linux dependencies
   sudo yum install epel-release
   sudo yum install libxml2-devel libxslt-devel libssh-devel libtool pcre-devel cmake3 wget
   sudo yum install gcc-g++ python36-devel

If installed gcc compiler version is lowers than 4.8.1, upgrade the compiler to gcc-5

.. code-block:: sh

   # gcc-5 and g++5 for modern c++
   sudo yum install centos-release-scl -y > /dev/null
   sudo yum install devtoolset-4-gcc* -y > /dev/null
   sudo ln -sf /opt/rh/devtoolset-4/root/usr/bin/gcc /usr/bin/cc
   sudo ln -sf /opt/rh/devtoolset-4/root/usr/bin/g++ /usr/bin/c++
   sudo ln -sf /opt/rh/devtoolset-4/root/usr/bin/gcc /usr/bin/gcc
   sudo ln -sf /opt/rh/devtoolset-4/root/usr/bin/g++ /usr/bin/g++

MacOS
~~~~~

It is recommended to install `homebrew <http://brew.sh>`_ and Xcode command line tools on your system before installing YDK-Cpp:

.. code-block:: sh

   /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
   brew install curl libssh pcre xml2 cmake
   xcode-select --install

Libssh Installation
-------------------

The libssh-0.8.0 `does not support <http://api.libssh.org/master/libssh_tutor_threads.html>`_ separate threading library,
which is required for YDK. If after installation of libssh package the `libssh_threads.a` is missing, please downgrade the installation to libssh-0.7.6,
or upgrade to libssh-0.8.1 or higher.

gNMI Requirements
-----------------

In order to enable YDK support for gNMI protocol, which is optional, the following third party software must be installed prior to gNMI YDK component installation.

Install Protobuf and Protoc
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: sh

   wget https://github.com/google/protobuf/releases/download/v3.5.0/protobuf-cpp-3.5.0.zip
   unzip protobuf-cpp-3.5.0.zip
   cd protobuf-3.5.0
   ./configure
   make
   sudo make install
   sudo ldconfig
   cd -

**Install gRPC**

.. code-block:: sh

   git clone -b v1.9.1 https://github.com/grpc/grpc
   cd grpc
   git submodule update --init
   make
   sudo make install
   sudo ldconfig
   cd -

Quick YDK Installation
----------------------

Install prebuilt libraries
~~~~~~~~~~~~~~~~~~~~~~~~~~

**Ubuntu**

Download and install YDK core library - `libydk`. You can install the library using prebuilt debian packages for Xenial and Bionic LTS distributions.
For other Ubuntu distributions it is recommended to build core library from source.
The prebuilt libraries compiled with specific C/C++ compilers versions, which corresponds to default one for the Linux disribution.
If your compiler is different from specified one, it is recommended to build the packages from source.

For Xenial (Ubuntu 16.04.4, gcc-5.5.0):

.. code-block:: sh

   wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/xenial/libydk-0.8.5-1.amd64.deb
   sudo gdebi libydk-0.8.5-1.amd64.deb

For Bionic (Ubuntu 18.04.1, gcc-7.4.0):

.. code-block:: sh

   wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/bionic/libydk-0.8.5-1.amd64.deb
   sudo gdebi libydk-0.8.5-1.amd64.deb

**CentOS**

The prebuilt package compiled with default version - gcc-4.8.5.

.. code-block:: sh

   sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.8.5/libydk-0.8.5-1.x86_64.rpm

**MacOS**

The prebuilt packages compiled with C++ compiler Clang-8.0.0, which is default version for MacOS 10.12.

.. code-block:: sh

   curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.8.5-beta/libydk-0.8.5-Darwin.pkg
   sudo installer -pkg libydk-0.8.5-Darwin.pkg -target /

gNMI Service Installation
-------------------------

Installing YDK gNMI library
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Ubuntu**

For Xenial (Ubuntu 16.04.4):

.. code-block:: sh

   wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/xenial/libydk_gnmi-0.4.0-5.amd64.deb
   sudo gdebi libydk_gnmi-0.4.0-5.amd64.deb

For Bionic (Ubuntu 18.04.1):

.. code-block:: sh

   wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/bionic/libydk_gnmi-0.4.0-5.amd64.deb
   sudo gdebi libydk_gnmi-0.4.0-5.amd64.deb

**CentOS**

The prebuilt package compiled with default version - gcc-4.8.5.

.. code-block:: sh

   sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.8.5/libydk_gnmi-0.4.0-5.x86_64.rpm

**MacOS**

The prebuilt packages compiled with C++ compiler Clang-8.0.0.

.. code-block:: sh

   curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.8.5/libydk_gnmi-0.4.0-5.Darwin.pkg
   sudo installer -pkg libydk_gnmi-0.4.0-5.Darwin.pkg -target /

Set runtime environment
~~~~~~~~~~~~~~~~~~~~~~~

The YDK based application runtime environment must include setting of **LD_LIBRARY_PATH** variable:

.. code-block:: sh

   PROTO="/Your-Protobuf-and-Grpc-installation-directory"
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROTO/grpc/libs/opt:$PROTO/protobuf-3.5.0/src/.libs:/usr/local/lib64

Installing from source
----------------------

In order to build YDK components from source, download or clone source files from `YDK-Cpp repository <https://github.com/CiscoDevNet/ydk-cpp>`_

Installing YDK core library
~~~~~~~~~~~~~~~~~~~~~~~~~~~

YDK uses ``cmake`` as the build system of choice. To install the ``core`` package, execute:

.. code-block:: sh

  $ cd ydk-cpp/core/ydk
  ydk$ mkdir build && cd build
  build$ cmake .. && make
  build$ sudo make install

Installing model bundles
~~~~~~~~~~~~~~~~~~~~~~~~

Once you have installed the ``core`` package, you can install one or more model bundles.  Note that some bundles have dependencies on other bundles.  Those dependencies are captured in the bundle packages used for quick installation.

To install the ``ietf`` bundle, execute:

.. code-block:: sh

  $ cd ydk-cpp/ietf
  ietf$ mkdir build && cd build
  build$ cmake .. && make
  build$ sudo make install

To install the ``openconfig`` bundle, execute:

.. code-block:: sh

  $ cd ydk-cpp/openconfig
  openconfig$ mkdir build && cd build
  build$ cmake .. && make
  build$ sudo make install

To install the ``cisco-ios-xr`` bundle, execute:

.. code-block:: sh

  $ cd ydk-cpp/cisco-ios-xr
  cisco-ios-xr$ mkdir build && cd build
  build$ cmake .. && make
  build$ sudo make install

Installing YDK gNMI library
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Optionally the YDK gNMI Service library can be installed. Prior to this installation the YDK core library must be installed (see above).

.. code-block:: sh

  $ cd ydk-cpp/gnmi
  gnmi$ mkdir -p build
  gnmi$ cd build
  build$ cmake ..
  build$ make
  build$ sudo make install

Samples
=======

To get started using the YDK API, there are sample apps available in the `YDK-Cpp samples repository <https://github.com/CiscoDevNet/ydk-cpp/tree/master/core/ydk/samples>`_. For example, to run the ``bgp_create.cpp`` sample execute:

.. code-block:: sh

  $ ydk-cpp$ cd core/samples
  samples$ mkdir build && cd build
  build$ cmake .. && make
  build$ ./bgp_create ssh://<username>:<password>@<host-address>:<port> [-v]

Documentation and Support
=========================
- Numerous additional samples can be found in the `YDK-Cpp samples repository <https://github.com/CiscoDevNet/ydk-cpp/tree/master/core/ydk/samples>`_
- Join the `YDK community <https://communities.cisco.com/community/developer/ydk>`_ to connect with other users and with the makers of YDK
