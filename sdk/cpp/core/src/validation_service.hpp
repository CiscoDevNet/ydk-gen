//
// @file validation_service.hpp
// @brief The entity validation service.
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

#ifndef VALIDATION_SERVICE_HPP
#define VALIDATION_SERVICE_HPP

#include <string>
#include <memory>
#include "service.hpp"
#include "path_api.hpp"
#include "types.hpp"

namespace ydk {


class EntityDiagnostic : public ydk::path::DiagnosticNode<Entity*, ydk::path::ValidationError>
{
  public:

    std::vector<DiagnosticNode<std::string, ydk::path::ValidationError>> attrs;

    bool has_errors()
    {
        if(!errors.empty() || !attrs.empty()){
            return true;
        }

        for(auto c : children) {
            if(c.has_errors()){
                return true;
            }
        }
        return false;
    }
};




class ValidationService {
  public:

    ///
    /// @brief Options for validation.
    ///
    /// All validation is performed in the context of some operation.
    /// These options capture the context of use.
    ///
    enum class Option {
        DATASTORE,  /// Datastore validation. Note the DataNode Tree should contain everything for cross reference resolution
        GET_CONFIG, // Get config validation. Checks to see if only config nodes are references
        GET, // Get validation
        EDIT_CONFIG // Edit validation. Checks on the values of leafs etc
    };

    ValidationService()
    {
    }
    ~ValidationService(){}

    EntityDiagnostic validate(const ydk::path::ServiceProvider& sp, ydk::Entity& entity, ydk::ValidationService::Option option);
};


}

#endif /* VALIDATION_SERVICE_HPP */
