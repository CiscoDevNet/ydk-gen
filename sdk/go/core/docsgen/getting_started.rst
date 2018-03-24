===============
Getting Started
===============
.. contents:: Table of Contents

Overview
========

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.

Docker
======

A `docker image <https://docs.docker.com/engine/reference/run/>`_ is automatically built with the latest ydk-go installed. This be used to run ydk-go without installing anything natively on your machine.

To use the docker image, `install docker <https://docs.docker.com/install/>`_ on your system and run the below command. See the `docker documentation <https://docs.docker.com/engine/reference/run/>`_ for more details::

  docker run -it ydkdev/ydk-go


System Requirements
===================

Linux
-----

**Ubuntu (Debian-based)**

The following packages must be present in your system before installing YDK-Go:

.. code-block:: sh

	$ sudo apt-get install libcurl4-openssl-dev libpcre3-dev libssh-dev libxml2-dev libxslt1-dev libtool-bin cmake
	$ wget https://devhub.cisco.com/artifactory/debian-ydk/0.7.0/libydk_0.7.0-1_amd64.deb
	$ sudo gdebi libydk_0.7.0-1_amd64.deb

**Centos (Fedora-based)**

The following packages must be present in your system before installing YDK-Go:

.. code-block:: sh
	
	$ sudo yum install epel-release
	$ sudo yum install libxml2-devel libxslt-devel libssh-devel libtool gcc-c++ pcre-devel cmake
	$ sudo yum install https://devhub.cisco.com/artifactory/rpm-ydk/0.7.0/libydk-0.7.0-1.x86_64.rpm

Mac
---

It is recommended to install `homebrew <http://brew.sh>`_ and Xcode command line tools on your system before installing YDK-Go:

.. code-block:: sh

	$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
	$ brew install pkg-config libssh libxml2 xml2 curl pcre cmake
	$ xcode-select --install

	$ curl -O https://devhub.cisco.com/artifactory/osx-ydk/0.7.0/libydk-0.7.0-Darwin.pkg
	$ sudo installer -pkg libydk-0.7.0-Darwin.pkg -target /

.. _howto-install:

How to install
==============

You can install YDK-Go on macOS or Linux.  It is not currently supported on Windows.

To install the latest model packages, use ``go get``. Note that, in some systems, you need to install the new package as root.

.. code-block:: sh
	
	$ go get github.com/CiscoDevNet/ydk-go/ydk


Documentation and Support
=========================

- Read the `API documentation <http://ydk.cisco.com/go/docs>`_ for details on how to use the API and specific models
- Samples can be found under the `samples directory <https://github.com/CiscoDevNet/ydk-go/tree/master/samples>`_
- Additional samples can be found in the `YDK-Go samples repository <https://github.com/CiscoDevNet/ydk-go-samples>`_ (coming soon)
- Join the `YDK community <https://communities.cisco.com/community/developer/ydk>`_ to connect with other users and with the makers of YDK
- Additional YDK information can be found at `ydk.io <http://ydk.io>`_

Release Notes
=============

The current YDK release version is 0.7.0 (alpha). YDK-Go is licensed under the Apache 2.0 License.
