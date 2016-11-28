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

#define BOOST_TEST_MODULE NetconfServiceTest
#include <boost/test/unit_test.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <string.h>
#include <iostream>

#include "ydk/netconf_provider.hpp"
#include "ydk/netconf_service.hpp"
#include "ydk_ydktest/ydktest_sanity.hpp"
#include "ydk_ydktest/openconfig_bgp.hpp"
#include "config.hpp"
#include "ydk/ietf_netconf.hpp"
#include "ydk/types.hpp"

using namespace ydk;
using namespace std;

struct YdkTest
{
	YdkTest()
    {
    	boost::log::core::get()->set_filter(
    	        boost::log::trivial::severity >= boost::log::trivial::error
    	    );
    }

    ~YdkTest()
    {
    }
};

BOOST_FIXTURE_TEST_SUITE(netconf_service, YdkTest )

// cancel_commit
BOOST_AUTO_TEST_CASE(cancel_commit)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    bool reply = ns.cancel_commit(provider);
    BOOST_REQUIRE(reply);
}

// close_session
BOOST_AUTO_TEST_CASE(close_session)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    bool reply = ns.close_session(provider);
    BOOST_REQUIRE(reply);
}

// commit
BOOST_AUTO_TEST_CASE(commit)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    bool reply = ns.commit(provider);
    BOOST_REQUIRE(reply);
}

// copy_config
BOOST_AUTO_TEST_CASE(copy_config)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;
    DataStore source = DataStore::candidate;

    bool reply = ns.copy_config(provider, target, source);
    BOOST_REQUIRE(reply);
}

// delete_config
BOOST_AUTO_TEST_CASE(delete_config)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::startup;

    bool reply = ns.delete_config(provider, target);
    BOOST_REQUIRE(reply);
}

// discard_changes
BOOST_AUTO_TEST_CASE(discard_changes)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    bool reply = ns.discard_changes(provider);
    BOOST_REQUIRE(reply);
}

// edit_config
BOOST_AUTO_TEST_CASE(edit_config)
{
	// provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;
    openconfig_bgp::Bgp config = {};

    bool reply = ns.edit_config(provider, target, config);
    BOOST_REQUIRE(reply);
}

// get_config
BOOST_AUTO_TEST_CASE(get_config)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore source = DataStore::candidate;
    openconfig_bgp::Bgp filter = {};

    bool reply = ns.get_config(provider, source, filter);
    BOOST_REQUIRE(reply);
}

// get
BOOST_AUTO_TEST_CASE(get)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    openconfig_bgp::Bgp filter = {};

    bool reply = ns.get(provider, filter);
    BOOST_REQUIRE(reply);
}

// kill_session
BOOST_AUTO_TEST_CASE(kill_session)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    std::string session_id = "session-id";

    bool reply = ns.cancel_commit(provider, session_id);
    BOOST_REQUIRE(reply);
}

// lock
BOOST_AUTO_TEST_CASE(lock)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;

    bool reply = ns.lock(provider, target);
    BOOST_REQUIRE(reply);
}

// unlock
BOOST_AUTO_TEST_CASE(unlock)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore target = DataStore::candidate;

    bool reply = ns.unlock(provider, target);
    BOOST_REQUIRE(reply);
}

// validate
BOOST_AUTO_TEST_CASE(validate)
{
    // provider
    ydk::path::Repository repo{TEST_HOME};
    NetconfServiceProvider provider{&repo, "127.0.0.1", "admin", "admin", 12022};
    NetconfService ns{};

    DataStore source = DataStore::candidate;

    bool reply = ns.validate(provider, source);
    BOOST_REQUIRE(reply);
}

BOOST_AUTO_TEST_SUITE_END()
