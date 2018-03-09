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

#include <iostream>

#include "errors.hpp"
#include "logger.hpp"
#include "xml_util.hpp"

using namespace std;

namespace ydk
{

//////////////////////////////////////////////////////////////////
// XML miscellaneous utilities
//////////////////////////////////////////////////////////////////
string to_string(const xmlChar * s)
{
    if(s == NULL) return "";

    const char* buffer = reinterpret_cast<const char*>(s);
    return buffer;
}

const xmlChar * to_xmlchar(const string & s)
{
    const xmlChar* buffer = reinterpret_cast<const xmlChar*>(s.c_str());
    return buffer;
}

string to_string(xmlDocPtr doc, xmlNodePtr root)
{
    string str;
    xmlBufferPtr buf = xmlBufferCreate();
    if (buf != NULL)
    {
        xmlNodeDump(buf, doc, root, 0, 1);
        str = to_string(buf->content);
        xmlBufferFree(buf);
    }
    else
    {
        YLOG_ERROR("Error creating the xml buffer");
        throw YServiceProviderError{"Error creating the xml buffer"};
    }
    return str;
}

void set_xml_namespace(const string & name_space, xmlNodePtr xml_node)
{
    xmlNewProp(xml_node, to_xmlchar("xmlns"), to_xmlchar(name_space));
}

bool isonlywhitespace(xmlChar *	content)
{
    if(content == NULL)
        return true;

    for(auto g:to_string(content))
    {
        if (!isspace(g))
            return false;
    }
    return true;
}
}
