<a href="https://github.com/CiscoDevNet/ydk-gen"><img src="https://cloud.githubusercontent.com/assets/17089095/14834057/2e1fe270-0bb7-11e6-9e94-73dd7d71e87d.png" height="240" width="240" ></a>

# YDK-GEN

[![Build Status](https://travis-ci.org/CiscoDevNet/ydk-gen.svg?branch=master)](https://travis-ci.org/CiscoDevNet/ydk-gen)
[![Coverage Status](https://coveralls.io/repos/github/CiscoDevNet/ydk-gen/badge.svg?branch=master)](https://coveralls.io/github/CiscoDevNet/ydk-gen?branch=master)


**ydk-gen** is a developer tool that can generate API bindings to YANG data models for, today, Python. Work is underway to support C++, and the ydk-gen may be used as the starting point for supporting bindings to any language.

Other tools and libraries are used to deliver ydk-gen's functionality. In particular:

* YANG model analysis and code generation is implemented as an extension to [pyang](https://github.com/mbj4668/pyang)
* Core libraries are built on [ncclient](https://github.com/ncclient/ncclient)
* Documentation is generated using [Sphinx](http://www.sphinx-doc.org/en/stable/)

Of course, many other libraries are used as an integral part of ydk-gen and its dependencies, too many to mention!

Developers can either use pre-packaged generated code (e.g. [ydk-py](http://cs.co/ydk-py)), or they can define the YANG models that code is to be generated for are specified in a profile file. This gives a developer the ability to customize the scope of their SDK based on their requirements.


##System Requirements:

####Linux
Ubuntu (Debian-based): The following packages must be present in your system before installing YDK-Py:
```
user-machine# sudo apt-get install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev libboost-dev libboost-python-dev libssh-dev libcurl4-openssl-dev libtool-bin
```

Centos (Fedora-based): The following packages must be present in your system before installing YDK-Py:
```
user-machine# sudo yum install epel-release
user-machine# sudo yum install python-pip python-devel libxml2-devel libxslt-devel libssh-devel boost-devel boost-python libcurl-devel libtool gcc-c++
```

####Mac
It is recommended to install homebrew (http://brew.sh) and Xcode command line tools on your system before installing YDK-Py:
```
user-machine# /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
user-machine# xcode-select --install
user-machine# brew install boost boost-python pkg-config cmake libssh
```

####Windows
It is recommended to install a python distribution like [PythonXY](https://python-xy.github.io/) on your system before installing YDK-Py


##Python Requirements:
* Python 2.7 or 3.4


## Installation

```
user-machine# git clone https://github.com/CiscoDevNet/ydk-gen.git
user-machine# cd ydk-gen
user-machine# source install.sh
```


## Usage

```
user-machine# python generate.py --help
Usage: generate.py [options]

Options:
  --version           show program's version number and exit
  -h, --help          show this help message and exit
  --profile=PROFILE   Take options from a profile file, any CLI targets
                      ignored. Profile options override CLI currently
  -p, --python        Generate Python SDK
  -c, --cpp           Generate C++ SDK
  -v, --verbose       Verbose mode
  --generate-doc      Generation documentation
  --output-directory  The output-directory . If not specified the output can be found under ydk-gen/gen-api/python
```

### Profiles

1. Construct a profile file, such as [```xr600-native-oc-bgp.json```](profiles/cisco-ios-xr/xr600-native-oc-bgp.json)

2. Generate the SDK using a command of the form:

```
python generate.py --python --profile profiles/cisco-ios-xr/xr600-native-oc-bgp.json
```

The generated SDK will in ```ydk-gen/gen-api/python```.

#### Details

A sample profile file is described below.

As should be fairly obvious, the file is in a JSON format. The initial section of metadata is mostly ignored for now. It will be used later.

```
{
    "version": "0.1.0",
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
- ```dir``` - List of **relative** directory paths within git repository. All .yang files in this directory **and any sub-directories** will be pulled into the generated SDK.
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

### Bundles

1. Construct a bundle profile file, specify its dependencies, see example [```openconfig.json```](profiles/bundles/openconfig.json)

2. Generate the SDK using a command of the form:

```
python generate.py --python --bundle profiles/bundles/openconfig.json
```

The generated SDK will in ```ydk-gen/gen-api/python```.

3. Bundle packages are generated as Python namespace packages, and are depend on YDK core library, to generate YDK core library, use command:

```
python generate.py --core
```

The core library package will in ```ydk-gen/gen-api/python```.

#### Details

The dependency relationships defined in bundle profile file are expressed in pip, to install above bundle, the user need to generate and install its dependency [```ietf.json```](profiles/bundles/ietf.json) first.

## Notes

YANG Development Kit Generator:

- Tools that auto generate different programming language binding API's (Python, Ruby, GPB, Thrift, Objective C), Developers can use these objects/APIs, to write application
- Runtime libraries which provided "services" and transport code for App to talk to network devices (runtime for: Python, Ruby, gRPCServer). These runtime libraries also have protocol plugin, currently netconf plug has been added for testing.
- The runtime libraries have three parts:
    - Entity:  X object definitions for YANG model. X here is programming language (Python, Ruby, Obj-C, GPBIDL, ThriftIDL etc)
    - ServiceProvider: Provides concrete implementation that abstracts underlying protocol details
    - Services: Provides simple API interface to be used with the entity and provider


### Python Notes

- If your environment has both python 2 and python 3 and uses python 2 by default, you may need to use 'python3' and 'pip3' instead of 'python' and 'pip' in the commands mentioned in this document.

For Python entities and netconf session, CRUD service invoked on python class will:

- Encode python data objects to netconf XML payload
- Perform transport operation with device, collect the netconf response,
- Decode netconf response in python class, return result to python app.



## Directory Structure

```
README          - install and usage notes
install.sh      - Simple one-shot installation script
gen-api         - source dir or autogenerated SDK
					- python (Python SDK)

generate.py     - bootstrap script to generate SDK for yang data models
profiles        - profile files used during generation
yang            - some yang models used for testing
requirements.txt- python dependencies used during installation (refer README)
sdk             - sdk stubs
test            - test code, engineering playground
```

## Running Unit Tests

Make sure that PYTHONPATH is set properly

```
user-machine# cd ydk-gen
user-machine# export PYTHONPATH=.:$PYTHONPATH
```

To run the sanity tests, do the following after running install.sh.

```
user-machine# cd ydk-gen/sdk/python
user-machine# python test/test_sanity_types.py
user-machine# python test/test_sanity_levels.py
user-machine# python test/test_sanity_filters.py
...
```

To run the generator test case, do the following after running install.sh.

```
user-machine# cd ydk-gen
user-machine# python test/pygen_tests.py
```
