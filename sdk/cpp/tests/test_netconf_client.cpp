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

TEST_FIXTURE(TestClient, Error)
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

int main(int, const char *[])
{
	return UnitTest::RunAllTests();
}
