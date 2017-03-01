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
#include <iostream>
#include <fstream>
#include <sstream>
#include "../logger.hpp"


////////////////////////////////////////////////////////////////////
/// Function segmentalize()
////////////////////////////////////////////////////////////////////
std::vector<std::string>
ydk::path::segmentalize(const std::string& path)
{
    const std::string token {"/"};
    std::vector<std::string> output;
    size_t pos = std::string::npos; // size_t to avoid improbable overflow
    size_t prev_pos = 0;
    std::string data{path};
    do
    {
        pos = data.find(token);
        size_t first_quote_pos = data.find("'", prev_pos+1);
        size_t second_quote_pos = data.find("'", first_quote_pos+1);
        while((pos<second_quote_pos) && (pos>first_quote_pos))
            pos = data.find(token, pos+1);
        output.push_back(data.substr(0, pos));
        if (std::string::npos != pos)
            data = data.substr(pos + token.size());
        prev_pos = pos;
    } while (std::string::npos != pos);
    return output;
}

////////////////////////////////////////////////////////////////////
/// ServiceProvider
///////////////////////////////////////////////////////////////////
ydk::path::ServiceProvider::~ServiceProvider()
{

}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// class ydk::ValidationService
//////////////////////////////////////////////////////////////////////////
ydk::path::ValidationService::ValidationService()
{
}

ydk::path::ValidationService::~ValidationService()
{
}

void
ydk::path::ValidationService::validate(const ydk::path::DataNode & dn, ydk::ValidationService::Option option)
{
    std::string option_str = "";
    int ly_option = 0;
    switch(option) {
        case ydk::ValidationService::Option::DATASTORE:
            option_str="DATATSTORE";
            ly_option = LYD_OPT_CONFIG;
            break;
        case ydk::ValidationService::Option::EDIT_CONFIG:
            option_str="EDIT-CONFIG";
            ly_option = LYD_OPT_EDIT;
            break;
        case ydk::ValidationService::Option::GET:
            option_str="GET";
            ly_option = LYD_OPT_GET;
            break;
        case ydk::ValidationService::Option::GET_CONFIG:
            option_str="GET-CONFIG";
            ly_option = LYD_OPT_GETCONFIG;
            break;

    }
    ly_option = ly_option | LYD_OPT_NOAUTODEL;

    YLOG_DEBUG("Validation called on {} with option {}", dn.path(), option_str);

    //what kind of a DataNode is this
    const ydk::path::DataNodeImpl & dn_impl = dynamic_cast<const ydk::path::DataNodeImpl&>(dn);
    struct lyd_node* lynode = dn_impl.m_node;
    int rc = lyd_validate(&lynode,ly_option, NULL);
    if(rc) {
        YLOG_ERROR("Data validation failed: {}", ly_errmsg());
        throw(ydk::path::YCPPDataValidationError{});
    }

}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// class ydk::CodecService
//////////////////////////////////////////////////////////////////////////
ydk::path::CodecService::CodecService()
{
}

ydk::path::CodecService::~CodecService()
{
}

std::string
ydk::path::CodecService::encode(const ydk::path::DataNode& dn, ydk::EncodingFormat format, bool pretty)
{
    std::string ret{};


    LYD_FORMAT scheme = LYD_XML;


    if(format == ydk::EncodingFormat::JSON)
    {
    	YLOG_TRACE("Performing encode operation on JSON");
        scheme = LYD_JSON;
    }
    else
    {
    	YLOG_TRACE("Performing encode operation on XML");
    }

    struct lyd_node* m_node = nullptr;

    const DataNodeImpl& impl = dynamic_cast<const DataNodeImpl &>(dn);
    m_node = impl.m_node;

    if(m_node == nullptr){
        throw(YCPPInvalidArgumentError{"No data in data node"});
    }
    char* buffer;

    if(!lyd_print_mem(&buffer, m_node,scheme, (pretty ? LYP_FORMAT : 0)|LYP_WD_ALL|LYP_KEEPEMPTYCONT)) {
        if(!buffer)
        {
            std::ostringstream os;
            os << "Could not encode datanode: "<< m_node->schema->name;
            YLOG_ERROR(os.str().c_str());
            throw(YCPPCoreError{os.str()});
        }
        ret = buffer;
        std::free(buffer);
    }

    return ret;

}

std::unique_ptr<ydk::path::DataNode>
ydk::path::CodecService::decode(const RootSchemaNode & root_schema, const std::string& buffer, EncodingFormat format)
{
    LYD_FORMAT scheme = LYD_XML;
    if (format == EncodingFormat::JSON)
    {
    	YLOG_TRACE("Performing decode operation on JSON");
        scheme = LYD_JSON;
    }
    else
    {
    	YLOG_TRACE("Performing decode operation on XML");
    }

    const RootSchemaNodeImpl & rs_impl = dynamic_cast<const RootSchemaNodeImpl &>(root_schema);

    struct lyd_node *root = lyd_parse_mem(rs_impl.m_ctx, buffer.c_str(), scheme, LYD_OPT_TRUSTED |  LYD_OPT_GET);
    if( root == nullptr || ly_errno )
    {

        YLOG_ERROR( "Parsing failed with message {}", ly_errmsg());
        throw(YCPPCodecError{YCPPCodecError::Error::XML_INVAL});
    }


    YLOG_TRACE("Performing decode operation");
    RootDataImpl* rd = new RootDataImpl{rs_impl, rs_impl.m_ctx, "/"};
    rd->m_node = root;

    struct lyd_node* dnode = rd->m_node;
    do
    {

        rd->child_map.insert(std::make_pair(rd->m_node, std::make_shared<DataNodeImpl>(rd, rd->m_node)));
        dnode = dnode->next;
    } while(dnode && dnode != nullptr && dnode != root);

    return std::unique_ptr<ydk::path::DataNode>(rd);
}
