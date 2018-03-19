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


#include "path_private.hpp"


/////////////////////////////////////////////////////////////////////////////
/// Capability
/////////////////////////////////////////////////////////////////////////////

ydk::path::Capability::Capability(const std::string& mod, const std::string& rev): module{mod}, revision{rev}
{

}

ydk::path::Capability::Capability(const std::string& mod, const std::string& rev, const std::vector<std::string>& f,
           const std::vector<std::string>& d): module{mod}, revision{rev}, features{f}, deviations({d})
{

}

ydk::path::Capability::Capability(const Capability& cap): module{cap.module}, revision{cap.revision}, features{cap.features}, deviations{cap.deviations}
{

}

ydk::path::Capability::Capability(ydk::path::Capability&& cap): module{std::move(cap.module)}, revision{std::move(cap.revision)},
features{std::move(cap.features)}, deviations{std::move(cap.deviations)}
{

}

ydk::path::Capability&
ydk::path::Capability::operator=(const ydk::path::Capability& cap)
{
    module = cap.module;
    revision = cap.revision;
    features = cap.features;
    deviations = cap.deviations;

    return *this;
}


ydk::path::Capability&
ydk::path::Capability::operator=(ydk::path::Capability&& cap)
{
    module = std::move(cap.module);
    revision = std::move(cap.revision);
    features = std::move(cap.features);
    deviations = std::move(cap.deviations);

    return *this;
}

bool
ydk::path::Capability::operator==(const ydk::path::Capability& cap)
{
    if( cap.module != module || cap.revision != revision ) {
        return false;
    }

    if (cap.features.size() != features.size()){
        return false;
    } else if(cap.features.size() != 0){

        //sort and compare
        std::vector<std::string> cap_features{cap.features};
        std::sort(cap_features.begin(), cap_features.end());

        //sort ourselves
        std::sort(features.begin(), features.end());

        if(cap_features != features) {
            return false;
        }

    }

    if (cap.deviations.size() != deviations.size()) {
        return false;
    } else if(cap.deviations.size() != 0){

        //sort and compare
        std::vector<std::string> cap_deviations{cap.deviations};
        std::sort(cap_deviations.begin(), cap_deviations.end());

        //sort ourselves
        std::sort(deviations.begin(), deviations.end());

        if(cap_deviations != deviations) {
            return false;
        }

    }

    return true;
}

namespace ydk
{
namespace path
{
std::ostream& operator<< (std::ostream& stream, const Capability& cap)
{
    stream<<cap.module<<"@"<<cap.revision;
    return stream;
}
}
}