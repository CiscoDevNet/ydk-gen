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
// KIND, either express or ied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//////////////////////////////////////////////////////////////////

#include "path_private.hpp"
#include "../logger.hpp"

namespace ydk
{
namespace path
{
////////////////////////////////////////////////////////////////////////
/// DataNodeCollection
////////////////////////////////////////////////////////////////////////

static std::string trim_first_character(std::string path)
{
    if(path.size() >= 2)
        path = path.substr(1);
    return path;
}

DataNodeCollection::DataNodeCollection()
{
}

DataNodeCollection::DataNodeCollection(const std::vector<std::shared_ptr<DataNode>> & data)
{
    for(auto d : data)
    {
        if(d != nullptr)
        {
            std::string path = trim_first_character(d->get_path());
            YLOG_DEBUG("Storing datanode in DataNodeCollection with path '{}'", path);
            data_nodes[path] = d;
        }
    }
}

DataNodeCollection::DataNodeCollection(const std::vector<DataNode*> & data)
{
    for(auto d : data)
    {
        if(d != nullptr)
        {
            std::string path = trim_first_character(d->get_path());
            YLOG_DEBUG("Storing datanode in DataNodeCollection with path '{}'", path);
            data_nodes[path] = std::make_shared<path::DataNodeImpl>(dynamic_cast<path::DataNodeImpl*>(d));
        }
    }
}

DataNodeCollection::DataNodeCollection(const std::map<std::string, std::shared_ptr<DataNode>> & data_nodes)
    : data_nodes(data_nodes)
{
}

DataNodeCollection::~DataNodeCollection()
{
}

std::map<std::string, std::shared_ptr<path::DataNode>> DataNodeCollection::get_data_nodes() const
{
    return data_nodes;
}

}
}
