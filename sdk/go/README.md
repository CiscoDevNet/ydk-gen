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
  - [Installing](#installing)
- [Documentation and Support](#documentation-and-support)
- [Release Notes](#release-notes)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Overview

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.  

## Docker
A [docker image](https://docs.docker.com/engine/reference/run/) is automatically built with the latest ydk-go installed. This be used to run ydk-go without installing anything natively on your machine.

To use the docker image, [install docker](https://docs.docker.com/install/) on your system and run the below command. See the [docker documentation](https://docs.docker.com/engine/reference/run/) for more details.

```
docker run -it ydkdev/ydk-go
```

## How to Install

You can install YDK-Go on macOS or Linux.  It is not currently supported on Windows.

### System Requirements
**Linux**  
Ubuntu (Debian-based) - The following packages must be present in your system before installing YDK-Go:

```
$ sudo apt-get install libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev libtool-bin cmake

$ wget https://devhub.cisco.com/artifactory/debian-ydk/0.7.1/libydk_0.7.1-1_amd64.deb
$ sudo gdebi libydk_0.7.1-1_amd64.deb
```

Centos (Fedora-based) - The following packages must be present in your system before installing YDK-Go:

```
$ sudo yum install epel-release
$ sudo yum install libxml2-devel libxslt-devel libssh-devel libtool gcc-c++ pcre-devel cmake libstdc++-static git

# Upgrade gcc to > 5.*
$ yum install centos-release-scl -y > /dev/null
$ yum install devtoolset-4-gcc* -y > /dev/null
$ ln -sf /opt/rh/devtoolset-4/root/usr/bin/gcc /usr/bin/gcc
$ ln -sf /opt/rh/devtoolset-4/root/usr/bin/g++ /usr/bin/g++

$ sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.7.1/libydk-0.7.1-1.x86_64.rpm

```

**Mac**  
It is recommended to install [homebrew](http://brew.sh) and Xcode command line tools on your system before installing YDK-Go:
```
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
$ brew install pkg-config libssh libxml2 xml2 curl pcre cmake
$ xcode-select --install

$ curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.7.1/libydk-0.7.1-Darwin.pkg
$ sudo installer -pkg libydk-0.7.1-Darwin.pkg -target /
```

### Installing

You can install the latest `ydk` package using:
```
$ go get github.com/CiscoDevNet/ydk-go/ydk
```

## Documentation and Support
- Read the [API documentation](http://ydk.cisco.com/go/docs) for details on how to use the API and specific models
- Samples can be found under the [samples directory](https://github.com/CiscoDevNet/ydk-go/tree/master/samples)
- Additional samples can be found in the [YDK-Go samples repository](https://github.com/CiscoDevNet/ydk-go-samples) (coming soon)
- Join the [YDK community](https://communities.cisco.com/community/developer/ydk) to connect with other users and with the makers of YDK
- Additional YDK information can be found at [ydk.io](http://ydk.io)

## Release Notes
The current YDK release version is 0.7.1 (alpha). YDK-Go is licensed under the Apache 2.0 License.
