<a href="https://github.com/CiscoDevNet/ydk-gen"><img src="https://cloud.githubusercontent.com/assets/17089095/14834057/2e1fe270-0bb7-11e6-9e94-73dd7d71e87d.png" height="240" width="240" ></a>

# YDK-GEN

 [![License](https://cloud.githubusercontent.com/assets/17089095/19458582/dd626d2c-9481-11e6-8019-8227c5c66a06.png)](https://github.com/CiscoDevNet/ydk-gen/blob/master/LICENSE) [![Build Status](https://travis-ci.org/CiscoDevNet/ydk-gen.svg?branch=master)](https://travis-ci.org/CiscoDevNet/ydk-gen)
[![Coverage Status](https://coveralls.io/repos/github/CiscoDevNet/ydk-gen/badge.svg?branch=master)](https://coveralls.io/github/CiscoDevNet/ydk-gen?branch=master)


**ydk-gen** is a developer tool that can generate API bindings to YANG data models for, today, Python and C++, with planned future support for other language bindings.

Other tools and libraries are used to deliver ydk-gen's functionality. In particular:

* YANG model analysis and code generation is implemented using APIs from the [pyang](https://github.com/mbj4668/pyang) library
* Documentation is generated using [Sphinx](http://www.sphinx-doc.org/en/stable/)

Of course, many other libraries are used as an integral part of ydk-gen and its dependencies, too many to mention!

The output of ydk-gen is either a core package, that defines services and providers, or a module bundle, consisting of APIs based on YANG models. Each module bundle is generated using a bundle profile and the ydk-gen tool. Developers can either use pre-packaged generated bundles (e.g. [ydk-py](http://cs.co/ydk-py)), or they can define their own bundle, consisting of a set of YANG models, using a bundle profile (e.g. [```ietf_0_1_1.json```](profiles/bundles/ietf_0_1_1.json)). This gives a developer the ability to customize the scope of their bundle based on their requirements.


##System Requirements:

####Linux
Ubuntu (Debian-based): The following packages must be present in your system before installing YDK-Py:
```
$ sudo apt-get install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev libboost-dev libboost-python-dev libssh-dev libcurl4-openssl-dev libtool-bin
```

Centos (Fedora-based): The following packages must be present in your system before installing YDK-Py:
```
$ sudo yum install epel-release
$ sudo yum install python-pip python-devel libxml2-devel libxslt-devel libssh-devel boost-devel boost-python libcurl-devel libtool gcc-c++
```

####Mac
It is recommended to install homebrew (http://brew.sh) and Xcode command line tools on your system before installing YDK-Py:
```
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
$ xcode-select --install
$ brew install boost boost-python pkg-config cmake libssh
```

####Windows
It is recommended to install a python distribution like [PythonXY](https://python-xy.github.io/) on your system before installing YDK-Py

## Installation

```
$ git clone https://github.com/CiscoDevNet/ydk-gen.git
$ cd ydk-gen
$ pip install -r requirements
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
  --sudo              Use superuser permission (during C++ bundle/core generation if necessary)
  -v, --verbose       Verbose mode
  --generate-doc      Generation documentation
  --output-directory  The output-directory . If not specified the output can be found under `ydk-gen/gen-api/python`
```

### Installing the YDK core

```
 ./generate.py --core
```

### Bundle profiles

1. Construct a bundle profile file, such as [```ietf_0_1_1.json```](profiles/bundles/ietf_0_1_1.json) and specify its dependencies

2. Generate the bundle using a command of the form:

```
./generate.py --python --bundle profiles/bundles/ietf_0_1_1.json
```

The generated bundle will in ```ydk-gen/gen-api/python``` or ```ydk-gen/gen-api/cpp```.

#### Details

A sample bundle profile file is described below. The file is in a JSON format. The initial section of metadata is mostly ignored for now. It will be used later.

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

## Notes

### Python version

- If your environment has both python 2 and python 3 and uses python 2 by default, you may need to use 'python3' and 'pip3' instead of 'python' and 'pip' in the commands mentioned in this document.


### Directory Structure

```
README          - install and usage notes
gen-api         - generated bundle/core
					- python (Python SDK)
					- cpp (C++ SDK)

generate.py     - script used to generate SDK for yang models
profiles        - profile files used during generation
yang            - some yang models used for testing
requirements.txt- python dependencies used during installation
sdk             - sdk core and stubs
test            - test code
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
$./generate.py --core --cpp
$./generate.py --bundle profiles/test/ydktest-cpp.json --cpp
```

To run the core and bundle tests, do the following

```
$ cd ydk-gen/sdk/cpp/ydk
$ mkdir build && cd build
$ cmake .. && make all test

$ cd ydk-gen/sdk/cpp/tests
$ mkdir build && cd build
$ cmake .. && make all test
```
