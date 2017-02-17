<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [YDK-GEN](#ydk-gen)
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
  - [Notes](#notes)
    - [Python version](#python-version)
    - [Directory structure](#directory-structure)
    - [Troubleshooting](#troubleshooting)
  - [Running Unit Tests](#running-unit-tests)
    - [Python](#python)
    - [C++](#c)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

<a href="https://github.com/CiscoDevNet/ydk-gen"><img src="https://cloud.githubusercontent.com/assets/17089095/14834057/2e1fe270-0bb7-11e6-9e94-73dd7d71e87d.png" height="240" width="240" ></a>

# YDK-GEN

 [![License](https://cloud.githubusercontent.com/assets/17089095/19458582/dd626d2c-9481-11e6-8019-8227c5c66a06.png)](https://github.com/CiscoDevNet/ydk-gen/blob/master/LICENSE) [![Build Status](https://travis-ci.org/CiscoDevNet/ydk-gen.svg?branch=master)](https://travis-ci.org/CiscoDevNet/ydk-gen)
[![codecov](https://codecov.io/gh/CiscoDevNet/ydk-gen/branch/master/graph/badge.svg)](https://codecov.io/gh/CiscoDevNet/ydk-gen)


**ydk-gen** is a developer tool that can generate API bindings to YANG data models for, today, Python and C++, with planned future support for other language bindings.

Other tools and libraries are used to deliver `ydk-gen`'s functionality. In particular:

* YANG model analysis and code generation is implemented using APIs from the [pyang](https://github.com/mbj4668/pyang) library
* Documentation is generated using [Sphinx](http://www.sphinx-doc.org/en/stable/)

Of course, many other libraries are used as an integral part of ydk-gen and its dependencies, too many to mention!

The output of ydk-gen is either a core package, that defines services and providers, or a module bundle, consisting of APIs based on YANG models. Each module bundle is generated using a bundle profile and the ydk-gen tool. Developers can either use pre-packaged generated bundles (e.g. [ydk-py](http://cs.co/ydk-py)), or they can define their own bundle, consisting of a set of YANG models, using a bundle profile (e.g. [```ietf_0_1_1.json```](profiles/bundles/ietf_0_1_1.json)). This gives a developer the ability to customize the scope of their bundle based on their requirements.


##System Requirements:

####Linux
Ubuntu (Debian-based): The following packages must be present in your system before installing YDK-Py:
```
$ sudo apt-get install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev libssh-dev libcurl4-openssl-dev libtool-bin libpcre3-dev libpcre++-dev libtool pkg-config python3-dev python3-lxml cmake
```

Centos (Fedora-based): The following packages must be present in your system before installing YDK-Py:
```
$ sudo yum install epel-release
$ sudo yum install python-pip python-devel libxml2-devel libxslt-devel libssh-devel libcurl-devel libtool gcc-c++ cmake
```

####Mac
It is recommended to install homebrew (http://brew.sh) and Xcode command line tools on your system before installing YDK-Py:
```
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
$ xcode-select --install
$ brew install pkg-config cmake libssh xml2 curl pcre
```

####Windows
It is recommended to install a python distribution like [PythonXY](https://python-xy.github.io/) on your system before installing YDK-Py

## Installation
### Setting up your environment

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
### Clone ydk-gen and install the requirements
```
$ git clone https://github.com/CiscoDevNet/ydk-gen.git
$ cd ydk-gen
$ pip install -r requirements.txt
```

## Usage

```
$ ./generate.py --help
Usage: generate.py [options]

Options:
  --version           show program's version number and exit
  -h, --help          show this help message and exit
  -p, --python        Generate Python bundle/core. This is currently the default option
  -c, --cpp           Generate C++ bundle/core
  --core              Install the python/C++ core
  --bundle=PROFILE    Take options from a bundle profile file describing YANG
  -v, --verbose       Verbose mode
  --generate-doc      Generation documentation
  --output-directory  The output-directory . If not specified the output can be found under `ydk-gen/gen-api/python`
```
The below steps specify how to use `ydk-gen` to generate the python core and a python bundle. Similar steps can be followed for C++. Pre-generated bundles and core are available for python and C++: [ydk-py](https://github.com/CiscoDevNet/ydk-py) and [ydk-cpp](https://github.com/CiscoDevNet/ydk-cpp).

### First step: choose your bundle profile

The first step in using ydk-gen is either using one of the already existing [bundle profiles](https://github.com/CiscoDevNet/ydk-gen/tree/master/profiles/bundles) or constructing your own bundle profile, consisting of the YANG models you are interested in:

Construct a bundle profile file, such as [```ietf_0_1_1.json```](profiles/bundles/ietf_0_1_1.json) and specify its dependencies

#### Details

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

### Second step: Generate & install the core
 
First, generate the core and install it:

```
$ ./generate.py --python --core
$ pip install gen-api/python/ydk/dist/ydk*.tar.gz
```
 
### Third step: Generate & install your bundle
Then, generate your bundle using a bundle profile and install it:
 
```
$ ./generate.py --python --bundle profiles/<name-of-profile>.json 
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

### Fourth step: Writing your first app

Now, you can start creating apps based on the models in your bundle. Assuming you generated a python bundle, the models will be available for importing in your app under `ydk.models.<name-of-your-bundle>`. See [ydk-py-samples](https://github.com/CiscoDevNet/ydk-py-samples#a-hello-world-app) for examples. Also refer to the [documentation](http://ydk.cisco.com/py/docs/developer_guide.html).

### Documentation

When generating the YDK documentation for several bundles and the core, it is recommended to generate the bundles without the `--generate-doc` option. After generating all the bundles, the combined documentation for all the bundles and the core can be generated using the `--core --generate-doc` option. For example, the below sequence of commands will generate the documentation for the three bundles and the core. Note that this process could take a few hours due to the size of the `cisco_ios_xr` bundle:

```
./generate.py --python --bundle profiles/bundles/ietf_0_1_1.json
./generate.py --python --bundle profiles/bundles/openconfig_0_1_1.json
./generate.py --python --bundle profiles/bundles/cisco_ios_xr_6_1_1.json
./generate.py --python --core --generate-doc
```
Pre-generated documentation for [ydk-py](http://ydk.cisco.com/py/docs/) and [ydk-cpp](http://ydk.cisco.com/cpp/docs/) are available. 

## Notes

### Python version

- If your environment has both python 2 and python 3 and uses python 2 by default, you may need to use 'python3' and 'pip3' instead of 'python' and 'pip' in the commands mentioned in this document.


### Directory structure

```
README          - install and usage notes
gen-api         - generated bundle/core
					- python (Python SDK)
					- cpp (C++ SDK)

generate.py     - script used to generate SDK for yang models
profiles        - profile files used during generation
yang            - some yang models used for testing
requirements.txt- python dependencies used during installation
sdk             - sdk core and stubs for python and cpp
test            - test code
```

### Troubleshooting
Sometimes, developers using ydk-gen may run across errors when generating a YDK bundle using generate.py with some yang models. If there are issues with the .json profile file being used, such errors will be easily evident. Other times, when the problem is not so evident, it is recommended to try running with the `--verbose|-v` flag, which may reveal syntax problems with the yang models being used. For example,

```
./generate.py --python --bundle profiles/bundles/ietf_0_1_1.json --verbose
```

Also, it may be a good idea to obtain a local copy of the yang models and compile them using `pyang` to ensure the validity of the models,
```
cd /path/to/yang/models
pyang *.yang
```

## Running Unit Tests

### Python
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

### C++
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
