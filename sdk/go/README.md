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

# YANG Development Kit (Go)

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Overview](#overview)
- [Docker](#docker)
- [How to Install](#how-to-install)
  - [System Requirements](#system-requirements)
  - [gNMI Requirements](#gnmi-requirements)
- [Documentation and Support](#documentation-and-support)
- [Release Notes](#release-notes)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Overview

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.  

## Docker

Currently the [docker image](https://docs.docker.com/engine/reference/run/) for ydk-go is not been generated.
Please use [ydk-gen](https://github.com/ygorelik/ydk-gen/tree/0.8.5#docker>) docker image, which has complete environment for ydk-go based development.

<!---
A [docker image](https://docs.docker.com/engine/reference/run/) is automatically built with the latest ydk-go installed. This be used to run ydk-go without installing anything natively on your machine.

To use the docker image, [install docker](https://docs.docker.com/install/) on your system and run the below command. See the [docker documentation](https://docs.docker.com/engine/reference/run/) for more details.

```
  docker run -it ydkdev/ydk-go
```
-->

## How to Install

You can install YDK-Go on macOS or Linux.  It is not currently supported on Windows.

### System Requirements
#### Linux
##### Ubuntu (Debian-based)

The following packages must be present in your system before installing YDK-Go:

Install third-party dependency software:

```
  sudo apt-get install gdebi-core python3-dev python-dev libtool-bin
  sudo apt-get install libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev cmake
```

Install YDK core library:

For Xenial (Ubuntu 16.04.4):

```
  # Upgrade compiler to gcc 5.*
  sudo apt-get install gcc-5 g++-5 -y > /dev/null
  sudo ln -sf /usr/bin/g++-5 /usr/bin/g++
  sudo ln -sf /usr/bin/gcc-5 /usr/bin/gcc

  wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/xenial/libydk-0.8.5-1.amd64.deb
  sudo gdebi libydk-0.8.5-1.amd64.deb
```

For Bionic (Ubuntu 18.04.1):

```
  wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/bionic/libydk-0.8.5-1.amd64.deb
  sudo gdebi libydk-0.8.5-1.amd64.deb
```

#### Centos (Fedora-based)

The following packages must be present in your system before installing YDK-Go:

```
  sudo yum install epel-release
  sudo yum install libxml2-devel libxslt-devel libssh-devel libtool gcc-c++ pcre-devel cmake libstdc++-static git

  # Install gcc-5 and g++-5
  yum install centos-release-scl -y > /dev/null
  yum install devtoolset-4-gcc* -y > /dev/null
  ln -sf /opt/rh/devtoolset-4/root/usr/bin/gcc /usr/bin/gcc
  ln -sf /opt/rh/devtoolset-4/root/usr/bin/g++ /usr/bin/g++

  # Install YDK core library
  sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.8.5/libydk-0.8.5-1.x86_64.rpm
```

#### Mac OS

It is recommended to install [homebrew](http://brew.sh) and Xcode command line tools on your system before installing YDK-Go:

```
  /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  brew install pkg-config libssh libxml2 xml2 curl pcre cmake
  xcode-select --install

  # Install YDK core library
  curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.8.5/libydk-0.8.5-Darwin.pkg
  sudo installer -pkg libydk-0.8.5-Darwin.pkg -target /
```

#### Libssh installation

Please note that libssh-0.8.0 `does not support <http://api.libssh.org/master/libssh_tutor_threads.html>`_ separate threading library,
which is required for YDK. Therefore, if after installation of libssh package you find that the `libssh_threads.a` library is missing,
please downgrade the installation of libssh to version 0.7.6, or upgrade to 0.8.1 or higher. Example:

```
  wget https://git.libssh.org/projects/libssh.git/snapshot/libssh-0.7.6.tar.gz
  tar zxf libssh-0.7.6.tar.gz && rm -f libssh-0.7.6.tar.gz
  mkdir libssh-0.7.6/build && cd libssh-0.7.6/build
  cmake ..
  sudo make install
```

### Golang

The YDK requires Go version 1.9.2 or higher. If this is not the case, follow below installation steps. Make sure that environment variables GOROOT and GOPATH are properly set.

#### Linux

```
  sudo wget https://storage.googleapis.com/golang/go1.9.2.linux-amd64.tar.gz &> /dev/null
  sudo tar -zxf  go1.9.2.linux-amd64.tar.gz -C /usr/local/
  export GOROOT="/usr/local/go"
  export PATH=$GOROOT/bin:$PATH
```

#### Mac OS

```
  export CGO_ENABLED=0
  export GOROOT_BOOTSTRAP=$GOROOT
  gvm install go1.9.2
```

#### Runtime Setting

For security reasons starting from Go version 1.10 only a limited set of flags is allowed in the CGO code, notably -D, -I, and -l.
Current ydk-go code includes few additional CGO LDFLAGS flags in order to allow coverage testing; they are: "-fprofile-arcs -ftest-coverage --coverage". In order to allow these additional flags to be used, it is necessary to set environment variable CGO_LDFLAGS_ALLOW before running ydk-go based application:

```
  export CGO_LDFLAGS_ALLOW="-fprofile-arcs|-ftest-coverage|--coverage"
```

### gNMI Requirements

In order to enable YDK support for gNMI protocol, which is optional, the following third party software must be installed prior to gNMI YDK component installation.

#### Install protobuf and protoc

```
  wget https://github.com/google/protobuf/releases/download/v3.5.0/protobuf-cpp-3.5.0.zip
  unzip protobuf-cpp-3.5.0.zip
  cd protobuf-3.5.0
  ./configure
  make
  sudo make install
  sudo ldconfig
```

#### Install gRPC

```
  git clone -b v1.9.1 https://github.com/grpc/grpc
  cd grpc
  git submodule update --init
  make
  sudo make install
  sudo ldconfig
```

#### Install gNMI library

##### Linux

For Ubuntu/Xenial:

```
  wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/xenial/libydk_gnmi-0.4.0-5.amd64.deb
  sudo gdebi libydk_gnmi-0.4.0-5.amd64.deb
```

For Ubuntu/Bionic:

```
  wget https://devhub.cisco.com/artifactory/debian-ydk/0.8.5/bionic/libydk_gnmi-0.4.0-5.amd64.deb
  sudo gdebi libydk_gnmi-0.4.0-5.amd64.deb
```

For CentOS

```
  sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.8.5/libydk_gnmi-0.4.0-5.x86_64.rpm
```

##### MacOS:

```
  curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.8.5/libydk_gnmi-0.4.0-5.Darwin.pkg
  sudo installer -pkg libydk_gnmi-0.4.0-5.Darwin.pkg -target /
```

#### Runtime environment

There is an open issue with gRPC on Centos/Fedora, which requires an extra step before running any YDK gNMI application.
See this issue on [GRPC GitHub](https://github.com/grpc/grpc/issues/10942#issuecomment-312565041) for details.
As a workaround, the YDK based application runtime environment must include setting of `LD_LIBRARY_PATH` variable:

```
  PROTO="/Your-Protobuf-and-Grpc-installation-directory"
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROTO/grpc/libs/opt:$PROTO/protobuf-3.5.0/src/.libs:/usr/local/lib64
```

### YDK Go Source

You can download the latest YDK Go source code, which include core, and model bundles, from GitHub:

```
  go get github.com/CiscoDevNet/ydk-go/ydk
```

## Documentation and Support
- Read the [API documentation](http://ydk.cisco.com/go/docs) for details on how to use the API and specific models
- Samples can be found under the [samples directory](https://github.com/CiscoDevNet/ydk-go/tree/master/samples)
- Additional samples can be found in the [YDK-Go samples repository](https://github.com/CiscoDevNet/ydk-go-samples) (coming soon)
- Join the [YDK community](https://communities.cisco.com/community/developer/ydk) to connect with other users and with the makers of YDK
- Additional YDK information can be found at [ydk.io](http://ydk.io)

## Release Notes

The current YDK release version is 0.8.5. YDK-Go is licensed under the Apache 2.0 License.
