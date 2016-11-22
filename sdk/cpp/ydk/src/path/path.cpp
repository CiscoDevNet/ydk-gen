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
#include <boost/log/trivial.hpp>


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



/////////////////////////////////////////////////////////////////////////
namespace ydk {
    namespace path {

        template<typename T>
        void parse_range_intervals(LengthRangeIntervals<T>& intervals, const char* str_restr)
        {
            const char* seg_ptr = str_restr;
            while(1) {
                // min
                const char* ptr = seg_ptr;
                Range<T> range{intervals.default_range.min, intervals.default_range.max};

                if(ptr) {
                    // start processing min
                    while(isspace(ptr[0])) {
                        ++ptr;
                    }

                    if (isdigit(ptr[0]) || (ptr[0] == '+') ||  (ptr[0] == '-')) {
                        range.min = atoll(ptr);
                    if((ptr[0] == '+') || (ptr[0] == '-')) {
                        ++ptr;
                    }

                    while (isdigit(ptr[0])) {
                        ++ptr;
                    }

                    } else if (!strncmp(ptr, "min", 3)) {
                        ptr += 3;
                    } else if(!strncmp(ptr, "max", 3)) {
                        ptr += 3;
                    } else {
                        BOOST_LOG_TRIVIAL(error) << "Error parsing range " << str_restr;
                        BOOST_THROW_EXCEPTION(YDKIllegalStateException{"Error parsing range"});
                    }

                    while (isspace(ptr[0])) {
                        ptr++;
                    }

                    //no interval or interval
                    if ((ptr[0] == '|' || !ptr[0])) {
                        range.max = atoll(ptr);

                    } else if( !strncmp(ptr, "..", 2)) {
                        // skip ..
                        ptr += 2;
                        while (isspace(ptr[0])) {
                            ++ptr;
                        }

                        // max
                        if (isdigit(ptr[0]) || (ptr[0] == '+') || (ptr[0] == '-')) {
                            range.max = atoll(ptr);
                        } else if (!strncmp(ptr, "max", 3)) {
                            // do nothing since max is already set

                        } else {
                            BOOST_LOG_TRIVIAL(error) << "Error parsing range " << str_restr;
                            BOOST_THROW_EXCEPTION(YDKIllegalStateException{"Error parsing range"});
                        }
                    } else {
                        BOOST_LOG_TRIVIAL(error) << "Error parsing range " << str_restr;
                        BOOST_THROW_EXCEPTION(YDKIllegalStateException{"Error parsing range"});
                    }
                    intervals.intervals.push_back(range);

                    /* next segment (next OR) */
                    seg_ptr = strchr(seg_ptr, '|');
                    if (!seg_ptr) {
                        break;
                    }
                    seg_ptr++;
                }
            }

        }


        std::unique_ptr<SchemaValueIdentityType> create_identity_type(struct ly_set *ident)
        {
            auto identity_type = std::make_unique<SchemaValueIdentityType>();
            if(!ident) return identity_type;

            for(unsigned int i=0;i<ident->number && i<=272;i++) //272 temporary workaround for ietf-interfaces
            {
				identity_type->module_name = ident->set.s[i]->module->name;
				identity_type->name = ident->set.s[i]->name;
                                struct lys_ident* iden = ((struct lys_ident*)ident->set.s[i]);

                if(iden->der && iden->der->number > 0)
				{
					identity_type->derived.push_back(create_identity_type(iden->der));
				}

            }
            return identity_type;

        }


        std::unique_ptr<SchemaValueIdentityType> create_identity_type(struct lys_ident **ident, int count)
        {

            auto identity_type = std::make_unique<SchemaValueIdentityType>();
            if(!ident) return identity_type;

            for(int i=0;i<count;i++)
            {
				identity_type->module_name = ident[i]->module->name;
				identity_type->name = ident[i]->name;

                if(ident[i]->der && ident[i]->der->number > 0)
				{
					identity_type->derived.push_back(create_identity_type(ident[i]->der));
				}

            }
            return identity_type;
        }

        std::unique_ptr<SchemaValueType> create_schema_value_type(struct lys_node_leaf* leaf,
                                                  struct lys_type* type)
        {

            std::unique_ptr<SchemaValueType> m_type = nullptr;

            LY_DATA_TYPE data_type = type->base;

            switch(data_type){
                case LY_TYPE_BINARY: {
                    if(type->info.binary.length){
                        auto binary = std::make_unique<SchemaValueBinaryType>();
                        parse_range_intervals(binary->length, type->info.binary.length->expr);
                        m_type = std::move(binary);
                    } else if(type->der){
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                    	m_type = std::make_unique<SchemaValueBinaryType>();
                    }

                    break;
                    }
                case LY_TYPE_BITS: {
                	std::vector<SchemaValueBitsType::Bit> bit_values;
                	for(int index=0; index < type->info.bits.count; index++)
                	{
                		bit_values.push_back(
                				SchemaValueBitsType::Bit{
                					type->info.bits.bit[index].name,
                					type->info.bits.bit[index].pos
                		     }
                		);
                	}

                    m_type = std::make_unique<SchemaValueBitsType>(bit_values);
                    break;
                }
                case LY_TYPE_BOOL: {
                    m_type = std::make_unique<SchemaValueBoolType>();
                    break;
                }
                case LY_TYPE_DEC64: {
                    m_type = std::make_unique<SchemaValueDec64Type>();
                    break;
                }
                case LY_TYPE_EMPTY: {
                    m_type = std::make_unique<SchemaValueEmptyType>(leaf->name);
                    break;
                }
                case LY_TYPE_ENUM: {
                    if(type->info.enums.count > 0) {
                        auto enum_type = std::make_unique<SchemaValueEnumerationType>();
                        for(int i=0; i<type->info.enums.count; i++)
                        {
                            SchemaValueEnumerationType::Enum enum_ {
										type->info.enums.enm[i].name, type->info.enums.enm[i].value
									};
                            enum_type->enums.push_back(enum_);
                        }
                        m_type = std::move(enum_type);
                    } else if(type->der){
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        BOOST_LOG_TRIVIAL(error) << "Unable to determine union's types: " << leaf->name <<", module: "<< leaf->module->name;
                        BOOST_THROW_EXCEPTION(ydk::YDKIllegalStateException{"Unable to determine union's types"});
                    }
                    break;
                }
                case LY_TYPE_IDENT: {
                    if(type->info.ident.ref) {
                        m_type = create_identity_type(type->info.ident.ref, type->info.ident.count);
                    } else if(type->der){
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        BOOST_LOG_TRIVIAL(error) << "Unable to determine identity type: " << leaf->name <<", module: "<< leaf->module->name;
                        std::ostringstream os;
                        os << "Unable to determine identity type: " << leaf->name <<", module: "<< leaf->module->name;
                        BOOST_THROW_EXCEPTION(ydk::YDKIllegalStateException{os.str()});
                    }
                    break;
                }
                case LY_TYPE_INST: {
                    m_type = std::make_unique<SchemaValueInstanceIdType>();
                    break;
                }
                case LY_TYPE_LEAFREF: {
                    if(type->info.lref.target) {
                        m_type = create_schema_value_type(type->info.lref.target, &(type->info.lref.target->type));
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                    	m_type = std::make_unique<SchemaValueStringType>();//TODO temporary workaround
//                        BOOST_LOG_TRIVIAL(error) << "Unable to determine leafref type: " << leaf->name <<", module: "<< leaf->module->name;
//                        std::ostringstream os;
//                        os << "Unable to determine leafref type: " << leaf->name <<", module: "<< leaf->module->name;
//                        BOOST_THROW_EXCEPTION(ydk::YDKIllegalStateException{os.str()});
                    }
                    break;
                }
                case LY_TYPE_STRING: {
                    if(type->info.str.length) {
                        auto stringType = std::make_unique<SchemaValueStringType>();
                        parse_range_intervals(stringType->length, type->info.str.length->expr);

                        if(type->info.str.pat_count != 0){
                            for(int i =0; i < type->info.str.pat_count; i++) {
                                stringType->patterns.push_back(type->info.str.patterns[i].expr);
                            }
                        }
                        m_type = std::move(stringType);

                    } else if(type->der){
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        auto stringType = std::make_unique<SchemaValueStringType>();

                        if(type->info.str.pat_count != 0){
                            for(int i=0; i < type->info.str.pat_count; i++) {
                                stringType->patterns.push_back(type->info.str.patterns[i].expr);
                            }
                        }

                        m_type = std::move(stringType);

                    }
                    break;
                }
                case LY_TYPE_UNION: {

                    if(type->info.uni.count != 0) {
                        auto unionType = std::make_unique<SchemaValueUnionType>();
                        for(int i=0; i< type->info.uni.count; ++i) {
                            unionType->types.push_back(create_schema_value_type(leaf,&(type->info.uni.types[i])));
                        }
                        m_type = std::move(unionType);
                    } else if(type->der){
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        BOOST_LOG_TRIVIAL(error) << "Unable to determine union's types: " << leaf->name <<", module: "<< leaf->module->name;
                        std::ostringstream os;
                        os<<"Unable to determine union's types: " << leaf->name <<", module: "<< leaf->module->name;
                        BOOST_THROW_EXCEPTION(ydk::YDKIllegalStateException{os.str()});
                    }


                    break;
                }
                case LY_TYPE_INT8: {
                    if(type->info.num.range) {
                        auto int8_type = std::make_unique<SchemaValueNumberType<int8_t>>(
                        					static_cast<int8_t>(-128),
                            				static_cast<int8_t>(127)
                        				);
                        parse_range_intervals(int8_type->range, type->info.num.range->expr);
                        m_type = std::move(int8_type);
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        m_type = std::make_unique<SchemaValueNumberType<int8_t>>( static_cast<int8_t>(-128),
                            static_cast<int8_t>(127) );
                    }
                    break;
                }
                case LY_TYPE_UINT8:
                {
                    if(type->info.num.range) {
                        auto uint8_type = std::make_unique<SchemaValueNumberType<uint8_t>>( static_cast<uint8_t>(0),static_cast<uint8_t>(255) );
                        parse_range_intervals(uint8_type->range, type->info.num.range->expr);
                        m_type = std::move(uint8_type);
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        m_type = std::make_unique<SchemaValueNumberType<uint8_t>>( static_cast<uint8_t>(0),static_cast<uint8_t>(255) );
                    }
                    break;
                }

                case LY_TYPE_INT16:
                {
                    if(type->info.num.range) {
                        auto int16_type = std::make_unique<SchemaValueNumberType<int16_t>>( static_cast<int16_t>(-32768),static_cast<int16_t>(32767) );
                        parse_range_intervals(int16_type->range, type->info.num.range->expr);
                        m_type = std::move(int16_type);
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        m_type = std::make_unique<SchemaValueNumberType<int16_t>>( static_cast<int16_t>(-32768),static_cast<int16_t>(32767) );
                    }
                    break;
                }
                case LY_TYPE_UINT16:
                {
                    if(type->info.num.range) {
                        auto uint16_type = std::make_unique<SchemaValueNumberType<uint16_t>>( static_cast<uint16_t>(0),static_cast<uint16_t>(65535) );
                        parse_range_intervals(uint16_type->range, type->info.num.range->expr);
                        m_type = std::move(uint16_type);
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        m_type = std::make_unique<SchemaValueNumberType<uint16_t>>( static_cast<uint16_t>(0),static_cast<uint16_t>(65535) );
                    }
                    break;
                }
                case LY_TYPE_INT32:
                {
                    if(type->info.num.range) {
                        auto int32_type = std::make_unique<SchemaValueNumberType<int32_t>>( static_cast<int32_t>(-2147483648),static_cast<int32_t>(2147483647) );
                        parse_range_intervals(int32_type->range, type->info.num.range->expr);
                        m_type = std::move(int32_type);
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        m_type = std::make_unique<SchemaValueNumberType<int32_t>>( static_cast<int32_t>(-2147483648),static_cast<int32_t>(2147483647) );
                    }
                    break;
                }
                case LY_TYPE_UINT32:
                {
                    if(type->info.num.range) {
                        auto uint32_type = std::make_unique<SchemaValueNumberType<uint32_t>>( static_cast<uint32_t>(0),static_cast<uint32_t>(4294967295) );
                        parse_range_intervals(uint32_type->range, type->info.num.range->expr);
                        m_type = std::move(uint32_type);
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        m_type = std::make_unique<SchemaValueNumberType<uint32_t>>( static_cast<uint32_t>(0),static_cast<uint32_t>(4294967295) );
                    }
                    break;
                }

                case LY_TYPE_INT64:
                {
                    if(type->info.num.range) {
                        auto int64_type = std::make_unique<SchemaValueNumberType<int64_t>>( static_cast<int64_t>(-9223372036854775807),static_cast<int64_t>(9223372036854775807) );
                        parse_range_intervals(int64_type->range, type->info.num.range->expr);
                        m_type = std::move(int64_type);
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        m_type = std::make_unique<SchemaValueNumberType<int64_t>>( static_cast<int64_t>(-9223372036854775807),static_cast<int64_t>(9223372036854775807) );
                    }
                    break;
                }
                case LY_TYPE_UINT64: {
                    if(type->info.num.range) {
                        auto uint64_type = std::make_unique<SchemaValueNumberType<uint64_t>>( static_cast<uint64_t>(0),static_cast<uint64_t>(18446744073709551615ULL) );
                        parse_range_intervals(uint64_type->range, type->info.num.range->expr);
                        m_type = std::move(uint64_type);
                    } else if(type->der) {
                        m_type = create_schema_value_type(leaf, &(type->der->type));
                    } else {
                        m_type = std::make_unique<SchemaValueNumberType<uint64_t>>( static_cast<uint64_t>(0),static_cast<uint64_t>(18446744073709551615ULL) );
                    }
                    break;
                }
                default:
                    BOOST_LOG_TRIVIAL(error) << "Unknown type to process for schema: " << leaf->name <<", module: "<< leaf->module->name;
                    std::ostringstream os;
                    os<<"Unknown type to process for schema: " << leaf->name <<", module: "<< leaf->module->name;
                    BOOST_THROW_EXCEPTION(YDKIllegalStateException{os.str()});

            }

            return m_type;
        }

        std::unique_ptr<SchemaValueType> create_schema_value_type(struct lys_node_leaf* leaf)
        {
            return create_schema_value_type(leaf, &(leaf->type));
        }


    }
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// class ydk::ValidationService
//////////////////////////////////////////////////////////////////////////
void
ydk::path::ValidationService::validate(const ydk::path::DataNode* dn, ydk::path::ValidationService::Option option)
{
    std::string option_str = "";
    int ly_option = 0;
    switch(option) {
        case ValidationService::Option::DATASTORE:
            option_str="DATATSTORE";
            ly_option = LYD_OPT_CONFIG;
            break;
        case ValidationService::Option::EDIT_CONFIG:
            option_str="EDIT-CONFIG";
            ly_option = LYD_OPT_EDIT;
            break;
        case ValidationService::Option::GET:
            option_str="GET";
            ly_option = LYD_OPT_GET;
            break;
        case ValidationService::Option::GET_CONFIG:
            option_str="GET-CONFIG";
            ly_option = LYD_OPT_GETCONFIG;
            break;

    }
    ly_option = ly_option | LYD_OPT_NOAUTODEL;

    BOOST_LOG_TRIVIAL(debug) << "Validation called on " << dn->path() << " with option " << option_str;

    //what kind of a DataNode is this
    const ydk::path::DataNodeImpl* dn_impl = dynamic_cast<const ydk::path::DataNodeImpl*>(dn);
    if(dn_impl){
        struct lyd_node* lynode = dn_impl->m_node;
        int rc = lyd_validate(&lynode,ly_option, NULL);
        if(rc) {
            BOOST_LOG_TRIVIAL(error) << "Data validation failed";
            BOOST_THROW_EXCEPTION(ydk::path::YDKDataValidationException{});
        }

    } else {
        BOOST_LOG_TRIVIAL(error) << "Cast of DataNode to impl failed!!";
        BOOST_THROW_EXCEPTION(YDKIllegalStateException{"Illegal state"});
    }

}


///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// class ydk::CodecService
//////////////////////////////////////////////////////////////////////////
std::string
ydk::path::CodecService::encode(const ydk::path::DataNode* dn, ydk::path::CodecService::Format format, bool pretty)
{
    std::string ret{};


    LYD_FORMAT scheme = LYD_XML;


    if(format == ydk::path::CodecService::Format::JSON) {
        scheme = LYD_JSON;
    }

    struct lyd_node* m_node = nullptr;

    const DataNodeImpl* impl = dynamic_cast<const DataNodeImpl *>(dn);
    if( !impl) {
        BOOST_LOG_TRIVIAL(error) << "DataNode is nullptr";
        BOOST_THROW_EXCEPTION(YDKCoreException{"DataNode is null"});
    }
    m_node = impl->m_node;

    if(m_node == nullptr){
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"No data in data node"});
    }
    char* buffer;
    BOOST_LOG_TRIVIAL(trace) << "Performing encode operation";

    if(!lyd_print_mem(&buffer, m_node,scheme, (pretty ? LYP_FORMAT : 0)|LYP_WD_ALL|LYP_KEEPEMPTYCONT)) {
    	if(!buffer)
    	{
    		std::ostringstream os;
    		os << "Could not encode datanode: "<< m_node->schema->name;
			BOOST_LOG_TRIVIAL(error) << os.str();
			BOOST_THROW_EXCEPTION(YDKCoreException{os.str()});
    	}
        ret = buffer;
        std::free(buffer);
    }

    return ret;

}

ydk::path::DataNode*
ydk::path::CodecService::decode(const RootSchemaNode* root_schema, const std::string& buffer, CodecService::Format format)
{
    LYD_FORMAT scheme = LYD_XML;
    if (format == CodecService::Format::JSON)
    {
        scheme = LYD_JSON;
    }

    const RootSchemaNodeImpl* rs_impl = dynamic_cast<const RootSchemaNodeImpl*>(root_schema);
    if(!rs_impl)
    {
        BOOST_LOG_TRIVIAL(error) << "Root Schema Node is nullptr";
        BOOST_THROW_EXCEPTION(YDKCoreException{"Root Schema Node is null"});
    }

    struct lyd_node *root = lyd_parse_mem(rs_impl->m_ctx, buffer.c_str(), scheme, LYD_OPT_TRUSTED |  LYD_OPT_GET);
    if( root == nullptr || ly_errno )
    {

        BOOST_LOG_TRIVIAL(error) << "Parsing failed with message " << ly_errmsg();
        BOOST_THROW_EXCEPTION(YDKCodecException{YDKCodecException::Error::XML_INVAL});
    }

    BOOST_LOG_TRIVIAL(trace) << "Performing decode operation";
    RootDataImpl* rd = new RootDataImpl{rs_impl, rs_impl->m_ctx, "/"};
    rd->m_node = root;

    struct lyd_node* dnode = root;
    do
    {
        DataNodeImpl* nodeImpl = new DataNodeImpl{rd, dnode};
        rd->child_map.insert(std::make_pair(root, nodeImpl));
        dnode = dnode->next;
    } while(dnode && dnode != nullptr && dnode != root);

    return rd;
}

