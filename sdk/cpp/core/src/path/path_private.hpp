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

#ifndef YDK_PRIVATE_HPP
#define YDK_PRIVATE_HPP

#include <algorithm>
#include <map>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <cstring>
#include <cassert>
#include <sstream>
#include <regex>

#include "libyang/libyang.h"
#include "libyang/tree_schema.h"
#include "libyang/tree_data.h"
#include "libyang/xml.h"

#include "../path_api.hpp"


namespace ydk {
    namespace path {

        std::vector<std::string> segmentalize(const std::string& path);


        class SchemaNodeImpl : public SchemaNode
        {
        public:
            SchemaNodeImpl(const SchemaNode* parent, struct lys_node* node);

            ~SchemaNodeImpl();

            std::string path() const;

            std::vector<SchemaNode*> find(const std::string& path) const;

            const SchemaNode* parent() const noexcept;

            const std::vector<std::unique_ptr<SchemaNode>>& children() const;

            const SchemaNode& root() const noexcept;

            Statement statement() const;

            std::vector<Statement> keys() const;

            const SchemaNode* m_parent;
            struct lys_node* m_node;
            std::vector<std::unique_ptr<SchemaNode>> m_children;
        };


        class RootSchemaNodeImpl : public RootSchemaNode
        {
        public:
            RootSchemaNodeImpl(struct ly_ctx* ctx) ;

            ~RootSchemaNodeImpl();

            std::vector<SchemaNode*> find(const std::string& path) const;

            const std::vector<std::unique_ptr<SchemaNode>>& children() const;

            DataNode& create(const std::string& path);

            DataNode& create(const std::string& path, const std::string& value);

            std::shared_ptr<Rpc> rpc(const std::string& path) const;


            struct ly_ctx* m_ctx;
            std::vector<std::unique_ptr<DataNode>> m_root_data_nodes;
            std::vector<std::unique_ptr<SchemaNode>> m_children;
        };


        class DataNodeImpl : public DataNode{

        public:
            DataNodeImpl(DataNode* parent, struct lyd_node* node);

            //no copy constructor
            DataNodeImpl(const DataNodeImpl& dn) = delete;

            DataNodeImpl(DataNodeImpl&& dn) = delete;

            virtual ~DataNodeImpl();

            virtual const SchemaNode& schema() const;

            virtual std::string path() const;

            // Create a new data node based on a simple XPath
            // The new node is normally inserted at the end, either as the last child of a parent.
            //
            // If path points to a list key and the list does not exist, the key value from the predicate is used
            // and value is ignored
            //
            // returns the first created or updated node

            virtual DataNode& create(const std::string& path, const std::string& value);

            void set(const std::string& value);

            virtual std::string get() const;

            virtual std::vector<std::shared_ptr<DataNode>> find(const std::string& path) const;

            DataNode* parent() const;

            virtual std::vector<std::shared_ptr<DataNode>> children() const;

            virtual const DataNode& root() const;

            void add_annotation(const Annotation& an);

            bool remove_annotation(const Annotation& an);

            std::vector<Annotation> annotations();

            std::string xml() const;

            std::shared_ptr<DataNode> get_dn_for_desc_node(struct lyd_node* desc_node) const;

        private:

            DataNode& create_helper(const std::string& path, const std::string& value);

        public:

            DataNode* m_parent;
            struct lyd_node* m_node;
            std::map<struct lyd_node*, std::shared_ptr<DataNode>> child_map;

        };


        class RootDataImpl : public DataNodeImpl {
        public:
            RootDataImpl(const SchemaNode& schema, struct ly_ctx* ctx, const std::string path);

            ~RootDataImpl();

            const SchemaNode& schema() const;

            std::string path() const;

            DataNode& create(const std::string& path, const std::string& value);

            void set(const std::string& value);

            std::string get() const;

            std::vector<std::shared_ptr<DataNode>> children() const;

            const DataNode& root() const;

            std::vector<std::shared_ptr<DataNode>> find(const std::string& path) const;


            const SchemaNode& m_schema;
            struct ly_ctx* m_ctx;
            std::string m_path;
        };


        class RpcImpl : public Rpc {
        public:

            RpcImpl(SchemaNodeImpl& sn, struct ly_ctx* ctx);

            ~RpcImpl();

            std::shared_ptr<DataNode> operator()(const ServiceProvider& provider);

            DataNode& input() const;

            SchemaNode& schema() const;


            SchemaNodeImpl& schema_node;
            std::unique_ptr<DataNode> data_node;

        };

    }




}




#endif /* YDK_PRIVATE_HPP */
