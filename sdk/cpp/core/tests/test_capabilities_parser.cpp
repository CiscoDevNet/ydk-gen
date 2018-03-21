//
// @file types.hpp
// @brief Header for ydk entity
//
// YANG Development Kit
// Copyright 2016 Cisco Systems. All rights reserved
//
////////////////////////////////////////////////////////////////
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//////////////////////////////////////////////////////////////////

#include <iostream>
#include "../src/ietf_parser.hpp"
#include "../src/path_api.hpp"
#include "../src/opendaylight_parser.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace std;


TEST_CASE("test_ietf")
{
    IetfCapabilitiesXmlParser parser{};
    vector<string> caps = parser.parse("<capabilities>\n"
            "<capability>urn:ietf:params:netconf:base:1.0</capability>\n"
            "<capability>urn:ietf:params:netconf:base:1.1</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:writable-running:1.0</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:candidate:1.0</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:confirmed-commit:1.0</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:confirmed-commit:1.1</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:xpath:1.0</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:validate:1.0</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:validate:1.1</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:rollback-on-error:1.0</capability>\n"
            "<capability>http://tail-f.com/ns/netconf/actions/1.0</capability>\n"
            "<capability>http://tail-f.com/ns/netconf/extensions</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:with-defaults:1.0?basic-mode=explicit&amp;also-supported=report-all-tagged</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults?revision=2011-06-01&amp;module=ietf-netconf-with-defaults</capability>\n"
            "<capability>urn:ietf:params:netconf:capability:yang-library:1.0?revision=2016-06-21&amp;module-set-id=5313fbbc1364e1c9b8d16f75e364e7c3</capability>\n"
            "<capability>http://cisco.com/ns/yang/main?module=main&amp;revision=2015-11-17</capability>\n"
            "<capability>http://cisco.com/ns/yang/main-aug1?module=main-aug1&amp;revision=2015-11-17</capability>\n"
            "<capability>http://cisco.com/ns/yang/oc-pattern?module=oc-pattern&amp;revision=2015-11-17</capability>\n"
            "<capability>http://cisco.com/ns/yang/ydk-filter?module=ydktest-filterread&amp;revision=2015-11-17</capability>\n"
            "<capability>http://cisco.com/ns/yang/ydktest-sanity?module=ydktest-sanity&amp;revision=2015-11-17</capability>\n"
            "<capability>http://cisco.com/ns/yang/ydktest-sanity-augm?module=ydktest-sanity-augm&amp;revision=2015-11-17</capability>\n"
            "<capability>http://cisco.com/ns/yang/ydktest-sanity-types?module=ydktest-sanity-types&amp;revision=2016-04-11</capability>\n"
            "<capability>http://cisco.com/ns/yang/ydktest-types?module=ydktest-types&amp;revision=2016-05-23</capability>\n"
            "<capability>http://openconfig.net/yang/bgp?module=openconfig-bgp&amp;revision=2016-06-21</capability>\n"
            "<capability>http://openconfig.net/yang/bgp-policy?module=openconfig-bgp-policy&amp;revision=2016-06-21</capability>\n"
            "<capability>http://openconfig.net/yang/bgp-types?module=openconfig-bgp-types&amp;revision=2016-06-21</capability>\n"
            "<capability>http://openconfig.net/yang/interfaces?module=openconfig-interfaces&amp;revision=2016-05-26</capability>\n"
            "<capability>http://openconfig.net/yang/openconfig-ext?module=openconfig-extensions&amp;revision=2015-10-09</capability>\n"
            "<capability>http://openconfig.net/yang/openconfig-types?module=openconfig-types&amp;revision=2016-05-31</capability>\n"
            "<capability>http://openconfig.net/yang/policy-types?module=openconfig-policy-types&amp;revision=2016-05-12</capability>\n"
            "<capability>http://openconfig.net/yang/routing-policy?module=openconfig-routing-policy&amp;revision=2016-05-12</capability>\n"
            "<capability>http://tail-f.com/ns/aaa/1.1?module=tailf-aaa&amp;revision=2015-06-16</capability>\n"
            "<capability>http://tail-f.com/ns/kicker?module=tailf-kicker&amp;revision=2016-11-24</capability>\n"
            "<capability>http://tail-f.com/ns/webui?module=tailf-webui&amp;revision=2013-03-07</capability>\n"
            "<capability>http://tail-f.com/yang/acm?module=tailf-acm&amp;revision=2013-03-07</capability>\n"
            "<capability>http://tail-f.com/yang/common-monitoring?module=tailf-common-monitoring&amp;revision=2013-06-14</capability>\n"
            "<capability>http://tail-f.com/yang/confd-monitoring?module=tailf-confd-monitoring&amp;revision=2013-06-14</capability>\n"
            "<capability>http://tail-f.com/yang/netconf-monitoring?module=tailf-netconf-monitoring&amp;revision=2016-11-24</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:iana-crypt-hash?module=iana-crypt-hash&amp;revision=2014-08-06&amp;features=crypt-hash-sha-512,crypt-hash-sha-256,crypt-hash-md5</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:iana-if-type?module=iana-if-type&amp;revision=2014-05-08</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-inet-types?module=ietf-inet-types&amp;revision=2013-07-15</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-interfaces?module=ietf-interfaces&amp;revision=2014-05-08&amp;features=pre-provisioning,if-mib,arbitrary-names</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-acm?module=ietf-netconf-acm&amp;revision=2012-02-22</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring?module=ietf-netconf-monitoring&amp;revision=2010-10-04</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-notifications?module=ietf-netconf-notifications&amp;revision=2012-02-06</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-restconf-monitoring?module=ietf-restconf-monitoring&amp;revision=2016-08-15</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-yang-library?module=ietf-yang-library&amp;revision=2016-06-21</capability>\n"
            "<capability>urn:ietf:params:xml:ns:yang:ietf-yang-types?module=ietf-yang-types&amp;revision=2013-07-15</capability>\n"
            "</capabilities>");

    REQUIRE(caps.size() > 0);

    for(const auto & cap : caps)
    {
        INFO(cap);
    }
}

TEST_CASE("test_error")
{
    IetfCapabilitiesXmlParser parser{};
    vector<string> caps = parser.parse("");
    REQUIRE(caps.size() == 0);

    caps = parser.parse_yang_1_1("");
    REQUIRE(caps.size() == 0);
}

TEST_CASE("test_error_1")
{
    IetfCapabilitiesXmlParser parser{};
    vector<string> caps = parser.parse("<test></test>");
    REQUIRE(caps.size() == 0);

    caps = parser.parse_yang_1_1("<test></test>");
    REQUIRE(caps.size() == 0);
}

TEST_CASE("test_error_2")
{
    IetfCapabilitiesXmlParser parser{};
    vector<string> caps = parser.parse("<capabilities></capabilities>");
    REQUIRE(caps.size() == 0);

    caps = parser.parse_yang_1_1("<capabilities></capabilities>");
    REQUIRE(caps.size() == 0);
}

TEST_CASE("test_error_3")
{
    IetfCapabilitiesXmlParser parser{};
    vector<string> caps = parser.parse("<capabilities><test></test></capabilities>");
    REQUIRE(caps.size() == 0);

    caps = parser.parse_yang_1_1("<capabilities><test></test></capabilities>");
    REQUIRE(caps.size() == 0);
}

TEST_CASE("test_odl")
{
    OpenDaylightCapabilitiesJsonParser parser{};

    string capabilities_json = "{\"network-topology\":{\"topology\":[{\"topology-id\":\"topology-netconf\",\"node\":[{\"node-id\":\"xr\",\"netconf-node-topology:host\":\"localhost\",\"netconf-node-topology:connection-status\":\"connected\",\"netconf-node-topology:port\":1220,\"netconf-node-topology:available-capabilities\":{\"available-capability\":[\"(http://cisco.com/ns/yang/Cisco-IOS-XR-config-mibs-cfg?revision=2015-01-07)Cisco-IOS-XR-config-mibs-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-vpn-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-vpn-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-clns-isis-datatypes?revision=2015-11-09)Cisco-IOS-XR-clns-isis-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-policy-repository-oper?revision=2015-11-09)Cisco-IOS-XR-policy-repository-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-controller-otu-oper?revision=2015-11-09)Cisco-IOS-XR-controller-otu-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-fib-common-oper?revision=2015-11-09)Cisco-IOS-XR-fib-common-oper-sub4\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-installmgr-admin-oper?revision=2015-11-09)Cisco-IOS-XR-installmgr-admin-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-crypto-sam-oper?revision=2015-01-07)Cisco-IOS-XR-crypto-sam-oper\",\"(http://openconfig.net/yang/bgp?revision=2015-10-09)openconfig-bgp\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-ifmib-oper?revision=2015-01-07)Cisco-IOS-XR-snmp-ifmib-oper-sub1\",\"(urn:ietf:params:xml:ns:yang:ietf-syslog-types?revision=2015-11-09)ietf-syslog-types\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-io-oper?revision=2015-10-20)Cisco-IOS-XR-ipv4-io-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-traffmon-netflow-cfg?revision=2015-11-09)Cisco-IOS-XR-traffmon-netflow-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-vrrp-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-vrrp-oper\",\"(http://cisco.com/ns/yang/cisco-xr-openconfig-if-ethernet-deviations?revision=2016-05-16)cisco-xr-openconfig-if-ethernet-deviations\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-controller-otu-cfg?revision=2015-11-09)Cisco-IOS-XR-controller-otu-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-procmem-oper?revision=2015-11-09)Cisco-IOS-XR-procmem-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-fib-common-oper?revision=2015-11-09)Cisco-IOS-XR-fib-common-oper-sub3\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-installmgr-admin-oper?revision=2015-11-09)Cisco-IOS-XR-installmgr-admin-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-shellutil-filesystem-oper?revision=2015-11-09)Cisco-IOS-XR-shellutil-filesystem-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-sbfd-cfg?revision=2015-11-09)Cisco-IOS-XR-ip-sbfd-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-management-cfg?revision=2015-09-25)Cisco-IOS-XR-tty-management-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2vpn-oper?revision=2015-11-09)Cisco-IOS-XR-l2vpn-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-server-oper?revision=2015-07-30)Cisco-IOS-XR-tty-server-oper-sub4\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-crypto-ssh-oper?revision=2015-06-02)Cisco-IOS-XR-crypto-ssh-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lib-keychain-macsec-cfg?revision=2015-11-09)Cisco-IOS-XR-lib-keychain-macsec-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-shellutil-filesystem-oper?revision=2015-11-09)Cisco-IOS-XR-shellutil-filesystem-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-bfd-oper?revision=2015-11-09)Cisco-IOS-XR-ip-bfd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-protocol-radius-oper?revision=2015-11-09)Cisco-IOS-XR-aaa-protocol-radius-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-lib-cfg?revision=2015-11-09)Cisco-IOS-XR-aaa-lib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-procmem-oper?revision=2015-11-09)Cisco-IOS-XR-procmem-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-management-cmd-oper?revision=2015-11-09)Cisco-IOS-XR-tty-management-cmd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-fib-common-oper?revision=2015-11-09)Cisco-IOS-XR-fib-common-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-man-xml-ttyagent-oper?revision=2015-07-30)Cisco-IOS-XR-man-xml-ttyagent-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-installmgr-admin-oper?revision=2015-11-09)Cisco-IOS-XR-installmgr-admin-oper-sub3\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rib-ipv6-oper?revision=2015-11-09)Cisco-IOS-XR-ip-rib-ipv6-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pbr-cfg?revision=2016-03-08)Cisco-IOS-XR-pbr-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-tacacs-cfg?revision=2015-11-09)Cisco-IOS-XR-aaa-tacacs-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-ospf-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-ospf-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-installmgr-admin-oper?revision=2015-11-09)Cisco-IOS-XR-installmgr-admin-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-server-oper?revision=2015-07-30)Cisco-IOS-XR-tty-server-oper-sub5\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lib-mpp-oper?revision=2015-01-07)Cisco-IOS-XR-lib-mpp-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-manageability-object-tracking-datatypes?revision=2015-11-09)Cisco-IOS-XR-manageability-object-tracking-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-cdp-cfg?revision=2015-07-30)Cisco-IOS-XR-cdp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-alarmgr-server-oper?revision=2015-11-09)Cisco-IOS-XR-alarmgr-server-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-protocol-radius-oper?revision=2015-11-09)Cisco-IOS-XR-aaa-protocol-radius-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-controller-optics-cfg?revision=2015-11-09)Cisco-IOS-XR-controller-optics-cfg\",\"(http://openconfig.net/yang/openconfig-types?revision=2015-10-09)openconfig-types\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-fib-common-oper?revision=2015-11-09)Cisco-IOS-XR-fib-common-oper-sub1\",\"(http://cisco.com/ns/yang/cisco-xr-openconfig-mpls-deviations?revision=2016-05-16)cisco-xr-openconfig-mpls-deviations\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-wdsysmon-fd-oper?revision=2015-11-09)Cisco-IOS-XR-wdsysmon-fd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-segment-routing-ms-cfg?revision=2015-11-09)Cisco-IOS-XR-segment-routing-ms-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-smiap-cfg?revision=2016-07-04)Cisco-IOS-XR-ipv4-smiap-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-config-cfgmgr-exec-oper?revision=2015-11-09)Cisco-IOS-XR-config-cfgmgr-exec-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-sbfd-oper?revision=2015-11-09)Cisco-IOS-XR-ip-sbfd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-server-oper?revision=2015-07-30)Cisco-IOS-XR-tty-server-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-manageability-perfmgmt-oper?revision=2015-11-09)Cisco-IOS-XR-manageability-perfmgmt-oper-sub1\",\"(http://openconfig.net/yang/bgp-policy?revision=2015-10-09)openconfig-bgp-policy\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rsi-oper?revision=2015-01-07)Cisco-IOS-XR-infra-rsi-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rsvp-cfg?revision=2015-11-09)Cisco-IOS-XR-ip-rsvp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-iep-cfg?revision=2015-11-09)Cisco-IOS-XR-ip-iep-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-controller-optics-oper?revision=2015-11-09)Cisco-IOS-XR-controller-optics-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ifmgr-oper?revision=2015-07-30)Cisco-IOS-XR-ifmgr-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-ospfv3-oper?revision=2015-11-09)Cisco-IOS-XR-ipv6-ospfv3-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-ldp-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-ldp-oper-sub2\",\"(http://cisco.com/ns/yang/cisco-xr-openconfig-if-ip-deviations?revision=2016-05-16)cisco-xr-openconfig-if-ip-deviations\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-objmgr-oper?revision=2015-11-09)Cisco-IOS-XR-infra-objmgr-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pmengine-oper?revision=2015-11-09)Cisco-IOS-XR-pmengine-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rcmd-oper?revision=2015-11-09)Cisco-IOS-XR-infra-rcmd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-ospfv3-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv6-ospfv3-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-server-oper?revision=2015-07-30)Cisco-IOS-XR-tty-server-oper-sub3\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-ceredundancymib-cfg?revision=2015-01-07)Cisco-IOS-XR-infra-ceredundancymib-cfg\",\"urn:ietf:params:netconf:capability:validate:1.1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pmengine-oper?revision=2015-11-09)Cisco-IOS-XR-pmengine-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-ldp-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-ldp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-manageability-perfmgmt-oper?revision=2015-11-09)Cisco-IOS-XR-manageability-perfmgmt-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-management-cmd-oper?revision=2015-11-09)Cisco-IOS-XR-tty-management-cmd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-syslog-cfg?revision=2016-06-22)Cisco-IOS-XR-infra-syslog-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-asic-errors-oper?revision=2015-11-09)Cisco-IOS-XR-asic-errors-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-cmproxy-oper?revision=2015-11-09)Cisco-IOS-XR-cmproxy-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-ldp-oper-datatypes?revision=2015-11-09)Cisco-IOS-XR-mpls-ldp-oper-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-spirit-install-instmgr-oper?revision=2015-11-09)Cisco-IOS-XR-spirit-install-instmgr-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-infra-clock-linux-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-infra-clock-linux-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-ospf-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-ospf-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-alarm-logger-cfg?revision=2015-07-30)Cisco-IOS-XR-infra-alarm-logger-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-hsrp-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-hsrp-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-crypto-ssh-cfg?revision=2015-07-30)Cisco-IOS-XR-crypto-ssh-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-statsd-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-statsd-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-io-cfg?revision=2016-05-10)Cisco-IOS-XR-ipv6-io-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-lsd-cfg?revision=2015-11-09)Cisco-IOS-XR-mpls-lsd-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-man-xml-ttyagent-cfg?revision=2015-07-30)Cisco-IOS-XR-man-xml-ttyagent-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-spirit-install-instmgr-oper?revision=2015-11-09)Cisco-IOS-XR-spirit-install-instmgr-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-static-cfg?revision=2015-11-09)Cisco-IOS-XR-mpls-static-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-server-oper?revision=2015-07-30)Cisco-IOS-XR-tty-server-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-oper?revision=2016-06-01)Cisco-IOS-XR-snmp-agent-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-ifmib-cfg?revision=2015-05-14)Cisco-IOS-XR-snmp-ifmib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pfi-im-cmd-oper?revision=2015-11-09)Cisco-IOS-XR-pfi-im-cmd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-dwdm-ui-oper?revision=2015-11-09)Cisco-IOS-XR-dwdm-ui-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-spirit-install-instmgr-oper?revision=2015-11-09)Cisco-IOS-XR-spirit-install-instmgr-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-mib-rfmib-cfg?revision=2016-05-13)Cisco-IOS-XR-snmp-mib-rfmib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-ospf-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-ospf-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pmengine-cfg?revision=2015-11-09)Cisco-IOS-XR-pmengine-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-ldp-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-ldp-oper-sub3\",\"(http://openconfig.net/yang/bgp-multiprotocol?revision=2015-10-09)openconfig-bgp-multiprotocol\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-static-cfg?revision=2015-09-10)Cisco-IOS-XR-ip-static-cfg\",\"(http://cisco.com/ns/yang/cisco-xr-openconfig-if-aggregate-deviations?revision=2016-05-16)cisco-xr-openconfig-if-aggregate-deviations\",\"(http://openconfig.net/yang/mpls-types?revision=2015-11-05)openconfig-mpls-types\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-rgmgr-oper?revision=2015-01-07)Cisco-IOS-XR-rgmgr-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lib-keychain-cfg?revision=2015-07-30)Cisco-IOS-XR-lib-keychain-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-io-oper?revision=2015-10-20)Cisco-IOS-XR-ipv4-io-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-bgp-oper?revision=2015-08-27)Cisco-IOS-XR-ipv4-bgp-oper\",\"urn:ietf:params:netconf:capability:rollback-on-error:1.0\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-systemmib-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-systemmib-cfg\",\"(http://openconfig.net/yang/sr?revision=2015-11-05)openconfig-mpls-sr\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-controller-otu-oper?revision=2015-11-09)Cisco-IOS-XR-controller-otu-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-policymgr-oper?revision=2015-11-09)Cisco-IOS-XR-infra-policymgr-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-syslog-act?revision=2016-04-17)Cisco-IOS-XR-syslog-act\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-lsd-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-lsd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-io-oper?revision=2015-10-20)Cisco-IOS-XR-ipv4-io-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-static-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-static-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-crypto-sam-oper?revision=2015-01-07)Cisco-IOS-XR-crypto-sam-oper-sub1\",\"urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring?deviation=cisco-xr-netconf-monitoring-deviations\",\"(http://cisco.com/ns/yang/cisco-xr-routing-policy-deviations?revision=2015-10-21)cisco-xr-routing-policy-deviations\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-sbfd-oper?revision=2015-11-09)Cisco-IOS-XR-ip-sbfd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2-eth-infra-cfg?revision=2015-11-09)Cisco-IOS-XR-l2-eth-infra-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2vpn-cfg?revision=2015-11-09)Cisco-IOS-XR-l2vpn-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-drivers-media-eth-cfg?revision=2015-07-30)Cisco-IOS-XR-drivers-media-eth-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-iarm-datatypes?revision=2015-01-07)Cisco-IOS-XR-ip-iarm-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-iep-oper?revision=2015-11-09)Cisco-IOS-XR-ip-iep-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-tc-oper?revision=2015-11-09)Cisco-IOS-XR-infra-tc-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rib-ipv4-oper?revision=2015-11-09)Cisco-IOS-XR-ip-rib-ipv4-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-policymgr-oper?revision=2015-11-09)Cisco-IOS-XR-infra-policymgr-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rcmd-oper?revision=2015-11-09)Cisco-IOS-XR-infra-rcmd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-dhcpd-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-dhcpd-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-entitymib-cfg?revision=2015-01-07)Cisco-IOS-XR-snmp-entitymib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-dwdm-ui-oper?revision=2015-11-09)Cisco-IOS-XR-dwdm-ui-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-wdsysmon-fd-oper?revision=2015-11-09)Cisco-IOS-XR-wdsysmon-fd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-alarm-logger-oper?revision=2015-01-07)Cisco-IOS-XR-infra-alarm-logger-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-ntp-admin-oper?revision=2015-11-09)Cisco-IOS-XR-ip-ntp-admin-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-wanphy-ui-cfg?revision=2015-11-09)Cisco-IOS-XR-wanphy-ui-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-cfg?revision=2015-10-27)Cisco-IOS-XR-snmp-agent-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-bfd-oper?revision=2015-11-09)Cisco-IOS-XR-ip-bfd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rsi-oper?revision=2015-01-07)Cisco-IOS-XR-infra-rsi-oper-sub2\",\"(http://openconfig.net/yang/vlan?revision=2015-10-09)openconfig-vlan\",\"(http://openconfig.net/yang/interfaces/ethernet?revision=2015-11-20)openconfig-if-ethernet\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ifmgr-oper?revision=2015-07-30)Cisco-IOS-XR-ifmgr-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-ciscosensormib-cfg?revision=2015-11-09)Cisco-IOS-XR-snmp-ciscosensormib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-shellutil-oper?revision=2015-01-07)Cisco-IOS-XR-shellutil-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-static-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-static-oper\",\"(http://openconfig.net/yang/ldp?revision=2015-11-05)openconfig-mpls-ldp\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-hsrp-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-hsrp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-locald-oper?revision=2015-11-09)Cisco-IOS-XR-aaa-locald-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-acl-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv6-acl-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-common-acl-datatypes?revision=2015-11-09)Cisco-IOS-XR-common-acl-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-manageability-object-tracking-oper?revision=2015-11-09)Cisco-IOS-XR-manageability-object-tracking-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-icmp-cfg?revision=2015-11-09)Cisco-IOS-XR-ip-icmp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-vrrp-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-vrrp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rib-ipv6-oper?revision=2015-11-09)Cisco-IOS-XR-ip-rib-ipv6-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-drivers-media-eth-oper?revision=2015-10-14)Cisco-IOS-XR-drivers-media-eth-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-qos-mibs-cfg?revision=2015-11-09)Cisco-IOS-XR-qos-mibs-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pfi-im-cmd-oper?revision=2015-11-09)Cisco-IOS-XR-pfi-im-cmd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-bundlemgr-oper?revision=2015-11-09)Cisco-IOS-XR-bundlemgr-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-sensormib-oper?revision=2015-11-09)Cisco-IOS-XR-snmp-sensormib-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rsvp-oper?revision=2015-11-09)Cisco-IOS-XR-ip-rsvp-oper-sub2\",\"(http://cisco.com/ns/yang/cisco-xr-bgp-deviations?revision=2016-03-21)cisco-xr-openconfig-bgp-deviations\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-bundlemgr-cfg?revision=2016-05-12)Cisco-IOS-XR-bundlemgr-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-fib-common-oper?revision=2015-11-09)Cisco-IOS-XR-fib-common-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-ma-oper?revision=2015-10-20)Cisco-IOS-XR-ipv4-ma-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-pfilter-oper?revision=2015-11-09)Cisco-IOS-XR-ip-pfilter-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-ldp-cfg-datatypes?revision=2015-11-09)Cisco-IOS-XR-mpls-ldp-cfg-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-acl-oper?revision=2015-11-09)Cisco-IOS-XR-ipv6-acl-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lib-mpp-cfg?revision=2015-07-30)Cisco-IOS-XR-lib-mpp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-oam-cfg?revision=2015-11-09)Cisco-IOS-XR-mpls-oam-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-rgmgr-oper?revision=2015-01-07)Cisco-IOS-XR-rgmgr-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pfi-im-cmd-oper?revision=2015-11-09)Cisco-IOS-XR-pfi-im-cmd-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-bundlemgr-oper?revision=2015-11-09)Cisco-IOS-XR-bundlemgr-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rsvp-oper?revision=2015-11-09)Cisco-IOS-XR-ip-rsvp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-es-acl-cfg?revision=2015-11-09)Cisco-IOS-XR-es-acl-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-policy-repository-cfg?revision=2015-08-27)Cisco-IOS-XR-policy-repository-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-domain-oper?revision=2015-09-29)Cisco-IOS-XR-ip-domain-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-bgp-oper?revision=2015-08-27)Cisco-IOS-XR-ipv4-bgp-oper-sub1\",\"(http://openconfig.net/yang/rsvp?revision=2015-11-05)openconfig-mpls-rsvp\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-io-oper?revision=2015-11-09)Cisco-IOS-XR-ipv6-io-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-alarm-logger-oper?revision=2015-01-07)Cisco-IOS-XR-infra-alarm-logger-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-ntp-oper?revision=2015-11-09)Cisco-IOS-XR-ip-ntp-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-vpn-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-vpn-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-dwdm-ui-cfg?revision=2015-12-16)Cisco-IOS-XR-dwdm-ui-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rmf-oper?revision=2015-11-09)Cisco-IOS-XR-infra-rmf-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-bundlemgr-oper?revision=2015-11-09)Cisco-IOS-XR-bundlemgr-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-management-oper?revision=2015-01-07)Cisco-IOS-XR-tty-management-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-ma-oper?revision=2015-10-20)Cisco-IOS-XR-ipv4-ma-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-man-ems-cfg?revision=2015-11-09)Cisco-IOS-XR-man-ems-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-tc-oper?revision=2015-11-09)Cisco-IOS-XR-infra-tc-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-ntp-cfg?revision=2015-11-09)Cisco-IOS-XR-ip-ntp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-acl-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-acl-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-vty-cfg?revision=2015-01-07)Cisco-IOS-XR-tty-vty-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-invmgr-oper?revision=2015-11-09)Cisco-IOS-XR-invmgr-oper-sub3\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-locald-oper?revision=2015-11-09)Cisco-IOS-XR-aaa-locald-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-cfg?revision=2015-11-09)Cisco-IOS-XR-mpls-te-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub3\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-linux-os-reboot-history-oper?revision=2015-11-09)Cisco-IOS-XR-linux-os-reboot-history-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-flashmib-cfg?revision=2015-12-15)Cisco-IOS-XR-flashmib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-ma-oper?revision=2015-10-20)Cisco-IOS-XR-ipv6-ma-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-telnet-mgmt-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-telnet-mgmt-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-ospfv3-oper?revision=2015-11-09)Cisco-IOS-XR-ipv6-ospfv3-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-invmgr-oper?revision=2015-11-09)Cisco-IOS-XR-invmgr-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lib-keychain-oper?revision=2015-01-07)Cisco-IOS-XR-lib-keychain-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rib-cfg?revision=2015-11-09)Cisco-IOS-XR-ip-rib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-sensormib-oper?revision=2015-11-09)Cisco-IOS-XR-snmp-sensormib-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-invmgr-oper?revision=2015-11-09)Cisco-IOS-XR-invmgr-oper-sub4\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2-eth-infra-oper?revision=2015-11-09)Cisco-IOS-XR-l2-eth-infra-oper-sub3\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-tcp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-tcp-oper-sub5\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub4\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-ntp-admin-oper?revision=2015-11-09)Cisco-IOS-XR-ip-ntp-admin-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-acl-datatypes?revision=2015-11-09)Cisco-IOS-XR-ipv4-acl-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-policymgr-cfg?revision=2015-05-18)Cisco-IOS-XR-infra-policymgr-cfg\",\"(http://openconfig.net/yang/interfaces/ip?revision=2015-11-20)openconfig-if-ip\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-qos-ma-cfg?revision=2016-03-03)Cisco-IOS-XR-qos-ma-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lpts-lib-cfg?revision=2015-11-09)Cisco-IOS-XR-lpts-lib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-man-xml-ttyagent-oper?revision=2015-07-30)Cisco-IOS-XR-man-xml-ttyagent-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-segment-routing-ms-oper?revision=2015-11-09)Cisco-IOS-XR-segment-routing-ms-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pbr-oper?revision=2015-11-09)Cisco-IOS-XR-pbr-oper\",\"(urn:ietf:params:xml:ns:yang:iana-if-type?revision=2014-05-08)iana-if-type\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-sensormib-oper?revision=2015-11-09)Cisco-IOS-XR-snmp-sensormib-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-invmgr-oper?revision=2015-11-09)Cisco-IOS-XR-invmgr-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2-eth-infra-oper?revision=2015-11-09)Cisco-IOS-XR-l2-eth-infra-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-ifmib-oper?revision=2015-01-07)Cisco-IOS-XR-snmp-ifmib-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-tcp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-tcp-oper-sub4\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rsi-oper?revision=2015-01-07)Cisco-IOS-XR-infra-rsi-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub5\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ethernet-lldp-oper?revision=2015-11-09)Cisco-IOS-XR-ethernet-lldp-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-manageability-object-tracking-cfg?revision=2015-11-09)Cisco-IOS-XR-manageability-object-tracking-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-frucontrolmib-cfg?revision=2015-01-07)Cisco-IOS-XR-snmp-frucontrolmib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-pfilter-oper?revision=2015-11-09)Cisco-IOS-XR-ip-pfilter-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-clns-isis-cfg?revision=2015-11-09)Cisco-IOS-XR-clns-isis-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lib-keychain-oper?revision=2015-01-07)Cisco-IOS-XR-lib-keychain-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-tacacs-oper?revision=2015-11-09)Cisco-IOS-XR-aaa-tacacs-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-alarmgr-server-oper?revision=2015-11-09)Cisco-IOS-XR-alarmgr-server-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-vrrp-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-vrrp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rsi-cfg?revision=2015-07-30)Cisco-IOS-XR-infra-rsi-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-nd-oper?revision=2015-11-09)Cisco-IOS-XR-ipv6-nd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-ma-oper?revision=2015-10-20)Cisco-IOS-XR-ipv6-ma-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-sdr-invmgr-oper?revision=2015-11-09)Cisco-IOS-XR-sdr-invmgr-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-invmgr-oper?revision=2015-11-09)Cisco-IOS-XR-invmgr-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2-eth-infra-oper?revision=2015-11-09)Cisco-IOS-XR-l2-eth-infra-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-management-datatypes?revision=2015-01-07)Cisco-IOS-XR-tty-management-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-tcp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-tcp-oper-sub3\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-statsd-oper?revision=2015-11-09)Cisco-IOS-XR-infra-statsd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub6\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ethernet-lldp-oper?revision=2015-11-09)Cisco-IOS-XR-ethernet-lldp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-server-oper?revision=2015-07-30)Cisco-IOS-XR-tty-server-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-cfgmgr-rollback-act?revision=2016-04-17)Cisco-IOS-XR-cfgmgr-rollback-act\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-hsrp-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-hsrp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tunnel-gre-cfg?revision=2016-05-25)Cisco-IOS-XR-tunnel-gre-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-config-cfgmgr-exec-oper?revision=2015-11-09)Cisco-IOS-XR-config-cfgmgr-exec-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-alarm-logger-datatypes?revision=2015-01-07)Cisco-IOS-XR-infra-alarm-logger-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-correlator-oper?revision=2015-11-09)Cisco-IOS-XR-infra-correlator-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-man-ems-oper?revision=2015-11-09)Cisco-IOS-XR-man-ems-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-controller-optics-oper?revision=2015-11-09)Cisco-IOS-XR-controller-optics-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2-eth-infra-datatypes?revision=2015-11-09)Cisco-IOS-XR-l2-eth-infra-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-tacacs-oper?revision=2015-11-09)Cisco-IOS-XR-aaa-tacacs-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-nd-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv6-nd-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-ospf-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-ospf-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rib-ipv4-oper?revision=2015-11-09)Cisco-IOS-XR-ip-rib-ipv4-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ethernet-lldp-cfg?revision=2015-11-09)Cisco-IOS-XR-ethernet-lldp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub7\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-plat-chas-invmgr-oper?revision=2015-01-07)Cisco-IOS-XR-plat-chas-invmgr-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-ldp-cfg?revision=2015-11-09)Cisco-IOS-XR-mpls-ldp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-acl-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-acl-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-segment-routing-ms-oper?revision=2015-11-09)Cisco-IOS-XR-segment-routing-ms-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-server-cfg?revision=2015-07-30)Cisco-IOS-XR-tty-server-cfg\",\"(http://openconfig.net/yang/interface/aggregate?revision=2015-11-20)openconfig-if-aggregate\",\"(http://cisco.com/ns/yang/cisco-xr-types?revision=2015-06-29)Cisco-IOS-XR-types\",\"(http://openconfig.net/yang/policy-types?revision=2015-10-09)openconfig-policy-types\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-manageability-perfmgmt-datatypes?revision=2015-11-09)Cisco-IOS-XR-manageability-perfmgmt-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-manageability-object-tracking-oper?revision=2015-11-09)Cisco-IOS-XR-manageability-object-tracking-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-clns-isis-oper?revision=2015-11-09)Cisco-IOS-XR-clns-isis-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-tty-management-oper?revision=2015-01-07)Cisco-IOS-XR-tty-management-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-correlator-oper?revision=2015-11-09)Cisco-IOS-XR-infra-correlator-oper-sub1\",\"(http://cisco.com/ns/yang/cisco-xr-openconfig-vlan-deviations?revision=2016-07-08)cisco-xr-openconfig-vlan-deviations\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub8\",\"(http://openconfig.net/yang/telemetry?revision=2016-02-04)openconfig-telemetry\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-clns-isis-oper?revision=2015-11-09)Cisco-IOS-XR-clns-isis-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-config-mda-cfg?revision=2015-11-09)Cisco-IOS-XR-config-mda-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-clns-isis-oper?revision=2015-11-09)Cisco-IOS-XR-clns-isis-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-domain-oper?revision=2015-09-29)Cisco-IOS-XR-ip-domain-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2-eth-infra-oper?revision=2015-11-09)Cisco-IOS-XR-l2-eth-infra-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-tcp-cfg?revision=2016-02-26)Cisco-IOS-XR-ip-tcp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-sdr-invmgr-diag-oper?revision=2015-11-09)Cisco-IOS-XR-sdr-invmgr-diag-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-man-ems-oper?revision=2015-11-09)Cisco-IOS-XR-man-ems-oper\",\"(urn:ietf:params:xml:ns:yang:ietf-yang-types?revision=2013-07-15)ietf-yang-types\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-parser-cfg?revision=2015-06-02)Cisco-IOS-XR-parser-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub9\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-iep-oper?revision=2015-11-09)Cisco-IOS-XR-ip-iep-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-cmproxy-oper?revision=2015-11-09)Cisco-IOS-XR-cmproxy-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-udp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-udp-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-linux-os-reboot-history-oper?revision=2015-11-09)Cisco-IOS-XR-linux-os-reboot-history-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-policy-repository-oper?revision=2015-11-09)Cisco-IOS-XR-policy-repository-oper-sub1\",\"(http://openconfig.net/yang/mpls?revision=2015-11-05)openconfig-mpls\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-io-oper?revision=2015-11-09)Cisco-IOS-XR-ipv6-io-oper-sub1\",\"(http://openconfig.net/yang/interfaces?revision=2015-11-20)openconfig-interfaces\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-statsd-oper?revision=2015-11-09)Cisco-IOS-XR-infra-statsd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-smiap-cfg?revision=2016-07-04)Cisco-IOS-XR-ipv6-smiap-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-syslog-oper?revision=2016-06-24)Cisco-IOS-XR-infra-syslog-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-rgmgr-cfg?revision=2015-07-30)Cisco-IOS-XR-rgmgr-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-plat-chas-invmgr-oper?revision=2015-01-07)Cisco-IOS-XR-plat-chas-invmgr-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-entstatemib-cfg?revision=2015-07-27)Cisco-IOS-XR-snmp-entstatemib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-shellutil-cfg?revision=2015-10-12)Cisco-IOS-XR-shellutil-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-udp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-udp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-shellutil-oper?revision=2015-01-07)Cisco-IOS-XR-shellutil-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-show-fpd-loc-ng-oper?revision=2015-11-09)Cisco-IOS-XR-show-fpd-loc-ng-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mdrv-lib-cfg?revision=2015-11-09)Cisco-IOS-XR-mdrv-lib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-qos-ma-oper?revision=2016-03-03)Cisco-IOS-XR-qos-ma-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-datatypes?revision=2015-11-09)Cisco-IOS-XR-mpls-te-datatypes\",\"(urn:ietf:params:xml:ns:yang:ietf-interfaces?revision=2014-05-08)ietf-interfaces\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-ma-cfg?revision=2015-07-30)Cisco-IOS-XR-ipv6-ma-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-crypto-ssh-oper?revision=2015-06-02)Cisco-IOS-XR-crypto-ssh-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-acl-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-ace-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-tc-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-tc-cfg\",\"(http://openconfig.net/yang/mpls?revision=2015-11-05)openconfig-mpls-static\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-udp-cfg?revision=2016-02-26)Cisco-IOS-XR-ip-udp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-oper?revision=2016-06-01)Cisco-IOS-XR-snmp-agent-oper-sub3\",\"(http://openconfig.net/yang/mpls?revision=2015-11-05)openconfig-mpls-igp\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-otnifmib-cfg?revision=2015-11-09)Cisco-IOS-XR-otnifmib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-udp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-udp-oper-sub4\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-ntp-oper?revision=2015-11-09)Cisco-IOS-XR-ip-ntp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lpts-pre-ifib-cfg?revision=2015-11-09)Cisco-IOS-XR-lpts-pre-ifib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-acl-oper?revision=2015-11-09)Cisco-IOS-XR-ipv6-acl-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-asic-errors-oper?revision=2015-11-09)Cisco-IOS-XR-asic-errors-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-acl-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv6-ace-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-ltrace-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-ltrace-cfg\",\"(http://cisco.com/ns/yang/cisco-xr-ietf-interfaces-deviations?revision=2016-05-16)cisco-xr-ietf-interfaces-deviations\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-ldp-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-ldp-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-protocol-radius-oper?revision=2015-11-09)Cisco-IOS-XR-aaa-protocol-radius-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-wanphy-ui-oper?revision=2015-11-09)Cisco-IOS-XR-wanphy-ui-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-bfd-cfg?revision=2015-11-09)Cisco-IOS-XR-ip-bfd-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-oper?revision=2016-06-01)Cisco-IOS-XR-snmp-agent-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lpts-pre-ifib-oper?revision=2016-02-22)Cisco-IOS-XR-lpts-pre-ifib-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-locald-cfg?revision=2015-11-09)Cisco-IOS-XR-aaa-locald-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rmf-oper?revision=2015-11-09)Cisco-IOS-XR-infra-rmf-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-udp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-udp-oper-sub3\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lpts-pre-ifib-oper?revision=2016-02-22)Cisco-IOS-XR-lpts-pre-ifib-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-objmgr-oper?revision=2015-11-09)Cisco-IOS-XR-infra-objmgr-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-filesystems-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-filesystems-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-asic-errors-oper?revision=2015-11-09)Cisco-IOS-XR-asic-errors-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-pfilter-cfg?revision=2015-11-09)Cisco-IOS-XR-ip-pfilter-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-bgp-datatypes?revision=2015-08-27)Cisco-IOS-XR-ipv4-bgp-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-crypto-sam-cfg?revision=2015-01-07)Cisco-IOS-XR-crypto-sam-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-oper?revision=2016-06-01)Cisco-IOS-XR-snmp-agent-oper-sub5\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ha-eem-cfg?revision=2015-07-30)Cisco-IOS-XR-ha-eem-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-cdp-oper?revision=2015-07-30)Cisco-IOS-XR-cdp-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-telnet-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-telnet-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-man-netconf-cfg?revision=2016-03-15)Cisco-IOS-XR-man-netconf-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-nto-misc-oper?revision=2015-11-09)Cisco-IOS-XR-nto-misc-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-plat-chas-invmgr-oper?revision=2015-01-07)Cisco-IOS-XR-plat-chas-invmgr-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-test-trap-act?revision=2016-04-17)Cisco-IOS-XR-snmp-test-trap-act\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-manageability-perfmgmt-cfg?revision=2015-11-09)Cisco-IOS-XR-manageability-perfmgmt-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-oper?revision=2016-06-01)Cisco-IOS-XR-snmp-agent-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2vpn-oper?revision=2015-11-09)Cisco-IOS-XR-l2vpn-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-oper?revision=2016-06-01)Cisco-IOS-XR-snmp-agent-oper-sub4\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-syslogmib-cfg?revision=2015-12-01)Cisco-IOS-XR-snmp-syslogmib-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-locald-admin-cfg?revision=2015-11-09)Cisco-IOS-XR-aaa-locald-admin-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-es-acl-oper?revision=2015-11-09)Cisco-IOS-XR-es-acl-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-telemetry-model-driven-oper?revision=2015-11-09)Cisco-IOS-XR-telemetry-model-driven-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-syslog-oper?revision=2016-06-24)Cisco-IOS-XR-infra-syslog-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-es-acl-cfg?revision=2015-11-09)Cisco-IOS-XR-es-ace-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-acl-datatypes?revision=2015-11-09)Cisco-IOS-XR-ipv6-acl-datatypes\",\"urn:ietf:params:netconf:base:1.1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-es-acl-datatypes?revision=2015-11-09)Cisco-IOS-XR-es-acl-datatypes\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-nto-misc-oper?revision=2015-11-09)Cisco-IOS-XR-nto-misc-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-io-cfg?revision=2015-07-30)Cisco-IOS-XR-ipv4-io-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2vpn-oper?revision=2015-11-09)Cisco-IOS-XR-l2vpn-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-oper?revision=2016-06-01)Cisco-IOS-XR-snmp-agent-oper-sub7\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-es-acl-oper?revision=2015-11-09)Cisco-IOS-XR-es-acl-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-aaa-protocol-radius-cfg?revision=2015-11-09)Cisco-IOS-XR-aaa-protocol-radius-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-tcp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-tcp-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-arp-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-arp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-lsd-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-lsd-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-pbr-oper?revision=2015-11-09)Cisco-IOS-XR-pbr-oper-sub1\",\"(http://cisco.com/ns/yang/cisco-xr-bgp-policy-deviations?revision=2016-03-21)cisco-xr-openconfig-bgp-policy-deviations\",\"(http://cisco.com/ns/yang/cisco-xr-openconfig-interfaces-deviations?revision=2016-05-16)cisco-xr-openconfig-interfaces-deviations\",\"(urn:ietf:params:xml:ns:yang:ietf-inet-types?revision=2013-07-15)ietf-inet-types\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ifmgr-oper?revision=2015-07-30)Cisco-IOS-XR-ifmgr-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-watchd-cfg?revision=2015-11-09)Cisco-IOS-XR-watchd-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-infra-locale-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-infra-locale-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-ma-cfg?revision=2015-07-30)Cisco-IOS-XR-ipv4-ma-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2vpn-oper?revision=2015-11-09)Cisco-IOS-XR-l2vpn-oper-sub3\",\"(http://openconfig.net/yang/openconfig-ext?revision=2015-10-09)openconfig-extensions\",\"urn:ietf:params:netconf:capability:candidate:1.0\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-snmp-agent-oper?revision=2016-06-01)Cisco-IOS-XR-snmp-agent-oper-sub6\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-tcp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-tcp-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-tcp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-tcp-oper-sub1\",\"urn:ietf:params:netconf:capability:confirmed-commit:1.1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-sdr-invmgr-diag-oper?revision=2015-11-09)Cisco-IOS-XR-sdr-invmgr-diag-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-wanphy-ui-oper?revision=2015-11-09)Cisco-IOS-XR-wanphy-ui-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-show-fpd-loc-ng-oper?revision=2015-11-09)Cisco-IOS-XR-show-fpd-loc-ng-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-telemetry-model-driven-cfg?revision=2015-11-09)Cisco-IOS-XR-telemetry-model-driven-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-arp-cfg?revision=2015-11-09)Cisco-IOS-XR-ipv4-arp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-arp-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-arp-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-objmgr-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-objmgr-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv6-nd-oper?revision=2015-11-09)Cisco-IOS-XR-ipv6-nd-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-l2vpn-oper?revision=2015-11-09)Cisco-IOS-XR-l2vpn-oper-sub4\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ifmgr-cfg?revision=2015-07-30)Cisco-IOS-XR-ifmgr-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-udp-oper?revision=2016-02-26)Cisco-IOS-XR-ip-udp-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ethernet-lldp-oper?revision=2015-11-09)Cisco-IOS-XR-ethernet-lldp-oper\",\"(http://cisco.com/ns/yang/cisco-xr-openconfig-telemetry-deviations?revision=2016-07-08)cisco-xr-openconfig-telemetry-deviations\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-rsvp-oper?revision=2015-11-09)Cisco-IOS-XR-ip-rsvp-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-policymgr-oper?revision=2015-11-09)Cisco-IOS-XR-infra-policymgr-oper\",\"(http://openconfig.net/yang/mpls?revision=2015-11-05)openconfig-mpls-te\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-rcmd-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-rcmd-cfg\",\"(http://openconfig.net/yang/bgp-operational?revision=2015-10-09)openconfig-bgp-operational\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-drivers-media-eth-oper?revision=2015-10-14)Cisco-IOS-XR-drivers-media-eth-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-acl-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-acl-oper\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-mpls-te-oper?revision=2015-11-09)Cisco-IOS-XR-mpls-te-oper-sub2\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-bgp-cfg?revision=2015-08-27)Cisco-IOS-XR-ipv4-bgp-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ip-domain-cfg?revision=2015-05-13)Cisco-IOS-XR-ip-domain-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-lib-mpp-oper?revision=2015-01-07)Cisco-IOS-XR-lib-mpp-oper-sub1\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-infra-correlator-cfg?revision=2015-11-09)Cisco-IOS-XR-infra-correlator-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-cdp-oper?revision=2015-07-30)Cisco-IOS-XR-cdp-oper-sub1\",\"(http://openconfig.net/yang/routing-policy?revision=2015-10-09)openconfig-routing-policy\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-spirit-corehelper-cfg?revision=2015-11-09)Cisco-IOS-XR-spirit-corehelper-cfg\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-ipv4-arp-oper?revision=2015-11-09)Cisco-IOS-XR-ipv4-arp-oper\",\"(http://openconfig.net/yang/bgp-types?revision=2015-10-09)openconfig-bgp-types\",\"(http://cisco.com/ns/yang/Cisco-IOS-XR-telemetry-model-driven-oper?revision=2015-11-09)Cisco-IOS-XR-telemetry-model-driven-oper\"]},\"netconf-node-topology:unavailable-capabilities\":{\"unavailable-capability\":[{\"failure-reason\":\"unable-to-resolve\",\"capability\":\"(http://cisco.com/ns/yang/cisco-xr-netconf-monitoring-deviations?revision=2016-02-16)cisco-xr-netconf-monitoring-deviations\"}]}},{\"node-id\":\"controller-config\",\"netconf-node-topology:host\":\"127.0.0.1\",\"netconf-node-topology:connection-status\":\"connected\",\"netconf-node-topology:port\":1830,\"netconf-node-topology:available-capabilities\":{\"available-capability\":[\"(urn:TBD:params:xml:ns:yang:network:isis-topology?revision=2013-07-12)isis-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:netconf:client:dispatcher?revision=2014-04-08)odl-netconfig-client-cfg\",\"(urn:opendaylight:params:xml:ns:yang:controller:netty:timer?revision=2013-11-19)netty-timer\",\"(urn:ietf:params:xml:ns:yang:ietf-yang-types?revision=2010-09-24)ietf-yang-types\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:binding?revision=2013-10-28)opendaylight-md-sal-binding\",\"(urn:ietf:params:xml:ns:yang:ospf-topology?revision=2013-07-12)ospf-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:mdsal:notification?revision=2015-08-03)netconf-mdsal-notification\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:clustering:entity-owners?revision=2015-08-04)entity-owners\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:mdsal:yang:library?revision=2016-06-17)netconf-mdsal-yang-library\",\"(urn:TBD:params:xml:ns:yang:network:ted?revision=2013-10-21)ted\",\"(urn:TBD:params:xml:ns:yang:nt:l3-unicast-igp-topology?revision=2013-10-21)l3-unicast-igp-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:legacy-entity-ownership-service-provider?revision=2016-02-26)opendaylight-legacy-entity-ownership-service-provider\",\"(urn:TBD:params:xml:ns:yang:network-topology?revision=2013-10-21)network-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:north:mapper?revision=2015-01-14)netconf-northbound-mapper\",\"(urn:ietf:params:xml:ns:yang:iana-afn-safi?revision=2013-07-04)iana-afn-safi\",\"(urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring-extension?revision=2013-12-10)ietf-netconf-monitoring-extension\",\"(urn:opendaylight:params:xml:ns:yang:controller:sal:restconf:service?revision=2015-07-08)sal-restconf-service\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:actor-system-provider:service?revision=2015-10-05)actor-system-provider-service\",\"(urn:opendaylight:l2:types?revision=2013-08-27)opendaylight-l2-types\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:northbound:tcp?revision=2015-04-23)netconf-northbound-tcp\",\"(urn:ietf:params:xml:ns:yang:ietf-yang-library?revision=2016-04-09)ietf-yang-library\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:connector:netconf?revision=2015-08-03)odl-sal-netconf-connector-cfg\",\"(urn:opendaylight:netconf-node-topology?revision=2015-01-14)netconf-node-topology\",\"(urn:TBD:params:xml:ns:yang:network:isis-topology?revision=2013-10-21)isis-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:inmemory-datastore-provider?revision=2014-06-17)opendaylight-inmemory-datastore-provider\",\"(urn:opendaylight:params:xml:ns:yang:aaa:credential-store?revision=2015-02-26)credential-store\",\"(urn:opendaylight:params:xml:ns:yang:mdsal:core:general-entity?revision=2015-09-30)odl-general-entity\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:common?revision=2013-10-28)opendaylight-md-sal-common\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:netconf:auth?revision=2015-07-15)netconf-auth\",\"(urn:opendaylight:params:xml:ns:yang:md:sal:config:spi:cluster-singleton-service?revision=2016-07-18)cluster-singleton-service-spi\",\"(urn:ietf:params:xml:ns:yang:ietf-restconf?revision=2013-10-19)ietf-restconf\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:mdsal:mapper?revision=2015-01-14)netconf-mdsal-mapper\",\"(urn:opendaylight:netconf-node-inventory?revision=2014-01-08)netconf-node-inventory\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:mdsal:monitoring?revision=2015-02-18)netconf-mdsal-monitoring\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:rest:connector?revision=2014-07-24)opendaylight-rest-connector\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:core:spi:operational-dom-store?revision=2014-06-17)opendaylight-operational-dom-datastore\",\"(urn:ietf:params:xml:ns:yang:ietf-netconf-notifications?revision=2012-02-06)ietf-netconf-notifications\",\"(urn:ietf:params:xml:ns:yang:ietf-inet-types?revision=2010-09-24)ietf-inet-types\",\"(urn:ietf:params:xml:ns:yang:ietf-network?revision=2015-06-08)ietf-network\",\"(urn:opendaylight:netconf:console:provider:impl?revision=2016-03-23)netconf-console-provider-impl\",\"(config:aaa:authn:netconf:plugin?revision=2015-07-15)aaa-authn-netconf-plugin\",\"(urn:TBD:params:xml:ns:yang:nt:l3-unicast-igp-topology?revision=2013-07-12)l3-unicast-igp-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:netty:eventexecutor?revision=2013-11-12)netty-event-executor\",\"(urn:ietf:params:xml:ns:yang:ietf-access-control-list?revision=2016-02-18)ietf-access-control-list\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:northbound:ssh?revision=2015-01-14)netconf-northbound-ssh\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:netconf:northbound:impl?revision=2015-01-12)netconf-northbound-impl\",\"(instance:identifier:patch:module?revision=2015-11-21)instance-identifier-patch-module\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:cluster-admin-provider?revision=2015-10-13)cluster-admin-provider\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:northbound:notification?revision=2015-08-06)netconf-northbound-notification\",\"(urn:opendaylight:params:xml:ns:yang:controller:config?revision=2013-04-05)config\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:core:general-entity?revision=2015-08-20)general-entity\",\"urn:ietf:params:netconf:capability:exi:1.0\",\"(urn:ietf:params:xml:ns:yang:iana-if-type?revision=2014-05-08)iana-if-type\",\"(urn:TBD:params:xml:ns:yang:ospf-topology?revision=2013-10-21)ospf-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:core:spi:entity-ownership-service?revision=2015-08-10)opendaylight-entity-ownership-service\",\"(urn:opendaylight:params:xml:ns:yang:controller:threadpool?revision=2013-04-09)threadpool\",\"(urn:opendaylight:params:xml:ns:yang:md:sal:config:impl:cluster-singleton-service?revision=2016-07-18)cluster-singleton-service-impl\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:binding:v1:codec?revision=2016-07-14)opendaylight-md-sal-binding-v1-codec\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:dom:impl?revision=2013-10-28)opendaylight-sal-dom-broker-impl\",\"(urn:opendaylight:params:xml:ns:yang:controller:shutdown?revision=2013-12-18)shutdown\",\"(urn:opendaylight:params:xml:ns:yang:controller:threadpool:impl?revision=2013-04-05)threadpool-impl\",\"(urn:opendaylight:yang:extension:yang-ext?revision=2013-07-09)yang-ext\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:netconf?revision=2014-04-08)odl-netconf-cfg\",\"urn:ietf:params:netconf:capability:notification:1.0\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:remote-rpc-connector?revision=2014-07-07)remote-rpc-connector\",\"(urn:opendaylight:params:xml:ns:yang:controller:netty?revision=2013-11-19)netty\",\"(urn:sal:restconf:event:subscription?revision=2014-07-08)sal-remote-augment\",\"(urn:ietf:params:xml:ns:yang:ietf-yang-types?revision=2013-07-15)ietf-yang-types\",\"(urn:opendaylight:inventory?revision=2013-08-19)opendaylight-inventory\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:concurrent-data-broker?revision=2014-11-24)odl-concurrent-data-broker-cfg\",\"(urn:ietf:params:xml:ns:netconf:base:1.0?revision=2011-06-01)ietf-netconf\",\"(urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring?revision=2010-10-04)ietf-netconf-monitoring\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:northbound:notification:impl?revision=2015-08-07)netconf-northbound-notification-impl\",\"(urn:opendaylight:params:xml:ns:yang:controller:netty:threadgroup?revision=2013-11-07)threadgroup\",\"(urn:ietf:params:xml:ns:yang:ietf-interfaces?revision=2014-05-08)ietf-interfaces\",\"(urn:TBD:params:xml:ns:yang:network-topology?revision=2013-07-12)network-topology\",\"(urn:ietf:params:xml:ns:netconf:notification:1.0?revision=2008-07-14)notifications\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:core:spi:config-dom-store?revision=2014-06-17)opendaylight-config-dom-datastore\",\"(urn:ietf:params:xml:ns:yang:ietf-network-topology?revision=2015-06-08)ietf-network-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:cluster:admin?revision=2015-10-13)cluster-admin\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:topology:shared:schema:repository?revision=2015-07-27)shared-schema-repository\",\"urn:ietf:params:netconf:base:1.0\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:dom:pingpong?revision=2014-11-07)opendaylight-pingpong-broker\",\"(urn:opendaylight:params:xml:ns:yang:controller:netconf:topology?revision=2015-07-27)netconf-topology\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:distributed-datastore-provider?revision=2014-06-12)distributed-datastore-provider\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:binding:impl?revision=2013-10-28)opendaylight-sal-binding-broker-impl\",\"urn:ietf:params:netconf:base:1.1\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:netconf:northbound?revision=2015-01-14)netconf-northbound\",\"(urn:TBD:params:xml:ns:yang:network:ted?revision=2013-07-12)ted\",\"(urn:ietf:params:xml:ns:yang:ietf-packet-fields?revision=2016-02-18)ietf-packet-fields\",\"(urn:ietf:params:xml:ns:netmod:notification?revision=2008-07-14)nc-notifications\",\"(urn:ietf:params:xml:ns:yang:ietf-inet-types?revision=2013-07-15)ietf-inet-types\",\"(urn:opendaylight:params:xml:ns:yang:controller:threadpool:impl:fixed?revision=2013-12-01)threadpool-impl-fixed\",\"(urn:opendaylight:params:xml:ns:yang:controller:protocol:framework?revision=2014-03-13)protocol-framework\",\"urn:ietf:params:netconf:capability:candidate:1.0\",\"(urn:opendaylight:params:xml:ns:yang:controller:shutdown:impl?revision=2013-12-18)shutdown-impl\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:remote?revision=2014-01-14)sal-remote\",\"(urn:opendaylight:params:xml:ns:yang:controller:threadpool:impl:flexible?revision=2013-12-01)threadpool-impl-flexible\",\"(urn:ietf:params:xml:ns:yang:rpc-context?revision=2013-06-17)rpc-context\",\"(urn:opendaylight:params:xml:ns:yang:controller:threadpool:impl:scheduled?revision=2013-12-01)threadpool-impl-scheduled\",\"(urn:opendaylight:params:xml:ns:yang:controller:md:sal:dom?revision=2013-10-28)opendaylight-md-sal-dom\",\"(urn:opendaylight:params:xml:ns:yang:controller:config:actor-system-provider:impl?revision=2015-10-05)actor-system-provider-impl\"]},\"netconf-node-topology:unavailable-capabilities\":{}},{\"node-id\":\"confd\",\"netconf-node-topology:connection-status\":\"connecting\",\"netconf-node-topology:port\":12022,\"netconf-node-topology:host\":\"127.0.0.1\"}]}]}}\n";

    auto odl_nodes = parser.parse(capabilities_json);
    REQUIRE(odl_nodes.size() != 0);

    for(const auto & n : odl_nodes)
    {
        INFO(n.first + ", " + n.second->host.get());
    }
}

TEST_CASE("yang_1_1_xml_caps")
{
    string c = R"(<?xml version="1.0" encoding="UTF-8"?>
<rpc-reply xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="1">
  <data>
    <modules-state xmlns="urn:ietf:params:xml:ns:yang:ietf-yang-library">
      <module>
        <name>iana-crypt-hash</name>
        <revision>2014-08-06</revision>
        <namespace>urn:ietf:params:xml:ns:yang:iana-crypt-hash</namespace>
        <feature>crypt-hash-sha-512</feature>
        <feature>crypt-hash-sha-256</feature>
        <feature>crypt-hash-md5</feature>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>iana-if-type</name>
        <revision>2014-05-08</revision>
        <namespace>urn:ietf:params:xml:ns:yang:iana-if-type</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>ietf-aug-base-1</name>
        <revision>2016-07-01</revision>
        <namespace>http://cisco.com/ns/yang/ietf-aug-base-1</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-aug-base-2</name>
        <revision>2016-07-01</revision>
        <namespace>http://cisco.com/ns/yang/ietf-aug-base-2</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-inet-types</name>
        <revision>2013-07-15</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-inet-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>ietf-interfaces</name>
        <revision>2014-05-08</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-interfaces</namespace>
        <feature>pre-provisioning</feature>
        <feature>if-mib</feature>
        <feature>arbitrary-names</feature>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-netconf</name>
        <revision>2011-06-01</revision>
        <namespace>urn:ietf:params:xml:ns:netconf:base:1.0</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-netconf-acm</name>
        <revision>2012-02-22</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-netconf-acm</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-netconf-monitoring</name>
        <revision>2010-10-04</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-netconf-notifications</name>
        <revision>2012-02-06</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-netconf-notifications</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-netconf-with-defaults</name>
        <revision>2011-06-01</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-restconf-monitoring</name>
        <revision>2016-08-15</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-restconf-monitoring</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-yang-library</name>
        <revision>2016-06-21</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-yang-library</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ietf-yang-types</name>
        <revision>2013-07-15</revision>
        <namespace>urn:ietf:params:xml:ns:yang:ietf-yang-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>main</name>
        <revision>2015-11-17</revision>
        <namespace>http://cisco.com/ns/yang/main</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>main-aug1</name>
        <revision>2015-11-17</revision>
        <namespace>http://cisco.com/ns/yang/main-aug1</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>oc-pattern</name>
        <revision>2015-11-17</revision>
        <namespace>http://cisco.com/ns/yang/oc-pattern</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>openconfig-bgp</name>
        <revision>2016-06-21</revision>
        <namespace>http://openconfig.net/yang/bgp</namespace>
        <conformance-type>implement</conformance-type>
        <submodule>
          <name>openconfig-bgp-common</name>
          <revision>2016-06-21</revision>
        </submodule>
        <submodule>
          <name>openconfig-bgp-common-structure</name>
          <revision>2016-06-21</revision>
        </submodule>
        <submodule>
          <name>openconfig-bgp-global</name>
          <revision>2016-06-21</revision>
        </submodule>
        <submodule>
          <name>openconfig-bgp-neighbor</name>
          <revision>2016-06-21</revision>
        </submodule>
        <submodule>
          <name>openconfig-bgp-peer-group</name>
          <revision>2016-06-21</revision>
        </submodule>
        <submodule>
          <name>openconfig-bgp-common-multiprotocol</name>
          <revision>2016-06-21</revision>
        </submodule>
      </module>
      <module>
        <name>openconfig-bgp-policy</name>
        <revision>2016-06-21</revision>
        <namespace>http://openconfig.net/yang/bgp-policy</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>openconfig-bgp-types</name>
        <revision>2016-06-21</revision>
        <namespace>http://openconfig.net/yang/bgp-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>openconfig-extensions</name>
        <revision>2015-10-09</revision>
        <namespace>http://openconfig.net/yang/openconfig-ext</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>openconfig-if-ethernet</name>
        <revision>2016-05-26</revision>
        <namespace>http://openconfig.net/yang/interfaces/ethernet</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>openconfig-interfaces</name>
        <revision>2016-05-26</revision>
        <namespace>http://openconfig.net/yang/interfaces</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>openconfig-platform</name>
        <revision>2016-06-06</revision>
        <namespace>http://openconfig.net/yang/platform</namespace>
        <deviation>
          <name>cisco-xr-openconfig-platform-deviations</name>
          <revision>2016-10-16</revision>
        </deviation>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>openconfig-platform-transceiver</name>
        <revision>2016-05-24</revision>
        <namespace>http://openconfig.net/yang/platform/transceiver</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>openconfig-platform-types</name>
        <revision>2016-06-06</revision>
        <namespace>http://openconfig.net/yang/platform-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>openconfig-policy-types</name>
        <revision>2016-05-12</revision>
        <namespace>http://openconfig.net/yang/policy-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>openconfig-routing-policy</name>
        <revision>2016-05-12</revision>
        <namespace>http://openconfig.net/yang/routing-policy</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>openconfig-terminal-device</name>
        <revision>2016-06-17</revision>
        <namespace>http://openconfig.net/yang/terminal-device</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>openconfig-transport-types</name>
        <revision>2016-06-17</revision>
        <namespace>http://openconfig.net/yang/transport-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>openconfig-types</name>
        <revision>2016-05-31</revision>
        <namespace>http://openconfig.net/yang/openconfig-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>tailf-aaa</name>
        <revision>2015-06-16</revision>
        <namespace>http://tail-f.com/ns/aaa/1.1</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>tailf-acm</name>
        <revision>2013-03-07</revision>
        <namespace>http://tail-f.com/yang/acm</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>tailf-common-monitoring</name>
        <revision>2013-06-14</revision>
        <namespace>http://tail-f.com/yang/common-monitoring</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>tailf-confd-monitoring</name>
        <revision>2013-06-14</revision>
        <namespace>http://tail-f.com/yang/confd-monitoring</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>tailf-kicker</name>
        <revision>2016-11-24</revision>
        <namespace>http://tail-f.com/ns/kicker</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>tailf-netconf-monitoring</name>
        <revision>2016-11-24</revision>
        <namespace>http://tail-f.com/yang/netconf-monitoring</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>tailf-webui</name>
        <revision>2013-03-07</revision>
        <namespace>http://tail-f.com/ns/webui</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ydktest-aug-ietf-1</name>
        <revision>2016-06-17</revision>
        <namespace>http://cisco.com/ns/yang/yaug-one</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ydktest-aug-ietf-2</name>
        <revision>2016-06-22</revision>
        <namespace>http://cisco.com/ns/yang/yaug-two</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ydktest-aug-ietf-4</name>
        <revision>2016-06-27</revision>
        <namespace>http://cisco.com/ns/yang/yaug-four</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ydktest-aug-ietf-5</name>
        <revision>2017-07-26</revision>
        <namespace>http://cisco.com/ns/yang/yaug-five</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ydktest-filterread</name>
        <revision>2015-11-17</revision>
        <namespace>http://cisco.com/ns/yang/ydk-filter</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ydktest-sanity</name>
        <revision>2015-11-17</revision>
        <namespace>http://cisco.com/ns/yang/ydktest-sanity</namespace>
        <feature>ipv6-privacy-autoconf</feature>
        <feature>ipv4-non-contiguous-netmasks</feature>
        <conformance-type>implement</conformance-type>
        <submodule>
          <name>ydktest-sanity-submodule</name>
          <revision>2016-04-25</revision>
        </submodule>
      </module>
      <module>
        <name>ydktest-sanity-augm</name>
        <revision>2015-11-17</revision>
        <namespace>http://cisco.com/ns/yang/ydktest-sanity-augm</namespace>
        <conformance-type>implement</conformance-type>
      </module>
      <module>
        <name>ydktest-sanity-types</name>
        <revision>2016-04-11</revision>
        <namespace>http://cisco.com/ns/yang/ydktest-sanity-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
      <module>
        <name>ydktest-types</name>
        <revision>2016-05-23</revision>
        <namespace>http://cisco.com/ns/yang/ydktest-types</namespace>
        <conformance-type>import</conformance-type>
      </module>
    </modules-state>
  </data>
</rpc-reply>
)";

    string hello=R"(<?xml version="1.0" encoding="UTF-8"?>
<hello xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
<capabilities>
<capability>urn:ietf:params:netconf:base:1.0</capability>
<capability>urn:ietf:params:netconf:base:1.1</capability>
<capability>urn:ietf:params:netconf:capability:writable-running:1.0</capability>
<capability>urn:ietf:params:netconf:capability:candidate:1.0</capability>
<capability>urn:ietf:params:netconf:capability:confirmed-commit:1.0</capability>
<capability>urn:ietf:params:netconf:capability:confirmed-commit:1.1</capability>
<capability>urn:ietf:params:netconf:capability:xpath:1.0</capability>
<capability>urn:ietf:params:netconf:capability:validate:1.0</capability>
<capability>urn:ietf:params:netconf:capability:validate:1.1</capability>
<capability>urn:ietf:params:netconf:capability:rollback-on-error:1.0</capability>
<capability>urn:ietf:params:netconf:capability:with-defaults:1.0?basic-mode=explicit&amp;also-supported=report-all-tagged</capability>
<capability>urn:ietf:params:netconf:capability:yang-library:1.0?revision=2016-06-21&amp;module-set-id=fba28be41dbd9b62782518ee42d32f29</capability>
<capability>http://tail-f.com/ns/netconf/actions/1.0</capability>
<capability>http://tail-f.com/ns/netconf/extensions</capability>
<capability>http://cisco.com/ns/yang/ietf-aug-base-1?module=ietf-aug-base-1&amp;revision=2016-07-01</capability>
<capability>http://cisco.com/ns/yang/ietf-aug-base-2?module=ietf-aug-base-2&amp;revision=2016-07-01</capability>
<capability>http://cisco.com/ns/yang/main?module=main&amp;revision=2015-11-17</capability>
<capability>http://cisco.com/ns/yang/main-aug1?module=main-aug1&amp;revision=2015-11-17</capability>
<capability>http://cisco.com/ns/yang/oc-pattern?module=oc-pattern&amp;revision=2015-11-17</capability>
<capability>http://cisco.com/ns/yang/yaug-five?module=ydktest-aug-ietf-5&amp;revision=2017-07-26</capability>
<capability>http://cisco.com/ns/yang/yaug-four?module=ydktest-aug-ietf-4&amp;revision=2016-06-27</capability>
<capability>http://cisco.com/ns/yang/yaug-one?module=ydktest-aug-ietf-1&amp;revision=2016-06-17</capability>
<capability>http://cisco.com/ns/yang/yaug-two?module=ydktest-aug-ietf-2&amp;revision=2016-06-22</capability>
<capability>http://cisco.com/ns/yang/ydk-filter?module=ydktest-filterread&amp;revision=2015-11-17</capability>
<capability>http://cisco.com/ns/yang/ydktest-sanity?module=ydktest-sanity&amp;revision=2015-11-17&amp;features=ipv6-privacy-autoconf,ipv4-non-contiguous-netmasks</capability>
<capability>http://cisco.com/ns/yang/ydktest-sanity-augm?module=ydktest-sanity-augm&amp;revision=2015-11-17</capability>
<capability>http://cisco.com/ns/yang/ydktest-sanity-types?module=ydktest-sanity-types&amp;revision=2016-04-11</capability>
<capability>http://cisco.com/ns/yang/ydktest-types?module=ydktest-types&amp;revision=2016-05-23</capability>
<capability>http://openconfig.net/yang/bgp?module=openconfig-bgp&amp;revision=2016-06-21</capability>
<capability>http://openconfig.net/yang/bgp-policy?module=openconfig-bgp-policy&amp;revision=2016-06-21</capability>
<capability>http://openconfig.net/yang/bgp-types?module=openconfig-bgp-types&amp;revision=2016-06-21</capability>
<capability>http://openconfig.net/yang/interfaces?module=openconfig-interfaces&amp;revision=2016-05-26</capability>
<capability>http://openconfig.net/yang/interfaces/ethernet?module=openconfig-if-ethernet&amp;revision=2016-05-26</capability>
<capability>http://openconfig.net/yang/openconfig-ext?module=openconfig-extensions&amp;revision=2015-10-09</capability>
<capability>http://openconfig.net/yang/openconfig-types?module=openconfig-types&amp;revision=2016-05-31</capability>
<capability>http://openconfig.net/yang/platform?module=openconfig-platform&amp;revision=2016-06-06&amp;deviations=cisco-xr-openconfig-platform-deviations</capability>
<capability>http://openconfig.net/yang/platform-types?module=openconfig-platform-types&amp;revision=2016-06-06</capability>
<capability>http://openconfig.net/yang/platform/transceiver?module=openconfig-platform-transceiver&amp;revision=2016-05-24</capability>
<capability>http://openconfig.net/yang/policy-types?module=openconfig-policy-types&amp;revision=2016-05-12</capability>
<capability>http://openconfig.net/yang/routing-policy?module=openconfig-routing-policy&amp;revision=2016-05-12</capability>
<capability>http://openconfig.net/yang/terminal-device?module=openconfig-terminal-device&amp;revision=2016-06-17</capability>
<capability>http://openconfig.net/yang/transport-types?module=openconfig-transport-types&amp;revision=2016-06-17</capability>
<capability>http://tail-f.com/ns/aaa/1.1?module=tailf-aaa&amp;revision=2015-06-16</capability>
<capability>http://tail-f.com/ns/kicker?module=tailf-kicker&amp;revision=2016-11-24</capability>
<capability>http://tail-f.com/ns/webui?module=tailf-webui&amp;revision=2013-03-07</capability>
<capability>http://tail-f.com/yang/acm?module=tailf-acm&amp;revision=2013-03-07</capability>
<capability>http://tail-f.com/yang/common-monitoring?module=tailf-common-monitoring&amp;revision=2013-06-14</capability>
<capability>http://tail-f.com/yang/confd-monitoring?module=tailf-confd-monitoring&amp;revision=2013-06-14</capability>
<capability>http://tail-f.com/yang/netconf-monitoring?module=tailf-netconf-monitoring&amp;revision=2016-11-24</capability>
<capability>urn:ietf:params:xml:ns:yang:iana-crypt-hash?module=iana-crypt-hash&amp;revision=2014-08-06&amp;features=crypt-hash-sha-512,crypt-hash-sha-256,crypt-hash-md5</capability>
<capability>urn:ietf:params:xml:ns:yang:iana-if-type?module=iana-if-type&amp;revision=2014-05-08</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-inet-types?module=ietf-inet-types&amp;revision=2013-07-15</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-interfaces?module=ietf-interfaces&amp;revision=2014-05-08&amp;features=pre-provisioning,if-mib,arbitrary-names</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-acm?module=ietf-netconf-acm&amp;revision=2012-02-22</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring?module=ietf-netconf-monitoring&amp;revision=2010-10-04</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-notifications?module=ietf-netconf-notifications&amp;revision=2012-02-06</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-restconf-monitoring?module=ietf-restconf-monitoring&amp;revision=2016-08-15</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-yang-library?module=ietf-yang-library&amp;revision=2016-06-21</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-yang-types?module=ietf-yang-types&amp;revision=2013-07-15</capability>
<capability>urn:ietf:params:xml:ns:netconf:base:1.0?module=ietf-netconf&amp;revision=2011-06-01</capability>
<capability>urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults?module=ietf-netconf-with-defaults&amp;revision=2011-06-01</capability>
</capabilities>
<session-id>1466</session-id></hello>)";

    IetfCapabilitiesXmlParser p;
    auto v1 = p.parse_yang_1_1(c);
    REQUIRE(v1.size()!=0);

    auto v2 = p.parse(hello);
    REQUIRE(v2.size()!=0);

    IetfCapabilitiesParser capabilities_parser{};
    map<string, path::Capability*> m;
    auto a1 = capabilities_parser.parse(v1);
    REQUIRE(a1.size()!=0);
    for(auto &s:a1)
    {
        cout<<s<<endl;
        m.insert(make_pair(s.module, &s));
    }
    cout<<endl;

    auto a2 = capabilities_parser.parse(v2);
    REQUIRE(a2.size()!=0);
    for(auto&s:a2)
    {
        cout<<s<<endl;
        auto i = m.find(s.module);
        REQUIRE(i!=m.end());
        bool same = s==*(i->second);
        REQUIRE(same == true);
    }

    REQUIRE(a1.size()==a2.size());
}
