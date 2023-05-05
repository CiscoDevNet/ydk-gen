### 2020-06-30 version 0.8.5

#### New features and enhancements
  * Develop utility function to clone Entity instance ([#967](https://github.com/CiscoDevNet/ydk-gen/issues/967))
  * Go CodecService fails to encode leaf-list data ([#968](https://github.com/CiscoDevNet/ydk-gen/issues/968))
  * Go fails to process filters on enum and identity leaves and leaf-lists ([#969](https://github.com/CiscoDevNet/ydk-gen/issues/969))

#### Bundle improvements
  * Updated cisco-ios-xr bundle to support Cisco IOS XR 6.6.3
  * Updated openconfig bundle to support YANG models of revision "2019-06-21"

#### Bundle improvements
  * Updated cisco-ios-xr bundle to support Cisco IOS XR 6.6.3
  * Updated openconfig bundle to support YANG models of revision "2019-06-21"


### 2019-10-15 version 0.8.4

#### New features and enhancements
  * Added utility function to compare two entities and build diff ([#925](https://github.com/CiscoDevNet/ydk-gen/issues/925))

#### Resolved GitHub issues
  * 'ietf-netconf-monitoring:get-schema' returns schema with escape sequences ([#614](https://github.com/CiscoDevNet/ydk-gen/issues/614))
  * NETCONF provider should raise more appropriate exceptions ([#774](https://github.com/CiscoDevNet/ydk-gen/issues/774))
  * Delete operation fails on container ([#931](https://github.com/CiscoDevNet/ydk-gen/issues/931))
  * Absolute path for generated entity of second level list child is incorrect ([#933](https://github.com/CiscoDevNet/ydk-gen/issues/933))
  * Go: When entity having children is built from model API the child entities parent is nil ([#938](https://github.com/CiscoDevNet/ydk-gen/issues/938))

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

##### Note
  The solution for GitHub issue ([#891](https://github.com/CiscoDevNet/ydk-gen/issues/891)) changed model API. However all model bundles generated with YDK-Gen version 0.7.3 and later are still compatible with core YDK components.


### 2019-03-15 version 0.8.2

#### Resolved GitHub issues
  * Libyang error: Reached limit (65535) for storing typedefs ([#874](https://github.com/CiscoDevNet/ydk-gen/issues/874))
  * Segmentation Fault in API when connection to Netconf fails ([#879](https://github.com/CiscoDevNet/ydk-gen/issues/879))
  * 'delete' and 'replace' filters do not work properly with gNMI and CRUD service ([#881](https://github.com/CiscoDevNet/ydk-gen/issues/881))
  * Go compiler failed to link gNMI service on Mac ([#892](https://github.com/CiscoDevNet/ydk-gen/issues/892))

#### Model bundle additions
  * Released cisco-ios-xr bundle to support Cisco IOS XR 6.5.2
  * Released cisco-ios-xe bundle to support Cisco IOS XE 16.9.3
  * Released cisco-nx-os bundle to support Cisco NX OS 9.2.3


### 2019-02-11 version 0.8.1

#### Go
  * Introduced support for non-top level objects/entities for CRUD, Netconf, and gNMI services.

#### ydk-gen
  * Added `generate.py` script options '-i --install' and '-s --sudo' to generate and install packages in one step.

#### Resolved GitHub issues
  * YFilter dropped when used with NetconfService and EntityCollection ([#803](https://github.com/CiscoDevNet/ydk-gen/issues/803))
  * YDK fails process RPC payload, which contain 'data' tag ([#851](https://github.com/CiscoDevNet/ydk-gen/issues/851))
  * gNMI script results in a segmentation fault if repository is missing a required model ([#852](https://github.com/CiscoDevNet/ydk-gen/issues/852))
  * CodecService fails correctly encode multiple entries in keyless list ([#854](https://github.com/CiscoDevNet/ydk-gen/issues/854))
  * Decoding gNMI payload fails with ValueError exception ([#858](https://github.com/CiscoDevNet/ydk-gen/issues/858))


### 2018-12-17 version 0.8.0

#### CRUD / Netconf / gNMI/ Codec / Path API
  * Introduced YDK support for gNMI protocol (protobuf version 0.4.0) including CRUD service with gNMI Service Provider.
  * Added Netconf support for certificate-based authentication for multiple servers

#### Bundle improvements
  * Updated cisco-ios-xr bundle to include previously missing action models in Cisco IOS XR 6.5.1 release
  * Released cisco-nx-os bundle to support Cisco NX OS 9.2.2 release


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
```

### 2018-07-02 version 0.7.2

#### Bundle improvements
* Released [`cisco-nx-os`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-nx-os-0_7_4.json) bundle to support Cisco NX OS 7.0-3-I7-4 release
* Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_4_1.json) bundle to support Cisco IOS XR 6.4.1 release
* Updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_6.json) bundle to introduce support for additional AFT models.
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

##### Documentation improvements
* Enhanced Go documentation ([#681](https://github.com/CiscoDevNet/ydk-gen/issues/681), [#684](https://github.com/CiscoDevNet/ydk-gen/issues/684), [#720](https://github.com/CiscoDevNet/ydk-gen/issues/720))
* Improved documentation logos ([#754](https://github.com/CiscoDevNet/ydk-gen/issues/754), [#755](https://github.com/CiscoDevNet/ydk-gen/issues/755), [#756](https://github.com/CiscoDevNet/ydk-gen/issues/756))

### 2018-04-09 version 0.7.1

#### Bundle improvements
  * Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_3_2.json) to support Cisco IOS XR 6.3.2 release
  * Updated [`cisco-ios-xe`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_8_1.json) to support Cisco IOS XE 16.8.1 release
  * Also updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_5.json) and [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_5.json) bundles

#### CRUD / Netconf / Codec / Path API improvements
  * Introduced support for multiple entities in Python and C++ ([#713](https://github.com/CiscoDevNet/ydk-gen/issues/713), [#719](https://github.com/CiscoDevNet/ydk-gen/issues/719), [#726](https://github.com/CiscoDevNet/ydk-gen/issues/726), [#736](https://github.com/CiscoDevNet/ydk-gen/issues/736))
  * Added support for yang models with more than 256 typedefs ([#678](https://github.com/CiscoDevNet/ydk-gen/issues/678), [#607](https://github.com/CiscoDevNet/ydk-gen/issues/607))
  * Fixed segfault with a `cisco-ios-xe` model ([#627](https://github.com/CiscoDevNet/ydk-gen/issues/627))
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

#### Go
##### Introduced Go language YDK support (alpha version)
* Added support for all existing `ydk core` services, providers, types and errors in Go
* Added support for all existing `ydk bundles` including `ietf`, `openconfig`, `cisco-ios-xr` and `cisco-ios-xe` in Go
* [#673](https://github.com/CiscoDevNet/ydk-gen/pull/673), [#663](https://github.com/CiscoDevNet/ydk-gen/pull/), [#660](https://github.com/CiscoDevNet/ydk-gen/pull/660), [#658](https://github.com/CiscoDevNet/ydk-gen/pull/658), [#606](https://github.com/CiscoDevNet/ydk-gen/pull/606), [#605](https://github.com/CiscoDevNet/ydk-gen/pull/605)
