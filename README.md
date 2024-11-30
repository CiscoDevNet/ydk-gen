<!---
# *************************************************************
#  YDK-YANG Development Kit
#  Copyright 2016-2019 Cisco Systems. All rights reserved
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

YANG Development Kit
====================

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
- [Generate YDK Components](#generate-ydk-components)
  - [Build model bundle profile](#build-model-bundle-profile)
  - [Generate and install model bundle](#generate-and-install-model-bundle)
  - [Writing your first app](#writing-your-first-app)
  - [Generating Documentation](#generating-documentation)
  - [Generating an "Adhoc" Bundle](#generating-an-adhoc-bundle)
- [Notes](#notes)
  - [Python requirements](#python-requirements)
  - [C++ requirements](#c-requirements)
  - [Directory structure](#directory-structure)
  - [Troubleshooting](#troubleshooting)
- [Running Unit Tests](#running-unit-tests)
  - [Python](#python)
  - [C++](#c)
  - [Go](#go)
- [Documentation and Support](#documentation-and-support)
- [Release Notes](#release-notes)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Overview

The YANG Development Kit (YDK) is a software development tool, which provides API for building applications based on YANG models.
The YDK allows generate YANG model API and provides services to apply generated API over various communication protocols.
Currently implemented protocols are: Netconf, Restconf, OpenDaylight and gNMI.
YDK provides CRUD and protocol specific services over above protocols.
YDK also provides codec services to translate API models to/from XML and JSON encoded strings.

The YDK is a multi-language software. Currently supported languages are Python, Go, and C++.

Other tools and libraries are used to deliver `YDK` functionality:
* YANG model analysis and code generation is implemented using APIs from the [pyang](https://github.com/mbj4668/pyang) library
* Documentation is generated using [Sphinx](http://www.sphinx-doc.org/en/stable/)
* Runtime YANG model analysis is done using [libyang](https://github.com/CESNET/libyang)
* C++ to python bindings are created using [pybind11](https://github.com/pybind/pybind11)
* C++ uses [catch](https://github.com/catchorg/Catch2) and [spdlog](https://github.com/gabime/spdlog) for tests and logging respectively

The output of ydk-gen is either a core package, that defines main services and providers,
or add-on service package like gNMI Service, or a module bundle, consisting of programming language APIs derived from YANG models.
Each module bundle is generated using a bundle profile and the ydk-gen tool.
Developers can either use pre-packaged generated bundles,
or define their own bundle, consisting of a set of YANG models, using a bundle profile
(e.g. [`ietf_0_1_5.json`](profiles/bundles/ietf_0_1_5.json)).
This gives the developer an ability to customize scope of their bundle based on their requirements.


# Backward Compatibility

The YDK-0.8.6 core is backward compatible with all previously generated model bundles starting from release of YDK-0.7.3.
However the YDK-0.8.6 generates different code and model API comparing to YDK-0.8.4.
The YDK-0.8.6 generated code is not compatible with YDK-0.7.2 and earlier bundle packages due to changes in modeling and handling of YList objects.

**NOTE.** Starting from release 0.8.6 the YDK does not support Python2 interpreter as it was deprecated.

# Docker

A [docker image](https://docs.docker.com/engine/reference/run/) is automatically built with the latest ydk-gen commit.
This docker can be used to run ydk-gen without installing anything natively on your platform.

To use the docker image, [install docker](https://docs.docker.com/install/) on your system and run the below command.
See the [docker documentation](https://docs.docker.com/engine/reference/run/) for more details.

```
docker run -it ydksolutions/ydk-gen:0.8.6.6
```

# System Requirements

The YDK is currently supported on the following platforms including native installations, virtual machines, and docker images:
 - Linux Ubuntu Xenial (16.04 LTS), Bionic (18.04 LTS), Focal (20.04 LTS), Jammy (22.04 LTS) and Noble (24.04 LTS)
 - Linux CentOS versions 7 and Centos Stream 8 (Centos 8.x has been EOL as of December 31 of 2021)
 - Linux RHEL version 7.x and 8.x 
 - MacOS up to 11.6.2 (Big Sur)

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

# Core Installation

## Installation script

For YDK installation it is recommended to use script `install_ydk.sh` from `ydk-gen` git repository.
The script detects platform OS, installs all the dependencies and builds complete set of YDK components for specified language.

If the script installs any YDK component, it also creates an environment activation file '.env' in the 'ydk-gen'
directory, which can be used to activate YDK runtime environment identical to the installation environment.
To activate YDK runtime environment simply run this command once in bash shell:

```
cd ydk-gen
source .env
```

The YDK extensively uses Python scripts for building its components and model API packages (bundles).
In order to isolate YDK Python environment from system installation, the script can build Python3 virtual environment.
If built, the user must manually activate virtual environment when generating model bundles and/or running YDK based application.
By default the Python virtual environment is installed under `$HOME/venv` directory.
For different location the PYTHON_VENV environment variable should be set to that location.

**NOTE.** It is strongly recommended to use Python virtual environment on Centos/RHEL and Mac platforms.

When installing YDK for Python or Go programming languages, the third party dependencies and C++ packages must be installed first.
This step requires sudo/root access to the installation platform.
Here is simple example of core YDK installation for Python programming language and Python virtual environment:

```
git clone https://github.com/ygorelik/ydk-gen.git
cd ydk-gen
export YDKGEN_HOME=`pwd`  # optional
export PYTHON_VENV=$HOME/ydk_vne  # optional
./install_ydk.sh --cpp --core --venv   # This step requires sudo access!
./install_ydk.sh --py --core --venv
```

The script also allows installing individual components like dependencies, core, and service packages
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

## Building from source

### Environment variables

In some OS configurations during YDK package installation the cmake fails to find C/C++ headers for previously installed YDK libraries.
In this case the header files location must be specified explicitly (in below commands the default location is shown):

```
  export C_INCLUDE_PATH=/usr/local/include
  export CPLUS_INCLUDE_PATH=/usr/local/include
```

When non-standard Python installation is used or there are multiple installations of Python on the platform,
the PATH and CMAKE_LIBRARY_PATH environment variables must be set accordingly in order for the installation scripts
to pick up correct Python binaries and shared libraries.

### Installing third party dependencies

If user platform is supported one, it is recommended to use `ydk-gen/install_ydk.sh` script. 
The script will also install Python virtual environment in default or specified location, when '--venv' is specified.

```
# Clone ydk-gen from GitHub
git clone https://github.com/ygorelik/ydk-gen.git
cd ydk-gen

# Define optional environment variables and install dependencies
export YDKGEN_HOME=`pwd`  
export PYTHON_VENV=$HOME/ydk_venv
./install_ydk.sh -v   # also builds Python virtual environment
```

For unsupported platforms it is recommended to follow logic of `ydk-gen/test/dependencies-*` scripts.
 
### Installing core components

```
# If created, activate YDK runtime environment
source .env

# Generate and install YDK core library
python3 generate.py -is --core --cpp

# For Python programming language add
python3 generate.py -i --core --py

# For Go programming language add
python3 generate.py -i --core --go
```

## Adding gNMI service

In order to enable YDK support for gNMI protocol, which is optional, the user need install third party software
and YDK gNMI service package. 

### gNMI service installation

Here is simple example how gNMI service package for Python virtual environment could be added:

```
cd ydk-gen
./install_ydk.sh --cpp --service gnmi -v  # requires sudo access
./install_ydk.sh --py --service gnmi -v
```

### Runtime environment

When YDK is installed using 'install_ydk.sh' script, the runtime environment is set by running 'source .env' in bash shell.
The below information is applicable only when YDK is installed manually, which is not recommended.

There is an open issue with gRPC on Centos/RHEL, which requires an extra step before running any YDK gNMI application.
See this issue on [GRPC GitHub](https://github.com/grpc/grpc/issues/10942#issuecomment-312565041) for details.
As a workaround, the YDK based application runtime environment must include setting of `LD_LIBRARY_PATH` variable:

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64
```

# Generate YDK components

All the YDK components/packages can be generated by using Python script `generate.py`. To get all of its options run:

```
cd ydk-gen
source .env
python3 generate.py --help
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
                        corresponding to YANG containers or lists.
```

The below steps specify how to use `generate.py` to generate YDK core, model bundle, and service packages.
In all the examples the script is executed from `ydk-gen` directory. 
It is assumed that Python virtual environment is activated.

## Build model bundle profile

The first step in using ydk-gen is either using one of the already built 
[bundle profiles](https://github.com/ygorelik/ydk-gen/tree/master/profiles/bundles) or constructing your own 
bundle profile, consisting of the YANG models you are interested to include into the bundle:

Construct a bundle profile file, such as [```ietf_0_1_5.json```](profiles/bundles/ietf_0_1_5.json)
and specify its dependencies.

A sample bundle profile file is described below. The file is in a JSON format. The profile must define the `"name"`, 
`"version"` and `"description"` of the bundle, and then the `"core_version"`, which refers to 
[the version](https://github.com/CiscoDevNet/ydk-gen/releases) of the ydk core package that you want to use with this bundle.
The `"name"` of the bundle will form part of the installation path of the bundle.
All other attributes, like `"author"` and `"copyright"`, are optional and will not affect the bundle generation. 

```
{
    "name":"cisco-ios-xr",
    "version": "6.5.3",
    "core_version": "0.8.6",
    "author": "Cisco",
    "copyright": "Cisco",
    "description": "Cisco IOS-XR Native Models From Git",
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
YDK runtime environment must be activated prior to these procedures.

### For Python

If applicable, the Python virtual environment must be activated prior to these procedures

```
python3 generate.py --python --bundle profiles/bundles/<name-of-profile>.json -i
```

Check Python packages installed:

```
pip list | grep ydk
ydk (0.8.6.6)
ydk-models-<name-of-bundle> (0.5.1)
...
```

### For Go

```
python3 generate.py --go --bundle profiles/bundles/<name-of-profile>.json -i
```

### For C++

```
python3 generate.py --cpp --bundle profiles/bundles/<name-of-profile>.json -is
```

## Writing your first app

Now, you can start creating apps based on the models in your bundle.
Assuming you have generated a python bundle, the models will be available for importing in your app under
`ydk.models.<name-of-your-bundle>`.
For examples, see [C++ samples](sdk/cpp/samples).
Also refer to the [documentation for python](http://ydk.cisco.com/py/docs/developer_guide.html),
[Go](http://ydk.cisco.com/go/docs/developer_guide.html) and
[C++](http://ydk.cisco.com/cpp/docs/developer_guide.html).

## Generating Documentation

In order to generate YDK core and bundles documentation, the `--generate-doc` option of `generate.py` script is used, 
while generating core package. Therefore the user should generate all the bundles without the `--generate-doc` 
option prior to the documentation generation.
For example, the below sequence of commands will generate the documentation for the three python bundles and the python core
(for C++, use `--cpp`; for Go, use `--go`).

```
python3 generate.py --python --bundle profiles/bundles/ietf_0_1_1.json
python3 generate.py --python --bundle profiles/bundles/openconfig_0_1_1.json
python3 generate.py --python --bundle profiles/bundles/cisco_ios_xr_6_1_1.json
python3 generate.py --python --core --generate-doc
```

**Note.** The documentation generation for bundles can take few hours due to their size. If you have previously 
generated documentation using the `--cached-output-dir --output-directory <dir>` option,
the add-on documentation generation time can be reduced. Adding cisco-ios-xr documentation as an example:

```
mkdir gen-api/cache
mv gen-api/python gen-api/cache

python3 generate.py --python --bundle profiles/bundles/cisco_ios_xr_6_6_3.json
python3 generate.py --python --core --generate-doc --output-directory gen-api --cached-output-dir
```

## Generating an "Adhoc" Bundle

When YANG models available on the hard drive, there is capability to generate small model bundles, which include
just few models. It is called an "adhoc" bundle. Such a bundle generated without profile directly from command line.
Here is simple example:

```
python3 generate.py -i --adhoc-bundle-name test --adhoc-bundle \
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-ipv4-bgp-oper*.yang \
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-types.yang
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-ipv4-bgp-datatypes.yang
```

This will generate a bundle that contains files specified in the `--adhoc-bundle` option and
create Python package `ydk-models-test-0.1.0.tar.gz`, which has dependency on the base IETF bundle.
Note that **all** dependencies for the bundle must be listed. It is expected that this option will be typically used
for generating point model bundles for specific testing. The `--verbose` option is automatically enabled to quickly
and easily let the user see if dependencies have been satisfied.

# Notes

## Python requirements

Starting from release 0.8.5 YDK supports only Python3 version.  At least Python 3.5 along with corresponding pip3 utility 
must be installed on your system. It is also required for Python installation to include corresponding shared 
library. As example:

 - python3.6m - /usr/lib/x86_64-linux-gnu/libpython3.6m.so

It is recommended to follow [Core Installation](#core-installation) procedure to ensure presence of shared Python libraries.

## C++ requirements

In some OS configurations during YDK package installation the `cmake` fails to find C/C++ headers for installed YDK libraries.
In this case the header location must be specified explicitly:

```
export C_INCLUDE_PATH=/usr/local/include
export CPLUS_INCLUDE_PATH=/usr/local/include
```

## Directory structure

```
gen-api         - generated code and packages for core and bundles
                    - python (Python SDK)
                    - go (Go SDK)
                    - cpp (C++ SDK)

3d_party        - suplemental code for third party software
profiles        - profile files used during generation
sdk             - sdk core and stubs for python, go and cpp
test            - dependencies and unit test shell scripts
yang            - some YANG models used for testing
ydkgen          - python code to extend generate.py script 

create_ydk_sdk_for_github.sh - convenience script to generate language specific repositories
generate_bundles.sh          - convinience script to generate core and bundle packages for deployment

generate.py     - script used to generate SDK for YANG models
install_ydk.sh  - YDK core components installation script
requirements.txt- python dependencies used during installation
README.md       - installation and usage notes
```

## Troubleshooting

Sometimes, developers using ydk-gen may run across errors when generating a YDK bundle using generate.py with some yang models.
If there are issues with the profile file being used, such JSON formatting errors will be evident.
Other times, when the problem is not so evident, it is recommended to try running the script with the
`[--verbose|-v]` flag, which may reveal syntax problems with the YANG models being used. For example:

```
python3 generate.py --python --bundle profiles/bundles/ietf_0_1_1.json --verbose
```

Also, it may be a good idea to obtain a local copy of the YANG models and compile them using `pyang` to ensure
the validity of the models and completeness of the model package:

```
cd /path/to/yang/models
pyang *.yang
```

## Running Unit Tests

The repository includes complete set of unit tests for all supported languages. To invoke the test for specific language,
simply get to 'ydk-gen' directory, activate runtime environment, if it was not done previously, and run language specific script.
The script will install core and bundle packages and then perform the unit tests.

**C++**

    ```
    cd ydk-gen
    source .env  # if previously not run
    ./test/run_cpp_tests
    ```

**Python**

    ```
    cd ydk-gen
    source .env  # if previously not run
    ./test/run_py_tests
    ```

**Go**

    ```
    cd ydk-gen
    source .env  # if previously not run
    ./test/run_go_tests
    ```

# Documentation and Support

<!--- - Read the online [YDK documentation](http://ydk.cisco.com/py/docs) (release 0.8.3) for details on how to use the YDK and API for specific models -->
- Check [GitHub Pages](https://ygorelik.github.io/ydk-gen/) for the latest YDK release documentation
- Find hundreds of sample apps in the [ydk-py-samples](https://github.com/CiscoDevNet/ydk-py-samples) repository
- Join the [YDK community](https://communities.cisco.com/community/developer/ydk) to connect with YDK users and developers

# Release Notes

The current YDK release version is 0.8.6.6.

YDK-Gen is licensed under the Apache 2.0 License.
