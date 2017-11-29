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
#include "../core/src/netconf_provider.hpp"
#include "../core/src/errors.hpp"
#include <iostream>
#include "config.hpp"
#include "catch.hpp"
using namespace ydk;
using namespace std;



TEST_CASE("CreateP")
{
    ydk::path::Repository repo{};
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", "admin", 12022};

    CHECK_NOTHROW(provider.get_encoding());
}

TEST_CASE("CreateNoRepoP")
{
    NetconfServiceProvider provider{ "127.0.0.1", "admin", "admin", 12022};
    CHECK_NOTHROW(provider.get_encoding());
}

// NOTE: The ./ ensures this test has to be explicitly run by name
TEST_CASE("./CreateNoRepoPTCP")
{
    NetconfServiceProvider provider{ "127.0.0.1", "admin", "admin", 12307, "tcp"};
    CHECK_NOTHROW(provider.get_encoding());
}

TEST_CASE("get_capabilities")
{
    NetconfServiceProvider provider{ "127.0.0.1", "admin", "admin", 12022};
    CHECK_NOTHROW(provider.get_capabilities());
}

TEST_CASE("CreateKeybaseAuthRepo_ConfD")
{
    ydk::path::Repository repo{};
    const string& private_key_path = "../ssh_host_rsa_key";
    const string& public_key_path = "../ssh_host_rsa_key.pub";
    NetconfServiceProvider provider{repo, "127.0.0.1", "admin", private_key_path, public_key_path, 12022};
    CHECK_NOTHROW(provider.get_encoding());
}

