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
  - [macOS](#macos)
  - [Windows](#windows)
- [Installation](#installation)
  - [Setting up your environment](#setting-up-your-environment)
  - [Clone ydk-gen and install the requirements](#clone-ydk-gen-and-install-the-requirements)
- [Usage](#usage)
  - [First step: choose your bundle profile](#first-step-choose-your-bundle-profile)
    - [Details](#details)
  - [Second step: Generate & install the core](#second-step-generate--install-the-core)
  - [Third step: Generate & install your bundle](#third-step-generate--install-your-bundle)
  - [Fourth step: Writing your first app](#fourth-step-writing-your-first-app)
  - [Documentation](#documentation)
- [Generating an "Adhoc" YDK-Py Bundle](#generating-an-adhoc-ydk-py-bundle)
- [Notes](#notes)
  - [Python version](#python-version)
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

**ydk-gen** is a developer tool that can generate API's that are modeled in YANG. Currently, it generates language binding for Python, Go and C++ with planned support for other language bindings in the future.

Other tools and libraries are used to deliver `ydk-gen`'s functionality. In particular:

* YANG model analysis and code generation is implemented using APIs from the [pyang](https://github.com/mbj4668/pyang) library
* Documentation is generated using [Sphinx](http://www.sphinx-doc.org/en/stable/)
* Run time yang model analysis is done using [libyang](https://github.com/CESNET/libyang)
* C++ to python bindings are created using [pybind11](https://github.com/pybind/pybind11)
* C++ uses [catch](https://github.com/catchorg/Catch2) and [spdlog](https://github.com/gabime/spdlog) for tests and logging respectively

Of course, many other libraries are used as an integral part of ydk-gen and it's dependencies, too many to mention!

The output of ydk-gen is either a core package, that defines services and providers, or a module bundle, consisting of APIs based on YANG models. Each module bundle is generated using a bundle profile and the ydk-gen tool. Developers can either use pre-packaged generated bundles (e.g. [ydk-py](http://cs.co/ydk-py)), or they can define their own bundle, consisting of a set of YANG models, using a bundle profile (e.g. [```ietf_0_1_1.json```](profiles/bundles/ietf_0_1_1.json)). This gives a developer the ability to customize the scope of their bundle based on their requirements.


# Backward compatibility
 Please see [this page](http://ydk.cisco.com/py/docs/guides/backward_compatibility.html) for details on some backward incompatible changes introduced as part of the 0.6.0 release. Note also that [#604](https://github.com/CiscoDevNet/ydk-gen/issues/604) and [#748](https://github.com/CiscoDevNet/ydk-gen/issues/748) introduced backward incompatibility for python. The bundles generated with `0.7.1` or newer ydk-gen will only work with ydk `core` version `0.7.1` or newer. Also error types for python were renamed from `YPYError` to `YError`.

# Docker

A [docker image](https://docs.docker.com/engine/reference/run/) is automatically built with the latest ydk-gen installed. This be used to run ydk-gen without installing anything natively on your machine.

To use the docker image, [install docker](https://docs.docker.com/install/) on your system and run the below command. See the [docker documentation](https://docs.docker.com/engine/reference/run/) for more details.

```
docker run -it ydkdev/ydk-gen
```

# System requirements
Please follow the below instructions to install the system requirements before installing YDK-Py/YDK-Cpp. **Please note** that if you are using the latest ydk-gen master branch code, you may not be able to use the below prebuilt `libydk` binaries. You need to [build libydk from source](#second-step-generate--install-the-core) after installing the below requirements:

## Linux
Ubuntu (Debian-based):

**Install prebuilt libydk binary:**
```
   $ sudo apt-get install gdebi-core python3-dev python-dev libtool-bin
   $ wget https://devhub.cisco.com/artifactory/debian-ydk/0.7.2/libydk_0.7.2-1_amd64.deb
   $ sudo gdebi libydk_0.7.2-1_amd64.deb
```
**To build from source:**
```
$ sudo apt-get install libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev libtool-bin cmake python3-dev python-dev
```
Centos (Fedora-based):

**Install prebuilt libydk binary:**
```
   $ sudo yum install epel-release
   $ sudo yum install libssh-devel gcc-c++
   $ sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.7.2/libydk-0.7.2-1.x86_64.rpm

   # Upgrade compiler to gcc 5.*
   $ yum install centos-release-scl -y > /dev/null
   $ yum install devtoolset-4-gcc* -y > /dev/null
   $ ln -sf /opt/rh/devtoolset-4/root/usr/bin/gcc /usr/bin/cc
   $ ln -sf /opt/rh/devtoolset-4/root/usr/bin/g++ /usr/bin/c++
```
**To build from source:**
```
   $ sudo yum install epel-release
   $ sudo yum install libxml2-devel libxslt-devel libssh-devel libtool gcc-c++ pcre-devel cmake3 clang libcurl-devel rpm-build redhat-lsb

   # Upgrade compiler to gcc 5.*
   $ yum install centos-release-scl -y > /dev/null
   $ yum install devtoolset-4-gcc* -y > /dev/null
   $ ln -sf /opt/rh/devtoolset-4/root/usr/bin/gcc /usr/bin/cc
   $ ln -sf /opt/rh/devtoolset-4/root/usr/bin/g++ /usr/bin/c++
```

## macOS
It is recommended to install [homebrew](http://brew.sh) and Xcode command line tools on your system before installing YDK-Py/YDK-Cpp.

You can download the latest python package from [here](https://www.python.org/downloads/). Please do not use the homebrew version of python as it causes issues with installing ydk packages. Please execute `brew rm python python3` to remove any homebrew python packages.

**Install prebuilt libydk binary:**
```
   $ xcode-select --install
   $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
   $ brew install pkg-config libssh xml2 curl pcre cmake libxml2 pybind11
   $ curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.7.2/libydk-0.7.2-Darwin.pkg
   $ sudo installer -pkg libydk-0.7.2-Darwin.pkg -target /
```
**To build from source:**
```
   $ xcode-select --install
   $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
   $ brew install pkg-config libssh xml2 curl pcre cmake
```

## Windows
Currently, ``YDK-Py`` and ``YDK-Cpp`` from release ``0.6.0`` onwards is not supported on Windows.

# Installation
## Setting up your environment

We recommend that you run ydk-gen under a Python virtual environment (``virtualenv``/``virtualenvwrapper``).  To install support in your system, execute

```
  $ pip install virtualenv virtualenvwrapper
  $ source /usr/local/bin/virtualenvwrapper.sh
```

In some systems (e.g. Debian-based Linux), you may need to install support for Python virtual environments as root

```
  $ sudo pip install virtualenv virtualenvwrapper
  $ source /usr/local/bin/virtualenvwrapper.sh
```

At this point, create a new virtual environment

```
  $ mkvirtualenv -p python2.7 py2
```
## Clone ydk-gen and install the requirements
```
$ git clone https://github.com/CiscoDevNet/ydk-gen.git
$ cd ydk-gen
$ pip install -r requirements.txt
```

# Usage

```
$ ./generate.py --help
Usage: generate.py [options]

Options:
  --version           show program's version number and exit
  -h, --help          show this help message and exit
  -p, --python        Select Python language. This is currently the default option
  -l, --libydk        Generate the libydk core package (required for using YDK Python, Go, C++)
  -c, --cpp           Select C++ language
  -g, --go            Select Go language
  --core              Generate the core for the selected language
  --bundle=PROFILE    Generate the bundle for the selected language
  --one-module-per-class
                      Generate python bundles by creating separate python modules for each class
  -v, --verbose       Verbose mode
  --generate-doc      Generation documentation
  --output-directory  The output-directory . If not specified the output can be found under `ydk-gen/gen-api/python`
```
The below steps specify how to use `ydk-gen` to generate the core and a bundle. Pre-generated bundles and core are available for python, go and C++: [ydk-py](https://github.com/CiscoDevNet/ydk-py),  [ydk-go](https://github.com/CiscoDevNet/ydk-go) and [ydk-cpp](https://github.com/CiscoDevNet/ydk-cpp). The script [create_ydk_sdk_for_github.sh](create_ydk_sdk_for_github.sh) can be used to generate the `ydk-py`, `ydk-cpp` and `ydk-go` repositories after having generated all the bundles and core packages using `generate.py`.

## First step: choose your bundle profile

The first step in using ydk-gen is either using one of the already existing [bundle profiles](https://github.com/CiscoDevNet/ydk-gen/tree/master/profiles/bundles) or constructing your own bundle profile, consisting of the YANG models you are interested in:

Construct a bundle profile file, such as [```ietf_0_1_1.json```](profiles/bundles/ietf_0_1_1.json) and specify its dependencies.

### Details

A sample bundle profile file is described below. The file is in a JSON format. Specify the `name` of your bundle, the `version` of the bundle and the `ydk_version`, which refers to [the version](https://github.com/CiscoDevNet/ydk-gen/releases) of the ydk core package you want to use with this bundle. The `name` of the bundle here is especially important as this will form part of the installation path of the bundle.

```
{
    "name":"cisco-ios-xr",
    "version": "0.1.0",
    "ydk_version": "0.5.0",
    "Author": "Cisco",
    "Copyright": "Cisco",
    "Description": "Cisco IOS-XR Native Models From Git",
```

The "models" section of the file describes where to source models from. There are 3 sources:

- Directories
- Specific files
- Git, within which specific relative directories and files may be referenced

The sample below shows the use of git sources only.

```
    "models": {
        "git": [
```

We have a list of git sources. Each source must specify a URL. This URL should be one that allows the repository to be cloned without requiring user intervention, so please use a public URL such as the example below. There are three further options that can be specified:

- ```commitid``` - Optional specification of a commit in string form. The files identified will be copied from the context of this commit.
- ```dir``` - List of **relative** directory paths within git repository. All .yang files in this directory **and any sub-directories** will be pulled into the generated bundle.
- ```file```- List of **relative** file paths within the git repository.

Only directory examples are shown below.

```
            {
                "url": "https://github.com/YangModels/yang.git",
                "dir": [
                    "vendor/cisco/xr/532"
                ]
            },
            {
                "url": "https://github.com/YangModels/yang.git",
                "commitid": "f6b4e2d59d4eedf31ae8b2fa3119468e4c38259c",
                "dir": [
                    "experimental/openconfig/bgp",
                    "experimental/openconfig/policy"
                ]
            }
        ]
    },
```

## Second step: Generate & install the core

On executing `pip install ydk-cisco-ios-xr`, you will be able to install the latest released PyPi version of all the `ydk` packages.

**Note:** 
There usually would have been changes on the master branch since the last [released version](https://github.com/CiscoDevNet/ydk-py/releases). To install the latest code at your own risk, you need to follow the below steps in the exact order.

First, generate the core and install it:

First generate and install ``libydk`` (**required for C++, Go or Python**):
```
$ ./generate.py --libydk
$ cd gen-api/cpp/ydk/build
$ make

# To create the libydk binary package to use for later installation, run the below command
$ make package

# To install the compiled libydk code, run the below command after the above
$ [sudo] make install
```

For python:
```
$ ./generate.py --python --core
$ pip install gen-api/python/ydk/dist/ydk*.tar.gz
```

For Go:
```
$ ./generate.py --go --core
$ cp -r gen-api/go/ydk/* $GOPATH/src/github.com/CiscoDevNet/ydk-go/ydk
```
## Third step: Generate & install your bundle
Then, generate your bundle using a bundle profile and install it:

For python:
```
$ ./generate.py --python --bundle profiles/bundles/<name-of-profile>.json
$ pip install gen-api/python/<name-of-bundle>-bundle/dist/ydk*.tar.gz
```

Now, doing `pip list` should show the `ydk` (refering to the core package) and `ydk-<name-of-bundle>` packages installed:
```
$ pip list
...
ydk (0.5.2)
ydk-models-<name-of-bundle> (0.5.1)
...
```

For Go:
```
$ ./generate.py --go --bundle profiles/bundles/<name-of-profile>.json
$ cp -r gen-api/go/<name-of-bundle>-bundle/ydk/models/*  $GOPATH/src/github.com/CiscoDevNet/ydk-go/ydk/models
```

For C++:
```
$ ./generate.py --cpp --bundle profiles/bundles/<name-of-profile>.json
$ cd gen-api/cpp/<name-of-bundle>-bundle/build && make && make install
```

## Fourth step: Writing your first app

Now, you can start creating apps based on the models in your bundle. Assuming you generated a python bundle, the models will be available for importing in your app under `ydk.models.<name-of-your-bundle>`. For examples, see [ydk-py-samples](https://github.com/CiscoDevNet/ydk-py-samples#a-hello-world-app) and [C++ samples](sdk/cpp/samples). Also refer to the [documentation for python](http://ydk.cisco.com/py/docs/developer_guide.html), [Go](http://ydk.cisco.com/go/docs/developer_guide.html) and [for C++](http://ydk.cisco.com/cpp/docs/developer_guide.html).

## Documentation

When generating the YDK documentation for several bundles and the core, it is recommended to generate the bundles without the `--generate-doc` option. After generating all the bundles, the combined documentation for all the bundles and the core can be generated using the `--core --generate-doc` option. For example, the below sequence of commands will generate the documentation for the three python bundles and the python core (for C++, use `--cpp`; for Go, use `--go`).

Note that the below process could take a few hours due to the size of the `cisco_ios_xr` bundle.

```
./generate.py --python --bundle profiles/bundles/ietf_0_1_1.json
./generate.py --python --bundle profiles/bundles/openconfig_0_1_1.json
./generate.py --python --bundle profiles/bundles/cisco_ios_xr_6_1_1.json
./generate.py --python --core --generate-doc
```

If you have previously generated documentation, using the `--cached-output-dir --output-directory <dir>` option can be used to reduce document generation time. Taking python as an example:

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
$ ./generate.py --adhoc-bundle-name test --adhoc-bundle \
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-ipv4-bgp-oper*.yang \
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-types.yang
    /opt/git-repos/clean-yang/vendor/cisco/xr/621/Cisco-IOS-XR-ipv4-bgp-datatypes.yang
```

When run in this way, we will generate a bundle that only contains tje files specified with the `--adhoc-bundle` option, creating a pip package name by the `--adhoc-bundle-name`, with a version `0.1.0` and a dependency on the base IETF bundle. Note that **all** dependencies for the bundle must be listed, and as the expectation is that this option will typically be used for generating point YDK-Py bundles for specific testing, the `--verbose` option is automatically enabled to quickly and easily let a user see if dependencies have been satisfied.

# Notes

## Python version

- If your environment has both python 2 and python 3 and uses python 2 by default, you may need to use `python3` and `pip3` instead of `python` and `pip` in the commands mentioned in this document.


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
Sometimes, developers using ydk-gen may run across errors when generating a YDK bundle using generate.py with some yang models. If there are issues with the .json profile file being used, such errors will be easily evident. Other times, when the problem is not so evident, it is recommended to try running with the `--verbose|-v` flag, which may reveal syntax problems with the yang models being used. For example,

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
First, generate and install the [test](profiles/test/ydktest.json) bundle and core package
```
$ ./generate.py --core
$ pip install gen-api/python/core/dist/ydk*.tar.gz

$ ./generate.py --bundle profiles/test/ydktest.json
$ pip install gen-api/python/ydktest-bundle/dist/ydk*.tar.gz
```

To run the sanity tests, do the following:

```
$ cd ydk-gen/sdk/python
$ python test/test_sanity_types.py
$ python test/test_sanity_levels.py
$ python test/test_sanity_filters.py
```

## C++
First, install the [test](profiles/test/ydktest-cpp.json) bundle and core package
```
$ ./generate.py --core --cpp
$ ./generate.py --bundle profiles/test/ydktest-cpp.json --cpp
$ cd gen-api/cpp/ydktest-bundle/build
$ sudo make install
$ cd -
```

To run the core and bundle tests, do the following

```
$ cd ydk-gen/sdk/cpp/ydk
$ mkdir build && cd build
$ cmake .. && sudo make all install test

$ cd ydk-gen/sdk/cpp/tests
$ mkdir build && cd build
$ cmake .. && make all test
```

## Go
Please refer [here](https://github.com/CiscoDevNet/ydk-gen/blob/master/sdk/go/core/README.md).

Support
=======
Join the [YDK community](https://communities.cisco.com/community/developer/ydk) to connect with other users and with the makers of YDK.

Release Notes
===============
The current YDK release version is 0.7.2 (alpha). The version of the latest YDK-Gen master branch is 0.7.3-dev. YDK-Gen is licensed under the Apache 2.0 License.
