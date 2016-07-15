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
#include <unittest++/UnitTest++.h>
#include <string.h>
#include "ydk/netconf_client.h"
#include "ydk/make_unique.h"
#include <iostream>
using namespace ydk;
using namespace std;
#define NC_VERB_VERBOSE 2

class TestClient
{
public:
	TestClient(){}
	~TestClient(){}

public:
	NetconfClient client{ "admin", "admin", "127.0.0.1", 12022, 0};
};

TEST_FIXTURE(TestClient, Create)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST_FIXTURE(TestClient, EditgetConfig)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);

	string reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<edit-config>"
	 "<target><candidate/></target>"
	 "<config>"
	 "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><number8>12</number8></built-in-t></ytypes></runner>"
	 "</config>"
	 "</edit-config>"
	 "</rpc>");
	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), "<ok/>"));

	reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<get-config>"
	 "<source><candidate/></source>"
	 "<filter>"
	 "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"/>"
	 "</filter>"
	 "</get-config>"
	 "</rpc>");
	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), "<number8>12</number8>"));

	reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<edit-config>"
	 "<target><candidate/></target>"
	 "<config>"
	 "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\" operation=\"delete\"/>"
	 "</config>"
	 "</edit-config>"
	 "</rpc>");
	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), "<ok/>"));

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST_FIXTURE(TestClient, Validate)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);

	string reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<validate>"
	 "<source><candidate/></source>"
	 "</validate>"
	 "</rpc>");

	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), "<ok/>"));

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST_FIXTURE(TestClient, LockUnlock)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);

	string reply = client.execute_payload(
		 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
		 "<lock>"
		 "<target><candidate/></target>"
		 "</lock>"
		 "</rpc>");

	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), "<ok/>"));

	reply = client.execute_payload(
		 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
		 "<unlock>"
		 "<target><candidate/></target>"
		 "</unlock>"
		 "</rpc>");

	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), "<ok/>"));

	reply = client.execute_payload(
		 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
		 "<unlock>"
		 "<target><candidate/></target>"
		 "</unlock>"
		 "</rpc>");

	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), "<rpc-error>"));

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST_FIXTURE(TestClient, RpcError)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);
	string reply = client.execute_payload(
	 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
	 "<edit-config>"
	 "<target><candidate/></target>"
	 "<config>"
	 "<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><number8>aaa</number8></built-in-t></ytypes></runner>"
	 "</config>"
	 "</edit-config>"
	 "</rpc>");
	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), "<rpc-error>"));

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST_FIXTURE(TestClient, DeviceNotConnectedClose)
{
	int result = client.close();
//	CHECK_EQUAL(1, 0!=client.get_status());
	CHECK_EQUAL(1, result);
}

TEST_FIXTURE(TestClient, DeviceNotConnectedExecute)
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
	CHECK_EQUAL(s, "");
//	CHECK_EQUAL(1, 0!=client.get_status());
}


TEST_FIXTURE(TestClient, RpcInvalid)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);

	string reply = client.execute_payload(
		 "<rpc xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
		 "<lock>"
		 "<source><candidate/></source>"
		 "</lock>"
		 "</rpc>");

	CHECK_EQUAL(1, NULL != strstr(reply.c_str(), ""));
//	CHECK_EQUAL(1, OK!=client.get_status());

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST_FIXTURE(TestClient, WrongXml)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);

	string reply = client.execute_payload(
	 "<testing>"
	 );
	CHECK_EQUAL(reply, "");
//	CHECK_EQUAL(1, OK!=client.get_status());

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST_FIXTURE(TestClient, CorrectXmlWrongRpc)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);

	string reply = client.execute_payload(
	 "<testing/>"
	 );
	CHECK_EQUAL(reply, "");
//	CHECK_EQUAL(1, OK!=client.get_status());

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST_FIXTURE(TestClient, EmptyRpc)
{
	int OK = 0;

	int result = client.connect();
	CHECK_EQUAL(result, OK);

	string reply = client.execute_payload("");
	CHECK_EQUAL(reply, "");
//	CHECK_EQUAL(1, OK!=client.get_status());

	result = client.close();
	CHECK_EQUAL(result, OK);
}

TEST(MultipleClients)
{
	NetconfClient client1{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client2{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client3{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client4{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client5{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client6{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client7{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client8{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client9{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client10{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client11{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client12{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client13{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client14{ "admin", "admin", "127.0.0.1", 12022, 0};
	NetconfClient client15{ "admin", "admin", "127.0.0.1", 12022, 0};

	int result = client1.connect() && client2.connect() && client3.connect() && client4.connect() && client5.connect()
		 && client6.connect() && client7.connect() && client8.connect() && client9.connect() && client10.connect()
		 && client11.connect() && client12.connect() && client13.connect() && client14.connect() && client15.connect();
	CHECK_EQUAL(result, 0);

	result = client1.close() && client2.close() && client3.close() && client4.close() && client5.close()
			 && client6.close() && client7.close() && client8.close() && client9.close() && client10.close()
			 && client11.close() && client12.close() && client13.close() && client14.close() && client15.close();
	CHECK_EQUAL(result, 0);
}

int main(int, const char *[])
{
	return UnitTest::RunAllTests();
}
