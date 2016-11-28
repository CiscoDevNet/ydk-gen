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

////////////////////////////////////////////////////////////////////////////
/// Annotation
///////////////////////////////////////////////////////////////////////////
ydk::path::Annotation::Annotation(const std::string& ns, const std::string& name, const std::string& val) : m_ns{ns}, m_name{name}, m_val{val}
{

}

ydk::path::Annotation::Annotation(const Annotation& an) : m_ns{an.m_ns}, m_name{an.m_name}, m_val{an.m_val}
{

}

ydk::path::Annotation::Annotation(Annotation&& an) : m_ns{std::move(an.m_ns)}, m_name{std::move(an.m_name)}, m_val{std::move(an.m_val)}
{

}

ydk::path::Annotation&
ydk::path::Annotation::operator=(const ydk::path::Annotation& an)
{
    m_ns = an.m_ns;
    m_name = an.m_name;
    m_val = an.m_val;

    return *this;
}

ydk::path::Annotation&
ydk::path::Annotation::operator=(ydk::path::Annotation&& an)
{
    m_ns = std::move(an.m_ns);
    m_name = std::move(an.m_name);
    m_val = std::move(an.m_val);

    return *this;
}

bool
ydk::path::Annotation::operator==(const Annotation& an) const
{
    return m_ns == an.m_ns && m_name == an.m_name;
}
