### 2018-10-02 version 0.7.3

#### Bundle improvements
  * Updated [`cisco-ios-xr`](http://ydk.cisco.com/py/docs/ydk.models.cisco_ios_xr.html) bundle to support Cisco IOS XR 6.5.1 release.
  * Updated [`cisco-ios-xe`](http://ydk.cisco.com/py/docs/ydk.models.cisco_ios_xe.html) bundle to support Cisco IOS XE 16.9.1 release.
  * Released [`cisco-nx-os`](http://ydk.cisco.com/py/docs/ydk.models.cisco_nx_os.html) bundle to support Cisco NX OS 9.2.1 release.
  * Updated [`openconfig`](http://ydk.cisco.com/py/docs/ydk.models.openconfig.html) to  to make it compatible with `ydk core` version 0.7.3.
  * Also updated [`ietf`](http://ydk.cisco.com/py/docs/ydk.models.ietf.html) bundle to make it compatible with `ydk core` version 0.7.3.

#### Python, C++ & Go

##### CRUD/ NETCONF / Codec API improvements
  * Introduced Codec feature to decode multiple JSON payload. ([#812](https://github.com/CiscoDevNet/ydk-gen/issues/812))
  * Improved support for `YList` ([#811](https://github.com/CiscoDevNet/ydk-gen/pull/811))

#### Note about `cisco-ios-xr` 6.5.1 bundle
  * Bundle excludes the following files due to duplicate namespaces:
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

##### Bundle improvements
* Released [`cisco-nx-os`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-nx-os-0_7_4.json) bundle to support Cisco NX OS 7.0-3-I7-4 release
* Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_4_1.json) bundle to support Cisco IOS XR 6.4.1 release
* Updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_6.json) bundle to introduce support for additional AFT models.
* Updated [`cisco-ios-xe`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_8_1_post1.json) bundle to continue to support Cisco IOS XE 16.8.1 release and make it compatible with `ydk core` version 0.7.2
* Also updated [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_5_post1.json) bundle to make it compatible with `ydk core` version 0.7.2
##### CRUD / Netconf / Codec / Path API improvements
* Introduced support for key-based access to list items in Python, C++ and Go model API ([#231](https://github.com/CiscoDevNet/ydk-gen/issues/231))
* Introduced support for multiple entities in Go ([#768](https://github.com/CiscoDevNet/ydk-gen/pull/768))
* Improved support for YANG `presence` nodes ([#629](https://github.com/CiscoDevNet/ydk-gen/pull/629), [#738](https://github.com/CiscoDevNet/ydk-gen/pull/738), [#763](https://github.com/CiscoDevNet/ydk-gen/pull/763))
* Fixed issue with invoking sequential CRUD operations on different model APIs ([#727](https://github.com/CiscoDevNet/ydk-gen/issues/727))
* Improved NETCONF service commit API ([#796](https://github.com/CiscoDevNet/ydk-gen/issues/796))
* Enhanced support for leaf value patterns ([#786](https://github.com/CiscoDevNet/ydk-gen/issues/786))
##### Netconf provider improvements
* Improved support for YANG `feature`s included in NETCONF hello message ([#777](https://github.com/CiscoDevNet/ydk-gen/issues/777))
##### Documentation improvements
* Enhanced Go documentation ([#681](https://github.com/CiscoDevNet/ydk-gen/issues/681), [#684](https://github.com/CiscoDevNet/ydk-gen/issues/684), [#720](https://github.com/CiscoDevNet/ydk-gen/issues/720))
* Improved documentation logos ([#754](https://github.com/CiscoDevNet/ydk-gen/issues/754), [#755](https://github.com/CiscoDevNet/ydk-gen/issues/755), [#756](https://github.com/CiscoDevNet/ydk-gen/issues/756))
##### ydk-gen improvements
* Fixed issue with mixed case objects in Go language model API ([#686](https://github.com/CiscoDevNet/ydk-gen/issues/686))

### 2018-04-09 version 0.7.1

#### Python, C++ & Go

##### Bundle improvements
  * Updated [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_3_2.json) to support Cisco IOS XR 6.3.2 release
  * Updated [`cisco-ios-xe`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xe_16_8_1.json) to support Cisco IOS XE 16.8.1 release
  * Also updated [`openconfig`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/openconfig_0_1_5.json) and [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_5.json) bundles
##### CRUD / Netconf / Codec / Path API improvements
  * Introduced support for multiple entities in Python and C++ ([#713](https://github.com/CiscoDevNet/ydk-gen/issues/713), [#719](https://github.com/CiscoDevNet/ydk-gen/issues/719), [#726](https://github.com/CiscoDevNet/ydk-gen/issues/726), [#736](https://github.com/CiscoDevNet/ydk-gen/issues/736))
  * Added support for yang models with more than 256 typedefs ([#678](https://github.com/CiscoDevNet/ydk-gen/issues/678), [#607](https://github.com/CiscoDevNet/ydk-gen/issues/607))
  * Fixed segfault with a `cisco-ios-xe` model ([#627](https://github.com/CiscoDevNet/ydk-gen/issues/627))
  * Changed default arguments to be more pythonic ([#682](https://github.com/CiscoDevNet/ydk-gen/issues/682))
  * Handled XML escape characters included in yang models ([#683](https://github.com/CiscoDevNet/ydk-gen/issues/683))
  * Improved handling XML declaration in XML payloads ([#662](https://github.com/CiscoDevNet/ydk-gen/issues/662))
  * Fixed support for yang models with lists as top-level nodes ([#728](https://github.com/CiscoDevNet/ydk-gen/issues/728))
  * Added support for yang 1.1 `action` statement in path API ([#717](https://github.com/CiscoDevNet/ydk-gen/issues/717))
##### Netconf provider improvements
  * Added support for connecting to devices with no `get-schema` support ([#554](https://github.com/CiscoDevNet/ydk-gen/issues/544))
##### ydk-gen improvements
  * Updated leafs in python model APIs to use native python types. ([604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Improved the size and performance of Golang model APIs ([604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Fixed issue with handling of some typedefs in Golang ([706](https://github.com/CiscoDevNet/ydk-gen/issues/706), [747](https://github.com/CiscoDevNet/ydk-gen/issues/747))
##### Documentation improvements
  * Improved enum documentation ([716](https://github.com/CiscoDevNet/ydk-gen/issues/716))
  * Enhanced table of contents for documentation ([715](https://github.com/CiscoDevNet/ydk-gen/issues/715))
##### Testing/error improvements
  * Improved ydk-gen error reporting and fixed `--one-class-per-module` option of generating python packages ([604](https://github.com/CiscoDevNet/ydk-gen/issues/604))
  * Added coverage for Golang and C++ ([740](https://github.com/CiscoDevNet/ydk-gen/issues/740), [705](https://github.com/CiscoDevNet/ydk-gen/issues/705))
##### Installation improvements
  * Introduced automated docker builds to produce docker images with `ydk-gen`, `ydk-py` and `ydk-go` pre-installed ([724](https://github.com/CiscoDevNet/ydk-gen/issues/724))
  * Removed `epel-release` as one of the requirements for libydk RPM ([#627](https://github.com/CiscoDevNet/ydk-gen/issues/627))
  * Added testing for `libydk` packages ([604](https://github.com/CiscoDevNet/ydk-gen/issues/604))

### 2018-01-31 version 0.7.0

#### Go
##### Introduced Go language YDK support (alpha version)
* Added support for all existing `ydk core` services, providers, types and errors in Go
* Added support for all existing `ydk bundles` including `ietf`, `openconfig`, `cisco-ios-xr` and `cisco-ios-xe` in Go
* [#673](https://github.com/CiscoDevNet/ydk-gen/pull/673), [#663](https://github.com/CiscoDevNet/ydk-gen/pull/), [#660](https://github.com/CiscoDevNet/ydk-gen/pull/660), [#658](https://github.com/CiscoDevNet/ydk-gen/pull/658), [#606](https://github.com/CiscoDevNet/ydk-gen/pull/606), [#605](https://github.com/CiscoDevNet/ydk-gen/pull/605)