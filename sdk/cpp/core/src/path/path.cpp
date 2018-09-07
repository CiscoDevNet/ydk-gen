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


#include <pcre.h>
#include <fstream>
#include <sstream>
#include <typeinfo>

#include "../logger.hpp"
#include "path_private.hpp"
#include "../common_utilities.hpp"

#define SLASH_CHAR "##SLASH##"

namespace ydk
{
static void escape_slashes(std::string& data, const char* pattern)
{
    pcre *re = NULL;
    const char *err_msg;
    int err;
    int offsets[3000];
    const char *psubStrMatchStr = NULL;
    unsigned int offset = 0;
    int rc;

    re = pcre_compile(pattern, 0, &err_msg, &err, NULL);
    if(re == NULL)
    {
        YLOG_ERROR("ERROR: Could not compile '{}': {}", pattern, err_msg);
        return;
    }

    while (offset < data.size() && (rc = pcre_exec(re, 0, data.c_str(), data.size(), offset, 0, offsets, sizeof(offsets))) >= 0)
    {
       for(int i = 0; i < rc; ++i)
       {
           pcre_get_substring(data.c_str(), offsets, rc, i, &(psubStrMatchStr));
           std::string original{psubStrMatchStr};
           std::string s{psubStrMatchStr};
           if(replace(s,"/",SLASH_CHAR))
           {
               replace(data, original, s);
           }
           if(psubStrMatchStr != NULL)
           {
                pcre_free_substring(psubStrMatchStr);
                psubStrMatchStr = NULL;
           }
       }
       offset = offsets[1];
    }
    if(psubStrMatchStr != NULL)
        pcre_free_substring(psubStrMatchStr);
    free(re);
}
}

std::unordered_set<std::string> ydk::path::segmentalize_module_names(const std::string& value)
{
    std::unordered_set<std::string> module_names;
    auto segs = segmentalize(value);
    for (auto &s: segs)
    {
        auto found = s.find(":");
        if (found != std::string::npos)
        {
            size_t start = 0;
            size_t colon = s.rfind("'", found);
            start = colon == std::string::npos ? 0 : colon + 1;
            module_names.insert(s.substr(start, found-start));
        }
    }
    return module_names;
}

std::vector<std::string> ydk::path::segmentalize(const std::string& path)
{
    const std::string token {"/"};
    std::vector<std::string> output;
    size_t pos = std::string::npos; // size_t to avoid improbable overflow
    std::string data{path};
    escape_slashes(data, "'[^\[]+'");
    escape_slashes(data, "\"[^\[]+\"");
    do
    {
        pos = data.find(token);
        auto q=data.substr(0, pos);
        replace(q, SLASH_CHAR, "/");
        output.push_back(q);
        if (std::string::npos != pos)
            data = data.substr(pos + token.size());
    } while (std::string::npos != pos);

    return output;
}

ydk::path::Session::~Session()
{
}

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
            option_str="DATASTORE";
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
            ly_option = LYD_OPT_GETCONFIG | LYD_OPT_NOAUTODEL;
            break;
    }

    YLOG_INFO("Validation called on {} with option {}", dn.get_path(), option_str);

    //what kind of a DataNode is this
    const ydk::path::DataNodeImpl & dn_impl = dynamic_cast<const ydk::path::DataNodeImpl&>(dn);
    struct lyd_node* lynode = dn_impl.m_node;
    int rc = lyd_validate(&lynode,ly_option, NULL);
    if(rc) {
        YLOG_ERROR("Data validation failed: {}. Path: {}", ly_errmsg(), ly_errpath());
        throw(ydk::YModelError{""});
    }

}

//////////////////////////////////////////////////////////////////////////
// Utility functions
//////////////////////////////////////////////////////////////////////////

static LYD_FORMAT get_ly_format(ydk::EncodingFormat format)
{
    LYD_FORMAT scheme = LYD_XML;
    if (format == ydk::EncodingFormat::JSON)
    {
        scheme = LYD_JSON;
    }
    return scheme;
}

static ydk::path::RootSchemaNodeImpl & get_root_schema_impl(ydk::path::RootSchemaNode & root_schema)
{
    ydk::path::RootSchemaNodeImpl & rs_impl = dynamic_cast<ydk::path::RootSchemaNodeImpl &>(root_schema);
    return rs_impl;
}

static std::shared_ptr<ydk::path::DataNode> perform_decode(ydk::path::RootSchemaNodeImpl & rs_impl, struct lyd_node *lnode)
{
    ydk::YLOG_DEBUG("Performing decode operation");
    ydk::path::RootDataImpl* rd = new ydk::path::RootDataImpl{rs_impl, rs_impl.m_ctx, "/"};
    rd->m_node = lnode;

    struct lyd_node* first_dnode = lyd_first_sibling(lnode);
    struct lyd_node* dnode = first_dnode;
    do
    {
        rd->child_map.insert(std::make_pair(dnode, std::make_shared<ydk::path::DataNodeImpl>(rd, dnode, nullptr)));
        dnode = dnode->next;
    } while(dnode && dnode != first_dnode);

    return std::shared_ptr<ydk::path::DataNode>(rd);
}

static const struct lyd_node* create_lyd_node_for_rpc(ydk::path::RootSchemaNodeImpl & rs_impl, const std::string & rpc_path)
{
    const struct lyd_node* rpc = lyd_new_path(NULL, rs_impl.m_ctx, rpc_path.c_str(), NULL, LYD_ANYDATA_SXML, 0);
    if( rpc == nullptr || ly_errno )
    {
        ydk::YLOG_ERROR( "Parsing failed with message {}", ly_errmsg());
        throw(ydk::path::YCodecError{ydk::path::YCodecError::Error::XML_INVAL});
    }
    return rpc;
}

//////////////////////////////////////////////////////////////////////////
// class ydk::Codec
//////////////////////////////////////////////////////////////////////////
ydk::path::Codec::Codec()
{
}

ydk::path::Codec::~Codec()
{
}

std::string
ydk::path::Codec::encode(std::vector<ydk::path::DataNode*> & data_nodes, ydk::EncodingFormat format, bool pretty)
{
    YLOG_DEBUG("ydk::path::Codec: Encoding list of data nodes to {} formated string", format==ydk::EncodingFormat::JSON ? "JSON" : "XML");
    std::string ret{};
    for (auto dn : data_nodes) {
        ret += encode(*dn, format, pretty);
    }
    return ret;
}

std::string
ydk::path::Codec::encode(const ydk::path::DataNode& dn, ydk::EncodingFormat format, bool pretty)
{
    YLOG_DEBUG("ydk::path::Codec: Encoding data node '{}' to {} formated string", dn.get_path(), format==ydk::EncodingFormat::JSON ? "JSON" : "XML");
    std::string ret{};
    if (typeid(dn) == typeid(RootDataImpl)) {
        std::vector<std::shared_ptr<ydk::path::DataNode>> data_nodes = dn.get_children();
        for (auto dn : data_nodes) {
            ret += encode(*dn, format, pretty);
        }
    }
    else {
        LYD_FORMAT scheme = get_ly_format(format);
        const ydk::path::DataNodeImpl& impl = dynamic_cast<const ydk::path::DataNodeImpl &>(dn);
        struct lyd_node* m_node = impl.m_node;
        if (m_node == nullptr) {
            throw(ydk::YInvalidArgumentError{"No data in data node"});
        }
        char* buffer;
        if(!lyd_print_mem(&buffer, m_node, scheme, (pretty ? LYP_FORMAT : 0)|LYP_WD_ALL|LYP_KEEPEMPTYCONT)) {
            if(!buffer)
            {
                std::ostringstream os;
                os << "Could not encode datanode: "<< m_node->schema->name;
                ydk::YLOG_ERROR(os.str().c_str());
                throw(ydk::path::YCoreError{os.str()});
            }
            ret = buffer;
            std::free(buffer);
        }
    }
    return ret;
}

std::shared_ptr<ydk::path::DataNode>
ydk::path::Codec::decode(RootSchemaNode & root_schema, const std::string& buffer, EncodingFormat format)
{
    YLOG_DEBUG("ydk::path::Codec: Decoding from {} formatted payload:\n{}", format==ydk::EncodingFormat::JSON ? "JSON" : "XML", buffer);

    RootSchemaNodeImpl & rs_impl = get_root_schema_impl(root_schema);
    rs_impl.populate_new_schemas_from_payload(buffer, format);

    struct lyd_node *root = lyd_parse_mem(rs_impl.m_ctx, buffer.c_str(),
                get_ly_format(format), LYD_OPT_TRUSTED |  LYD_OPT_GET);

    if( root == nullptr || ly_errno )
    {
        YLOG_ERROR( "Parsing failed with message {}", ly_errmsg());
        throw(YCodecError{YCodecError::Error::XML_INVAL});
    }
    return perform_decode(rs_impl, root);
}

std::shared_ptr<ydk::path::DataNode>
ydk::path::Codec::decode_rpc_output(RootSchemaNode & root_schema, const std::string& buffer,
            const std::string & rpc_path, EncodingFormat format)
{
    YLOG_DEBUG( "Decoding output for RPC '{}'. Output is: {}", rpc_path, buffer);

    RootSchemaNodeImpl & rs_impl = get_root_schema_impl(root_schema);
    rs_impl.populate_new_schemas_from_payload(buffer, format);
    const struct lyd_node* rpc = create_lyd_node_for_rpc(rs_impl, rpc_path);

    struct lyd_node* root = lyd_parse_mem(rs_impl.m_ctx, buffer.c_str(),
                get_ly_format(format), LYD_OPT_TRUSTED |  LYD_OPT_RPCREPLY, rpc, NULL);
    if( root == nullptr || ly_errno )
    {
        YLOG_ERROR( "Parsing failed with message {}", ly_errmsg());
        throw(YCodecError{YCodecError::Error::XML_INVAL});
    }

    return perform_decode(rs_impl, root);
}

std::shared_ptr<ydk::path::DataNode>
ydk::path::Codec::decode_json_output(RootSchemaNode & root_schema, const std::vector<std::string> & buffer_list)
{
    YLOG_DEBUG("ydk::path::Codec: Decoding JSON formatted list of payloads");

    RootSchemaNodeImpl & rs_impl = get_root_schema_impl(root_schema);

    ydk::path::RootDataImpl* rd = new ydk::path::RootDataImpl{rs_impl, rs_impl.m_ctx, "/"};
    lyd_node* prev_sibling = nullptr;
    lyd_node* first_sibling = nullptr;

    for (auto buffer : buffer_list) {
        rs_impl.populate_new_schemas_from_payload(buffer, ydk::EncodingFormat::JSON);

        struct lyd_node *dnode = lyd_parse_mem(rs_impl.m_ctx, buffer.c_str(), LYD_JSON, LYD_OPT_TRUSTED | LYD_OPT_GET);
        if (dnode == nullptr || ly_errno) {
            YLOG_ERROR( "Parsing failed with message {}", ly_errmsg());
            throw(YCodecError{YCodecError::Error::XML_INVAL});
        }

        // Attach first node to the root
        if (!rd->m_node) {
            rd->m_node = dnode;
            first_sibling = dnode;
        }

        // Populate children map and connect siblings
        rd->child_map.insert(std::make_pair(dnode, std::make_shared<ydk::path::DataNodeImpl>(rd, dnode, nullptr)));

        if (prev_sibling) {
            prev_sibling->next = dnode;
            dnode->prev = prev_sibling;
        }
        prev_sibling = dnode;
        first_sibling->prev = dnode;
    }

    return std::shared_ptr<ydk::path::DataNode>(rd);
}

#undef SLASH_CHAR
