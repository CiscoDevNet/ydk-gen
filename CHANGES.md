### 2024-11-30 version 

#### New features and enhancements
  * Added support for Ubuntu 24.04 LTS (noble)
  * Upgraded pyang to version 2.6.1
  * Upgraded pybind11 to version 2.13.6

#### Resolved GitHub issues
  * Incorrect loading of submodules into YANG repository ([#1094](https://github.com/CiscoDevNet/ydk-gen/issues/1094))
  * Added support for finding the Root Resource as specified by RFC 8040 ([#1096](https://github.com/CiscoDevNet/ydk-gen/issues/1096))


### 2023-08-31 version 0.8.6.5

#### New features and enhancements
  * Added support for Ubuntu:jammy (22.04)

#### Resolved GitHub issues
  * Segmentation Fault on Python application exit with Python 3.9.5 ([#1050](https://github.com/CiscoDevNet/ydk-gen/issues/1050))
  * ydk.io and ciscodevnet.github.io/ydk-py/ not working correctly ([#1088](https://github.com/CiscoDevNet/ydk-gen/issues/1088))
  * cannot decode json blob in python without encoding it first ([#1089](https://github.com/CiscoDevNet/ydk-gen/issues/1089))
  * Incorrect bundle model reference ([#1091](https://github.com/CiscoDevNet/ydk-gen/issues/1091))

### 2023-01-31 version 0.8.6.4

#### New features and enhancements
  * install_ydk.sh: Installation of Python and Go YDK packages should not require root access ([#1069](https://github.com/CiscoDevNet/ydk-gen/issues/1069))
  * Expanded unit test cases to retrieve list elements

#### Resolved GitHub issues
  * CodecService fails to decode XML in python when referencing namespace prefix ([#1071](https://github.com/CiscoDevNet/ydk-gen/issues/1071))
  * CiscoDevNet YDK C++ README needs Documentation & Support links updated ([#1077](https://github.com/CiscoDevNet/ydk-gen/issues/1077))
  * Error in ydk-gen-master/test/dependencies_centos.sh version check ([#1078](https://github.com/CiscoDevNet/ydk-gen/issues/1078))
  * not_eq is a c++ keyword ([#1080](https://github.com/CiscoDevNet/ydk-gen/issues/1080))
  * libyang issue with arguments that are swapped ([#1081](https://github.com/CiscoDevNet/ydk-gen/issues/1081))

### Resolved non-GitHub issues
  * Fixed and tested C++ sample apps


### 2022-07-31 version 0.8.6.3

#### New features and enhancements
  * Added automatic build of environment setup file .env by the installation script

#### Resolved GitHub issues
  * IndexError on decoding empty string value in JSON payload ([#1061](https://github.com/CiscoDevNet/ydk-gen/issues/1061))
  * gNMI service installation on Centos 8.2 fails due to C++ compiler error
([#1062](https://github.com/CiscoDevNet/ydk-gen/issues/1062))
  * The installation script fails on fresh new CentOS 7 docker container ([#1064](https://github.com/CiscoDevNet/ydk-gen/issues/1064))
  * ExecutorService fails on non-ietf RPC with Libyang error ([#1067](https://github.com/CiscoDevNet/ydk-gen/issues/1067))

### Resolved non-GitHub issues
  * Upgraded third party packages Sphinx (1.5.6) and wheel (0.37.1) due to documentation
generation issues on Mac Big Sur
  * Fixed installation script for the case, when virtual environment is not used

#### Documentation improvements
  * Fixed documentation issue [#1066](https://github.com/CiscoDevNet/ydk-gen/issues/1066)

### 2022-03-31 version 0.8.6.2

#### New features and enhancements
  * Improved installation script to allow use of Python system installation
  * Corrected few Codacy issues
  * Added support for MacOS Big Sur

#### Resolved GitHub issues
  * Fixed Python 3.5 code compatibility issue for Ubuntu:xenial
  * install_ydk.sh tries to create a new venv ([#1034](https://github.com/CiscoDevNet/ydk-gen/issues/1034))
  * getting YCodecError when leaf value has string 'sftp://' ([#1056](https://github.com/CiscoDevNet/ydk-gen/issues/1056))
  * vrf route-target leaf stitching(bool) fails as invalid ([#1059](https://github.com/CiscoDevNet/ydk-gen/issues/1059))
  
### Resolved non-GitHub issues
  * Addressed segmentation fault error when required modules are not loaded.
  * Patched catch.hpp for Linux distributions that do not define SIGSTKSZ

#### Bundle improvements
  * Cleaned up older profiles


### 2021-09-30 version 0.8.6

#### New features and enhancements
  * Upgraded pyang version to 2.5.0
  * Create go wrapper for NetconfSession and other path APIs ([#779](https://github.com/CiscoDevNet/ydk-gen/issues/779))
  * Added SSL support for NetconfServiceProvider in Go
  * Dropped Python2 support in the code

#### Resolved GitHub issues
  * Go function EntityEqual result depends on parameters order ([#1053](https://github.com/CiscoDevNet/ydk-gen/issues/1053))
  * Failed to get "ietf-netconf" module with Executor Service and get-schema RPC ([#1047](https://github.com/CiscoDevNet/ydk-gen/issues/1047))
  * The Executor Service fails to return module content back to user ([#1048](https://github.com/CiscoDevNet/ydk-gen/issues/1048))

#### Bundle improvements
  * Updated cisco-ios-xr bundle (profile cisco-ios-xr_6_7_3.json) to support Cisco IOS XR 6.7.3
  * Updated openconfig bundle (profile openconfig_0_1_9.json) to support YANG models of revision "2020-05-06"
  * Updated ietf bundle (profile ietf_0_1_6.json) to support YANG models of revision "2018-02-14"
  * Added bundle profiles to support Cisco IOS XR 7.0.1 and 7.0.2


### 2021-05-30 version 0.8.5.2

#### Resolved GitHub issues
  * Version numbers in bundle resolver to comply with PEP 440 ([#1007](https://github.com/CiscoDevNet/ydk-gen/issues/1007))
  * The Executor Service fails to return module content to user ([#1035](https://github.com/CiscoDevNet/ydk-gen/issues/1035))
  * Docker base image python error ([#1036](https://github.com/CiscoDevNet/ydk-gen/issues/1036))
  * XmlSubtreeCodec fails encode leaf-list data ([#1045](https://github.com/CiscoDevNet/ydk-gen/issues/1045))
  * C++ CodecService fails to encode non-top level entity to XML subtree ([#1049](https://github.com/CiscoDevNet/ydk-gen/issues/1049))

#### Other resolved issues and enhancements
  * Fixed few minor bugs in C++ code
  * Added installation of flex and bison for CentOS platform
  * Added scripts for local unit testing of YDK release
  * Tested YDK on Windows 10 using Windows Subsystem for Linux and Ubuntu Bionic (18.04 LTS) and Focal (20.04 LTS) virtual machines.
  

### 2020-09-30 version 0.8.5

#### New features and enhancements
  * Develop utility function to clone Entity instance ([#967](https://github.com/CiscoDevNet/ydk-gen/issues/967))
  * Go CodecService fails to encode leaf-list data ([#968](https://github.com/CiscoDevNet/ydk-gen/issues/968))
  * Developed script install_ydk, which allows to install YDK from source in one CLI command
  * Added support for CentOS/RHEL-8 and Ubuntu-20.04 (focal) ([#1019](https://github.com/CiscoDevNet/ydk-gen/issues/1019))

#### Resolved GitHub issues
  * gNMI set RPC fails when JSON payload is formatted string
  * Enhance generator to indicate how a YANG modules was processed ([#894](https://github.com/CiscoDevNet/ydk-gen/issues/894))
  * Restconf in YDK should support HTTPS (partially resolved) ([#917](https://github.com/CiscoDevNet/ydk-gen/issues/917))
  * Go fails to process filters on enum and identity leaves and leaf-lists ([#969](https://github.com/CiscoDevNet/ydk-gen/issues/969))
  * YDK fails create bundle package when package name contains '.' ([#978](https://github.com/CiscoDevNet/ydk-gen/issues/978))
  * python bundle generator must escape 'async' as variable name ([#980](https://github.com/CiscoDevNet/ydk-gen/issues/980))
  * YDK fails instantiate some of the generated bundle classes ([#983](https://github.com/CiscoDevNet/ydk-gen/issues/983))
  * Python generated bundle class fails instantiate when model contains leaf 'logger' ([#996](https://github.com/CiscoDevNet/ydk-gen/issues/996))
  * C++ YList class fails to process key values when its name contains '-' character ([#997](https://github.com/CiscoDevNet/ydk-gen/issues/997))
  * create_datanode in Path API returns incorrect object and sets incorrect value ([#1003](https://github.com/CiscoDevNet/ydk-gen/issues/1003))
  * create_datanode rejects absolute paths with a leading "/" as defined in RFC 7950 ([#1005](https://github.com/CiscoDevNet/ydk-gen/issues/1005))
  * Path API create_datanode rejects valid value ([#1006](https://github.com/CiscoDevNet/ydk-gen/issues/1006))
  * data tag not recognized when attribute present ([#1021](https://github.com/CiscoDevNet/ydk-gen/issues/1021))
  * c++ generated bundle class fails to compile in gcc when model contains leaf 'minor' ([#1024](https://github.com/CiscoDevNet/ydk-gen/issues/1024))
  * ydk.path.codec.decode validates incorrect XML payload ([#1026](https://github.com/CiscoDevNet/ydk-gen/issues/1026))
  * Python 3 TypeError while initializing CRUDService class ([#1032](https://github.com/CiscoDevNet/ydk-gen/issues/1032))

#### Bundle improvements
  * Updated cisco-ios-xe bundle to support Cisco IOS XE 16.9.3 due to bug in Python generated code
  * Updated cisco-ios-xr bundle to support Cisco IOS XR 6.6.3
  * Updated openconfig bundle to support YANG models of revision "2019-06-21"

#### Documentation improvements
  * Bundle profile description is not accurate ([#971](https://github.com/CiscoDevNet/ydk-gen/issues/971))
  * Undocumented method: ydk.path.SchemaNode.get_keys() ([#1012](https://github.com/CiscoDevNet/ydk-gen/issues/1012))
  * ydk.path.DataNode.get_value() is listed as .get() in the documentation ([#1013](https://github.com/CiscoDevNet/ydk-gen/issues/1013))

### 2019-10-15 version 0.8.4

#### New features and enhancements
  * Expanded meta data in Python bundles by adding flags `has_must` and `has_when` ([#927](https://github.com/CiscoDevNet/ydk-gen/issues/927))
  * Added utility function to compare two entities and build diff ([#925](https://github.com/CiscoDevNet/ydk-gen/issues/925))
  * Adjusted C++ code to work with gcc compiler version 4.8.5 (default for CentOS-7.x)

#### Resolved GitHub issues
  * 'ietf-netconf-monitoring:get-schema' returns schema with escape sequences ([#614](https://github.com/CiscoDevNet/ydk-gen/issues/614))
  * NETCONF provider should raise more appropriate exceptions ([#774](https://github.com/CiscoDevNet/ydk-gen/issues/774))
  * YList test fails when list entity added before keys are initialized ([#800](https://github.com/CiscoDevNet/ydk-gen/issues/800))
  * Inherited classes not initialized properly in Python3 when constructor uses super() ([#921](https://github.com/CiscoDevNet/ydk-gen/issues/921))
  * Delete operation fails on container ([#931](https://github.com/CiscoDevNet/ydk-gen/issues/931))
  * Absolute path for generated entity of second level list child is incorrect ([#933](https://github.com/CiscoDevNet/ydk-gen/issues/933))
  * Go: When entity having children is built from model API the child entities parent is nil ([#938](https://github.com/CiscoDevNet/ydk-gen/issues/938))
  * gNMI Provider does not preserve original strings ([#940](https://github.com/CiscoDevNet/ydk-gen/issues/940))
  * gNMISession fails to decode GetRequest response when returned values is empty string ([#943](https://github.com/CiscoDevNet/ydk-gen/issues/943))
  * No way to retrieve numeric value of enums in cpp generated code ([#944](https://github.com/CiscoDevNet/ydk-gen/issues/944))
  * XmlSubtreeCodec fails decode payload with augmented component ([#956](https://github.com/CiscoDevNet/ydk-gen/issues/956))
  * Debian package for C++ bundles getting installed to wrong location ([#962](https://github.com/CiscoDevNet/ydk-gen/issues/962))

#### Documentation improvements
  Added documentation for XmlSubtreeCodec and JsonSubtreeCodec.

#### Bundle improvements
  * Released cisco-nx-os bundle to support Cisco NX OS 9.3.1
  * Released cisco-ios-xr bundle to support Cisco IOS XR OS 6.5.3


### 2019-05-15 version 0.8.3

#### Bundle improvements
  * Updated cisco-ios-xr bundle to support Cisco IOS XR 6.6.2
  * Updated openconfig bundle to introduce support for additional models.

#### Resolved GitHub issues
  * cgo compile error ([#781](https://github.com/CiscoDevNet/ydk-gen/issues/781))
  * YDK netconf read fails when <data> tag has namespace prefix ([#799](https://github.com/CiscoDevNet/ydk-gen/issues/799))
  * YDK return value of YANG action missing some attributes ([#871](https://github.com/CiscoDevNet/ydk-gen/issues/871))
  * Duplicate code in generated cisco-ios-xe Go bundle ([#891](https://github.com/CiscoDevNet/ydk-gen/issues/891))
  * Memory leaks in YDK C++ core ([#899](https://github.com/CiscoDevNet/ydk-gen/issues/899))
  * Memory leaks in YDK C++ gNMI Service component ([#902](https://github.com/CiscoDevNet/ydk-gen/issues/902))
  * RPC execution stuck when NETCONF server closes session unexpectedly ([#914](https://github.com/CiscoDevNet/ydk-gen/issues/914))
  * YDK attempting to send Commit command when 'writable-running' in capabilities ([#915](https://github.com/CiscoDevNet/ydk-gen/issues/915))
  * Max value of range is set to None when not specified in the Yang model ([#916](https://github.com/CiscoDevNet/ydk-gen/issues/916))
  * YDK MetaInfo should have a field for mandatory leaf ([#918](https://github.com/CiscoDevNet/ydk-gen/issues/918))

##### Note
  The solution for GitHub issue ([#891](https://github.com/CiscoDevNet/ydk-gen/issues/891)) changed model API. However all model bundles generated with YDK-Gen version 0.7.3 and later are still compatible with core YDK components.

#### Documentation improvements
  Addressed multiple documentation issues:
  * Installation documentation for YDK-Py needs an update ([#906](https://github.com/CiscoDevNet/ydk-gen/issues/906))
  * README file for YDK-Py repo is not rendering correctly ([#907](https://github.com/CiscoDevNet/ydk-gen/issues/907))

### 2019-03-15 version 0.8.2

#### New features and enhancements
  * Expanded disabling of data validation to read operations with NetconfServiceProvider.
  * Expanded disabling of data validation to gNMIServiceProvider.

#### Resolved GitHub issues
  * ydk-py and ydk-gen Travis tests fail on Mac ([#834](https://github.com/CiscoDevNet/ydk-gen/issues/834))
  * When MacOS is updated from Homebrew any Python2 YDK application fails ([#837](https://github.com/CiscoDevNet/ydk-gen/issues/837))
  * ImportError: dynamic module does not define module export function (PyInit_ydk_) ([#840](https://github.com/CiscoDevNet/ydk-gen/issues/840))
  * Libyang error: Reached limit (65535) for storing typedefs ([#874](https://github.com/CiscoDevNet/ydk-gen/issues/874))
  * AttributeError: 'Config' object has no attribute 'logger' ([#876](https://github.com/CiscoDevNet/ydk-gen/issues/876))
  * Segmentation Fault in API when connection to Netconf fails ([#879](https://github.com/CiscoDevNet/ydk-gen/issues/879))
  * gNMI provider requires 'port' argument ([#880](https://github.com/CiscoDevNet/ydk-gen/issues/880))
  * 'delete' and 'replace' filters do not work properly with gNMI and CRUD service ([#881](https://github.com/CiscoDevNet/ydk-gen/issues/881))
  * Go compiler failed to link gNMI service on Mac ([#892](https://github.com/CiscoDevNet/ydk-gen/issues/892))

#### Model bundle additions
  * Released cisco-ios-xr bundle to support Cisco IOS XR 6.5.2
  * Released cisco-ios-xe bundle to support Cisco IOS XE 16.9.3
  * Released cisco-nx-os bundle to support Cisco NX OS 9.2.3


### 2019-02-11 version 0.8.1

#### Python, C++, Go
  * Introduced support for non-top level objects/entities for CRUD, Netconf, and gNMI services.
  * Introduced a feature, which allows to disable validation of entity data processed by NetconfServiceProvider.

#### ydk-gen
  * Added capability to generate meta-data in Python bundles by adding `--generate-meta` flag in `generate.py` script.
  * Added `generate.py` script options '-i --install' and '-s --sudo' to generate and install packages in one step.
  * Use CMake build number for release numbers with suffix ([#864](https://github.com/CiscoDevNet/ydk-gen/issues/864))

#### Resolved GitHub issues
  * YFilter dropped when used with NetconfService and EntityCollection ([#803](https://github.com/CiscoDevNet/ydk-gen/issues/803))
  * Test for non-top level Entity fails when bundle created with 'one-class-per-module' flag ([#839](https://github.com/CiscoDevNet/ydk-gen/issues/839))
  * generate.py always creates model bundles with requirements for current YDK version ([#844](https://github.com/CiscoDevNet/ydk-gen/issues/844))
  * NETCONF Session constructor doesn't allow port to be None ([#846](https://github.com/CiscoDevNet/ydk-gen/issues/846))
  * YDK fails process RPC payload, which contain 'data' tag ([#851](https://github.com/CiscoDevNet/ydk-gen/issues/851))
  * gNMI script results in a segmentation fault if repository is missing a required model ([#852](https://github.com/CiscoDevNet/ydk-gen/issues/852))
  * CodecService fails correctly encode multiple entries in keyless list ([#854](https://github.com/CiscoDevNet/ydk-gen/issues/854))
  * Segmentation fault when decoding gNMI config payload ([#856](https://github.com/CiscoDevNet/ydk-gen/issues/856))
  * Decoding gNMI payload fails with ValueError exception ([#858](https://github.com/CiscoDevNet/ydk-gen/issues/858))

#### Documentation improvements
  Addressed multiple documentation issues: [#848](https://github.com/CiscoDevNet/ydk-gen/issues/848),
  [#850](https://github.com/CiscoDevNet/ydk-gen/issues/850), [#861](https://github.com/CiscoDevNet/ydk-gen/issues/861),
  [#862](https://github.com/CiscoDevNet/ydk-gen/issues/862)


### 2018-12-17 version 0.8.0

#### CRUD / Netconf / gNMI/ Codec / Path API
  * Introduced YDK support for gNMI protocol (protobuf version 0.4.0) including CRUD service with gNMI Service Provider.
  * Added Netconf support for certificate-based authentication for multiple servers

#### Bundle improvements
  * Updated cisco-ios-xr bundle to include previously missing action models in Cisco IOS XR 6.5.1 release
  * Released cisco-nx-os bundle to support Cisco NX OS 9.2.2 release

#### ydk-gen
  * Added capability to generate YDK service packages from `generate.py` script.

### 2018-10-02 version 0.7.3

### Python, C++ and Go
CRUD/ NETCONF / Codec API improvements

    Introduced Codec feature to decode multiple JSON payload. (#812)
    Improved support for YList (#811)

### Python
CRUD/ NETCONF / Codec API improvements

    Improve handling of python native types in model API. (#733)
    Validate leaf values based on python type of model API. (#739)
    Improve checking of invalid attributes for model API objects. (#815)

### Bundle improvements

    Updated cisco-ios-xr bundle to support Cisco IOS XR 6.5.1 release.
    Updated cisco-ios-xe bundle to support Cisco IOS XE 16.9.1 release.
    Released cisco-nx-os bundle to support Cisco NX OS 9.2.1 release.
    Updated openconfig to to make it compatible with ydk core version 0.7.3.
    Also updated ietf bundle to make it compatible with ydk core version 0.7.3.

#### Note about cisco-ios-xr 6.5.1 bundle
    Bundle excludes the following files due to duplicate namespaces:
```
    Cisco-IOS-XR-sysadmin-clear-ncs5500.yang
    Cisco-IOS-XR-sysadmin-clear-ncs5502.yang
    Cisco-IOS-XR-sysadmin-clear-ncs55A1.yang
    Cisco-IOS-XR-sysadmin-controllers-ncs5500.yang
    Cisco-IOS-XR-sysadmin-controllers-ncs5501.yang
    Cisco-IOS-XR-sysadmin-controllers-ncs5502.yang
    Cisco-IOS-XR-sysadmin-controllers-ncs55A1.yang
    Cisco-IOS-XR-sysadmin-fabric-mgr-fsdb-aggregator-ncs5500.yang
    Cisco-IOS-XR-sysadmin-fabric-mgr-fsdb-aggregator-ncs5502.yang
    Cisco-IOS-XR-sysadmin-fabric-mgr-fsdb-server-ncs5500.yang
    Cisco-IOS-XR-sysadmin-fabric-mgr-fsdb-server-ncs5502.yang
    Cisco-IOS-XR-sysadmin-fabric-ncs5500.yang
    Cisco-IOS-XR-sysadmin-fabric-ncs5501.yang
    Cisco-IOS-XR-sysadmin-fabric-ncs5502.yang
```

### 2018-07-02 version 0.7.2

#### Bundle improvements
  * Released [`cisco-nx-os`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-nx-os-0_7_4.json) bundle to support Cisco NX OS 7.0-3-I7-4 release
  * Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_4_1.json) bundle to support Cisco IOS XR 6.4.1 release
  * Updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_6.json) to introduce support for additional AFT models.
  * Updated [`cisco-ios-xe`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_8_1_post1.json) bundle to continue to support Cisco IOS XE 16.8.1 release and make it compatible with `ydk core` version 0.7.2
  * Also updated [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_5_post1.json) bundle to make it compatible with `ydk core` version 0.7.2

#### CRUD / Netconf / Codec / Path API improvements
  * Introduced support for key-based access to list items in Python, C++ and Go model API ([#231](https://github.com/CiscoDevNet/ydk-gen/issues/231))
  * Introduced support for multiple entities in Go ([#768](https://github.com/CiscoDevNet/ydk-gen/pull/768))
  * Improved support for YANG `presence` nodes ([#629](https://github.com/CiscoDevNet/ydk-gen/pull/629), [#738](https://github.com/CiscoDevNet/ydk-gen/pull/738), [#763](https://github.com/CiscoDevNet/ydk-gen/pull/763))
  * Fixed issue with invoking sequential CRUD operations on different model APIs ([#727](https://github.com/CiscoDevNet/ydk-gen/issues/727))
  * Improved NETCONF service commit API ([#796](https://github.com/CiscoDevNet/ydk-gen/issues/796))
  * Enhanced support for leaf value patterns ([#786](https://github.com/CiscoDevNet/ydk-gen/issues/786))
  * Improved support for YANG `feature`s included in NETCONF hello message ([#777](https://github.com/CiscoDevNet/ydk-gen/issues/777))

#### Documentation improvements
  * Enhanced Go documentation ([#681](https://github.com/CiscoDevNet/ydk-gen/issues/681), [#684](https://github.com/CiscoDevNet/ydk-gen/issues/684), [#720](https://github.com/CiscoDevNet/ydk-gen/issues/720))
  * Improved documentation logos ([#754](https://github.com/CiscoDevNet/ydk-gen/issues/754), [#755](https://github.com/CiscoDevNet/ydk-gen/issues/755), [#756](https://github.com/CiscoDevNet/ydk-gen/issues/756))

#### ydk-gen improvements
  * Fixed issue with mixed case objects in Go language model API ([#686](https://github.com/CiscoDevNet/ydk-gen/issues/686))

### 2018-04-09 version 0.7.1

#### Python, C++ and Go

##### Bundle improvements
 **NOTE:** [#604](https://github.com/CiscoDevNet/ydk-gen/issues/604) and [#748](https://github.com/CiscoDevNet/ydk-gen/issues/748) introduced a backward incompatibility for python bundles. The below bundles generated with `0.7.1` or newer ydk-gen will only work with ydk `core` version `0.7.1` or newer
  * Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_3_2.json) to support Cisco IOS XR 6.3.2 release
  * Updated [`cisco-ios-xe`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_8_1.json) to support Cisco IOS XE 16.8.1 release
  * Updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_5.json) bundle to introduce support for AFT, ISIS, LLDP, Network Instance and Segment Routing models.
  * Updated [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_5.json) bundles to introduce support for NETCONF access control model (ACM)

##### CRUD / Netconf / Codec / Path API improvements
  * Introduced support for multiple entities in Python and C++ ([#713](https://github.com/CiscoDevNet/ydk-gen/issues/713), [#719](https://github.com/CiscoDevNet/ydk-gen/issues/719), [#726](https://github.com/CiscoDevNet/ydk-gen/issues/726), [#736](https://github.com/CiscoDevNet/ydk-gen/issues/736))
  * Added support for yang models with more than 256 typedefs ([#678](https://github.com/CiscoDevNet/ydk-gen/issues/678), [#607](https://github.com/CiscoDevNet/ydk-gen/issues/607))
  * Fixed segfault with `cisco-ios-xe` model ([#627](https://github.com/CiscoDevNet/ydk-gen/issues/627))
  * Changed default arguments to be more pythonic ([#682](https://github.com/CiscoDevNet/ydk-gen/issues/682))
  * Handled XML escape characters included in yang models ([#683](https://github.com/CiscoDevNet/ydk-gen/issues/683))
  * Improved handling XML declaration in XML payloads ([#662](https://github.com/CiscoDevNet/ydk-gen/issues/662))
  * Fixed support for yang models with lists as top-level nodes ([#728](https://github.com/CiscoDevNet/ydk-gen/issues/728))
  * Added support for yang 1.1 `action` statement in path API ([#717](https://github.com/CiscoDevNet/ydk-gen/issues/717))

##### Netconf provider improvements
  * Added support for connecting to devices with no `get-schema` support ([#554](https://github.com/CiscoDevNet/ydk-gen/issues/544))

##### ydk-gen improvements
  * Updated leafs in python model APIs to use native python types ([#604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Improved the size and performance of Golang model APIs ([#604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Fixed issue with handling of some typedefs in Golang ([#706](https://github.com/CiscoDevNet/ydk-gen/issues/706), [747](https://github.com/CiscoDevNet/ydk-gen/issues/747))

##### Documentation improvements
  * Improved enum documentation ([#716](https://github.com/CiscoDevNet/ydk-gen/issues/716))
  * Enhanced table of contents for documentation ([#715](https://github.com/CiscoDevNet/ydk-gen/issues/715))

##### Testing/error improvements
  * Improved ydk-gen error reporting and fixed `--one-class-per-module` option of generating python packages ([#604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Added coverage for Golang and C++ ([#740](https://github.com/CiscoDevNet/ydk-gen/issues/740), [#705](https://github.com/CiscoDevNet/ydk-gen/issues/705))

##### Installation improvements
  * Introduced automated docker builds to produce docker images with `ydk-gen`, `ydk-py` and `ydk-go` pre-installed ([#724](https://github.com/CiscoDevNet/ydk-gen/issues/724))
  * Removed `epel-release` as one of the requirements for libydk RPM ([#627](https://github.com/CiscoDevNet/ydk-gen/issues/627))
  * Added testing for `libydk` packages ([#604](https://github.com/CiscoDevNet/ydk-gen/issues/604))

### 2018-01-31 version 0.7.0

#### Python, C++, Go
##### Introduced Go language YDK support
  * Added support for all existing `ydk core` services, providers, types and errors in Go
  * Added support for all existing `ydk bundles` including `ietf`, `openconfig`, `cisco-ios-xr` and `cisco-ios-xe` in Go
  * [#673](https://github.com/CiscoDevNet/ydk-gen/pull/673), [#663](https://github.com/CiscoDevNet/ydk-gen/pull/), [#660](https://github.com/CiscoDevNet/ydk-gen/pull/660), [#658](https://github.com/CiscoDevNet/ydk-gen/pull/658), [#606](https://github.com/CiscoDevNet/ydk-gen/pull/606), [#605](https://github.com/CiscoDevNet/ydk-gen/pull/605)

##### CRUD service improvements
  * Fixed handling of reading operational data nodes ([#664](https://github.com/CiscoDevNet/ydk-gen/issues/664))
  * Improved formatting of payloads in logging output ([#670](https://github.com/CiscoDevNet/ydk-gen/issues/670))

##### Error handling improvements
  * Fixed naming of errors across C++ and Go to be consistent. Changed YCPPError, YError etc to YError ([#669](https://github.com/CiscoDevNet/ydk-gen/issues/669), [#668](https://github.com/CiscoDevNet/ydk-gen/issues/668))
  * Fixed warning in CMake build system to look for CMake version of `3.0.0` or greater ([#655](https://github.com/CiscoDevNet/ydk-gen/issues/655))
##### Documentation improvements
  * Improved documentation for models which augment other models ([#426](https://github.com/CiscoDevNet/ydk-gen/issues/426))

### 2017-12-15 version 0.6.3

#### Python & C++
##### Model API updates
  * Updated `cisco-ios-xe` bundle to support Cisco IOS XE [16.6.2](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_6_2.json) and [16.7.1](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_7_1.json) releases

##### Netconf provider improvements
  * Added support for keybase authentication ([#619](https://github.com/CiscoDevNet/ydk-gen/issues/619))

##### CRUD service improvements
  * Fixed various issues with usage of XE model API ([#640](https://github.com/CiscoDevNet/ydk-gen/issues/640), [#535](https://github.com/CiscoDevNet/ydk-gen/issues/535), [#612](https://github.com/CiscoDevNet/ydk-gen/issues/612), [#632](https://github.com/CiscoDevNet/ydk-gen/issues/632))
  * Improved handling of leaf-lists ([#621](https://github.com/CiscoDevNet/ydk-gen/issues/621), [#631](https://github.com/CiscoDevNet/ydk-gen/issues/631), [#646](https://github.com/CiscoDevNet/ydk-gen/issues/646))

##### Documentation improvements
  * Improved documentation for unions ([#642](https://github.com/CiscoDevNet/ydk-gen/issues/642)) and string patterns ([#651](https://github.com/CiscoDevNet/ydk-gen/issues/651))
  * Improved developer guide ([#622](https://github.com/CiscoDevNet/ydk-gen/issues/622), [#625](https://github.com/CiscoDevNet/ydk-gen/issues/625))  

##### Testing improvements
  * Fixed CI failures and added CI on CentOS & Ubuntu Xenial platforms ([#637](https://github.com/CiscoDevNet/ydk-gen/issues/637), [#644](https://github.com/CiscoDevNet/ydk-gen/issues/644))

### 2017-10-30 version 0.6.2

#### Python & C++
* CRUD / Executor / Codec service improvements
  * Improved `CRUDService` support for `openconfig-routing-policy` yang module ([#580](https://github.com/CiscoDevNet/ydk-gen/issues/580), [#540](https://github.com/CiscoDevNet/ydk-gen/issues/540))
  * Improved CRUD support for parent-child yang nodes with the same name ([#566](https://github.com/CiscoDevNet/ydk-gen/issues/566), [#598](https://github.com/CiscoDevNet/ydk-gen/issues/598), [#596](https://github.com/CiscoDevNet/ydk-gen/issues/596))
  * Improved CRUD support for `openconfig-if-ethernet` and `iana-if-types` modules ([#513](https://github.com/CiscoDevNet/ydk-gen/issues/513))
  * Fixed CRUD issue with encoding containers and list instances in user-selected order ([#563](https://github.com/CiscoDevNet/ydk-gen/issues/563), [#564](https://github.com/CiscoDevNet/ydk-gen/issues/564))
  * Fixed issue with `ExecutorService` ([#590](https://github.com/CiscoDevNet/ydk-gen/issues/590), [#558](https://github.com/CiscoDevNet/ydk-gen/issues/558))
  * Improved `CodecService` performance ([#537](https://github.com/CiscoDevNet/ydk-gen/issues/537))
* Documentation improvements
  * Fixed YDK-Py installation documentation on macOS ([#513](https://github.com/CiscoDevNet/ydk-gen/issues/513))
  * Fixed `libydk` installation documentation ([#584](https://github.com/CiscoDevNet/ydk-gen/issues/584))

### 2017-09-25 version 0.6.1

#### Python & C++
  * Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_3_1.json) bundle to support Cisco IOS XR 6.3.1 release
 * Also updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_4.json) bundle version 0.1.4 with additional support for optical transport (channel monitor, optical amplifier, terminal device and transport line)
 * Improved Service Providers
   * Improved Netconf Service Provider to support timeout and retrieving device capabilities ([#217](https://github.com/CiscoDevNet/ydk-gen/issues/217), [#492](https://github.com/CiscoDevNet/ydk-gen/issues/492), [#557](https://github.com/CiscoDevNet/ydk-gen/issues/557))
   * Decoupled path API-specific details from Service Provider and created Netconf & Restconf Session to be used instead of Provider in path API ([#494](https://github.com/CiscoDevNet/ydk-gen/issues/494), [#511](https://github.com/CiscoDevNet/ydk-gen/issues/511))
   * Fixed segmentation fault with the `openconfig-platform` model ([#527](https://github.com/CiscoDevNet/ydk-gen/issues/527))
 * Improved Netconf Service's `kill_session` method ([#528](https://github.com/CiscoDevNet/ydk-gen/issues/528))

#### Documentation
 * Fixed documentation issues for installation ([#529](https://github.com/CiscoDevNet/ydk-gen/issues/529), [#531](https://github.com/CiscoDevNet/ydk-gen/issues/531), [#542](https://github.com/CiscoDevNet/ydk-gen/issues/542), [#541](https://github.com/CiscoDevNet/ydk-gen/issues/541))
 * Improved API documentation ([#424](https://github.com/CiscoDevNet/ydk-gen/issues/424), [#94](https://github.com/CiscoDevNet/ydk-gen/issues/94))

#### ydk-gen
 * Improved model API generation
   * reduced size of generated python model API ([#544](https://github.com/CiscoDevNet/ydk-gen/issues/544))
   * fixed issues with class names not following the CapWords style and models containing enum leafrefs ([#538](https://github.com/CiscoDevNet/ydk-gen/issues/538), [#550](https://github.com/CiscoDevNet/ydk-gen/issues/550), [#475](https://github.com/CiscoDevNet/ydk-gen/issues/475))


### 2017-08-01 version 0.6.0

#### Python
 * Introduced new YDK python [`core`](https://github.com/CiscoDevNet/ydk-py/tree/master/core) package using [pybind11](https://github.com/pybind/pybind11) to wrap around YDK C++ [`core`](https://github.com/CiscoDevNet/ydk-cpp/tree/master/core) ([#507](https://github.com/CiscoDevNet/ydk-gen/pull/507))
   * Introduced `ydk.path` module consisting of APIs to read, manipulate and write YANG data using XPath-like expressions
   * Updated YDK services and providers to internally use the path API
   * Introduced `RestconfServiceProvider` and `OpenDaylightServiceProvider`
   * Updated `NetconfServiceProvider` to be able to download the device yang models on connecting to a device
   * Introduced ability to encode/decode subtree XML in `CodecService` and changed `CRUDService` to use XML subtree filtering to create filters for the `read` operation
   * Added equality/inequality operators to compare YDK model API objects

##### Note on backward compatibility
The [backward incompatible](http://ydk.cisco.com/py/docs/backward_compatibility.html) changes introduced with `0.6.0` python release include:
   * **Installation:** When installing `YDK-Py`, there is a new system requirement which needs to be installed. This is the `libydk` library, which is available on the DevHub website for various OS platforms. Please refer to the [system requirements](https://github.com/CiscoDevNet/ydk-py/blob/master/README.rst#system-requirements) for details.
   * **Windows support:** From release ``0.6.0`` onwards, YDK is no longer supported on the Windows platform. We plan to add back support for this platform in the future.
   * **API changes:** Please refer to the [developer guide](http://ydk.cisco.com/py/docs/developer_guide.html) and [API guide](http://ydk.cisco.com/py/docs/api_guide.html) for details about APIs and how to use them.
   * `NetconfServiceProvider` no longer has the `close()` method. There is no need to explicitly close the provider as it will be automatically cleaned up when the object goes out of scope.
   * `YFilter` has replaced the functionality of the `READ` and `DELETE` objects
   * When using logging, the suggested level for users of YDK is `INFO` as `DEBUG` provides highly detailed logging suitable for developers working on YDK
   * The type names of `enumerations` and `identities` no longer have `Enum` or `Identity` in their names. For example, the  identity `InterfaceTypeIdentity` in `ydk.models.ietf.ietf_interfaces` is now renamed to just `InterfaceType`
   * The ``is_config()`` method is no longer available for the YDK model APIs.    This may be added back in a future release.

#### C++
 * Renamed some of the methods in path API to be verbs like `DataNode::get_schema()` instead of nouns like `DataNode::schema()` ([#498](https://github.com/CiscoDevNet/ydk-gen/pull/498))
 * Add option for TCP transport in `NetconfServiceProvider` ([#476](https://github.com/CiscoDevNet/ydk-gen/pull/476), [#444](https://github.com/CiscoDevNet/ydk-gen/pull/444))
  * Support `get`/`get-config` with no filter in path API ([#503](https://github.com/CiscoDevNet/ydk-gen/pull/503))
 * Introduce optimized on-demand yang model downloading for `NetconfServiceProvider` ([#499](https://github.com/CiscoDevNet/ydk-gen/pull/499))
 * Add support for choosing either a per-device or a common cache for storing downloaded yang models ([#502](https://github.com/CiscoDevNet/ydk-gen/pull/502))
 * Introduced encoding/decoding subtree XML in `CodecService` and changed `CRUDService` to use XML subtree filtering to create filters for the `read` operation ([#489](https://github.com/CiscoDevNet/ydk-gen/pull/489))
 * Added support for non-standard RPCs as well in path API ([#498](https://github.com/CiscoDevNet/ydk-gen/pull/498))
 * Add ability to generate Linux and MacOS packages in cmake using [cpack](https://cmake.org/Wiki/CMake:Packaging_With_CPack) ([#466](https://github.com/CiscoDevNet/ydk-gen/pull/466))

#### ydk-gen
 * Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_2_2.json) to support Cisco IOS XR 6.2.2 release
 * Updated [`cisco-ios-xe`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_6_1.json) to support Cisco IOS XE 16.6.1 release
 * Also updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_3.json) and [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_3.json) bundles

### 2017-06-06 version 0.5.5

#### Python
 * Fixed bundle `setup.py` to match ydk `core` dependency in bundle profile ([#433](https://github.com/CiscoDevNet/ydk-gen/issues/443))
 * Updated `lxml` dependency for ydk `core` package ([#427](https://github.com/CiscoDevNet/ydk-gen/issues/427))
 * Improved reading of data using `ExecutorService` ([#332](https://github.com/CiscoDevNet/ydk-gen/issues/332)) and `CRUDService` ([#457](https://github.com/CiscoDevNet/ydk-gen/issues/457))
 * Fixed encoding key elements of yang `list`s ([#363](https://github.com/CiscoDevNet/ydk-gen/issues/363)) and operational data ([#452](https://github.com/CiscoDevNet/ydk-gen/issues/452), [#455](https://github.com/CiscoDevNet/ydk-gen/issues/455))

#### C++
 * Added equality operator for model API objects ([#432](https://github.com/CiscoDevNet/ydk-gen/pull/432))
 * Improved handling of presence `container`s ([#437](https://github.com/CiscoDevNet/ydk-gen/pull/437))

#### ydk-gen
 * Added [`cisco-ios-xe` bundle](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_5_1.json) to support Cisco IOS XE 16.5.1 release
 * Improved exception handling in the ydk-gen `generate.py` script ([#440](https://github.com/CiscoDevNet/ydk-gen/pull/440))

#### Documentation
 * Improved getting-started guides for YDK-Py and YDK-Cpp  ([#418](https://github.com/CiscoDevNet/ydk-gen/pull/418), [#419](https://github.com/CiscoDevNet/ydk-gen/pull/419))
 * Made table of contents for bundle documentation be sorted alphabetically ([#446](https://github.com/CiscoDevNet/ydk-gen/pull/418), [#419](https://github.com/CiscoDevNet/ydk-gen/pull/446))
 * Improved documentation of `rpc` classes ([#435](https://github.com/CiscoDevNet/ydk-gen/issues/435))

### 2017-03-17 version 0.5.4

#### Python
 * Improved logging to indicate message directionality ([#388](https://github.com/CiscoDevNet/ydk-gen/pull/388))
 * Provide more details for validation error message for leaf-lists ([#398](https://github.com/CiscoDevNet/ydk-gen/pull/398))
 * Remove indirect python requirements from `setup.py` ([#392](https://github.com/CiscoDevNet/ydk-gen/pull/392))
 * If validation error occurs when decoding payload, include payload as an attribute of the `YPYModelError` raised ([#381](https://github.com/CiscoDevNet/ydk-gen/pull/381))
 * Update Python package generation and post YDK-Py on the Python package index - PyPi ([#404](https://github.com/CiscoDevNet/ydk-gen/issues/404), [#406](https://github.com/CiscoDevNet/ydk-gen/issues/406))

#### C++
 * Changed dependent libraries ([#382](https://github.com/CiscoDevNet/ydk-gen/pull/382))
   * Use header-only [spdlog](https://github.com/gabime/spdlog) library for logging
   * Use header-only [catch](https://github.com/philsquared/Catch) library for testing
   * Completely remove boost dependency
 * Create default yang models repository for each bundle. Install yang models as part of bundle shared library ([#292](https://github.com/CiscoDevNet/ydk-gen/pull/292))
   * Make passing in `path::Repository` object optional for `CodecServiceProvider`
 * Fix issue with handling augmented leafs in `CrudService` ([#351](https://github.com/CiscoDevNet/ydk-gen/pull/351))
 * Use smart pointers (like `shared_ptr`) in place of raw pointers ([#382](https://github.com/CiscoDevNet/ydk-gen/pull/382)), ([#393](https://github.com/CiscoDevNet/ydk-gen/pull/393))
 * Support linking to multiple bundles ([#349](https://github.com/CiscoDevNet/ydk-gen/pull/349))
 * Added sample code for using YDK with JSON configs in combination with CRUD service ([#387](https://github.com/CiscoDevNet/ydk-gen/pull/387))

#### ydk-gen
 * Improved generation of C++ code to consume less compile-time memory for large yang models ([#386](https://github.com/CiscoDevNet/ydk-gen/pull/386)), ([#336](https://github.com/CiscoDevNet/ydk-gen/pull/336))
 * Add check for valid profile file to ydkgen ([#378](https://github.com/CiscoDevNet/ydk-gen/pull/378))
 * Added code coverage for C++ code ([#373](https://github.com/CiscoDevNet/ydk-gen/pull/373))
 * Updated [`cisco-ios-xr` bundle](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_2_1.json) to support Cisco IOS XR 6.2.1 release

#### Documentation
 * Separated top data classes from type classes in table of contents ([#372](https://github.com/CiscoDevNet/ydk-gen/pull/372))
 * Fixed ydk version not being correctly printed for C++ documentation ([#374](https://github.com/CiscoDevNet/ydk-gen/pull/374))
 * Indicate bundle version in C++ and python bundle documentation ([#383](https://github.com/CiscoDevNet/ydk-gen/pull/383))

### 2017-01-30 version 0.5.3

#### Python

* Fixed issues with netconf service ([#323](https://github.com/CiscoDevNet/ydk-gen/issues/323), [#305](https://github.com/CiscoDevNet/ydk-gen/issues/305))
* Disambiguated model API classes called 'None' ([#318](https://github.com/CiscoDevNet/ydk-gen/issues/318))
* Removed 'Bits' from classes representing bits leafs ([#318](https://github.com/CiscoDevNet/ydk-gen/issues/318), [#320](https://github.com/CiscoDevNet/ydk-gen/issues/320))

#### C++

* Introduced support for two new service providers ([#365](https://github.com/CiscoDevNet/ydk-gen/pull/365))
  * RestconfServiceProvider
  * OpenDaylightServiceProvider
* Introduced support for netconf service ([#341](https://github.com/CiscoDevNet/ydk-gen/pull/341), [#352](https://github.com/CiscoDevNet/ydk-gen/pull/352))
* Released ydk-cpp for MacOS platform (on [Homebrew](https://github.com/CiscoDevNet/homebrew-ydk)) and on Ubuntu platform (on [Lauchpad](https://launchpad.net/~ydk)) ([#362](https://github.com/CiscoDevNet/ydk-gen/pull/362), [#322](https://github.com/CiscoDevNet/ydk-gen/pull/322))
* Added support for generated CRUD model tests based on bundles ([#354](https://github.com/CiscoDevNet/ydk-gen/pull/354))
* Improved negative test cases and added support for netconf operations on leafs and leaf-lists ([#324](https://github.com/CiscoDevNet/ydk-gen/pull/324))

#### Documentation

* Added documentation with examples for C++ OpenDaylightServiceProvider and RestconfServiceProvider ([#365](https://github.com/CiscoDevNet/ydk-gen/pull/365))
* Included model revision in documentation ([#272](https://github.com/CiscoDevNet/ydk-gen/issues/272))
* Improved documentation for string leafs ([#346](https://github.com/CiscoDevNet/ydk-gen/issues/346)), decimal64 leafs ([#300](https://github.com/CiscoDevNet/ydk-gen/issues/300), [#294](https://github.com/CiscoDevNet/ydk-gen/issues/294))
* Added more detailed documentation for ydk-gen ([#335](https://github.com/CiscoDevNet/ydk-gen/pull/335), [#364](https://github.com/CiscoDevNet/ydk-gen/pull/364))
* Improved look and feel of documentation ([#361](https://github.com/CiscoDevNet/ydk-gen/pull/361), [#356](https://github.com/CiscoDevNet/ydk-gen/pull/356))
* Cleaned up unused bundle profiles and added READMEs ([#208](https://github.com/CiscoDevNet/ydk-gen/issues/208))

### 2016-11-30 version 0.5.2

#### Python

* CRUD service / Codec service / Netconf service improvements
  * Improved error handling for mismatched model API types ([#241](https://github.com/CiscoDevNet/ydk-gen/issues/241))
  * Fixed issues with certain operations in netconf service ([#247](https://github.com/CiscoDevNet/ydk-gen/issues/247), [#248](https://github.com/CiscoDevNet/ydk-gen/issues/248), [#252](https://github.com/CiscoDevNet/ydk-gen/issues/252), [#235](https://github.com/CiscoDevNet/ydk-gen/issues/235))
  * Fixed issue with CRUD service identityref keys ([#257](https://github.com/CiscoDevNet/ydk-gen/issues/257))

* Bundle improvements
  * Made generate.py executable ([#227](https://github.com/CiscoDevNet/ydk-gen/issues/227))
  * Removed auto capitalization of enum literals ([#230](https://github.com/CiscoDevNet/ydk-gen/issues/230))
  * Updated [`cisco-ios-xr` bundle](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_1_2.json) to support Cisco IOS XR 6.1.2 release ([#316](https://github.com/CiscoDevNet/ydk-gen/pull/316))

* Logging improvements
  * Improved logging for services and providers ([#251](https://github.com/CiscoDevNet/ydk-gen/issues/251), [#254](https://github.com/CiscoDevNet/ydk-gen/issues/254), [#280](https://github.com/CiscoDevNet/ydk-gen/issues/280), [#283](https://github.com/CiscoDevNet/ydk-gen/issues/283), [#284](https://github.com/CiscoDevNet/ydk-gen/issues/284))

* Documentation improvements
  * Added YDK logos and reorganized to be more readable ([#301](https://github.com/CiscoDevNet/ydk-gen/pull/301), [#296](https://github.com/CiscoDevNet/ydk-gen/pull/296), [#289](https://github.com/CiscoDevNet/ydk-gen/pull/289))
  * Improved documentation of YANG attributes like data type (configuration or state), default value, units, status etc ([#249](https://github.com/CiscoDevNet/ydk-gen/issues/249), [#290](https://github.com/CiscoDevNet/ydk-gen/issues/290))  
  * Improved netconf service documentation ([#235](https://github.com/CiscoDevNet/ydk-gen/issues/235))

#### C++ (alpha)

* Introduced support for C++ YDK bindings ([issue#118](https://github.com/CiscoDevNet/ydk-gen/issues/118), related [commits](https://github.com/manradhaCisco/ydk-gen/commits/ydk_core) and [pull requests](https://github.com/manradhaCisco/ydk-gen/pulls?q=is%3Apr+is%3Aclosed))
* Added support for CRUD, Validation and Codec services, along with Netconf and Codec providers, YDK types and errors
* Added support for Path API
* Used libyang and libnetconf libraries as part of the service and provider abstraction layer  
* Integrated with CMake build system
* Wrote unit tests using `boost::unit_test`
* Added support for logging using `boost::log`
* Added documentation using `sphinx`
* Integrated C++ testing into CI using travis-ci ([#286](https://github.com/CiscoDevNet/ydk-gen/issues/286))

### 2016-10-10 version 0.5.1

* Support for Python3
  * Introduced support for Python 3 ([#60](https://github.com/CiscoDevNet/ydk-gen/issues/60))
  * Both Python 2 and Python 3 are now supported for `ydk-gen` and `ydk-py`

* Bundle improvements
  * Improved usage of import statements in YDK model API to reduce chances of circular import dependency ([#216](https://github.com/CiscoDevNet/ydk-gen/issues/216))
  * Updated [`cisco-ios-xr` bundle](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_1_1.json) to support Cisco IOS XR 6.1.1 release ([#258](https://github.com/CiscoDevNet/ydk-gen/pull/258))
  * Updated [`openconfig` bundle](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_1.json) ([#258](https://github.com/CiscoDevNet/ydk-gen/pull/258))

* Documentation improvements
  * Improved documentation for bundle installation ([#244](https://github.com/CiscoDevNet/ydk-gen/pull/244))
  * Added documentation for executor service ([#263](https://github.com/CiscoDevNet/ydk-gen/pull/263))

### 2016-08-03 version 0.5.0
 * Introduced YDK bundles ([#43](https://github.com/CiscoDevNet/ydk-gen/issues/43), [#148](https://github.com/CiscoDevNet/ydk-gen/issues/148), [#149](https://github.com/CiscoDevNet/ydk-gen/issues/149))
  * Created YDK core library and pluggable namespace packages that share the same module prefix `ydk.models`
  * Generated documentation for YDK core and bundles

* CRUD service / Codec service / Netconf service improvements
  * Improved support for presence containers, nested enum and identity classes ([#169](https://github.com/CiscoDevNet/ydk-gen/pull/169))
  * Improved support for lists with multiple keys by ensuring that the order of keys is preserved ([#179](https://github.com/CiscoDevNet/ydk-gen/issues/179))
  * Improved support for leaf-list of identity type ([#186](https://github.com/CiscoDevNet/ydk-gen/issues/186))
  * Added check for user error which can occur when self-referencing YDK object as parent object ([#184](https://github.com/CiscoDevNet/ydk-gen/issues/184))
  * Improved error-reporting for commit-time error ([#190](https://github.com/CiscoDevNet/ydk-gen/issues/190))
  * Fixed CRUD read support for modules containing top-level list ([#194](https://github.com/CiscoDevNet/ydk-gen/issues/194))

* Testing improvements
  * Added Mac OS X installation and running codec service sanity tests to CI ([#175](https://github.com/CiscoDevNet/ydk-gen/pull/175))

* Documentation improvements
  * Indicated mandatory leafs in the documentation ([#177](https://github.com/CiscoDevNet/ydk-gen/issues/177))
  * Specified path to referred leaf for leafrefs ([#177](https://github.com/CiscoDevNet/ydk-gen/issues/177))
  * Fix documentation of presence containers ([#192](https://github.com/CiscoDevNet/ydk-gen/issues/192))
  * Enhanced documentation of leafs of identityref type by indicating all the subclasses of identity base class referred to by the identityref ([#161](https://github.com/CiscoDevNet/ydk-gen/issues/161))
  * Added documentation on how to use YDK delete operation and improved documentation for YDK read operation ([#204](https://github.com/CiscoDevNet/ydk-gen/pull/204))

### 2016-06-17 version 0.4.2
 * Error handling improvements
  * Fixed local validation to correctly check for types and values ([#116](https://github.com/CiscoDevNet/ydk-gen/issues/116))
  * Introduced error hierarchy to represent errors from various components, viz.: YPYModelErrors, YPYServiceError, YPYServiceProviderError ([#133](https://github.com/CiscoDevNet/ydk-gen/issues/133))
    * When raising YPYModelErrors, include errors dictionary with key as path to data, and value as tuple of error code and error message
  * Added more extensive negative test cases to ydk-gen to test handling of error ([#134](https://github.com/CiscoDevNet/ydk-gen/issues/134))
 * CRUD service / Codec service / Netconf service provider improvements
  * Added support for multiple objects to codec service ([#122](https://github.com/CiscoDevNet/ydk-gen/issues/122))
  * Added logging for codec service ([#97](https://github.com/CiscoDevNet/ydk-gen/issues/97))
  * Have logging hierarchy automatically follow package hierarchy ([#100](https://github.com/CiscoDevNet/ydk-gen/issues/100))
  * Have netconf service return YDK python objects instead of XML strings ([#120](https://github.com/CiscoDevNet/ydk-gen/issues/120))
  * Fixed decoding issue with leaf-list of enums ([#150](https://github.com/CiscoDevNet/ydk-gen/issues/150))
 * Removed requirements.txt from ydk-py and added all requirements to setup.py
 * Enforce PEP8 naming for Identity classes ([#152](https://github.com/CiscoDevNet/ydk-gen/issues/152))
 * Added full ydk-py version to the documentation ([#144](https://github.com/CiscoDevNet/ydk-gen/issues/144))

### 2016-05-20 version 0.4.1
 * Added openconfig bgp-policy APIs to ydk-py ([#102](https://github.com/CiscoDevNet/ydk-gen/issues/102))
 * Introduced ability to programmatically retrieve SDK version of ydk-py ([#8](https://github.com/CiscoDevNet/ydk-gen/issues/8))
 * Removed unused dependencies from ydk-py's requirements.txt ([#48](https://github.com/CiscoDevNet/ydk-gen/issues/48))
 * Introduced [coveralls](https://coveralls.io) and improved [travis CI](https://travis-ci.org) integration for ydk-gen github ([#84](https://github.com/CiscoDevNet/ydk-gen/issues/84), [#54](https://github.com/CiscoDevNet/ydk-gen/issues/54), [#15](https://github.com/CiscoDevNet/ydk-gen/issues/15), [#46](https://github.com/CiscoDevNet/ydk-gen/issues/46))
 * CRUD service / Netconf service provider improvements
  * Added timeout parameter to NetconfServiceProvider ([#1](https://github.com/CiscoDevNet/ydk-gen/issues/1))
  * Fixed issues with decoding leafs of union type and nodes defined in sub-modules  ([#5](https://github.com/CiscoDevNet/ydk-gen/issues/5), [#56](https://github.com/CiscoDevNet/ydk-gen/issues/56))
  * Fixed issue with encoding enums, identities defined in external modules ([#30](https://github.com/CiscoDevNet/ydk-gen/issues/30), [#103](https://github.com/CiscoDevNet/ydk-gen/issues/103))
  * Improved support for deleting leafs, leaf-lists and lists ([#55](https://github.com/CiscoDevNet/ydk-gen/issues/55), [#103](https://github.com/CiscoDevNet/ydk-gen/issues/103))
 * Documentation improvements
  * Added 'About ydk-py' page with information about ydk-gen used to generate ydk-py ([#6](https://github.com/CiscoDevNet/ydk-gen/issues/6))
  * Indicate in documentation YDK class attributes that are keys ([#41](https://github.com/CiscoDevNet/ydk-gen/issues/41))
  * Made top containers show up at the top of the table of contents for every module document ([#39](https://github.com/CiscoDevNet/ydk-gen/issues/39))

### 2016-04-15 version 0.4.0

  * Introduced netconf service and codec service
    * Netconf service provides APIs to execute netconf operations
    * Codec service provides APIs to encode python objects and decode payloads
  * Support for yang deviation
  * Support for subscribing to model-driven telemetry
  * Logging made more consistent
    * CRUDService outputs type of operation
    * When logging is enabled, all NETCONF messages are logged including commit
    * Log messages at various stages (send RPC request, receive reply, commit
      etc) instead of logging all at once at the end
  * Updated enums in YDK classes to use enum34
    * Improved enum documentation
  * Improved error reporting for ydk-py and ydk-gen

### 2016-03-11 version 0.3.0:

  * First public release.
