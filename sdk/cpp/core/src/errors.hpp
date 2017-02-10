//
// @file exception.hpp
// @brief The ydk exception header.
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

#ifndef _EXCEPTION_HPP_
#define _EXCEPTION_HPP_

#include <exception>
#include <string>


namespace ydk {
///
/// @brief Base class for YCPP Errors
///
/// The subclasses give a specialized view of the error that has occurred.
///
struct YCPPError : public std::exception
{
    YCPPError(const std::string& msg);

    const char* what() const noexcept;

    std::string err_msg;
};

struct YCPPClientError : public YCPPError
{
	YCPPClientError(const std::string& msg);
};

struct YCPPServiceProviderError : public YCPPError
{
    YCPPServiceProviderError(const std::string& msg);
};

struct YCPPServiceError : public YCPPError
{
    YCPPServiceError(const std::string& msg);
};

///
/// @brief Illegal State Error.
///
///
/// Thrown when an operation/service is invoked
/// on an object that is not in the right state. Use the err_msg for the error.
///
struct YCPPIllegalStateError : public YCPPError
{
     YCPPIllegalStateError(const std::string& msg);
};

///
/// @brief Invalid Argument
///
/// Use the err_msg for the error.
///
struct YCPPInvalidArgumentError : public YCPPError
{
     YCPPInvalidArgumentError(const std::string& msg);
};

///
/// @brief Operation Not Supported Error
///
/// Thrown when an operation is not supported.
///
struct YCPPOperationNotSupportedError : public YCPPError
{
    YCPPOperationNotSupportedError(const std::string& msg);
};

///
/// @brief Model Error
///
/// Thrown when a model constraint is violated.
///
struct YCPPModelError : public YCPPError
{
    YCPPModelError(const std::string& msg);
};

std::ostream & operator<<(std::ostream& o, const ydk::YCPPError& e);
std::ostream & operator<<(std::ostream& o, ydk::YCPPError& e);
}

#endif /* _EXCEPTION_HPP_ */
