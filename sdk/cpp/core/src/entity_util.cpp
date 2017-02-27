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

#include <algorithm>
#include <vector>

#include "entity_util.hpp"
#include "errors.hpp"

using namespace std;

namespace ydk
{

std::string get_relative_entity_path(const Entity* current_node, const Entity* ancestor, std::string path)
{
    std::ostringstream path_buffer;
    path_buffer << path;
    if(ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor should not be null."});
    }
    auto p = current_node->parent;
    std::vector<Entity*> parents {};
    while (p != nullptr && p != ancestor) {
        parents.push_back(p);
        p = p->parent;
    }

    if (p == nullptr) {
        throw(YCPPInvalidArgumentError{"parent is not in the ancestor hierarchy."});
    }

    std::reverse(parents.begin(), parents.end());

    p = nullptr;
    for (auto p1 : parents) {
        if (p) {
            path_buffer << "/";
        } else {
             p = p1;
        }
        path_buffer << p1->get_segment_path();
    }
    if(p)
        path_buffer << "/";
    path_buffer<<current_node->get_segment_path();
    return path_buffer.str();

}

bool is_set(const EditOperation & operation)
{
    return operation != EditOperation::not_set;
}

}
