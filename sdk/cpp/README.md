[![Build Status](https://travis-ci.org/CiscoDevNet/ydk-cpp.svg?branch=master)](https://travis-ci.org/CiscoDevNet/ydk-cpp)

![ydk-logo-128](https://cloud.githubusercontent.com/assets/16885441/24175899/2010f51e-0e56-11e7-8fb7-30a9f70fbb86.png)

YANG Development Kit (C++)
==========================

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Overview](#overview)
- [How to Install](#how-to-install)
  - [Quick Install](#quick-install)
  - [Installing from source](#installing-from-source)
    - [System Requirements](#system-requirements)
    - [Building YDK](#building-ydk)
- [Documentation and Support](#documentation-and-support)
- [Release Notes](#release-notes)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

Overview
========

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.  

How to Install
==============
You can install YDK-Cpp on macOS or Linux.  It is not currently supported on Windows.

Quick Install
-------------
**macOS**
You can install the latest model packages using [homebrew](http://brew.sh).  It will manage the dependencies between YDK packages and all other sytem dependencies.  First, add the third-party repository (homebrew tap) for YDK:
```
$ brew tap CiscoDevNet/ydk
```

You get a fully operational YDK environment by installing the ``cisco-ios-xr`` bundle which automatically installs all other YDK-related packages (``ydk``, ``cisco-ios-xr``, ``openconfig`` and ``ietf`` packages):
```
$ brew install ydk-cisco-ios-xr
```

Alternatively, you can perform a partial installation.  If you only want to install the ``openconfig`` bundle and its dependencies (``ydk`` and ``ietf`` packages), execute::
```
$ brew install ydk-openconfig
```

If you only want to install the ``ietf`` bundle and its dependencies (``ydk`` package), execute::
```
$ brew install ydk-ietf
```

**Linux**
Debian and RPM packages are coming soon.  Currently, you have to install it from source (see below).

Installing from source
----------------------
### System Requirements
**Linux**
Ubuntu (Debian-based) - The following packages must be present in your system before installing YDK-Cpp:
```
$ sudo apt-get install libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev libtool-bin cmake
```

Centos (Fedora-based) - The following packages must be present in your system before installing YDK-Cpp:
```
$ sudo yum install epel-release
$ sudo yum install libxml2-devel libxslt-devel libssh-devel libtool gcc-c++ pcre-devel cmake
```

**Mac**
It is recommended to install homebrew (http://brew.sh) and Xcode command line tools on your system before installing YDK-Cpp
```
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
$ brew install curl libssh pcre xml2 cmake
$ xcode-select --install
```
### Building YDK
YDK uses ``cmake`` as the build system of choice. To install the ``core`` package, execute:
```
$ ydk-cpp$ cd core/ydk
$ core$ mkdir build && cd build
$ build$ cmake .. && make
$ build$ sudo make install
```

Once you have installed the ``core`` package, you can install one or more model bundles.  Note that some bundles have dependencies on other bundles.  Those dependencies are captured in the bundle packages used for quick installation. To install the IETF bundle, execute
```
$ core$ cd ../../ietf
$ ietf$ mkdir build && cd build
$ build$ cmake .. && make
$ build$ sudo make install
```

To install the openconfig bundle, execute
```
$ ietf$ cd ../openconfig
$ openconfig$ mkdir build && cd build
$ build$ cmake .. && make
$ build$ sudo make install
```

To install the cisco-ios-xr bundle, execute
```
$ openconfig$ cd ../cisco-ios-xr
$ cisco-ios-xr$ mkdir build && cd build
$ build$ cmake .. && make
$ build$ sudo make install
$ build$ cd ../..
```

Documentation and Support
===============
- Samples can be found under the `core/samples` directory
- API documentation can be found at http://ydk.cisco.com/cpp/docs
- Additional samples can be found at https://github.com/CiscoDevNet/ydk-cpp-samples
- For queries related to usage of the API, please join the YDK community at https://communities.cisco.com/community/developer/ydk

Release Notes
===============
The current YDK release version is 0.5.4 (alpha). YDK-Cpp is licensed under the Apache 2.0 License.
