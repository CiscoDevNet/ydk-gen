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
#include <string.h>
#include "ydk/path_api.hpp"
#include "ydk/restconf_provider.hpp"
#include "../core/src/errors.hpp"
#include <iostream>
#include "config.hpp"
#include "catch.hpp"

using namespace ydk;
using namespace std;


TEST_CASE("CreateDelRead")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::RestconfServiceProvider provider{repo, "localhost", "admin", "admin", 12306, EncodingFormat::JSON};

    ydk::path::RootSchemaNode& schema = provider.get_session().get_root_schema();

    ydk::path::Codec s{};

    auto & runner = schema.create_datanode("ydktest-sanity:runner", "");

    //first delete
    std::shared_ptr<ydk::path::Rpc> delete_rpc { schema.create_rpc("ydk:delete") };
    auto json = s.encode(runner, EncodingFormat::JSON, false);
    delete_rpc->get_input_node().create_datanode("entity", json);
    //call delete
    (*delete_rpc)(provider.get_session());

    auto & number8 = runner.create_datanode("ytypes/built-in-t/number8", "3");

    json = s.encode(runner, EncodingFormat::JSON, false);
    CHECK( !json.empty());
    //call create
    std::shared_ptr<ydk::path::Rpc> create_rpc { schema.create_rpc("ydk:create") };
    create_rpc->get_input_node().create_datanode("entity", json);
    (*create_rpc)(provider.get_session());

    //read
    std::shared_ptr<ydk::path::Rpc> read_rpc { schema.create_rpc("ydk:read") };
    auto & runner_read = schema.create_datanode("ydktest-sanity:runner", "");

    json = s.encode(runner_read, EncodingFormat::JSON, false);
    REQUIRE( !json.empty() );
    read_rpc->get_input_node().create_datanode("filter", json);

    auto read_result = (*read_rpc)(provider.get_session());

    runner = schema.create_datanode("ydktest-sanity:runner", "");
    number8 = runner.create_datanode("ytypes/built-in-t/number8", "5");

    json = s.encode(runner, EncodingFormat::JSON, false);
    CHECK( !json.empty());
    //call update
    std::shared_ptr<ydk::path::Rpc> update_rpc { schema.create_rpc("ydk:update") };
    update_rpc->get_input_node().create_datanode("entity", json);
    (*update_rpc)(provider.get_session());


}

TEST_CASE("ActionRest")
{
    ydk::path::Repository repo{TEST_HOME};
    ydk::RestconfServiceProvider provider{repo, "localhost", "admin", "admin", 12306, EncodingFormat::JSON};

    ydk::path::RootSchemaNode& schema = provider.get_session().get_root_schema();

    ydk::path::Codec s{};

    auto & runner = schema.create_datanode("ydktest-sanity-action:data");
    REQUIRE_THROWS_AS((runner)(provider.get_session()), ydk::YOperationNotSupportedError);
}
