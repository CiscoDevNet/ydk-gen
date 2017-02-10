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

#ifndef EXECUTOR_SERVICE_HPP
#define EXECUTOR_SERVICE_HPP

#include <map>
#include <memory>
#include <string>
#include "service.hpp"
#include "netconf_provider.hpp"

namespace ydk
{

namespace core
{
class DataNode;
class ServiceProvider;
}

class Entity;

class ExecutorService : public Service
{
	public:
		ExecutorService();
		bool execute_rpc(NetconfServiceProvider & provider, Entity & entity);
};

}

#endif /* EXECUTOR_SERVICE_HPP */
