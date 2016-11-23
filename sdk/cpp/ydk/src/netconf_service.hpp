//
// @file crud_service.hpp
// @brief The main ydk public header.
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

#ifndef NETCONF_SERVICE_HPP
#define NETCONF_SERVICE_HPP

#include <map>
#include <memory>
#include <string>
#include "service.hpp"
#include "netconf_provider.hpp"
#include "types.hpp"

namespace ydk
{

namespace core
{
class DataNode;
class ServiceProvider;
}

class Entity;

enum class DataStore {
	candidate,
	running,
	startup,
	url,
	config
};

class NetconfService : public Service
{
	public:
		NetconfService();
		bool cancel_commit(NetconfServiceProvider & provider, std::string persist_id = "");
		bool close_session(NetconfServiceProvider & provider);
		bool commit(NetconfServiceProvider & provider, std::string confirmed = "", 
			std::string confirm_timeout = "", std::string persist = "", std::string persist_id = "");
		bool copy_config(NetconfServiceProvider & provider, DataStore target, DataStore source);
		bool copy_config(NetconfServiceProvider & provider, DataStore target, Entity& source);
		bool delete_config(NetconfServiceProvider & provider, DataStore target);
		bool discard_changes(NetconfServiceProvider & provider);
		bool edit_config(NetconfServiceProvider & provider, DataStore target, Entity& config,
			std::string default_operation = "", std::string test_option = "", std::string error_option = "");
		bool get_config(NetconfServiceProvider & provider, DataStore source, Entity& filter);
		bool get(NetconfServiceProvider & provider, Entity& filter);
		bool kill_session(NetconfServiceProvider & provider, int session_id); //convert session_id to string
		bool lock(NetconfServiceProvider & provider, DataStore target);
		bool unlock(NetconfServiceProvider & provider, DataStore target);
		bool validate(NetconfServiceProvider & provider, DataStore source);
		bool validate(NetconfServiceProvider & provider, Entity& source);
};

}

#endif /* NETCONF_SERVICE_HPP */