<!---
# *************************************************************
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
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6e111527081b48e1b2252c3562e08a3b)](https://www.codacy.com/app/ydk/ydk-gen?utm_source=github.com&utm_medium=referral&utm_content=CiscoDevNet/ydk-gen&utm_campaign=badger)
[![License](https://cloud.githubusercontent.com/assets/17089095/19458582/dd626d2c-9481-11e6-8019-8227c5c66a06.png)](https://github.com/CiscoDevNet/ydk-gen/blob/master/LICENSE) [![Build Status](https://travis-ci.org/CiscoDevNet/ydk-gen.svg?branch=master)](https://travis-ci.org/CiscoDevNet/ydk-gen)
[![codecov](https://codecov.io/gh/CiscoDevNet/ydk-gen/branch/master/graph/badge.svg)](https://codecov.io/gh/CiscoDevNet/ydk-gen)
[![Docker Automated build](https://img.shields.io/docker/automated/jrottenberg/ffmpeg.svg)](https://hub.docker.com/r/ydkdev/ydk-gen/)

![ydk-logo-128](https://cloud.githubusercontent.com/assets/16885441/24175899/2010f51e-0e56-11e7-8fb7-30a9f70fbb86.png)

YANG Development Kit (Generator)
================================

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Overview](#overview)
- [Backward compatibility](#backward-compatibility)
- [Docker](#docker)
- [System requirements](#system-requirements)
  - [Linux](#linux)
  - [MacOSX](#mac-osx)
  - [Windows](#windows)
- [YDK Python Installation](#ydk-python-installation)
  - [Setting up Python virtual environment](#setting-up-python-virtual-environment)
  - [Clone ydk-gen and install the requirements](#clone-ydk-gen-and-install-the-requirements)
- [Generate YDK components](#generate-ydk-components)
  - [Generate and install the core](#generate-and-install-the-core)
  - [Build model bundle profile](#build-model-bundle-profile)
  - [Generate and install model bundle](#generate-and-install-model-bundle)
  - [Writing your first app](#writing-your-first-app)
  - [Documentation](#documentation)
- [Generating an "Adhoc" YDK-Py Bundle](#generating-an-adhoc-ydk-py-bundle)
- [Notes](#notes)
  - [Python Requirements](#python-requirements)
  - [Directory structure](#directory-structure)
  - [Troubleshooting](#troubleshooting)
- [Running Unit Tests](#running-unit-tests)
  - [Python](#python)
  - [C++](#c)
  - [Go](#go)
- [Support](#support)
- [Release Notes](#release-notes)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Overview

**YDK** is a developer tool that allows generate YANG model API's in multiple languages and provides services
to apply generated API over multiple communication protocols.
Currently supported languages are: Python, Go and C++.
Currently implemented protocols are: Netconf, Restconf, OpenDaylight and gNMI.
YDK provides CRUD and protocol specific service over above protocols.
YDK also provides Codec services to translate API models to/from XML and JSON encoded strings.

Other tools and libraries are used to deliver `YDK` functionality:

* YANG model analysis and code generation is implemented using APIs from the [pyang](https://github.com/mbj4668/pyang) library
* Documentation is generated using [Sphinx](http://www.sphinx-doc.org/en/stable/)
* Runtime YANG model analysis is done using [libyang](https://github.com/CESNET/libyang)
* C++ to python bindings are created using [pybind11](https://github.com/pybind/pybind11)
* C++ uses [catch](https://github.com/catchorg/Catch2) and [spdlog](https://github.com/gabime/spdlog) for tests and logging respectively

The output of ydk-gen is either a core package, that defines main services and providers,
or add-on service package like gNMI Service, or a module bundle, consisting of programming language APIs derived from YANG models.
Each module bundle is generated using a bundle profile and the ydk-gen tool.
Developers can either use pre-packaged generated bundles (e.g. [ydk-py](http://cs.co/ydk-py)),
or define their own bundle, consisting of a set of YANG models, using a bundle profile
(e.g. [`ietf_0_1_1.json`](profiles/bundles/ietf_0_1_1.json)).
This gives the developer an ability to customize scope of their bundle based on their requirements.


# Backward compatibility

The YDK-0.8.5 core is backward compatible with all previously generated model bundles starting from release of YDK-0.7.3.
However the YDK-0.8.5 generates different code and model API comparing to YDK-0.8.4.
The YDK-0.8.5 generated code is not compatible with YDK-0.7.2 and earlier bundle packages due to changes in modeling and handling YList objects.


# Docker

A [docker image](https://docs.docker.com/engine/reference/run/) is automatically built with the latest ydk-gen commit.
This docker can be used to run ydk-gen without installing anything natively on your machine.

To use the docker image, [install docker](https://docs.docker.com/install/) on your system and run the below command.
See the [docker documentation](https://docs.docker.com/engine/reference/run/) for more details.

```
docker run -it ydkdev/ydk-gen
```

# System requirements

Please follow the below instructions to install the system requirements before installing YDK-Py/YDK-Cpp/YDK-Go.

**Please note**. If you are using the latest ydk-gen master branch code, you may not be able to use prebuilt libraries and packages.
In this case you need to build all the components [from source](#build-from-source) after installing the below requirements:

## Linux

### Ubuntu (Debian-based)

#### Install OS dependency packages

```
sudo apt-get install gdebi-core python3-dev libtool-bin
sudo apt-get install libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev cmake

# Upgrade compiler to gcc 5.*
sudo apt-get install gcc-5 g++-5 -y > /dev/null
sudo ln -sf /usr/bin/g++-5 /usr/bin/g++
sudo ln -sf /usr/bin/gcc-5 /usr/bin/gcc
```

#### Install libydk library

You can install the latest `libydk` core library using prebuilt binaries for Xenial and Bionic distributions.
The C++ code was compiled with default gcc compiler version for these distributions.
For other Ubuntu distributions and/or gcc compiler versions it is recommended to build `libydk` library [from source](#build-from-source).

##### Xenial (Ubuntu 16.04.4, gcc-5.5.0):

```
wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/xenial/libydk-0.8.5-1.amd64.deb
sudo gdebi libydk-0.8.5-1.amd64.deb
```

##### Bionic (Ubuntu 18.04.1, gcc-7.4.0):

```
wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/bionic/libydk-0.8.5-1.amd64.deb
sudo gdebi libydk-0.8.5-1.amd64.deb
```

### CentOS-7 (Fedora-based)

#### Install OS dependency packages

```
sudo yum install epel-release
sudo yum install libssh-devel gcc-c++ python3-devel
```

if your gcc compiler version is below 4.8.1, install gcc-5 and g++-5

```
sudo yum install centos-release-scl -y > /dev/null
sudo yum install devtoolset-4-gcc* -y > /dev/null
sudo ln -sf /opt/rh/devtoolset-4/root/usr/bin/gcc /usr/bin/gcc
sudo ln -sf /opt/rh/devtoolset-4/root/usr/bin/g++ /usr/bin/g++
```

#### Install libydk library

The C++ code was compiled with default gcc compiler version, which is 4.8.5. For other gcc compiler versions it is recommended to build `libydk` library [from source](#build-from-source).

```  
sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.8.5/libydk-0.8.5-1.x86_64.rpm
```

## Mac OSX

It is recommended to install [homebrew](http://brew.sh) and Xcode command line tools on your system before installing YDK software.

#### Install OS dependency packages

```
xcode-select --install
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install pkg-config libssh xml2 curl pcre cmake libxml2 doxygen libgcrypt
brew install python pybind11
```

#### Install libydk package

The prebuilt `libydk` package was compiled in MacOS-10.11.6 with clang-8.0.0 compiler.
For other C++ compilers it is recommended to build `libydk` [from source](#build-from-source).

```
curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.8.5/libydk-0.8.5-Darwin.pkg
sudo installer -pkg libydk-0.8.5-Darwin.pkg -target /
```

## Libssh installation

Please note that libssh-0.8.0 `does not support <http://api.libssh.org/master/libssh_tutor_threads.html>`_ separate threading library,
which is required by YDK. Therefore, if after installation of libssh package you find that the `libssh_threads.a` library is missing,
please downgrade the installation of libssh to version 0.7.6, or upgrade to 0.8.1 or higher.

**Note for MacOS**
Before installing `libssh` make sure the environment for `openssl` is setup:

```
brew reinstall openssl
export OPENSSL_ROOT_DIR=/usr/local/opt/openssl
```

Download libssh-0.7.6 source code, compile it and install:

```
wget https://git.libssh.org/projects/libssh.git/snapshot/libssh-0.7.6.tar.gz
tar zxf libssh-0.7.6.tar.gz && rm -f libssh-0.7.6.tar.gz
mkdir libssh-0.7.6/build && cd libssh-0.7.6/build
cmake ..
sudo make install
```

## Windows

From release ``0.6.0`` the YDK is not supported on Windows.

## gNMI Requirements

In order to enable YDK support for gNMI protocol, which is optional, the following third party software
must be installed prior to gNMI YDK component installation.

### Install protobuf and protoc

```
wget https://github.com/google/protobuf/releases/download/v3.5.0/protobuf-cpp-3.5.0.zip
unzip protobuf-cpp-3.5.0.zip
cd protobuf-3.5.0
./configure
make
sudo make install
sudo ldconfig
```

### Install gRPC

```
git clone -b v1.9.1 https://github.com/grpc/grpc
cd grpc
git submodule update --init
make
sudo make install
sudo ldconfig
cd -
```

### Install YDK gNMI library

#### Ubuntu

##### Xenial (Ubuntu 16.04.4, gcc-5.5.0):

```
wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/xenial/libydk_gnmi-0.4.0-5.amd64.deb
sudo gdebi libydk_gnmi-0.4.0-5.amd64.deb
```

##### Bionic (Ubuntu 18.04.1, gcc-7.4.0))

```
wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/bionic/libydk_gnmi-0.4.0-5.amd64.deb
sudo gdebi libydk_gnmi-0.4.0-5.amd64.deb
```

#### CentOS

The `libydk_gnmi` package was compiled with gcc-4.8.5.

```
sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.8.5/libydk_gnmi-0.4.0-5.x86_64.rpm
```

#### MacOS:

The prebuilt `libydk_gnmi` package was compiled in MacOS-10.11.6 with clang-8.0.0 compiler.
For other C++ compilers it is recommended to build `libydk` library [from source](#build-from-source)

```
curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.8.5/libydk_gnmi-0.4.0-5.Darwin.pkg
sudo installer -pkg libydk_gnmi-0.4.0-5.Darwin.pkg -target /
```

### Runtime environment

There is an open issue with gRPC on Centos/Fedora, which requires an extra step before running any YDK gNMI application.
See this issue on [GRPC GitHub](https://github.com/grpc/grpc/issues/10942#issuecomment-312565041) for details.
As a workaround, the YDK based application runtime environment must include setting of `LD_LIBRARY_PATH` variable:

```
PROTO="/Your-Protobuf-and-Grpc-installation-directory"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROTO/grpc/libs/opt:$PROTO/protobuf-3.5.0/src/.libs:/usr/local/lib64
```

## Build from source

Install dependencies OS dependencies then generate and install YDK C++ libraries; you might need
sudo access to install the libraries in default locations, which are `/usr/local/lib` and `/usr/local/include`.

```
# Clone ydk-gen from GitHub
git clone https://github.com/CiscoDevNet/ydk-gen.git -b 0.8.5
cd ydk-gen
pip install -r requirements.txt

# Generate and install libydk library
./generate -is --cpp --core

# Generate and install libydk_gnmi library (optional)
./generate -is --cpp --service profiles/services/gnmi-0.4.0.json
```

# YDK Python Installation

## Setting up Python virtual environment

We recommend that you run `ydk-gen` under Python virtual environment.

### Create Python virtual environment

To install Python virtual environment in your system, execute:

#### Linux OS

##### Python 3.5 and up

```
python3 -m venv /your/virtualenv/directory
```

##### Python 3.4 and below

```
pip install virtualenv
virtualenv -p python2.7 /your/virtualenv/directory
```

#### MacOS

```
pip3 install virtualenv
virtualenv -p python3 /your/virtualenv/directory
```

### Activate just created virtual environment

```
source /your/virtualenv/directory/bin/activate
```

### Exit virtual environment

```
deactivate
```

## Clone ydk-gen and install the requirements

```
git clone https://github.com/CiscoDevNet/ydk-gen.git
cd ydk-gen
pip install -r requirements.txt
```

## Generate YDK components
All the YDK components/packages can be generated by using Python script `generate.py`.

```
./generate.py --help
usage: generate.py [-h] [-l] [--core] [--service SERVICE] [--bundle BUNDLE]
                   [--adhoc-bundle-name ADHOC_BUNDLE_NAME]
                   [--adhoc-bundle ADHOC_BUNDLE [ADHOC_BUNDLE ...]]
                   [--generate-meta] [--generate-doc] [--generate-tests]
                   [--output-directory OUTPUT_DIRECTORY] [--cached-output-dir]
                   [-p] [-c] [-g] [-v] [-o]

Generate YDK artifacts:

optional arguments:
  -h, --help            show this help message and exit
  -l, --libydk          Generate libydk core package
  --core                Generate and/or install core library
  --service SERVICE     Location of service profile JSON file
  --bundle BUNDLE       Location of bundle profile JSON file
  --adhoc-bundle-name ADHOC_BUNDLE_NAME
                        Name of the adhoc bundle
  --adhoc-bundle ADHOC_BUNDLE [ADHOC_BUNDLE ...]
                        Generate an SDK from a specified list of files
  --generate-meta       Generate meta-data for Python bundle
  --generate-doc        Generate documentation
  --generate-tests      Generate tests
  --output-directory OUTPUT_DIRECTORY
                        The output directory where the sdk will get created.
  --cached-output-dir   The output directory specified with --output-directory
                        includes a cache of previously generated gen-
                        api/<language> files under a directory called 'cache'.
                        To be used to generate docs for --core
  -p, --python          Generate Python SDK
  -c, --cpp             Generate C++ SDK
  -g, --go              Generate Go SDK
  -v, --verbose         Verbose mode
  -o, --one-class-per-module
                        Generate separate modules for each python class
                        corresponding to yang containers or lists.
```
The below steps specify how to use `generate.py` to generate YDK core, model bundle, and service packages. All these packages are available for Python, Go and C++ in corresponding github repositories: [ydk-py](https://github.com/CiscoDevNet/ydk-py),  [ydk-go](https://github.com/CiscoDevNet/ydk-go) and [ydk-cpp](https://github.com/CiscoDevNet/ydk-cpp).

The script [create_ydk_sdk_for_github.sh](create_ydk_sdk_for_github.sh) can be used to generate the `ydk-py`, `ydk-cpp` and `ydk-go` repositories after having generated all the bundles and core packages using `generate.py`.

## Generate and install the core

Some model bundles have bin packaged and published in [Pypi](https://pypi.org) repository. These bundles can be installed with `pip` utility. For example, when executing `pip install ydk-models-cisco-ios-xr`, you will install the latest released in PyPi IOS XR device package.

**Note:**
There usually would have been changes on the master branch since the last [released version](https://github.com/CiscoDevNet/ydk-py/releases). To install the latest code at your own risk, you need to follow the below steps in the exact order.

First generate and install `libydk`

```
./generate.py --libydk
cd gen-api/cpp/ydk/build
make
[sudo] make install
```

To create the libydk binary package to use for later installation, run the below command

```
[sudo] make package
```

For Python:

```
./generate.py --python --core
pip install gen-api/python/ydk/dist/ydk*.tar.gz
```

For Go:

```
export $GOPATH=/your-go-path-installation-directory
./generate.py --go --core
```

## Build model bundle profile

The first step in using ydk-gen is either using one of the already built [bundle profiles](https://github.com/CiscoDevNet/ydk-gen/tree/master/profiles/bundles) or constructing your own bundle profile, consisting of the YANG models you are interested to include into the bundle:

Construct a bundle profile file, such as [```ietf_0_1_1.json```](profiles/bundles/ietf_0_1_1.json) and specify its dependencies.

A sample bundle profile file is described below. The file is in a JSON format. Specify the `"name"` of your bundle, the `"version"` of the bundle and the `"core_version"`, which refers to [the version](https://github.com/CiscoDevNet/ydk-gen/releases) of the ydk core package you want to use with this bundle. The `"name"` of the bundle here is especially important as this will form part of the installation path of the bundle.

```
{
    "name":"cisco-ios-xr",
    "version": "6.5.3",
    "core_version": "0.8.5",
    "Author": "Cisco",
    "Copyright": "Cisco",
    "Description": "Cisco IOS-XR Native Models From Git",
```

The `"models"` section of the profile describes sources of YANG models. It could contain combination of elements:

- `"dir"` - list of **relative** directory paths containing YANG files
- `"file"` - list of **relative** YANG file paths
- `"git"` - git repository, where YANG files are located

The sample below shows the use of git sources only. Other examples can be found in `profiles` directory README.md.

Each `"git"` source must specify `"url"` - git repository URL, and `"commits"` list. The specified URL must allow the repository
to be cloned without user intervention. Each element in `"commits"` list can specify:

- `"commitid"` - optional specification of a commit ID in string format. If not specified the HEAD revision is assumed.
The further specified directories and files will be copied from the context of this commit.
- `"dir"` - optional list of **relative** directory paths within the git repository.
All `*.yang` files in specified directory **and any sub-directories** will be pulled into the generated bundle.
- `"file"` - optional list of **relative** `*.yang` file paths within the git repository.

Only directory examples are shown in this example.

```
    "models": {
        "git": [
            {
                "url": "https://github.com/YangModels/yang.git",
                "commits": [
                  {
                    "dir": [
                        "vendor/cisco/xr/653"
                    ]
                  }
                ]
            },
            {
                "url": "https://github.com/YangModels/yang.git",
                "commits": [
                  {
                    "commitid": "f6b4e2d59d4eedf31ae8b2fa3119468e4c38259c",
                    "dir": [
                        "experimental/openconfig/bgp",
                        "experimental/openconfig/policy"
                    ]
                  }
                ]
            }
        ]
    },
```

## Generate and install model bundle

Generate model bundle using a bundle profile and install it.

For Python:

```
./generate.py --python --bundle profiles/bundles/<name-of-profile>.json
[sudo] pip install gen-api/python/<name-of-bundle>-bundle/dist/ydk*.tar.gz
```

or using installation options:

```
./generate.py --bundle profiles/bundles/<name-of-profile>.json -i [-s]
```

Now, the `pip list | grep ydk` should show the `ydk` (referring to the core package) and `ydk-<name-of-bundle>` packages installed:

```
pip list | grep ydk
...

ydk (0.8.5)
ydk-models-<name-of-bundle> (0.5.1)
...
```

For Go:

```
export $GOPATH=/your-go-path-installation-directory
./generate.py --go --bundle profiles/bundles/<name-of-profile>.json -i
```

For C++:

```
./generate.py --cpp --bundle profiles/bundles/<name-of-profile>.json
cd gen-api/cpp/<name-of-bundle>-bundle/build
make
[sudo] make install
```

or using installation options:

```
./generate.py --cpp --bundle profiles/bundles/<name-of-profile>.json -i [-s]
```

## Writing your first app

Now, you can start creating apps based on the models in your bundle.
Assuming you generated a python bundle, the models will be available for importing in your app under
`ydk.models.<name-of-your-bundle>`.
For examples, see [ydk-py-samples](https://github.com/CiscoDevNet/ydk-py-samples#a-hello-world-app) and
[C++ samples](sdk/cpp/samples).
Also refer to the [documentation for python](http://ydk.cisco.com/py/docs/developer_guide.html),
[Go](http://ydk.cisco.com/go/docs/developer_guide.html) and
[for C++](http://ydk.cisco.com/cpp/docs/developer_guide.html).

## Documentation

When generating the YDK documentation for several bundles and the core, it is recommended to generate the bundles without the `--generate-doc` option.
After generating all the bundles, the combined documentation for all the bundles and the core can be generated using the `--core --generate-doc` option.
For example, the below sequence of commands will generate the documentation for the three python bundles and the python core
(for C++, use `--cpp`; for Go, use `--go`).

Note that the below process could take few hours due to the size of the `cisco_ios_xr` bundle.

```
./generate.py --python --bundle profiles/bundles/ietf_0_1_1.json
./generate.py --python --bundle profiles/bundles/openconfig_0_1_1.json
./generate.py --python --bundle profiles/bundles/cisco_ios_xr_6_1_1.json
./generate.py --python --core --generate-doc
```

If you have previously generated documentation, using the `--cached-output-dir --output-directory <dir>` option can be used to reduce document generation time. Taking Python as an example:

```
mkdir gen-api/cache
mv gen-api/python gen-api/cache

./generate.py --python --bundle profiles/bundles/ietf_0_1_5.json
./generate.py --python --bundle profiles/bundles/openconfig_0_1_5.json
./generate.py --python --bundle profiles/bundles/cisco_ios_xr_6_3_2.json
./generate.py --python --core --generate-doc --output-directory gen-api --cached-output-dir -v
```

Pre-generated documentation for [ydk-py](http://ydk.cisco.com/py/docs/) and [ydk-cpp](http://ydk.cisco.com/cpp/docs/) are available.

# Generating an "Adhoc" YDK-Py Bundle

The ability to generate an adhoc bundle directly from the command line and without creating a bundle file can be done something like this:

```
./generate.py --adhoc-bundle-name test --adhoc-bundle \
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-ipv4-bgp-oper*.yang \
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-types.yang
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-ipv4-bgp-datatypes.yang
```

When run in this way, we will generate a bundle that only contains the files specified with the `--adhoc-bundle` option, creating a `pip` package name by the `--adhoc-bundle-name`, with a version `0.1.0` and a dependency on the base IETF bundle. Note that **all** dependencies for the bundle must be listed, and the expectation is that this option will typically be used for generating point YDK-Py bundles for specific testing, the `--verbose` option is automatically enabled to quickly and easily let a user see if dependencies have been satisfied.

# Notes

## Python Requirements

YDK supports both Python2 and Python3 versions.  At least Python2.7 or Python3.4 along with corresponding utilities pip and pip3 must be installed on your system.

It is also required for Python installation to include corresponding shared library. As example:

 - python2.7  - /usr/lib/x86_64-linux-gnu/libpython2.7.so
 - python3.5m - /usr/lib/x86_64-linux-gnu/libpython3.5m.so

Please follow [System requirements](#system-requirements) to assure presence of shared Python libraries.

In some OS configurations during YDK package installation the `cmake` fails to find C/C++ headers for installed YDK libraries.
In this case the header location must be specified explicitly:

```
export C_INCLUDE_PATH=/usr/local/include
export CPLUS_INCLUDE_PATH=/usr/local/include
```

### Mac OS

The developers of Python2 on Mac OS might face an issue ([#837](https://github.com/CiscoDevNet/ydk-gen/issues/837)).
This is well known and documented issue. Each developer might have different approaches for its resolution.
One of them is to use Python2 virtual environment. See [Setting up Python virtual environment](#setting-up-python-virtual-environment) for details.

## Directory structure

```
README          - install and usage notes
gen-api         - generated bundle/core
                    - python (Python SDK)
                    - go (Go SDK)
                    - cpp (C++ SDK)

generate.py     - script used to generate SDK for yang models
profiles        - profile files used during generation
yang            - some yang models used for testing
requirements.txt- python dependencies used during installation
sdk             - sdk core and stubs for python and cpp
test            - test code
```

## Troubleshooting

Sometimes, developers using ydk-gen may run across errors when generating a YDK bundle using generate.py with some yang models. If there are issues with the .json profile file being used, such errors will be easily evident. Other times, when the problem is not so evident, it is recommended to try running with the `[--verbose|-v]` flag, which may reveal syntax problems with the yang models being used. For example,

```
./generate.py --python --bundle profiles/bundles/ietf_0_1_1.json --verbose
```

Also, it may be a good idea to obtain a local copy of the yang models and compile them using `pyang` to ensure the validity of the models,
```
cd /path/to/yang/models
pyang *.yang
```

# Running Unit Tests

## Python

#### Install the core and bundle packages

After creating and activating [virtual environment](#setting-up-python-virtual-environment) install Python dependencies:

```
cd ydk-gen
pip install -r requirements.txt
```
Then install core and bundle packages:

```
cd ydk-gen
./generate.py -i --core
./generate.py -i --bundle profiles/test/ydktest-cpp.json
./generate.py -i --bundle profiles/test/ydktest-oc-nis.json
```

#### Start confd

```
source /confd/installation/directory/confdrc
cd ydk-gen/sdk/cpp/core/tests/confd/ydktest
make all
make start
```

#### Run unit tests

```
cd ydk-gen/sdk/python
python test/test_sanity_types.py
python test/test_sanity_levels.py
python test/test_sanity_filters.py
```

## C++

#### Install the core and bundle packages

```
cd ydk-gen
./generate.py -is --core --cpp
./generate.py -is --bundle profiles/test/ydktest-cpp.json --cpp
./generate.py -is --bundle profiles/test/ydktest-oc-nis.json --cpp
```

#### Run the core tests

```
cd ydkgen/gen-api/cpp/ydk/build
./test/ydk_core_test
```

#### Start confd

```
source /confd/installation/directory/confdrc
cd ydk-gen/sdk/cpp/core/tests/confd/ydktest
make all
make start
```

#### Build and run bundle tests

```
cd ydk-gen/sdk/cpp/tests
mkdir build && cd build
cmake .. && make
./ydk_bundle_test
```

## Go

Please refer [here](https://github.com/CiscoDevNet/ydk-gen/blob/master/sdk/go/core/README.md).

#### Support

Join the [YDK community](https://communities.cisco.com/community/developer/ydk) to connect with other users and with the makers of YDK.

#### Release Notes

The current YDK release version is 0.8.5. The version of the latest YDK-Gen master branch is 0.8.5.
YDK-Gen is licensed under the Apache 2.0 License.
