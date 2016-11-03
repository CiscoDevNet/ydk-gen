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


#include "core_private.hpp"
#include <boost/log/trivial.hpp>

///////////////////////////////////////////////////////////////////////////////////////////
/// SchemaValueType
///////////////////////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueType::~SchemaValueType()
{

}

/////////////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueBinaryType
/////////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueBinaryType::SchemaValueBinaryType(): length{Range<uint64_t>{0,18446744073709551615UL}}
{

}



ydk::core::SchemaValueBinaryType::~SchemaValueBinaryType()
{

}


ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueBinaryType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty()){
        diag.errors.push_back(ydk::core::ValidationError::INVALATTR);
    }
    return diag;
};



///////////////////////////////////////////////////////////////////////////////////
/// ydk::core::SchemaValueBitsType::Bit
//////////////////////////////////////////////////////////////////////////////////
ydk::core::SchemaValueBitsType::Bit::Bit(std::string m_name, uint32_t m_pos) : name{m_name} ,
pos{m_pos} {

}


namespace ydk {
    namespace core {
        static std::vector<std::string> split(const std::string &s, char delim) {
            std::stringstream ss(s);
            std::string item;
            std::vector<std::string> tokens;
            while (std::getline(ss, item, delim)) {
                tokens.push_back(item);
            }
            return tokens;
        }
    }
}


////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueBitsType
////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueBitsType::~SchemaValueBitsType()
{

}

ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueBitsType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty())
    {
        BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueBits";
        diag.errors.push_back(ydk::core::ValidationError::INVALATTR);
    } else {
        //tokenize and make sure all tokens are accounted for
        auto tokens = ydk::core::split(value, ' ');
        std::map<std::string, ydk::core::SchemaValueBitsType::Bit*> name_bit_map{};
        for(auto bit : bits){
            name_bit_map.insert(std::make_pair(bit.name,&bit));
        }
        for(auto token : tokens) {
            if(name_bit_map.find(token) == name_bit_map.end()){
                BOOST_LOG_TRIVIAL(error) << "Invalid bits value " << value;
                diag.errors.push_back(ydk::core::ValidationError::INVALATTR);
            }
        }

    }
    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueDec64Type
////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueDec64Type::~SchemaValueDec64Type()
{

}


ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueDec64Type::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueDec64Type";
        diag.errors.push_back(ydk::core::ValidationError::INVALATTR);
    }
    return diag;
}

//////////////////////////////////////////////////////////////////////
/// ydk::core::SchemaValueEnumerationType::Enum
/////////////////////////////////////////////////////////////////////
ydk::core::SchemaValueEnumerationType::Enum::Enum(std::string m_name, int32_t m_value) : name{m_name}, value{m_value}
{

}



////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueEnumerationType
////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueEnumerationType::~SchemaValueEnumerationType()
{

}

ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueEnumerationType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty()){
         BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueEnumerationType";
        diag.errors.push_back(ydk::core::ValidationError::INVALATTR);

    } else {

        for(auto e : enums){
            if(e.name == value){
                return diag;
            }
        }
    }
     BOOST_LOG_TRIVIAL(error) << "Invalid enum value " << value;
    diag.errors.push_back(ydk::core::ValidationError::INVALATTR);

    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueIdentityType
////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueIdentityType::~SchemaValueIdentityType()
{
    for(auto identity : derived) {
        delete identity;
    }
}

ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueIdentityType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty()){
         BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueIdentityType";
        diag.errors.push_back(ydk::core::ValidationError::INVALATTR);
        return diag;
    }

    auto tokens = ydk::core::split(value, ':');
    if(tokens.size() == 1) {
        //no module name just compare the name
        if(tokens[0] == name) {
            return diag;
        }
    } else if(tokens[0] == module_name && tokens[1] == name){
            return diag;
    }

    for(auto ident : derived) {
        if(!ident->validate(value).has_errors()){
            return diag;
        }
    }

    BOOST_LOG_TRIVIAL(error) << "Invalid identity" << value;
    diag.errors.push_back(ydk::core::ValidationError::INVALID_IDENTITY);
    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueInstanceIdType
////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueInstanceIdType::~SchemaValueInstanceIdType()
{

}

ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueInstanceIdType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueInstanceIdType";
        diag.errors.push_back(ydk::core::ValidationError::INVALATTR);
    }


    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueStringType
////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueStringType::SchemaValueStringType(): length{Range<uint64_t>{0,18446744073709551615UL}}
{

}


ydk::core::SchemaValueStringType::~SchemaValueStringType()
{

}

ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueStringType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaStringType";
        diag.errors.push_back(ydk::core::ValidationError::INVALATTR);
    }

    /// first do a length check
    auto size = value.length();
    if(length.intervals.empty()){
        if(size < length.default_range.min || size > length.default_range.max) {
            BOOST_LOG_TRIVIAL(error) << "Invalid length for string size is " << size;
            diag.errors.push_back(ValidationError::INVALID_LENGTH);

        }
    } else {
        bool constraint_satisfied = false;
        for(auto interval : length.intervals) {
            if( size >= interval.min && size <= interval.max) {
                constraint_satisfied = true;
                break;
            }
        }

        if(!constraint_satisfied){
            BOOST_LOG_TRIVIAL(error) << "Invalid length for string size is " << size;
            diag.errors.push_back(ValidationError::INVALID_LENGTH);
        }
    }


    /// then a pattern check
    /// all patterns have to be matched
    for(auto p : patterns) {
        std::regex r {p};
        if(!std::regex_match(p, r)){
           BOOST_LOG_TRIVIAL(error) << "String " << value << " failed pattern " << p << " match";
            diag.errors.push_back(ValidationError::INVALID_PATTERN);
        }
    }

    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueUnionType
////////////////////////////////////////////////////////////////////////


ydk::core::SchemaValueUnionType::~SchemaValueUnionType()
{
    for(auto type : types) {
        delete type;
    }
}


ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueUnionType::validate(const std::string& value) const
{

    for(auto type : types){
        auto diag = type->validate(value);
        if(!diag.has_errors()){
            return diag;
        }
    }

    DiagnosticNode<std::string, ydk::core::ValidationError> diag{};
    BOOST_LOG_TRIVIAL(error) << "Union type validation failed for value " << value;
    diag.errors.push_back(ValidationError::INVALATTR);

    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueEmptyType
////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueEmptyType::SchemaValueEmptyType(const std::string& mleaf_name) : leaf_name{mleaf_name}
{

}


ydk::core::SchemaValueEmptyType::~SchemaValueEmptyType()
{

}

ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueEmptyType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Value is empty for SchemaValueEmptyType" ;
        diag.errors.push_back(ydk::core::ValidationError::INVALID_EMPTY_VAL);
    } else if(value != leaf_name){
        BOOST_LOG_TRIVIAL(error) << "Mismatch between leaf name " << leaf_name << " and value " << value;
        diag.errors.push_back(ydk::core::ValidationError::INVALID_EMPTY_VAL);
    }
    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::core::SchemaValueBoolType
////////////////////////////////////////////////////////////////////////

ydk::core::SchemaValueBoolType::~SchemaValueBoolType()
{

}


ydk::core::DiagnosticNode<std::string, ydk::core::ValidationError>
ydk::core::SchemaValueBoolType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::core::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Value is empty for SchemaValueBoolType" ;
        diag.errors.push_back(ydk::core::ValidationError::INVALATTR);
    } else {

        if(value != "true" && value != "false") {
            BOOST_LOG_TRIVIAL(error) << "Invalid boolean value " << value;
            diag.errors.push_back(ValidationError::INVALID_BOOL_VAL);
        }
    }

    return diag;
}
