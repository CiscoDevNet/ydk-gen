<!---
#  YDK-YANG Development Kit
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
-->
[![Build Status](https://travis-ci.org/CiscoDevNet/ydk-go.svg?branch=master)](https://travis-ci.org/CiscoDevNet/ydk-go)
[![GoDoc](https://godoc.org/github.com/CiscoDevNet/ydk-go?status.svg)](https://godoc.org/github.com/CiscoDevNet/ydk-go)
[![Docker Automated build](https://img.shields.io/docker/automated/jrottenberg/ffmpeg.svg)](https://hub.docker.com/r/ydkdev/ydk-go/)

![ydk-logo-128](https://cloud.githubusercontent.com/assets/16885441/24175899/2010f51e-0e56-11e7-8fb7-30a9f70fbb86.png)

YANG Development Kit (Go)
=========================

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Overview](#overview)
- [Backward Compatibility](#backward-compatibility)
- [Docker](#docker)
- [System Requirements](#system-requirements)
- [Core Installation](#core-installation)
  - [Installation script](#installation-script)
  - [Building from source](#building-from-source)
- [Adding gNMI service](#adding-gnmi-service)
- [Documentation and Support](#documentation-and-support)
- [Release Notes](#release-notes)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Overview

**YDK** is a developer tool that allows generate YANG model API's in multiple languages and provides services
to apply generated API over multiple communication protocols.
Currently supported languages are: Python, Go and C++.
Currently implemented protocols are: Netconf, Restconf, OpenDaylight and gNMI.
YDK provides CRUD and protocol specific service over above protocols.
YDK also provides Codec service to translate API models to/from XML and JSON encoded strings.

## Backward Compatibility

The Go generated API starting from YDK-0.7.3 is fully compatible with YDK-0.9.0.

**NOTE.** Starting from release 0.8.5 the YDK does not support Python2 interpreter as it was deprecated.

## Docker

Currently the [docker image](https://docs.docker.com/engine/reference/run/) for ydk-go is not been generated.
Please use docker image below, which has complete environment for ydk-go based development.

The [docker image](https://docs.docker.com/engine/reference/run/) is automatically built with the latest ydk-gen
commit to git repository. 
The docker image is used to run YDK based apps without installing anything natively on your machine.

To use the docker image, [install docker](https://docs.docker.com/install/) on your system and run the below command.
See the [docker documentation](https://docs.docker.com/engine/reference/run/) for more details.

```
docker run -it ydksolutions/ydk-gen:0.8.6.5
```

## System Requirements

The YDK is currently supported on the following platforms including native installations, virtual machines, and docker images:
 - Linux Ubuntu Xenial (16.04 LTS), Bionic (18.04 LTS), Focal (20.04 LTS), and Jammy (22.04 LTS)
 - Linux CentOS versions 7 and Centos Stream 8 (Centos 8.x has been EOL as of December 31 of 2021)
 - Linux RHEL version 7.x and 8.x 
 - MacOS up to 11.7.2 (Big Sur)

On Windows 10 the Linux virtual machine can run using Windows Subsystem for Linux (WSL);
check [this](https://www.windowscentral.com/install-windows-subsystem-linux-windows-10) for virtual machine installation procedure.
The YDK has been tested in such environment on Ubuntu Bionic (18.04 LTS) and Focal (20.04 LTS) images obtained
from Microsoft Store.

On supported platforms the YDK can be installed using [installation script](#installation-script).
On other platforms the YDK should be installed manually [from source](#building-from-source).
For both the methods the user must install `git` package prior to the installation procedure.

All YDK core components are based on C and C++ code. These components compiled using default compilers for the supported platform.
Corresponding binaries, libraries, and header files are installed in default locations,
which are `/usr/local/bin`, `/usr/local/lib`, and `/usr/local/include`.
The user must have sudo access in order to install YDK core components to these locations.
Make sure the `sudo` package is installed on your platform prior to the YDK installation procedure.

## Core Installation

### Installation script

For YDK installation it is recommended to use script `install_ydk.sh` from `ydk-gen` git repository.
The script detects platform OS, installs all the dependencies and builds complete set of YDK components for specified language.
The user must have sudo access to these locations.

The YDK extensively uses Python scripts for building its components and model API packages (bundles).
In order to isolate YDK Python environment from system installation, the script can build Python3 virtual environment.
If built, the user must manually activate virtual environment when generating model bundles and/or running YDK based application.
By default the Python virtual environment is installed under `$HOME/venv` directory.
For different location the PYTHON_VENV environment variable should be set to that location.

**NOTE.** It is strongly recommended to use Python virtual environment on Centos/RHEL and Mac platforms.

When installing YDK for Go programming language, the third party dependencies and C++ packages must be installed first.
These steps require sudo/root access to the installation platform.
Here is simple example of core YDK installation for Go programming language and Python virtual environment:

```
git clone https://github.com/CiscoDevNet/ydk-gen.git
cd ydk-gen
export YDKGEN_HOME=`pwd`  # optional
export PYTHON_VENV=$HOME/ydk_vne  # optional
./install_ydk.sh --cpp --core --venv
./install_ydk.sh --go --core --venv
```

The script also allows to install individual components like dependencies, core, and service packages
for specified programming language or for all supported languages.
Full set of script capabilities could be viewed like this:

```
./install_ydk.sh --help
usage: install_ydk [ {--cpp|--py|--go|--all} ] [-c] [-s gnmi] [-h] [-n] [-v]
Options and arguments:
  --cpp                 install YDK for C++ programming language;
                        requires sudo access for dependencies and libraries installation
  --go                  install YDK for Go programming language
  --py|--python         install YDK for Python programming language
  --all                 install YDK for all available programming languages;
                        requires sudo access for dependencies and libraries installation
  -v|--venv             create python virtual environment
  -c|--core             install YDK core package
  -s|--service gnmi     install gNMI service package
  -n|--no-deps          skip installation of dependencies;
                        applicable only with --cpp and --all options
  -h|--help             print this help message and exit

Environment variables:
YDKGEN_HOME         specifies location of ydk-gen git repository;
                    if not set, $HOME/ydk-gen is assumed
PYTHON_VENV         specifies location of python virtual environment;
                    if not set, $HOME/venv is assumed
GOROOT              specifies installation directory of go software;
                    if not set, /usr/local/go is assumed
GOPATH              specifies location of go source directory;
                    if not set, $HOME/go is assumed
C_INCLUDE_PATH      location of C include files;
                    if not set, /usr/local/include is assumed
CPLUS_INCLUDE_PATH  location of C++ include files;
                    if not set, /usr/local/include is assumed
CMAKE_LIBRARY_PATH  Location of Python shared libraries;
                    if not set, default system library location is assumed
```

If user environment is different from the default one (different Python installation or different
location of libraries) then building from source method should be used.

### Building from source

#### Environment variables

In some OS configurations during YDK package installation the cmake fails to find C/C++ headers for previously installed YDK libraries.
In this case the header location must be specified explicitly (in below commands the default location is shown):

```
  export C_INCLUDE_PATH=/usr/local/include
  export CPLUS_INCLUDE_PATH=/usr/local/include
```

When non-standard Python installation is used or there are multiple installations of Python on the platform,
the PATH and CMAKE_LIBRARY_PATH environment variables must be set accordingly in order for the installation scripts
to pick up correct Python binaries and shared libraries.

#### Installing third party dependencies

If user platform is supported one, it is recommended to use `ydk-gen/install_ydk.sh` script. 
The script will also install Python virtual environment in default or specified location, when '--venv' is specified.

```
# Clone ydk-gen from GitHub
git clone https://github.com/CiscoDevNet/ydk-gen.git
cd ydk-gen

# Define optional environment variables and install dependencies
export YDKGEN_HOME=`pwd`  
export PYTHON_VENV=$HOME/ydk_venv
./install_ydk.sh
```

For unsupported platforms it is recommended to follow logic of `ydk-gen/test/dependencies-*` scripts.
 
#### Installing core components

Please follow this procedure to install YDK core components for Go apps development:

```
# If created, activate Python virtual environment
source $PYTHON_VENV/bin/activate

# Generate and install YDK core library
python3 generate.py -is --core --cpp

# Generate and install YDK-Go core package
python3 generate.py -i --core --go

```

## Adding gNMI Service

In order to enable YDK support for gNMI protocol, which is optional, the user need install third party software
and YDK gNMI service package. 

### gNMI service installation

Here is simple example how gNMI service package for Go programming language and Python virtual environment could be added:

```
cd ydk-gen
./install_ydk.sh --cpp --service gnmi --venv  # requires sudo access
./install_ydk.sh --go --service gnmi --venv
```

### Runtime environment

There is an open issue with gRPC on Centos/RHEL, which requires an extra step before running any YDK gNMI application.
See this issue on [GRPC GitHub](https://github.com/grpc/grpc/issues/10942#issuecomment-312565041) for details.
As a workaround, the YDK based application runtime environment must include setting of `LD_LIBRARY_PATH` variable:

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64
```

## Documentation and Support
- Read the [API documentation](http://ydk.cisco.com/go/docs) (release 0.8.3) for details on how to use the API and specific models
- Samples can be found under the [samples directory](https://github.com/CiscoDevNet/ydk-go/tree/master/samples)
- Additional samples can be found in the [YDK-Go samples repository](https://github.com/CiscoDevNet/ydk-go-samples) (coming soon)
- Join the [YDK community](https://communities.cisco.com/community/developer/ydk) to connect with YDK users and developers

## Release Notes

The current YDK release version is 0.8.6.5.

YDK is licensed under the Apache 2.0 License.
