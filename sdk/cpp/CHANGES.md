### 2023-08-31 version 0.8.6.5

#### New features and enhancements
  * Added support for Ubuntu:jammy (22.04)

#### Resolved GitHub issues
  * Segmentation Fault on Python application exit with Python 3.9.5 ([#1050](https://github.com/CiscoDevNet/ydk-gen/issues/1050))
  * ydk.io and ciscodevnet.github.io/ydk-py/ not working correctly ([#1088](https://github.com/CiscoDevNet/ydk-gen/issues/1088))
  * cannot decode json blob in python without encoding it first ([#1089](https://github.com/CiscoDevNet/ydk-gen/issues/1089))

### 2023-01-31 version 0.8.6.4

#### New features and enhancements
  * install_ydk.sh: Installation of Python YDK packages should not require root access ([#1069](https://github.com/CiscoDevNet/ydk-gen/issues/1069))
  * Expanded unit test cases to retrieve list elements

#### Resolved GitHub issues
  * CodecService fails to decode XML in python when referencing namespace prefix ([#1071](https://github.com/CiscoDevNet/ydk-gen/issues/1071))
  * not_eq is a c++ keyword ([#1080](https://github.com/CiscoDevNet/ydk-gen/issues/1080))
  * libyang issue with arguments that are swapped ([#1081](https://github.com/CiscoDevNet/ydk-gen/issues/1081))

### Resolved non-GitHub issues
  * Fixed and tested C++ sample apps


### 2022-07-31 version 0.8.6.3

#### Resolved GitHub issues
  * IndexError on decoding empty string value in JSON payload ([#1061](https://github.com/CiscoDevNet/ydk-gen/issues/1061))
  * ExecutorService fails on non-ietf RPC with Libyang error ([#1067](https://github.com/CiscoDevNet/ydk-gen/issues/1067))


### 2022-03-31 version 0.8.6.2

#### New features and enhancements
  * Added support for MacOS Big Sur

#### Resolved GitHub issues
  * getting YCodecError when leaf value has string 'sftp://' ([#1056](https://github.com/CiscoDevNet/ydk-gen/issues/1056))
  * vrf route-target leaf stitching(bool) fails as invalid ([#1059](https://github.com/CiscoDevNet/ydk-gen/issues/1059))

#### Resolved non-GitHub issues
  * Corrected few Codacy issues
  * Addressed segmentation fault error when required modules are not loaded.
  * Provided patch for catch.hpp, which ensures definition of constant SIGSTKSZ


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
  * Added Entity class member function to clone entity object
  * Added shortcut operator to set `yfilter` value in YLeaf and YLeafList class instances
  * Develop utility function to clone Entity instance ([#967](https://github.com/CiscoDevNet/ydk-gen/issues/967))

#### Resolved GitHub issues
  * Fix JsonSubtreeCodec only serializing up to 4B numbers
  * Fixed bug in extracting module name from namespace
  * gNMI set RPC fails when JSON payload is formatted string
  * Restconf in YDK should support HTTPS (partialy resolved) ([#917](https://github.com/CiscoDevNet/ydk-gen/issues/917))
  * C++ YList class fails to process key values when its name contains '-' character ([#997](https://github.com/CiscoDevNet/ydk-gen/issues/997))
  * create_datanode in Path API returns incorrect object and sets incorrect value ([#1003](https://github.com/CiscoDevNet/ydk-gen/issues/1003))
  * create_datanode rejects absolute paths with a leading "/" as defined in RFC 7950 ([#1005](https://github.com/CiscoDevNet/ydk-gen/issues/1005))
  * Path API create_datanode rejects valid value ([#1006](https://github.com/CiscoDevNet/ydk-gen/issues/1006))
  * data tag not recognized when attribute present ([#1021](https://github.com/CiscoDevNet/ydk-gen/issues/1021))
  * c++ generated bundle class fails to compile in gcc when model contains leaf 'minor' ([#1024](https://github.com/CiscoDevNet/ydk-gen/issues/1024))

#### Bundle improvements
  * Updated cisco-ios-xr bundle to support Cisco IOS XR 6.6.3
  * Updated openconfig bundle to support YANG models of revision "2019-06-21"


### 2019-10-15 version 0.8.4

#### New features and enhancements
  * Added utility function to compare two entities and build diff ([#925](https://github.com/CiscoDevNet/ydk-gen/issues/925))
  * Adjusted C++ code to work with gcc compiler version 4.8.5 (default for CentOS-7.x)

#### Resolved GitHub issues
  * 'ietf-netconf-monitoring:get-schema' returns schema with escape sequences ([#614](https://github.com/CiscoDevNet/ydk-gen/issues/614))
  * NETCONF provider should raise more appropriate exceptions ([#774](https://github.com/CiscoDevNet/ydk-gen/issues/774))
  * YList test fails when list entity added before keys are initialized ([#800](https://github.com/CiscoDevNet/ydk-gen/issues/800))
  * Delete operation fails on container ([#931](https://github.com/CiscoDevNet/ydk-gen/issues/931))
  * Absolute path for generated entity of second level list child is incorrect ([#933](https://github.com/CiscoDevNet/ydk-gen/issues/933))
  * gNMI Provider does not preserve original strings ([#940](https://github.com/CiscoDevNet/ydk-gen/issues/940))
  * gNMISession fails to decode GetRequest response when returned values is empty string ([#943](https://github.com/CiscoDevNet/ydk-gen/issues/943))
  * No way to retrieve numeric value of enums in cpp generated code ([#944](https://github.com/CiscoDevNet/ydk-gen/issues/944))
  * XmlSubtreeCodec fails decode payload with augmented component ([#956](https://github.com/CiscoDevNet/ydk-gen/issues/956))
  * Debian package for C++ bundles getting installed to wrong location ([#962](https://github.com/CiscoDevNet/ydk-gen/issues/962))

#### Documentation improvements
  * Added documentation for XmlSubtreeCodec and JsonSubtreeCodec.
  * Improved documentation for System Requirements and Python Requirements

#### Bundle improvements
  * Updated cisco-ios-xr bundle to support Cisco IOS XR 5.2.3
  * Released cisco-nx-os bundle to support Cisco NX OS 9.3.1


### 2019-05-15 version 0.8.3

#### Bundle improvements
  * Updated cisco-ios-xr bundle to support Cisco IOS XR 6.6.2
  * Updated openconfig bundle to introduce support for additional models.  

#### Resolved GitHub issues
  * YDK netconf read fails when <data> tag has namespace prefix ([#799](https://github.com/CiscoDevNet/ydk-gen/issues/799))
  * YDK return value of YANG action missing some attributes ([#871](https://github.com/CiscoDevNet/ydk-gen/issues/871))
  * Duplicate code in generated cisco-ios-xe Go bundle ([#891](https://github.com/CiscoDevNet/ydk-gen/issues/891))
  * Memory leaks in YDK C++ core ([#899](https://github.com/CiscoDevNet/ydk-gen/issues/899))
  * Memory leaks in YDK C++ gNMI Service component ([#902](https://github.com/CiscoDevNet/ydk-gen/issues/902))
  * RPC execution stuck when NETCONF server closes session unexpectedly ([#914](https://github.com/CiscoDevNet/ydk-gen/issues/914))
  * YDK attempting to send Commit command when 'writable-running' in capabilities ([#915](https://github.com/CiscoDevNet/ydk-gen/issues/915))

##### Note
  The solution for GitHub issue ([#891](https://github.com/CiscoDevNet/ydk-gen/issues/891)) changed model API. However all model bundles generated with YDK-Gen version 0.7.3 and later are still compatible with core YDK components.


### 2019-03-15 version 0.8.2

#### New features and enhancements
  * Expanded disabling of data validation to read operations with NetconfServiceProvider.
  * Expanded disabling of data validation to gNMIServiceProvider.

#### Resolved GitHub issues
  * Libyang error: Reached limit (65535) for storing typedefs ([#874](https://github.com/CiscoDevNet/ydk-gen/issues/874))
  * Segmentation Fault in API when connection to Netconf fails ([#879](https://github.com/CiscoDevNet/ydk-gen/issues/879))
  * 'delete' and 'replace' filters do not work properly with gNMI and CRUD service ([#881](https://github.com/CiscoDevNet/ydk-gen/issues/881))

#### Model bundle additions
  * Released cisco-ios-xr bundle to support Cisco IOS XR 6.5.2
  * Released cisco-ios-xe bundle to support Cisco IOS XE 16.9.3
  * Released cisco-nx-os bundle to support Cisco NX OS 9.2.3


### 2019-02-11 version 0.8.1

#### CRUD / Netconf / gNMI/ Codec / Path API
  * Introduced support for non-top level objects/entities for CRUD, Netconf, and gNMI services.
  * Introduced a feature, which allows to disable validation of entity data processed by NetconfServiceProvider.

#### ydk-gen
  * Use CMake build number for release numbers with suffix ([#864](https://github.com/CiscoDevNet/ydk-gen/issues/864))
  * Added `generate.py` script options '-i --install' and '-s --sudo' to generate and install packages in one step.

#### Resolved GitHub issues
  * YFilter dropped when used with NetconfService and EntityCollection ([#803](https://github.com/CiscoDevNet/ydk-gen/issues/803))
  * YDK fails process RPC payload, which contain 'data' tag ([#851](https://github.com/CiscoDevNet/ydk-gen/issues/851))
  * gNMI script results in a segmentation fault if repository is missing a required model ([#852](https://github.com/CiscoDevNet/ydk-gen/issues/852))
  * CodecService fails correctly encode multiple entries in keyless list ([#854](https://github.com/CiscoDevNet/ydk-gen/issues/854))
  * Segmentation fault when decoding gNMI config payload ([#856](https://github.com/CiscoDevNet/ydk-gen/issues/856))
  * Decoding gNMI payload fails with ValueError exception ([#858](https://github.com/CiscoDevNet/ydk-gen/issues/858))

#### Documentation improvements
  Addressed multiple documentation issues: [#848](https://github.com/CiscoDevNet/ydk-gen/issues/848),
  [#850](https://github.com/CiscoDevNet/ydk-gen/issues/850), [#861](https://github.com/CiscoDevNet/ydk-gen/issues/861),
  [#862](https://github.com/CiscoDevNet/ydk-gen/issues/862)


### 2018-11-27 version 0.8.0

#### CRUD / Netconf / gNMI/ Codec / Path API
  * Introduced YDK support for gNMI protocol (protobuf version 0.4.0) including CRUD service with gNMI Service Provider.
  * Added Netconf support for certificate-based authentication for multiple servers

#### Bundle improvements
  * Updated cisco-ios-xr bundle to include previously missing action models in Cisco IOS XR 6.5.1 release
  * Released cisco-nx-os bundle to support Cisco NX OS 9.2.2 release

#### ydk-gen
  * Added capability to generate YDK service packages from `generate.py` script.

### 2018-10-02 version 0.7.3

#### Resolved issues
  * Introduced Codec feature to decode multiple JSON payload. (#812)
  * Improved support for YList (#811)
  * Improved handling of Python native types in model API. (#733)
  * Added capability to validate leaf values based on Python type of model API. (#739)
  * Improved checking of invalid attributes for model API objects. (#815)

#### Bundle improvements
  * Updated cisco-ios-xr bundle to support Cisco IOS XR 6.5.1 release.
  * Updated cisco-ios-xe bundle to support Cisco IOS XE 16.9.1 release.
  * Released cisco-nx-os bundle to support Cisco NX OS 9.2.1 release.
  * Updated openconfig to to make it compatible with ydk core version 0.7.3.
  * Also updated ietf bundle to make it compatible with ydk core version 0.7.3.


#### Note about `cisco-ios-xr` 6.5.1 bundle
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

### 2018-07-02 version 0.7.2

#### Bundle improvements
* Released [`cisco-nx-os`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-nx-os-0_7_4.json) bundle to support Cisco NX OS 7.0-3-I7-4 release
* Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_4_1.json) bundle to support Cisco IOS XR 6.4.1 release
* Updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_6.json) bundle to introduce support for additional AFT models.
* Updated [`cisco-ios-xe`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_8_1_post1.json) bundle to continue to support Cisco IOS XE 16.8.1 release and make it compatible with `ydk core` version 0.7.2
* Also updated [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_5_post1.json) bundle to make it compatible with `ydk core` version 0.7.2

#### CRUD / Netconf / Codec / Path API improvements
* Introduced support for key-based access to list items in Python, C++ and Go model API ([#231](https://github.com/CiscoDevNet/ydk-gen/issues/231))
* Improved support for YANG `presence` nodes ([#629](https://github.com/CiscoDevNet/ydk-gen/pull/629), [#738](https://github.com/CiscoDevNet/ydk-gen/pull/738), [#763](https://github.com/CiscoDevNet/ydk-gen/pull/763))
* Fixed issue with invoking sequential CRUD operations on different model APIs ([#727](https://github.com/CiscoDevNet/ydk-gen/issues/727))
* Improved NETCONF service commit API ([#796](https://github.com/CiscoDevNet/ydk-gen/issues/796))
* Enhanced support for leaf value patterns ([#786](https://github.com/CiscoDevNet/ydk-gen/issues/786))
* Improved support for YANG `feature`s included in NETCONF hello message ([#777](https://github.com/CiscoDevNet/ydk-gen/issues/777))

#### Documentation improvements
* Improved documentation logos ([#754](https://github.com/CiscoDevNet/ydk-gen/issues/754), [#755](https://github.com/CiscoDevNet/ydk-gen/issues/755), [#756](https://github.com/CiscoDevNet/ydk-gen/issues/756))

### 2018-04-09 version 0.7.1

#### Bundle improvements
  * Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_3_2.json) to support Cisco IOS XR 6.3.2 release
  * Updated [`cisco-ios-xe`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_8_1.json) to support Cisco IOS XE 16.8.1 release
  * Also updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_5.json) and [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_5.json) bundles

#### CRUD / Netconf / Codec / Path API improvements
  * Introduced support for multiple entities in Python and C++ ([#713](https://github.com/CiscoDevNet/ydk-gen/issues/713), [#719](https://github.com/CiscoDevNet/ydk-gen/issues/719), [#726](https://github.com/CiscoDevNet/ydk-gen/issues/726), [#736](https://github.com/CiscoDevNet/ydk-gen/issues/736))
  * Added support for yang models with more than 256 typedefs ([#678](https://github.com/CiscoDevNet/ydk-gen/issues/678), [#607](https://github.com/CiscoDevNet/ydk-gen/issues/607))
  * Fixed segfault with `cisco-ios-xe` model ([#627](https://github.com/CiscoDevNet/ydk-gen/issues/627))
  * Changed default arguments to be more pythonic ([#682](https://github.com/CiscoDevNet/ydk-gen/issues/682))
  * Handled XML escape characters included in yang models ([#683](https://github.com/CiscoDevNet/ydk-gen/issues/683))
  * Improved handling XML declaration in XML payloads ([#662](https://github.com/CiscoDevNet/ydk-gen/issues/662))
  * Fixed support for yang models with lists as top-level nodes ([#728](https://github.com/CiscoDevNet/ydk-gen/issues/728))
  * Added support for yang 1.1 `action` statement in path API ([#717](https://github.com/CiscoDevNet/ydk-gen/issues/717))
  * Added support for connecting to devices with no `get-schema` support ([#554](https://github.com/CiscoDevNet/ydk-gen/issues/544))

#### ydk-gen improvements
  * Updated leafs in python model APIs to use native python types. ([604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Improved the size and performance of Golang model APIs ([604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Fixed issue with handling of some typedefs in Golang ([706](https://github.com/CiscoDevNet/ydk-gen/issues/706), [747](https://github.com/CiscoDevNet/ydk-gen/issues/747))

#### Documentation improvements
  * Improved enum documentation ([716](https://github.com/CiscoDevNet/ydk-gen/issues/716))
  * Enhanced table of contents for documentation ([715](https://github.com/CiscoDevNet/ydk-gen/issues/715))

#### Testing/error improvements
  * Improved ydk-gen error reporting and fixed `--one-class-per-module` option of generating python packages ([604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Added coverage for Golang and C++ ([740](https://github.com/CiscoDevNet/ydk-gen/issues/740), [705](https://github.com/CiscoDevNet/ydk-gen/issues/705))

#### Installation improvements
  * Introduced automated docker builds to produce docker images with `ydk-gen`, `ydk-py` and `ydk-go` pre-installed ([724](https://github.com/CiscoDevNet/ydk-gen/issues/724))
  * Removed `epel-release` as one of the requirements for libydk RPM ([#627](https://github.com/CiscoDevNet/ydk-gen/issues/627))
  * Added testing for `libydk` packages ([604](https://github.com/CiscoDevNet/ydk-gen/issues/604))

### 2018-01-31 version 0.7.0

#### Python, C++ & Go
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

#### C++
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

#### Python and C++
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

* Added equality operator for model API objects ([#432](https://github.com/CiscoDevNet/ydk-gen/pull/432))
* Improved handling of presence `container`s ([#437](https://github.com/CiscoDevNet/ydk-gen/pull/437))
* Added [`cisco-ios-xe` bundle](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_5_1.json) to support Cisco IOS XE 16.5.1 release

#### Documentation
* Improved getting-started guides for YDK-Py and YDK-Cpp  ([#418](https://github.com/CiscoDevNet/ydk-gen/pull/418), [#419](https://github.com/CiscoDevNet/ydk-gen/pull/419))
* Made table of contents for bundle documentation be sorted alphabetically ([#446](https://github.com/CiscoDevNet/ydk-gen/pull/418), [#419](https://github.com/CiscoDevNet/ydk-gen/pull/446))
* Improved documentation of `rpc` classes ([#435](https://github.com/CiscoDevNet/ydk-gen/issues/435))

### 2017-03-17 version 0.5.4

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
 * Updated [`cisco-ios-xr` bundle](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_2_1.json) to support Cisco IOS XR 6.2.1 release

#### ydk-gen
 * Improved generation of C++ code to consume less compile-time memory for large yang models ([#386](https://github.com/CiscoDevNet/ydk-gen/pull/386)), ([#336](https://github.com/CiscoDevNet/ydk-gen/pull/336))
 * Add check for valid profile file to ydkgen ([#378](https://github.com/CiscoDevNet/ydk-gen/pull/378))
 * Added code coverage for C++ code ([#373](https://github.com/CiscoDevNet/ydk-gen/pull/373))

#### Documentation
 * Separated top data classes from type classes in table of contents ([#372](https://github.com/CiscoDevNet/ydk-gen/pull/372))
 * Fixed ydk version not being correctly printed for C++ documentation ([#374](https://github.com/CiscoDevNet/ydk-gen/pull/374))
 * Indicate bundle version in C++ and python bundle documentation ([#383](https://github.com/CiscoDevNet/ydk-gen/pull/383))

### 2017-01-30 version 0.5.3:

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
* Improved look and feel of documentation ([#361](https://github.com/CiscoDevNet/ydk-gen/pull/361), [#356](https://github.com/CiscoDevNet/ydk-gen/pull/356))

### 2016-11-30 version 0.5.2 (alpha):

* Introduced support for C++ YDK bindings ([issue#118](https://github.com/CiscoDevNet/ydk-gen/issues/118), related [commits](https://github.com/manradhaCisco/ydk-gen/commits/ydk_core) and [pull requests](https://github.com/manradhaCisco/ydk-gen/pulls?q=is%3Apr+is%3Aclosed))
* Added support for CRUD, Validation and Codec services, along with Netconf and Codec providers, YDK types and errors
* Added support for Path API
* Used libyang and libnetconf libraries as part of the service and provider abstraction layer  
* Integrated with CMake build system
* Wrote unit tests using `boost::unit_test`
* Added support for logging using `boost::log`
* Added documentation using `sphinx`
* Integrated C++ testing into CI using travis-ci ([#286](https://github.com/CiscoDevNet/ydk-gen/issues/286))
