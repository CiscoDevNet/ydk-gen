Getting Started
===============
.. contents:: Table of Contents

Overview
--------

The YANG Development Kit (YDK) is a Software Development Kit that provides API's that are modeled in YANG. The main goal of YDK is to reduce the learning curve of YANG data models by expressing the model semantics in an API and abstracting protocol/encoding details.  YDK is composed of a core package that defines services and providers, plus one or more module bundles that are based on YANG models.  Each module bundle is generated using a `bundle profile <https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles>`_ and the `ydk-gen <https://github.com/CiscoDevNet/ydk-gen>`_ tool.

System Requirements
-------------------
Linux
  Ubuntu (Debian-based) - The following packages must be present in your system before installing YDK-Py::

    $ sudo apt-get install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev

  Centos (Fedora-based) - The following packages must be present in your system before installing YDK-Py::
    
    $ sudo yum install epel-release
    $ sudo yum install python-pip python-devel libxml2-devel libxslt-devel libcurl-devel libtool

Mac
  It is recommended to install homebrew (http://brew.sh), homebrew python package and Xcode command line tools on your system before installing YDK-Py::

    $ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    $ brew install python
    $ xcode-select --install

Windows
    It is recommended to install a python distribution like `PythonXY <https://python-xy.github.io/>`_ on your system before installing YDK-Py

Python Requirements
~~~~~~~~~~~~
Python2.7 or Python 3.4

Install Tips
------------
We recommend that you perform the installation under a Python virtual environment (``virtualenv``/``virtualenvwrapper``).  To install support in your system, execute::

  ydk-py$ pip install virtualenv virtualenvwrapper
  ydk-py$ source /usr/local/bin/virtualenvwrapper.sh

In some systems (e.g. Debian-based Linux), you may need to install support for Python virtual environments as root::

  ydk-py$ sudo pip install virtualenv virtualenvwrapper
  ydk-py$ source /usr/local/bin/virtualenvwrapper.sh

At this point, create a new virtual environment::

  ydk-py$ mkvirtualenv -p python2.7 ydk-py

Now you can install the ``core`` package::

  (ydk-py)ydk-py$ cd core
  (ydk-py)core$ python setup.py sdist
  (ydk-py)core$ pip install dist/ydk*.gz

Once you have installed the ``core`` package, you can install one more model bundles.  Note that some bundles have dependencies on other bundles.  Those dependencies are already captured in the bundle package.  To install the IETF bundle, execute::

  (ydk-py)core$ cd ../ietf
  (ydk-py)ietf$ python setup.py sdist
  (ydk-py)ietf$ pip install dist/ydk*.gz

To install the OpenConfig bundle, execute::

  (ydk-py)ietf$ cd ../openconfig
  (ydk-py)openconfig$ python setup.py sdist
  (ydk-py)openconfig$ pip install dist/ydk*.gz

To install the cisco-ios-xr bundle, execute::

  (ydk-py)openconfig$ cd ../cisco-ios-xr
  (ydk-py)cisco-ios-xr$ python setup.py sdist
  (ydk-py)cisco-ios-xr$ pip install dist/ydk*.gz
  (ydk-py)cisco-ios-xr$ cd ..

Samples
---------------
To get started with using the YDK API, there are sample apps available under the ``core/samples`` directory. For example, to run the ``core/samples/bgp.py`` sample, execute::

    (ydk-py)ydk-py$ cd core/samples
    (ydk-py)samples$ ./bgp.py -h
    Usage: bgp.py [-h | --help] [options]

    Options:
    -h, --help            show this help message and exit
    -v VERSION, --version=VERSION
                        force NETCONF version 1.0 or 1.1
    -u USERNAME, --user=USERNAME
    -p PASSWORD, --password=PASSWORD
                        password
    --proto=PROTO         Which transport protocol to use, one of ssh or tcp
    --host=HOST           NETCONF agent hostname
    --port=PORT           NETCONF agent SSH port

    (ydk-py)samples$ ./bgp.py --host <ip-address-of-netconf-server> -u <username> -p <password> --port <port-number>


Release Notes
--------------
The current YDK release version is 0.5.3 (beta). YDK-Py is licensed under the Apache 2.0 License.

Documentation and Support
--------------------------
- Samples can be found under the ``ydk-cpp/core/samples`` directory
- API documentation can be found at http://ydk.cisco.com/py/docs
- Additional samples can be found at https://github.com/CiscoDevNet/ydk-py-samples
- For queries related to usage of the API, please join the YDK community at https://communities.cisco.com/community/developer/ydk
