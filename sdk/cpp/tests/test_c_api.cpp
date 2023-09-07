/*  ----------------------------------------------------------------
 YDK - YANG Development Kit
 Copyright 2017-2019 Cisco Systems. All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 -------------------------------------------------------------------
 This file has been modified by Yan Gorelik, YDK Solutions.
 All modifications in original under CiscoDevNet domain
 introduced since October 2019 are copyrighted.
 All rights reserved under Apache License, Version 2.0.
 ------------------------------------------------------------------*/

#include <iostream>

#include <ydk/ydk.h>

#include "config.hpp"
#include "catch.hpp"

using namespace std;

const char* test_string="<runner xmlns=\"http://cisco.com/ns/yang/ydktest-sanity\"><ytypes><built-in-t><number8>2</number8></built-in-t></ytypes></runner>";

TEST_CASE( "c_api_codec_encode"  )
{
    YDKStatePtr state = YDKStateCreate();
    Codec c = CodecInit();
    Repository repo = RepositoryInitWithPath(state, "/usr/local/share/ydktest@0.1.1");
    REQUIRE(repo!=NULL);
    ServiceProvider provider = NetconfServiceProviderInit(
        state, repo,
        "localhost", "admin", "admin", 12022, "ssh",
        true, false, -1, "", "");
    REQUIRE(provider!=NULL);

    RootSchemaNode root_schema = ServiceProviderGetRootSchema(state, provider);

    DataNode runner = RootSchemaNodeCreate(state, root_schema, "ydktest-sanity:runner", "");

    DataNodeCreate(state, runner, "ytypes/built-in-t/number8", "2");

    const char* encode_xml = CodecEncode(state, c, runner, XML, 0);
    string s { encode_xml };
    free((void*)encode_xml);
    REQUIRE(s == test_string);

    NetconfServiceProviderFree(provider);
    RepositoryFree(repo);
    CodecFree(c);
    YDKStateFree(state);
}

TEST_CASE( "c_api_codec_decode"  )
{
    YDKStatePtr state = YDKStateCreate();
    Codec c = CodecInit();
    Repository repo = RepositoryInitWithPath(state, "/usr/local/share/ydktest@0.1.1");
    REQUIRE(repo!=NULL);
    ServiceProvider provider = NetconfServiceProviderInit(
        state, repo,
        "localhost", "admin", "admin", 12022, "ssh",
        true, false, -1, "", "");
    REQUIRE(provider!=NULL);

    RootSchemaNode root_schema = ServiceProviderGetRootSchema(state, provider);

    DataNode runner = CodecDecode(state, c, root_schema, test_string, XML);

    REQUIRE(runner!=NULL);

    const char* encode_xml = CodecEncode(state, c, runner, XML, 0);
    string s { encode_xml };
    free((void*)encode_xml);
    REQUIRE(s == test_string);

    NetconfServiceProviderFree(provider);
    RepositoryFree(repo);
    CodecFree(c);
    YDKStateFree(state);
}

TEST_CASE( "c_api_provider_withpath"  )
{
    YDKStatePtr state = YDKStateCreate();
    Repository repo = RepositoryInitWithPath(state, "/usr/local/share/ydktest@0.1.1");
    REQUIRE(repo!=NULL);
    ServiceProvider provider = NetconfServiceProviderInit(
        state, repo,
        "localhost", "admin", "admin", 12022, "ssh",
        true, false, -1, "", "");
    REQUIRE(provider!=NULL);

    NetconfServiceProviderFree(provider);
    RepositoryFree(repo);
    YDKStateFree(state);
}

TEST_CASE( "c_api_provider"  )
{
    YDKStatePtr state = YDKStateCreate();
    Repository repo = RepositoryInit();
    REQUIRE(repo!=NULL);
    ServiceProvider provider = NetconfServiceProviderInit(
        state, repo,
        "localhost", "admin", "admin", 12022, "ssh",
        true, false, -1, "", "");
    REQUIRE(provider!=NULL);

    NetconfServiceProviderFree(provider);
    RepositoryFree(repo);
    YDKStateFree(state);
}

TEST_CASE( "c_api_rpc" )
{
    YDKStatePtr state = YDKStateCreate();
    Codec c = CodecInit();

    Repository repo = RepositoryInitWithPath(state, "/usr/local/share/ydktest@0.1.1");
    REQUIRE(repo!=NULL);
    ServiceProvider provider = NetconfServiceProviderInit(
        state, repo,
        "localhost", "admin", "admin", 12022, "ssh",
        true, false, -1, "", "");
    REQUIRE(provider!=NULL);

    RootSchemaNode root_schema = ServiceProviderGetRootSchema(state, provider);

    DataNode runner = RootSchemaNodeCreate(state, root_schema, "ydktest-sanity:runner", "");

    DataNodeCreate(state, runner, "ytypes/built-in-t/number8", "2");
    const char* create_xml = CodecEncode(state, c, runner, XML, 0);

    Rpc create_rpc = RootSchemaNodeRpc(state, root_schema, "ydk:create");
    DataNode input = RpcInput(state, create_rpc);
    DataNodeCreate(state, input, "entity", create_xml);
    RpcExecute(state, create_rpc, provider);

    Rpc read_rpc = RootSchemaNodeRpc(state, root_schema, "ydk:read");
    input = RpcInput(state, read_rpc);
    DataNode runner_filter = RootSchemaNodeCreate(state, root_schema, "ydktest-sanity:runner", "");
    const char* read_xml = CodecEncode(state, c, runner_filter, XML, 0);

    DataNodeCreate(state, input, "filter", read_xml);
    DataNode read_data = RpcExecute(state, read_rpc, provider);

    free ((void*)create_xml);
    free ((void*)read_xml);
    NetconfServiceProviderFree(provider);
    RepositoryFree(repo);
    CodecFree(c);
    YDKStateFree(state);
}

TEST_CASE( "c_api_netconf_session" )
{
    // Connect to Netconf server
    YDKStatePtr state = YDKStateCreate();
    Repository repo = RepositoryInit();
    REQUIRE(repo!=NULL);
    auto session = NetconfSessionInit(state, repo, "localhost", "admin", "admin", 12022,
                                      "ssh", true, false, -1, "", "");
    REQUIRE(session!=NULL);

    // Get capabilities
    int len = 0;
    auto capabilities = SessionGetCapabilities(state, session, &len);
    CHECK(len > 0);
    cout << "Total capabilities - "<< len << endl;
    cout << capabilities[0] << endl;
    CapabilitiesArrayFree(capabilities, len);

    // Disconnect from Netconf server
    NetconfSessionFree(session);
    RepositoryFree(repo);
    YDKStateFree(state);
}

// Failing in the batch, but passing as individual test
//TEST_CASE( "c_api_restconf_session" )
//{
//    // Connect to Restconf server
//    YDKStatePtr state = YDKStateCreate();
//    Repository repo = RepositoryInitWithPath(state, TEST_HOME);
//    REQUIRE(repo);
//    auto session = RestconfSessionInit(state, repo, "localhost", "admin", "admin", 12306,
//                                       EncodingFormat::JSON, "/data", "/data");
//    REQUIRE(session);
//
//    // Get capabilities
//    int len = 0;
//    auto capabilities = SessionGetCapabilities(state, session, &len);
//    CHECK(len > 0);
//    cout << "Total capabilities - "<< len << endl;
//    cout << capabilities[0] << endl;
//    CapabilitiesArrayFree(capabilities, len);
//
//    // Disconnect from Netconf server
//    RestconfSessionFree(session);
//    RepositoryFree(repo);
//    YDKStateFree(state);
//}
