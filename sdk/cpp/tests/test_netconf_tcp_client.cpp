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


#define TEST_MODULE NetconfTCPClientTest
#include <string.h>
#include "../core/src/netconf_tcp_client.hpp"
#include "../core/src/errors.hpp"
#include <iostream>
#include <sys/time.h>
#include "catch.hpp"

using namespace ydk;
using namespace std;
#define NC_VERB_VERBOSE 2

// NOTE: The ./ ensures this test has to be explicitly run by name
TEST_CASE("tcp_xr")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int OK = 0;

    int result = client.connect();
    REQUIRE(result == OK);

    struct timeval t1, t2;
    double elapsedTime;

    // start timer
    gettimeofday(&t1, NULL);

    client.execute_payload(
     "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
     "<get-config>"
     "<source><running/></source>"
     "</get-config>"
     "</rpc>");


    // stop timer
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cout << elapsedTime << " ms.\n";

    REQUIRE(result == OK);
}


TEST_CASE("tcp_create")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int OK = 0;

    int result = client.connect();
    REQUIRE(result == OK);


    REQUIRE(result == OK);
}

TEST_CASE("tcp_edit_get_config")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int OK = 0;

    int result = client.connect();
    REQUIRE(result == OK);

    std::string reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "  <edit-config>"
        "    <target>"
        "      <candidate />"
        "    </target>"
        "    <error-option>rollback-on-error</error-option>"
        "    <config>"
        "      <runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\" xmlns:nc=\"urn:ietf:params:xml:ns:netconf:base:1.0\" nc:operation=\"merge\">"
        "        <two-list>"
        "          <ldata>"
        "            <number>21</number>"
        "            <name>runner:twolist:ldata[21]:name</name>"
        "            <subl1>"
        "              <number>211</number>"
        "              <name>runner:twolist:ldata[21]:subl1[211]:name</name>"
        "            </subl1>"
        "            <subl1>"
        "              <number>212</number>"
        "              <name>runner:twolist:ldata[21]:subl1[212]:name</name>"
        "            </subl1>"
        "          </ldata>"
        "          <ldata>"
        "            <number>22</number>"
        "            <name>runner:twolist:ldata[22]:name</name>"
        "            <subl1>"
        "              <number>221</number>"
        "              <name>runner:twolist:ldata[22]:subl1[221]:name</name>"
        "            </subl1>"
        "            <subl1>"
        "              <number>222</number>"
        "              <name>runner:twolist:ldata[22]:subl1[222]:name</name>"
        "            </subl1>"
        "          </ldata>"
        "        </two-list>"
        "      </runner>"
        "    </config>"
        "  </edit-config>"
        "</rpc>"
        );
    REQUIRE(NULL != strstr(reply.c_str(), "<ok/>"));

    reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "  <commit/>"
        "</rpc>"
    );
    REQUIRE(NULL != strstr(reply.c_str(), "<ok/>"));

    reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "<get-config>"
        "<source><candidate/></source>"
        "<filter>"
        "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"/>"
        "</filter>"
        "</get-config>"
        "</rpc>");
    REQUIRE(NULL != strstr(reply.c_str(), "<name>runner:twolist:ldata[22]:subl1[221]:name</name>"));

    reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "<edit-config>"
        "<target><candidate/></target>"
        "<config xmlns:xc=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\" xc:operation=\"delete\"/>"
        "</config>"
        "</edit-config>"
        "</rpc>");
    REQUIRE(NULL != strstr(reply.c_str(), "<ok/>"));
}

TEST_CASE("tcp_validate")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int OK = 0;

    int result = client.connect();
    REQUIRE(result == OK);

    string reply = client.execute_payload(
     "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
     "<validate>"
     "<source><candidate/></source>"
     "</validate>"
     "</rpc>");

    REQUIRE(NULL != strstr(reply.c_str(), "<ok/>"));


    REQUIRE(result == OK);
}

TEST_CASE("tcp_lock_unlock")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int OK = 0;

    int result = client.connect();
    REQUIRE(result == OK);

    string reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "<discard-changes/>"
        "</rpc>");
    REQUIRE(NULL != strstr(reply.c_str(), "<ok/>"));

    reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "<lock>"
        "<target><candidate/></target>"
        "</lock>"
        "</rpc>");
    REQUIRE(NULL != strstr(reply.c_str(), "<ok/>"));

    reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "<unlock>"
        "<target><candidate/></target>"
        "</unlock>"
        "</rpc>");
    REQUIRE(NULL != strstr(reply.c_str(), "<ok/>"));

    reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "<unlock>"
        "<target><candidate/></target>"
        "</unlock>"
        "</rpc>");
    REQUIRE(NULL != strstr(reply.c_str(), "<rpc-error>"));

    REQUIRE(result == OK);
}

TEST_CASE("./tcp_rpc_error")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int OK = 0;

    int result = client.connect();
    REQUIRE(result == OK);

    string reply = client.execute_payload(
        "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
        "  <edit-config>"
        "    <target>"
        "      <candidate />"
        "    </target>"
        "    <error-option>rollback-on-error</error-option>"
        "    <config>"
        "      <runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\" xmlns:nc=\"urn:ietf:params:xml:ns:netconf:base:1.0\" nc:operation=\"merge\">"
        "        <two-list>"
        "          <ldata>"
        "            <number>21</number>"
        "            <name>runner:twolist:ldata[21]:name</name>"
        "            <subl1>"
        "              <number>WRONG VALUE</number>"
        "              <name>runner:twolist:ldata[21]:subl1[211]:name</name>"
        "            </subl1>"
        "            <subl1>"
        "              <number>212</number>"
        "              <name>runner:twolist:ldata[21]:subl1[212]:name</name>"
        "            </subl1>"
        "          </ldata>"
        "          <ldata>"
        "            <number>22</number>"
        "            <name>runner:twolist:ldata[22]:name</name>"
        "            <subl1>"
        "              <number>221</number>"
        "              <name>runner:twolist:ldata[22]:subl1[221]:name</name>"
        "            </subl1>"
        "            <subl1>"
        "              <number>222</number>"
        "              <name>runner:twolist:ldata[22]:subl1[222]:name</name>"
        "            </subl1>"
        "          </ldata>"
        "        </two-list>"
        "      </runner>"
        "    </config>"
        "  </edit-config>"
        "</rpc>"
        );
    REQUIRE(NULL != strstr(reply.c_str(), "<rpc-error>"));

    REQUIRE(result == OK);
}

TEST_CASE("tcp_device_not_connected_execute")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    try
    {
        string s = client.execute_payload(
                 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
                 "<edit-config>"
                 "<target><candidate/></target>"
                 "<config>"
                 "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><number8>aaa</number8></built-in-t></ytypes></runner>"
                 "</config>"
                 "</edit-config>"
                 "</rpc>");
        REQUIRE(s== "");
    }
    catch (YError & e)
    {
        REQUIRE(e.err_msg=="YClientError: Could not execute payload. Not connected to 127.0.0.1");
    }

}

TEST_CASE("tcp_rpc_invalid")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int ok = 0;

    int result = client.connect();
    REQUIRE(result == ok);

    try
    {
        string reply = client.execute_payload(
             "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
             "<lock>"
             "<source><candidate/></source>"
             "</lock>"
             "</rpc>");
        REQUIRE(NULL != strstr(reply.c_str(), ""));
    }
    catch (YError & e)
    {
        REQUIRE(e.err_msg=="YClientError: could not build payload");
    }

    REQUIRE(result == ok);
}

// TCP Client does not validate any input
// TEST_CASE("./tcp_wrong_xml")
// {
//     NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
//     int OK = 0;

//     int result = client.connect();
//     REQUIRE(result == OK);

//     try
//     {
//         string reply = client.execute_payload(
//                 "<testing>"
//          );
//         REQUIRE(reply== "");
//     }
//     catch (YError & e)
//     {
//         REQUIRE(e.err_msg=="YClientError: Could not build payload");
//     }


//     REQUIRE(result == OK);
// }

TEST_CASE("tcp_correct_xml_wrong_rpc")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int OK = 0;

    int result = client.connect();
    REQUIRE(result == OK);

    string reply = client.execute_payload(
            "<testing/>"
    );
    REQUIRE(NULL != strstr(reply.c_str(), "<rpc-error>"));

    REQUIRE(result == OK);
}

TEST_CASE("tcp_empty_rpc")
{
    NetconfTCPClient client{"admin", "admin", "127.0.0.1", 12307};
    int OK = 0;

    int result = client.connect();
    REQUIRE(result == OK);

    try
    {
        string reply = client.execute_payload("");
        REQUIRE(reply== "");
    }
    catch (YError & e)
    {
        REQUIRE(e.err_msg=="YClientError: Could not build payload");
    }
    REQUIRE(result == OK);
}

// Timeout error
TEST_CASE("tcp_multiple_clients")
{
    NetconfTCPClient client1{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client2{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client3{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client4{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client5{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client6{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client7{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client8{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client9{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client10{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client11{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client12{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client13{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client14{"admin", "admin", "127.0.0.1", 12307};
    NetconfTCPClient client15{"admin", "admin", "127.0.0.1", 12307};

    int result = client1.connect() && client2.connect() && client3.connect() && client4.connect() && client5.connect()
         && client6.connect()
         && client7.connect() && client8.connect() && client9.connect() && client10.connect()
         && client11.connect() && client12.connect() && client13.connect() && client14.connect() && client15.connect();
    REQUIRE(result == 0);

}
