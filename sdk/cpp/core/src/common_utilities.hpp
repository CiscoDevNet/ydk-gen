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

#ifndef YDK_UTILITIES
#define YDK_UTILITIES

#include <string>
#include <vector>

#include "logger.hpp"
#include "service_provider.hpp"

namespace ydk
{
    std::string trim(const std::string& str);
    bool replace(std::string& subject, const std::string& search, const std::string& replace);
    std::string entity_vector_to_string(std::vector<Entity*>& v);

    bool has_xml_escape_sequences(const std::string& xml);
    std::string replace_xml_escape_sequences(const std::string& xml);

    std::shared_ptr<Entity> get_top_entity_from_filter(Entity & filter);
    std::shared_ptr<Entity> read_datanode(Entity & filter, std::shared_ptr<path::DataNode> read_data_node);
    std::string get_data_payload(Entity & entity, const ServiceProvider & provider);
    std::string get_xml_subtree_filter_payload(Entity & entity, const ServiceProvider & provider);

    std::vector<std::string> get_union(std::vector<std::string> & v1, std::vector<std::string> & v2);
}

#endif /* YDK_UTILITIES */
