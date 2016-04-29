<a href="https://github.com/CiscoDevNet/ydk-gen"><img src="https://cloud.githubusercontent.com/assets/17089095/14834057/2e1fe270-0bb7-11e6-9e94-73dd7d71e87d.png" height="240" width="240" ></a>

# YDK-GEN

[![Build Status](https://travis-ci.org/CiscoDevNet/ydk-gen.svg?branch=master)](https://travis-ci.org/CiscoDevNet/ydk-gen)

##System Requirements:

####Linux
Ubuntu (Debian-based): The following packages must be present in your system before installing YDK-Py:
```
user-machine# sudo apt-get install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev
```

####Mac
It is recommended to install homebrew (http://brew.sh) and Xcode command line tools on your system before installing YDK-Py:
```
user-machine# /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
user-machine# xcode-select --install
```

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
  -v, --verbose       Verbose mode
  --generate-doc      Generation documentation
  --output-directory  The output-directory . If not specified the output can be found under ydk-gen/gen-api/python
```

### Profiles

1. Construct a profile file, such as [```xr600-native-oc-bgp.json```](profiles/cisco-ios-xr/xr600-native-oc-bgp.json)

1. Generate the SDK using a command of the form:

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


## Notes

YANG Development Kit Generator:

- Tools that auto generate different programming language binding API's (Python, Ruby, GPB, Thrift, Objective C), Developers can use these objects/APIs, to write application
- Runtime libraries which provided "services" and transport code for App to talk to network devices (runtime for: Python, Ruby, gRPCServer). These runtime libraries also have protocol plugin, currently netconf plug has been added for testing.
- The runtime libraries have three parts:
    - Entity:  X object definitions for YANG model. X here is programming language (Python, Ruby, Obj-C, GPBIDL, ThriftIDL etc)
    - ServiceProvider: Provides concrete implementation that abstracts underlying protocol details
    - Services: Provides simple API interface to be used with the entity and provider 


### Python Notes

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
