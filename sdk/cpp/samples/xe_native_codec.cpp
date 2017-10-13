/*  ----------------------------------------------------------------
 Copyright 2016 Cisco Systems

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
------------------------------------------------------------------*/
#include <iostream>

#include <ydk/types.hpp>
#include <ydk/path_api.hpp>
#include <ydk/codec_service.hpp>
#include <ydk/codec_provider.hpp>

#include <ydk_cisco_ios_xe/Cisco_IOS_XE_native.hpp>
#include <spdlog/spdlog.h>

#include "args_parser.h"

using namespace ydk;
using namespace cisco_ios_xe::Cisco_IOS_XE_native;
using namespace std;

static const string giga_payload = R"(
         <native xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-native">
              <interface>
                <GigabitEthernet>
                  <name>2</name>
                  <description>xyz abc</description>
                  <ip>
                    <address>
                      <primary>
                        <address>10.0.0.30</address>
                        <mask>255.255.255.252</mask>
                      </primary>
                    </address>
                  </ip>
                  <mop>
                    <enabled>false</enabled>
                  </mop>
                  <cdp xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-cdp">
                    <enable>true</enable>
                  </cdp>
                  <negotiation xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">
                    <auto>true</auto>
                  </negotiation>
                </GigabitEthernet>
              </interface>
            </native>
        )";

static const string xe_native_payload = R"(
    <native xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-native">
      <version>16.5</version>
      <boot>
        <system>
          <switch xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <all>
              <flash>cat3k_caa-universalk9.BLD_V165_1_THROTTLE_LATEST_20170621_061252.SSA.bin</flash>
            </all>
          </switch>
        </system>
      </boot>
      <service>
        <timestamps>
          <debug>
            <datetime>
              <msec>
                <localtime/>
              </msec>
            </datetime>
          </debug>
          <log>
            <datetime>
              <msec>
                <localtime/>
              </msec>
            </datetime>
          </log>
        </timestamps>
        <compress-config/>
      </service>
      <hostname>delta8</hostname>
      <enable>
        <password>
          <type>7</type>
          <secret>01100F175804575D720C</secret>
        </password>
      </enable>
      <username>
        <name>admin</name>
        <privilege>15</privilege>
        <password>
          <encryption>7</encryption>
          <password>045802150C2E1D1C5A</password>
        </password>
      </username>
      <vrf>
        <definition>
          <name>Mgmt-intf</name>
          <address-family>
            <ipv4/>
          </address-family>
        </definition>
        <definition>
          <name>Mgmt-vrf</name>
          <address-family>
            <ipv4/>
            <ipv6/>
          </address-family>
        </definition>
      </vrf>
      <flow>
        <exporter xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-flow">
          <name>172.16.10.126</name>
          <destination>
            <ip>172.16.10.126</ip>
          </destination>
        </exporter>
      </flow>
      <ip>
        <bgp-community>
          <new-format/>
        </bgp-community>
        <domain>
          <lookup>false</lookup>
          <name>yale.edu</name>
        </domain>
        <forward-protocol>
          <protocol>nd</protocol>
        </forward-protocol>
        <ftp>
          <source-interface>
            <GigabitEthernet>0/0</GigabitEthernet>
          </source-interface>
          <password>
            <password-container>
              <encryption>7</encryption>
              <password>111312</password>
            </password-container>
          </password>
          <username>zk</username>
        </ftp>
        <name-server>
          <no-vrf>172.16.10.50</no-vrf>
        </name-server>
        <route>
          <vrf>
            <name>Mgmt-intf</name>
            <ip-route-interface-forwarding-list>
              <prefix>0.0.0.0</prefix>
              <mask>0.0.0.0</mask>
              <fwd-list>
                <fwd>172.26.47.97</fwd>
              </fwd-list>
            </ip-route-interface-forwarding-list>
          </vrf>
          <vrf>
            <name>Mgmt-vrf</name>
            <ip-route-interface-forwarding-list>
              <prefix>0.0.0.0</prefix>
              <mask>0.0.0.0</mask>
              <fwd-list>
                <fwd>172.26.47.97</fwd>
              </fwd-list>
            </ip-route-interface-forwarding-list>
          </vrf>
        </route>
        <routing/>
        <tcp>
          <path-mtu-discovery/>
        </tcp>
        <tftp>
          <source-interface>
            <GigabitEthernet>0/0</GigabitEthernet>
          </source-interface>
        </tftp>
        <access-list>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>AutoQos-4.0-wlan-Acl-Bulk-Data</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>22</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>20</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>465</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>30</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>143</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>40</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>993</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>50</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>995</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>60</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1914</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>70</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>ftp</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>80</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>ftp-data</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>90</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>smtp</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>100</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>pop3</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>AutoQos-4.0-wlan-Acl-MultiEnhanced-Conf</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>16384</dst-range1>
                <dst-range2>32767</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>20</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>50000</dst-range1>
                <dst-range2>59999</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>AutoQos-4.0-wlan-Acl-Scavanger</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>2300</dst-range1>
                <dst-range2>2400</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>20</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>2300</dst-range1>
                <dst-range2>2400</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>30</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>6881</dst-range1>
                <dst-range2>6999</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>40</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>28800</dst-range1>
                <dst-range2>29100</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>50</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1214</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>60</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1214</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>70</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>3689</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>80</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>3689</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>90</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>11999</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>AutoQos-4.0-wlan-Acl-Signaling</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>2000</dst-range1>
                <dst-range2>2002</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>20</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>5060</dst-range1>
                <dst-range2>5061</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>30</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-range1>5060</dst-range1>
                <dst-range2>5061</dst-range2>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>AutoQos-4.0-wlan-Acl-Transactional-Data</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>443</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>20</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1521</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>30</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1521</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>40</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1526</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>50</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1526</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>60</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1575</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>70</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1575</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>80</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1630</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>90</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1630</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>100</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>1527</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>110</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>6200</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>120</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>3389</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>130</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>5985</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>140</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>8080</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>CISCO-CWA-URL-REDIRECT-ACL</name>
            <access-list-seq-rule>
              <sequence>100</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>domain</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>101</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>domain</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>102</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>udp</protocol>
                <any/>
                <src-eq>bootps</src-eq>
                <dst-any/>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>103</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>bootpc</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>104</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>udp</protocol>
                <any/>
                <src-eq>bootpc</src-eq>
                <dst-any/>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>105</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>www</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>IP-Adm-V4-Int-ACL-global</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>www</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>20</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>443</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>implicit_deny</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>ip</protocol>
                <any/>
                <dst-any/>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
          <extended xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>preauth_v4</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>domain</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>20</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>domain</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>30</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <src-eq>bootps</src-eq>
                <dst-any/>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>40</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <dst-any/>
                <dst-eq>bootpc</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>50</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <src-eq>bootpc</src-eq>
                <dst-any/>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>60</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>ip</protocol>
                <any/>
                <dst-any/>
              </ace-rule>
            </access-list-seq-rule>
          </extended>
        </access-list>
        <radius xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <source-interface>
            <Loopback>0</Loopback>
          </source-interface>
        </radius>
        <http xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-http">
          <authentication>
            <local/>
          </authentication>
          <server>true</server>
          <secure-server>true</secure-server>
          <client>
            <source-interface>Loopback0</source-interface>
          </client>
        </http>
      </ip>
      <ipv6>
        <access-list>
          <named-acl xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>implicit_deny_v6</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>ipv6</protocol>
                <any/>
                <destination-any/>
              </ace-rule>
            </access-list-seq-rule>
          </named-acl>
          <named-acl xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-acl">
            <name>preauth_v6</name>
            <access-list-seq-rule>
              <sequence>10</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <destination-any/>
                <dst-eq>domain</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>20</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>tcp</protocol>
                <any/>
                <destination-any/>
                <dst-eq>domain</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>30</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>icmp</protocol>
                <any/>
                <destination-any/>
                <named-msg-type>nd-ns</named-msg-type>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>40</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>icmp</protocol>
                <any/>
                <destination-any/>
                <named-msg-type>nd-na</named-msg-type>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>50</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>icmp</protocol>
                <any/>
                <destination-any/>
                <named-msg-type>router-solicitation</named-msg-type>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>60</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>icmp</protocol>
                <any/>
                <destination-any/>
                <named-msg-type>router-advertisement</named-msg-type>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>70</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>icmp</protocol>
                <any/>
                <destination-any/>
                <named-msg-type>redirect</named-msg-type>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>80</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <src-eq>547</src-eq>
                <destination-any/>
                <dst-eq>546</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>90</sequence>
              <ace-rule>
                <action>permit</action>
                <protocol>udp</protocol>
                <any/>
                <src-eq>546</src-eq>
                <destination-any/>
                <dst-eq>547</dst-eq>
              </ace-rule>
            </access-list-seq-rule>
            <access-list-seq-rule>
              <sequence>100</sequence>
              <ace-rule>
                <action>deny</action>
                <protocol>ipv6</protocol>
                <any/>
                <destination-any/>
              </ace-rule>
            </access-list-seq-rule>
          </named-acl>
        </access-list>
      </ipv6>
      <policy>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>non-client-nrt-class</name>
          <prematch>match-any</prematch>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-default</name>
          <prematch>match-any</prematch>
          <description>DHCP Snooping and Unused queues</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-control-low-priority</name>
          <prematch>match-any</prematch>
          <description>ICMP redirect and general punt</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-data</name>
          <prematch>match-any</prematch>
          <description>ICMP_GEN and BROADCAST</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-dot1x-auth</name>
          <prematch>match-any</prematch>
          <description>DOT1X Auth</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-forus</name>
          <prematch>match-any</prematch>
          <description>Forus Address resolution and Forus traffic</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-l2-control</name>
          <prematch>match-any</prematch>
          <description>L2 control, FSS</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-l2lvx-control</name>
          <prematch>match-any</prematch>
          <description>L2 LVX control packets</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-multicast</name>
          <prematch>match-any</prematch>
          <description>Transit Traffic and MCAST Data</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-multicast-end-station</name>
          <prematch>match-any</prematch>
          <description>MCAST END STATION</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-protocol-snooping</name>
          <prematch>match-any</prematch>
          <description>Protocol snooping</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-punt-webauth</name>
          <prematch>match-any</prematch>
          <description>Punt Webauth</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-routing-control</name>
          <prematch>match-any</prematch>
          <description>Routing control and Low latency</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-sw-forward</name>
          <prematch>match-any</prematch>
          <description>Sw forwarding, L2 LVX data, LOGGING</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-sys-data</name>
          <prematch>match-any</prematch>
          <description>Learning cache ovfl, Crypto Control, Exception, EGR Exception, NFL SAMPLED DATA, Gold Pkt, RPF Failed</description>
        </class-map>
        <class-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-police-topology-control</name>
          <prematch>match-any</prematch>
          <description>Topology control</description>
        </class-map>
        <policy-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>port_child_policy</name>
          <class>
            <name>non-client-nrt-class</name>
            <action-list>
              <action-type>bandwidth</action-type>
              <bandwidth>
                <remaining>
                  <rem-option>ratio</rem-option>
                  <ratio>10</ratio>
                </remaining>
              </bandwidth>
            </action-list>
          </class>
        </policy-map>
        <policy-map xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <name>system-cpp-policy</name>
          <class>
            <name>system-cpp-default</name>
          </class>
          <class>
            <name>system-cpp-police-control-low-priority</name>
            <action-list>
              <action-type>police</action-type>
              <police-rate-unit>
                <police>
                  <rate>
                    <units>200</units>
                    <xps>pps</xps>
                  </rate>
                </police>
              </police-rate-unit>
            </action-list>
          </class>
          <class>
            <name>system-cpp-police-data</name>
            <action-list>
              <action-type>police</action-type>
              <police-rate-unit>
                <police>
                  <rate>
                    <units>600</units>
                    <xps>pps</xps>
                  </rate>
                </police>
              </police-rate-unit>
            </action-list>
          </class>
          <class>
            <name>system-cpp-police-dot1x-auth</name>
          </class>
          <class>
            <name>system-cpp-police-forus</name>
          </class>
          <class>
            <name>system-cpp-police-l2-control</name>
          </class>
          <class>
            <name>system-cpp-police-l2lvx-control</name>
            <action-list>
              <action-type>police</action-type>
              <police-rate-unit>
                <police>
                  <rate>
                    <units>1000</units>
                    <xps>pps</xps>
                  </rate>
                </police>
              </police-rate-unit>
            </action-list>
          </class>
          <class>
            <name>system-cpp-police-multicast</name>
            <action-list>
              <action-type>police</action-type>
              <police-rate-unit>
                <police>
                  <rate>
                    <units>500</units>
                    <xps>pps</xps>
                  </rate>
                </police>
              </police-rate-unit>
            </action-list>
          </class>
          <class>
            <name>system-cpp-police-multicast-end-station</name>
            <action-list>
              <action-type>police</action-type>
              <police-rate-unit>
                <police>
                  <rate>
                    <units>2000</units>
                    <xps>pps</xps>
                  </rate>
                </police>
              </police-rate-unit>
            </action-list>
          </class>
          <class>
            <name>system-cpp-police-protocol-snooping</name>
          </class>
          <class>
            <name>system-cpp-police-punt-webauth</name>
          </class>
          <class>
            <name>system-cpp-police-routing-control</name>
            <action-list>
              <action-type>police</action-type>
              <police-rate-unit>
                <police>
                  <rate>
                    <units>1800</units>
                    <xps>pps</xps>
                  </rate>
                </police>
              </police-rate-unit>
            </action-list>
          </class>
          <class>
            <name>system-cpp-police-sw-forward</name>
            <action-list>
              <action-type>police</action-type>
              <police-rate-unit>
                <police>
                  <rate>
                    <units>1000</units>
                    <xps>pps</xps>
                  </rate>
                </police>
              </police-rate-unit>
            </action-list>
          </class>
          <class>
            <name>system-cpp-police-sys-data</name>
            <action-list>
              <action-type>police</action-type>
              <police-rate-unit>
                <police>
                  <rate>
                    <units>100</units>
                    <xps>pps</xps>
                  </rate>
                </police>
              </police-rate-unit>
            </action-list>
          </class>
          <class>
            <name>system-cpp-police-topology-control</name>
          </class>
        </policy-map>
      </policy>
      <interface>
        <GigabitEthernet>
          <name>0/0</name>
          <description>Gig 0/0 -&gt; delta-flash Gig 1/0/5</description>
          <vrf>
            <forwarding>Mgmt-vrf</forwarding>
          </vrf>
          <ip>
            <address>
              <primary>
                <address>172.26.47.103</address>
                <mask>255.255.255.224</mask>
              </primary>
            </address>
            <proxy-arp>false</proxy-arp>
          </ip>
          <negotiation xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">
            <auto>true</auto>
          </negotiation>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/1</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/10</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/11</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/12</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/13</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/14</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/15</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/16</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/17</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/18</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/19</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/2</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/20</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/21</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/22</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/23</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/24</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/25</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/26</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/27</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/28</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/29</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/3</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/30</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/31</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/32</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/33</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/34</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/35</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/36</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/4</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/5</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/6</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/7</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/8</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/0/9</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/1/1</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/1/2</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/1/3</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <GigabitEthernet>
          <name>1/1/4</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </GigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/37</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/38</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/39</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/40</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/41</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/42</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/43</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/44</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/45</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/46</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/47</name>
          <description>Ten 1/0/47 -&gt; papajohns Ten 1/0/47</description>
          <switchport-conf>
            <switchport>false</switchport>
          </switchport-conf>
          <bfd>
            <interval xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">
              <msecs>300</msecs>
              <min_rx>300</min_rx>
              <multiplier>3</multiplier>
            </interval>
            <echo xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">false</echo>
          </bfd>
          <dampening/>
          <ip>
            <address>
              <primary>
                <address>172.16.201.18</address>
                <mask>255.255.255.252</mask>
              </primary>
            </address>
            <proxy-arp>false</proxy-arp>
            <router>
            </router>
            <redirects>false</redirects>
          </ip>
          <load-interval>30</load-interval>
          <logging>
            <event>
              <link-status/>
            </event>
          </logging>
          <carrier-delay xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">
            <msec>0</msec>
          </carrier-delay>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/0/48</name>
          <description>Ten 1/0/48 -&gt; papajohns Ten 1/0/48</description>
          <switchport-conf>
            <switchport>false</switchport>
          </switchport-conf>
          <bfd>
            <interval xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">
              <msecs>300</msecs>
              <min_rx>300</min_rx>
              <multiplier>3</multiplier>
            </interval>
            <echo xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">false</echo>
          </bfd>
          <dampening/>
          <ip>
            <address>
              <primary>
                <address>172.16.201.22</address>
                <mask>255.255.255.252</mask>
              </primary>
            </address>
            <proxy-arp>false</proxy-arp>
            <router>

            </router>
            <redirects>false</redirects>
          </ip>
          <load-interval>30</load-interval>
          <logging>
            <event>
              <link-status/>
            </event>
          </logging>
          <carrier-delay xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">
            <msec>0</msec>
          </carrier-delay>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/1/1</name>
          <description>Ten 1/1/1 -&gt; quiznos Ten 1/1/1</description>
          <switchport-conf>
            <switchport>false</switchport>
          </switchport-conf>
          <bfd>
            <interval xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">
              <msecs>300</msecs>
              <min_rx>300</min_rx>
              <multiplier>3</multiplier>
            </interval>
            <echo xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">false</echo>
          </bfd>
          <dampening/>
          <ip>
            <address>
              <primary>
                <address>172.16.202.13</address>
                <mask>255.255.255.252</mask>
              </primary>
            </address>
            <proxy-arp>false</proxy-arp>
            <router>

            </router>
            <redirects>false</redirects>
          </ip>
          <load-interval>30</load-interval>
          <logging>
            <event>
              <link-status/>
            </event>
          </logging>
          <carrier-delay xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">
            <msec>0</msec>
          </carrier-delay>
          <duplex xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">auto</duplex>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/1/2</name>
          <description>Ten 1/1/2 -&gt; tacotime Ten 1/1/2</description>
          <switchport-conf>
            <switchport>false</switchport>
          </switchport-conf>
          <bfd>
            <interval xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">
              <msecs>300</msecs>
              <min_rx>300</min_rx>
              <multiplier>3</multiplier>
            </interval>
            <echo xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">false</echo>
          </bfd>
          <dampening/>
          <ip>
            <address>
              <primary>
                <address>172.16.202.5</address>
                <mask>255.255.255.252</mask>
              </primary>
            </address>
            <proxy-arp>false</proxy-arp>
            <router>

            </router>
            <redirects>false</redirects>
          </ip>
          <load-interval>30</load-interval>
          <logging>
            <event>
              <link-status/>
            </event>
          </logging>
          <carrier-delay xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">
            <msec>0</msec>
          </carrier-delay>
          <duplex xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">auto</duplex>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/1/3</name>
          <description>Ten 1/1/3 -&gt; bravo10 Ten 1/1/3</description>
          <switchport-conf>
            <switchport>false</switchport>
          </switchport-conf>
          <bfd>
            <interval xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">
              <msecs>300</msecs>
              <min_rx>300</min_rx>
              <multiplier>3</multiplier>
            </interval>
            <echo xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">false</echo>
          </bfd>
          <dampening/>
          <ip>
            <address>
              <primary>
                <address>172.16.201.14</address>
                <mask>255.255.255.252</mask>
              </primary>
            </address>
            <proxy-arp>false</proxy-arp>
            <router>

            </router>
            <redirects>false</redirects>
          </ip>
          <load-interval>30</load-interval>
          <logging>
            <event>
              <link-status/>
            </event>
          </logging>
          <carrier-delay xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">
            <msec>0</msec>
          </carrier-delay>
          <duplex xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">auto</duplex>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/1/4</name>
          <description>Ten 1/1/4 -&gt; bravo9 Ten 1/1/4</description>
          <switchport-conf>
            <switchport>false</switchport>
          </switchport-conf>
          <bfd>
            <interval xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">
              <msecs>300</msecs>
              <min_rx>300</min_rx>
              <multiplier>3</multiplier>
            </interval>
            <echo xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-bfd">false</echo>
          </bfd>
          <dampening/>
          <ip>
            <address>
              <primary>
                <address>172.16.201.10</address>
                <mask>255.255.255.252</mask>
              </primary>
            </address>
            <proxy-arp>false</proxy-arp>
            <router>

            </router>
            <redirects>false</redirects>
          </ip>
          <load-interval>30</load-interval>
          <logging>
            <event>
              <link-status/>
            </event>
          </logging>
          <carrier-delay xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">
            <msec>0</msec>
          </carrier-delay>
          <duplex xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ethernet">auto</duplex>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/1/5</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/1/6</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/1/7</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <TenGigabitEthernet>
          <name>1/1/8</name>
          <device-tracking xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
            <attach-policy>IPDT_MAX_10</attach-policy>
          </device-tracking>
        </TenGigabitEthernet>
        <FortyGigabitEthernet>
          <name>1/1/1</name>
        </FortyGigabitEthernet>
        <FortyGigabitEthernet>
          <name>1/1/2</name>
        </FortyGigabitEthernet>
        <Loopback>
          <name>0</name>
          <ip>
            <address>
              <primary>
                <address>172.16.99.222</address>
                <mask>255.255.255.255</mask>
              </primary>
            </address>
          </ip>
        </Loopback>
        <Vlan>
          <name>1</name>
          <shutdown/>
          <ip>
            <no-address>
              <address>false</address>
            </no-address>
          </ip>
        </Vlan>
      </interface>
      <control-plane>
        <service-policy xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-policy">
          <input>system-cpp-policy</input>
        </service-policy>
      </control-plane>
      <clock>
        <calendar-valid/>
        <summer-time>
          <zone>EDT</zone>
          <recurring/>
        </summer-time>
        <timezone>
          <zone>EST</zone>
          <hours>-5</hours>
          <minutes>0</minutes>
        </timezone>
      </clock>
      <aaa>
        <new-model xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa"/>
        <group xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <server>
            <radius>
              <name>dnac-radius-group</name>
              <server>
                <name>
                  <name>dnac-radius_172.16.10.150</name>
                </name>
              </server>
              <ip>
                <radius>
                  <source-interface>Loopback0</source-interface>
                </radius>
              </ip>
            </radius>
          </server>
        </group>
        <authentication xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <dot1x>
            <default>
              <group>dnac-radius-group</group>
            </default>
          </dot1x>
          <enable>
            <default>
              <enable/>
            </default>
          </enable>
          <login>
            <name>default</name>
            <a1>
              <local/>
            </a1>
          </login>
        </authentication>
        <authorization xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <exec>
            <name>default</name>
            <a1>
              <local/>
            </a1>
          </exec>
          <network>
            <id>default</id>
            <a1>
              <group>dnac-radius-group</group>
            </a1>
          </network>
          <network>
            <id>dnac-cts-list</id>
            <a1>
              <group>dnac-radius-group</group>
            </a1>
          </network>
        </authorization>
        <server xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <radius>
            <dynamic-author>
              <client>
                <ip>172.16.10.150</ip>
                <server-key>
                  <string>cisco123</string>
                </server-key>
              </client>
            </dynamic-author>
          </radius>
        </server>
        <session-id xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">common</session-id>
      </aaa>
      <redundancy>
        <mode>sso</mode>
      </redundancy>
      <spanning-tree>
        <extend xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-spanning-tree">
          <system-id/>
        </extend>
        <mode xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-spanning-tree">rapid-pvst</mode>
      </spanning-tree>
      <dot1x>
        <system-auth-control xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-dot1x"/>
      </dot1x>
      <vtp>
        <domain xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-vtp">cisco</domain>
        <mode xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-vtp">
          <transparent/>
        </mode>
      </vtp>
      <system>
        <mtu xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
          <size>9100</size>
        </mtu>
      </system>
      <radius-server>
        <attribute xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <number>6</number>
          <send-attribute>on-for-login-auth</send-attribute>
          <send-attribute>support-multiple</send-attribute>
        </attribute>
        <attribute xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <number>8</number>
          <send-attribute>include-in-access-req</send-attribute>
        </attribute>
        <attribute xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <number>25</number>
          <access-request>
            <include/>
          </access-request>
        </attribute>
        <dead-criteria xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <time>2</time>
          <tries>1</tries>
        </dead-criteria>
      </radius-server>
      <radius>
        <server xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <id>dnac-radius_172.16.10.150</id>
          <address>
            <ipv4>
              <host>172.16.10.150</host>
              <auth-port>1812</auth-port>
              <acct-port>1813</acct-port>
            </ipv4>
          </address>
          <pac>
            <key>
              <key>cisco123</key>
            </key>
          </pac>
        </server>
      </radius>
      <crypto>
        <pki xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-crypto">
          <certificate>
            <chain>
              <name>172.16.10.100</name>
              <certificate>
                <serial>2062516141598147150141284864953118629773</serial>
                <certtype>ca</certtype>
              </certificate>
              <certificate>
                <serial>quit</serial>
              </certificate>
            </chain>
            <chain>
              <name>TP-self-signed-2394274574</name>
              <certificate>
                <serial>01</serial>
                <certtype>self-signed</certtype>
              </certificate>
              <certificate>
                <serial>quit</serial>
              </certificate>
            </chain>
          </certificate>
          <trustpoint>
            <id>172.16.10.100</id>
            <enrollment>
              <mode>
                <ra/>
              </mode>
              <terminal/>
            </enrollment>
            <revocation-check>crl</revocation-check>
          </trustpoint>
          <trustpoint>
            <id>TP-self-signed-2394274574</id>
            <enrollment>
              <selfsigned/>
            </enrollment>
            <revocation-check>none</revocation-check>
            <rsakeypair>
              <key-label>TP-self-signed-2394274574</key-label>
            </rsakeypair>
            <subject-name>cn=IOS-Self-Signed-Certificate-2394274574</subject-name>
          </trustpoint>
        </pki>
      </crypto>
      <cts>
        <authorization xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-cts">
          <list>dnac-cts-list</list>
        </authorization>
        <role-based xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-cts">
          <enforcement/>
        </role-based>
      </cts>
      <router>
      </router>
      <snmp-server>
        <community xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-snmp">
          <name>private</name>
          <RW/>
        </community>
        <community xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-snmp">
          <name>public</name>
          <RO/>
        </community>
        <manager xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-snmp"/>
        <system-shutdown xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-snmp"/>
      </snmp-server>
      <device-tracking>
        <policy xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-device-tracking">
          <word>IPDT_MAX_10</word>
          <protocol>
            <udp>false</udp>
          </protocol>
          <tracking>
            <tracking-on-off>enable</tracking-on-off>
          </tracking>
          <limit>
            <address-count>10</address-count>
          </limit>
        </policy>
      </device-tracking>
      <line>
        <aux>
          <first>0</first>
          <stopbits>1</stopbits>
        </aux>
        <console>
          <first>0</first>
          <exec-timeout>
            <minutes>0</minutes>
            <seconds>0</seconds>
          </exec-timeout>
          <logging>
            <synchronous/>
          </logging>
          <privilege>
            <level>
              <number>15</number>
            </level>
          </privilege>
          <stopbits>1</stopbits>
        </console>
        <vty>
          <first>0</first>
          <last>4</last>
          <exec-timeout>
            <minutes>180</minutes>
            <seconds>0</seconds>
          </exec-timeout>
          <logging>
            <synchronous/>
          </logging>
          <privilege>
            <level>
              <number>15</number>
            </level>
          </privilege>
        </vty>
        <vty>
          <first>5</first>
          <last>15</last>
          <exec-timeout>
            <minutes>180</minutes>
            <seconds>0</seconds>
          </exec-timeout>
          <logging>
            <synchronous/>
          </logging>
          <privilege>
            <level>
              <number>15</number>
            </level>
          </privilege>
        </vty>
      </line>
      <ntp>
        <server xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-ntp">
          <vrf>
            <name>Mgmt-intf</name>
            <server-list>
              <ip-address>172.26.3.254</ip-address>
            </server-list>
          </vrf>
        </server>
      </ntp>
      <wsma>
        <agent xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-wsma">
          <exec>
            <profile>httplistener</profile>
            <profile>httpslistener</profile>
          </exec>
          <config>
            <profile>httplistener</profile>
            <profile>httpslistener</profile>
          </config>
          <filesys>
            <profile>httplistener</profile>
            <profile>httpslistener</profile>
          </filesys>
          <notify>
            <profile>httplistener</profile>
            <profile>httpslistener</profile>
          </notify>
        </agent>
        <profile xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-wsma">
          <listener>
            <name>httplistener</name>
            <transport>http</transport>
          </listener>
          <listener>
            <name>httpslistener</name>
            <transport>https</transport>
          </listener>
        </profile>
      </wsma>
      <diagnostic xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-diagnostics">
        <bootup>
          <level>minimal</level>
        </bootup>
      </diagnostic>
      <switch xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
        <number>1</number>
        <provision>ws-c3850-12x48u</provision>
      </switch>
      <hw-switch xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-switch">
        <switch>
          <switch-number>
            <switch-number>1</switch-number>
            <logging>
              <onboard>
                <message/>
              </onboard>
            </logging>
          </switch-number>
        </switch>
      </hw-switch>
    </native>
)";

/*
<isis xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-isis">
          <metric-style>
            <transition/>
          </metric-style>
          <domain-password>
            <name>cisco123</name>
          </domain-password>
          <is-type>level-2-only</is-type>
          <log-adjacency-changes/>
          <net>
            <tag>49.0000.0016.0099.0222.00</tag>
          </net>
          <passive-interface>
            <default/>
          </passive-interface>
          <bfd>
            <all-interfaces/>
          </bfd>
        </isis>
<accounting xmlns="http://cisco.com/ns/yang/Cisco-IOS-XE-aaa">
          <dot1x>
            <name>default</name>
            <start-stop>
              <group>dnac-radius-group</group>
            </start-stop>
          </dot1x>
        </accounting>
*/

int main(int argc, char* argv[])
{
    if (argc >= 2)
    {
        string arg = argv[1];

        if ((arg == "-v") || (arg == "--verbose"))
        {
            auto logger = spdlog::stdout_color_mt("ydk");
            logger->set_level(spdlog::level::debug);
        }
    }

    CodecServiceProvider provider{EncodingFormat::XML};
    CodecService codec{};
    path::Codec s{};
    path::NetconfSession session{"jose", "admin", "admin"};
    path::RootSchemaNode& schema = session.get_root_schema();
    auto new_bgp = s.decode(schema, xe_native_payload, ydk::EncodingFormat::XML);

//    auto n = make_shared<Native>();
//    auto decoded_entity = codec.decode(provider, xe_native_payload, n);
//    auto reply = crud.read(provider, n);

//    if(decoded_entity != nullptr) cout << "Create operation success" << endl << endl; else cout << "Operation failed" << endl << endl;

    auto g = make_shared<Native>();
    auto d = codec.decode(provider, giga_payload, g);

}
