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

#### C++
* Renamed some of the methods in path API to be verbs like `DataNode::get_schema()` instead of nouns like `DataNode::schema()` ([#498](https://github.com/CiscoDevNet/ydk-gen/pull/498))
* Add option for TCP transport in `NetconfServiceProvider` ([#476](https://github.com/CiscoDevNet/ydk-gen/pull/476), [#444](https://github.com/CiscoDevNet/ydk-gen/pull/444))
* Support `get`/`get-config` with no filter in path API ([#503](https://github.com/CiscoDevNet/ydk-gen/pull/503))
* Introduce optimized on-demand yang model downloading for `NetconfServiceProvider` ([#499](https://github.com/CiscoDevNet/ydk-gen/pull/499))
* Add support for choosing either a per-device or a common cache for storing downloaded yang models ([#502](https://github.com/CiscoDevNet/ydk-gen/pull/502))
* Introduced encoding/decoding subtree XML in `CodecService` and changed `CRUDService` to use XML subtree filtering to create filters for the `read` operation ([#489](https://github.com/CiscoDevNet/ydk-gen/pull/489))
* Added support for non-standard RPCs as well in path API ([#498](https://github.com/CiscoDevNet/ydk-gen/pull/498))
* Add ability to generate Linux and Mac OSX packages in cmake using [cpack](https://cmake.org/Wiki/CMake:Packaging_With_CPack) ([#466](https://github.com/CiscoDevNet/ydk-gen/pull/466))

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
* Released ydk-cpp for OSX platform (on [Homebrew](https://github.com/CiscoDevNet/homebrew-ydk)) and on Ubuntu platform (on [Lauchpad](https://launchpad.net/~ydk)) ([#362](https://github.com/CiscoDevNet/ydk-gen/pull/362), [#322](https://github.com/CiscoDevNet/ydk-gen/pull/322))
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
