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
#define TEST_MODULE NetconfClientTest
#include <string.h>
#include "../core/src/netconf_client.hpp"
#include "../core/src/errors.hpp"
#include <iostream>
#include <sys/time.h>
#include "catch.hpp"

using namespace ydk;
using namespace std;
#define NC_VERB_VERBOSE 2

TEST_CASE("xr")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
	int OK = 0;

	int result = client.connect();
	REQUIRE(result == OK);

    struct timeval t1, t2;
    double elapsedTime;

    // start timer
    gettimeofday(&t1, NULL);

	string reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<get-config>"
	 "<source><candidate/></source>"
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


TEST_CASE("Create")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
	int OK = 0;

	int result = client.connect();
	REQUIRE(result == OK);


	REQUIRE(result == OK);
}

TEST_CASE("EditgetConfig")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
	int OK = 0;

	int result = client.connect();
	REQUIRE(result == OK);

	string reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<edit-config>"
	 "<target><candidate/></target>"
	 "<config>"
	 "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><number8>12</number8></built-in-t></ytypes></runner>"
	 "</config>"
	 "</edit-config>"
	 "</rpc>");
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
	REQUIRE(NULL != strstr(reply.c_str(), "<number8>12</number8>"));

	reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<edit-config>"
	 "<target><candidate/></target>"
	 "<config>"
	 "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\" operation=\"delete\"/>"
	 "</config>"
	 "</edit-config>"
	 "</rpc>");
	REQUIRE(NULL != strstr(reply.c_str(), "<ok/>"));


	REQUIRE(result == OK);
}

TEST_CASE("Validate")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
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

TEST_CASE("LockUnlock")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
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

TEST_CASE("RpcError")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
	int OK = 0;

	int result = client.connect();
	REQUIRE(result == OK);
	string reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<edit-config>"
	 "<target><candidate/></target>"
	 "<config>"
	 "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><number8>aaa</number8></built-in-t></ytypes></runner>"
	 "</config>"
	 "</edit-config>"
	 "</rpc>");
	REQUIRE(NULL != strstr(reply.c_str(), "<rpc-error>"));


	REQUIRE(result == OK);
}

TEST_CASE("DeviceNotConnectedExecute")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
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
	catch (YCPPError & e)
	{
		REQUIRE(e.err_msg=="YCPPClientError: Could not execute payload. Not connected to 127.0.0.1");
	}

}


TEST_CASE("RpcInvalid")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
	int OK = 0;

	int result = client.connect();
	REQUIRE(result == OK);

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
	catch (YCPPError & e)
	{
		REQUIRE(e.err_msg=="YCPPClientError: Could not build payload");
	}


	REQUIRE(result == OK);
}

TEST_CASE("WrongXml")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
	int OK = 0;

	int result = client.connect();
	REQUIRE(result == OK);

	try
	{
		string reply = client.execute_payload(
				"<testing>"
		 );
		REQUIRE(reply== "");
	}
	catch (YCPPError & e)
	{
		REQUIRE(e.err_msg=="YCPPClientError: Could not build payload");
	}


	REQUIRE(result == OK);
}

TEST_CASE("CorrectXmlWrongRpc")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
	int OK = 0;

	int result = client.connect();
	REQUIRE(result == OK);

	try
	{
		string reply = client.execute_payload(
				"<testing/>"
		);
		REQUIRE(reply== "");
	}
	catch (YCPPError & e)
	{
		REQUIRE(e.err_msg=="YCPPClientError: Could not build payload");
	}



	REQUIRE(result == OK);
}

TEST_CASE("EmptyRpc")
{
	NetconfClient client{"admin", "admin", "127.0.0.1", 12022};
	int OK = 0;

	int result = client.connect();
	REQUIRE(result == OK);

	try
	{
		string reply = client.execute_payload("");
		REQUIRE(reply== "");
	}
	catch (YCPPError & e)
	{
		REQUIRE(e.err_msg=="YCPPClientError: Could not build payload");
	}


	REQUIRE(result == OK);
}

TEST_CASE("MultipleClients")
{
	NetconfClient client1{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client2{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client3{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client4{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client5{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client6{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client7{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client8{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client9{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client10{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client11{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client12{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client13{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client14{"admin", "admin", "127.0.0.1", 12022};
	NetconfClient client15{"admin", "admin", "127.0.0.1", 12022};

	int result = client1.connect() && client2.connect() && client3.connect() && client4.connect() && client5.connect()
		 && client6.connect() && client7.connect() && client8.connect() && client9.connect() && client10.connect()
		 && client11.connect() && client12.connect() && client13.connect() && client14.connect() && client15.connect();
	REQUIRE(result == 0);

}

