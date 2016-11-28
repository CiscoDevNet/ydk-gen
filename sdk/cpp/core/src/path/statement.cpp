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


///////////////////////////////////////////////////////////////////////////
/// Statement
///////////////////////////////////////////////////////////////////////////

ydk::path::Statement::Statement(): keyword{}, arg{}
{

}

ydk::path::Statement::Statement(const std::string& mkeyword, const std::string& marg) : keyword{mkeyword}, arg{marg}
{

}

ydk::path::Statement::Statement(const ydk::path::Statement& stmt): keyword{stmt.keyword}, arg{stmt.arg}
{

}

ydk::path::Statement::Statement(ydk::path::Statement&& stmt): keyword{std::move(stmt.keyword)}, arg{std::move(stmt.arg)}
{

}

ydk::path::Statement::~Statement()
{

}

ydk::path::Statement&
ydk::path::Statement::operator=(const ydk::path::Statement& stmt)
{
    keyword = stmt.keyword;
    arg = stmt.arg;
    return *this;
}

ydk::path::Statement&
ydk::path::Statement::operator=(ydk::path::Statement&& stmt)
{
    keyword = std::move(stmt.keyword);
    arg = std::move(stmt.arg);
    return *this;
}
