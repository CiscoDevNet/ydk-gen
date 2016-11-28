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
#include <boost/log/trivial.hpp>

///////////////////////////////////////////////////////////////////////////////////////////
/// SchemaValueType
///////////////////////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueType::~SchemaValueType()
{

}

/////////////////////////////////////////////////////////////////////////////
// ydk::path::SchemaValueBinaryType
/////////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueBinaryType::SchemaValueBinaryType(): length{Range<uint64_t>{0,18446744073709551615UL}}
{

}



ydk::path::SchemaValueBinaryType::~SchemaValueBinaryType()
{

}


ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueBinaryType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty()){
        diag.errors.push_back(ydk::path::ValidationError::INVALATTR);
    }
    return diag;
};



///////////////////////////////////////////////////////////////////////////////////
/// ydk::path::SchemaValueBitsType::Bit
//////////////////////////////////////////////////////////////////////////////////
ydk::path::SchemaValueBitsType::Bit::Bit(std::string m_name, uint32_t m_pos) : name{m_name} ,
pos{m_pos} {

}


namespace ydk {
    namespace path {
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
// ydk::path::SchemaValueBitsType
////////////////////////////////////////////////////////////////////////
ydk::path::SchemaValueBitsType::SchemaValueBitsType(std::vector<ydk::path::SchemaValueBitsType::Bit> bits) : bits(bits)
{

}

ydk::path::SchemaValueBitsType::~SchemaValueBitsType()
{

}

ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueBitsType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty())
    {
        BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueBits";
        diag.errors.push_back(ydk::path::ValidationError::INVALATTR);
    } else {
        //tokenize and make sure all tokens are accounted for
        auto tokens = ydk::path::split(value, ' ');
        std::map<std::string, ydk::path::SchemaValueBitsType::Bit*> name_bit_map{};
        for(auto bit : bits){
            name_bit_map.insert(std::make_pair(bit.name,&bit));
        }
        for(auto token : tokens) {
            if(name_bit_map.find(token) == name_bit_map.end()){
                BOOST_LOG_TRIVIAL(error) << "Invalid bits value " << value;
                diag.errors.push_back(ydk::path::ValidationError::INVALATTR);
            }
        }

    }
    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::path::SchemaValueDec64Type
////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueDec64Type::~SchemaValueDec64Type()
{

}


ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueDec64Type::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueDec64Type";
        diag.errors.push_back(ydk::path::ValidationError::INVALATTR);
    }
    return diag;
}

//////////////////////////////////////////////////////////////////////
/// ydk::path::SchemaValueEnumerationType::Enum
/////////////////////////////////////////////////////////////////////
ydk::path::SchemaValueEnumerationType::Enum::Enum(std::string m_name, int32_t m_value) : name{m_name}, value{m_value}
{

}



////////////////////////////////////////////////////////////////////
// ydk::path::SchemaValueEnumerationType
////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueEnumerationType::~SchemaValueEnumerationType()
{

}

ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueEnumerationType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty()){
         BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueEnumerationType";
        diag.errors.push_back(ydk::path::ValidationError::INVALATTR);

    } else {

        for(auto e : enums){
            if(e.name == value){
                return diag;
            }
        }
    }
     BOOST_LOG_TRIVIAL(error) << "Invalid enum value " << value;
    diag.errors.push_back(ydk::path::ValidationError::INVALATTR);

    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::path::SchemaValueIdentityType
////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueIdentityType::~SchemaValueIdentityType()
{

}

ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueIdentityType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty()){
         BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueIdentityType";
        diag.errors.push_back(ydk::path::ValidationError::INVALATTR);
        return diag;
    }

    auto tokens = ydk::path::split(value, ':');
    if(tokens.size() == 1) {
        //no module name just compare the name
        if(tokens[0] == name) {
            return diag;
        }
    } else if(tokens[0] == module_name && tokens[1] == name){
            return diag;
    }

    for(auto const& ident : derived) {
        if(!ident->validate(value).has_errors()){
            return diag;
        }
    }

    BOOST_LOG_TRIVIAL(error) << "Invalid identity" << value;
    diag.errors.push_back(ydk::path::ValidationError::INVALID_IDENTITY);
    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::path::SchemaValueInstanceIdType
////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueInstanceIdType::~SchemaValueInstanceIdType()
{

}

ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueInstanceIdType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaValueInstanceIdType";
        diag.errors.push_back(ydk::path::ValidationError::INVALATTR);
    }


    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::path::SchemaValueStringType
////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueStringType::SchemaValueStringType(): length{Range<uint64_t>{0,18446744073709551615UL}}
{

}


ydk::path::SchemaValueStringType::~SchemaValueStringType()
{

}

ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueStringType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Empty attribute error for SchemaStringType";
        diag.errors.push_back(ydk::path::ValidationError::INVALATTR);
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
// ydk::path::SchemaValueUnionType
////////////////////////////////////////////////////////////////////////


ydk::path::SchemaValueUnionType::~SchemaValueUnionType()
{

}


ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueUnionType::validate(const std::string& value) const
{

    for(auto const& type : types){
        auto diag = type->validate(value);
        if(!diag.has_errors()){
            return diag;
        }
    }

    DiagnosticNode<std::string, ydk::path::ValidationError> diag{};
    BOOST_LOG_TRIVIAL(error) << "Union type validation failed for value " << value;
    diag.errors.push_back(ValidationError::INVALATTR);

    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::path::SchemaValueEmptyType
////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueEmptyType::SchemaValueEmptyType(const std::string& mleaf_name) : leaf_name{mleaf_name}
{

}


ydk::path::SchemaValueEmptyType::~SchemaValueEmptyType()
{

}

ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueEmptyType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Value is empty for SchemaValueEmptyType" ;
        diag.errors.push_back(ydk::path::ValidationError::INVALID_EMPTY_VAL);
    } else if(value != leaf_name){
        BOOST_LOG_TRIVIAL(error) << "Mismatch between leaf name " << leaf_name << " and value " << value;
        diag.errors.push_back(ydk::path::ValidationError::INVALID_EMPTY_VAL);
    }
    return diag;
}

////////////////////////////////////////////////////////////////////
// ydk::path::SchemaValueBoolType
////////////////////////////////////////////////////////////////////////

ydk::path::SchemaValueBoolType::~SchemaValueBoolType()
{

}


ydk::path::DiagnosticNode<std::string, ydk::path::ValidationError>
ydk::path::SchemaValueBoolType::validate(const std::string& value) const
{
    DiagnosticNode<std::string, ydk::path::ValidationError> diag {};

    if(value.empty()){
        BOOST_LOG_TRIVIAL(error) << "Value is empty for SchemaValueBoolType" ;
        diag.errors.push_back(ydk::path::ValidationError::INVALATTR);
    } else {

        if(value != "true" && value != "false") {
            BOOST_LOG_TRIVIAL(error) << "Invalid boolean value " << value;
            diag.errors.push_back(ValidationError::INVALID_BOOL_VAL);
        }
    }

    return diag;
}
