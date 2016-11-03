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

#include <boost/exception/all.hpp>
#include <exception>
#include <string>


namespace ydk {
///
/// @brief Base class for YDK Exceptions
///
/// The subclasses give a specialized view of the error that has occurred.
///
struct YDKException : public std::exception, public boost::exception
{
	YDKException(const std::string& msg) : err_msg{msg}
	{

	}

	std::string err_msg;
};

struct YDKServiceProviderException : public YDKException
{
    YDKServiceProviderException(const std::string& msg) : YDKException{msg}
    {

    }
};

///
/// @brief Illegal State Exception.
///
///
/// Thrown when an operation/service is invoked
/// on an object that is not in the right state. Use the err_msg for the error.
///
struct YDKIllegalStateException : public YDKException
{
     YDKIllegalStateException(const std::string& msg);
};

///
/// @brief Invalid Argument
///
/// Use the err_msg for the error.
///
struct YDKInvalidArgumentException : public YDKException
{
     YDKInvalidArgumentException(const std::string& msg);
};

///
/// @brief Operation Not Supported Exception
///
/// Thrown when an operation is not supported.
///
struct YDKOperationNotSupportedException : public YDKException
{
    YDKOperationNotSupportedException(const std::string& msg);
};

}

#endif /* _EXCEPTION_HPP_ */
