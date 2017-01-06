/// YANG Development Kit
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

#include "entity_data_node_walker.hpp"
#include "path_api.hpp"
#include "service.hpp"
#include "validation_service.hpp"
#include <map>
#include <boost/log/trivial.hpp>

using namespace std;

namespace ydk {

void
ValidationService::validate(const path::ServiceProvider& sp, Entity& entity,
                            ValidationService::Option option)
{
	auto root_schema = sp.get_root_schema();
	path::DataNode * datanode = get_data_node_from_entity(entity, *root_schema);

	path::ValidationService path_validation_service{};
    path_validation_service.validate(*datanode, option);

}
}
