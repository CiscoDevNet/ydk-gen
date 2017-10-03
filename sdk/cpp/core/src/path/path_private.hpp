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
#include <cstdlib>
#include <functional>
#include <cstring>
#include <cassert>
#include <unordered_set>
#include <unordered_map>

#include "libyang/libyang.h"
#include "libyang/tree_schema.h"
#include "libyang/tree_data.h"
#include "libyang/xml.h"

#include "../path_api.hpp"


namespace ydk {
    namespace path {

        std::vector<std::string> segmentalize(const std::string& path);

        std::unordered_set<std::string> segmentalize_module_names(const std::string& value);

        class RepositoryPtr : public std::enable_shared_from_this<RepositoryPtr> {
        public:
            explicit RepositoryPtr(ModelCachingOption caching_option);
            explicit RepositoryPtr(const std::string& search_dir, ModelCachingOption caching_option);
            ~RepositoryPtr();

            std::shared_ptr<RootSchemaNode> create_root_schema(const std::unordered_map<std::string, path::Capability>& lookup_table,
                                                               const std::vector<path::Capability>& caps_to_load);

            void add_model_provider(ModelProvider* model_provider);
            void remove_model_provider(ModelProvider* model_provider);
            std::vector<ModelProvider*> get_model_providers() const;

            std::vector<const lys_module*> get_new_ly_modules_from_lookup(ly_ctx* ctx,
                                                                          const std::unordered_set<std::string>& namespace_module_names,
                                                                          const std::unordered_map<std::string, path::Capability>& lookup_table);
            std::vector<const lys_module*> get_new_ly_modules_from_path(ly_ctx* ctx,
                                                                        const std::string& path,
                                                                        const std::unordered_map<std::string, path::Capability>& lookup_table);

        public:
            std::string path;

        private:
            ly_ctx* create_ly_context();

            void load_module_from_capabilities(ly_ctx* ctx, const std::vector<path::Capability>& caps);

            const lys_module* load_module(ly_ctx* ctx, const std::string& module_name);
            const lys_module* load_module(ly_ctx* ctx, const std::string& module_name, bool& new_module);
            const lys_module* load_module(ly_ctx* ctx, ydk::path::Capability& capability);
            const lys_module* load_module(ly_ctx* ctx, ydk::path::Capability& capability, bool& new_module);
            const lys_module* load_module(ly_ctx* ctx, const std::string& module_name, const std::string& revision);
            const lys_module* load_module(ly_ctx* ctx, const std::string& module_name, const std::string& revision, const std::vector<std::string>& features, bool& new_module);

         private:
            std::vector<ModelProvider*> model_providers;
            bool using_temp_directory;
            ModelCachingOption caching_option;
        };

        class SchemaNodeImpl : public SchemaNode
        {
        public:
            SchemaNodeImpl(const SchemaNode* parent, struct lys_node* node);

            ~SchemaNodeImpl();

            std::string get_path() const;

            std::vector<SchemaNode*> find(const std::string& path);

            const SchemaNode* get_parent() const noexcept;

            const std::vector<std::unique_ptr<SchemaNode>>& get_children() const;

            const SchemaNode& get_root() const noexcept;

            Statement get_statement() const;

            std::vector<Statement> get_keys() const;

            void populate_augmented_schema_node(std::vector<lys_node*>& ancestors, struct lys_node* target);

            void populate_new_schemas_from_path(const std::string& path);

            const SchemaNode* m_parent;
            struct lys_node* m_node;
            std::vector<std::unique_ptr<SchemaNode>> m_children;
        };


        class RootSchemaNodeImpl : public RootSchemaNode
        {
        public:
            explicit RootSchemaNodeImpl(struct ly_ctx* ctx, const std::shared_ptr<RepositoryPtr> & repo);
            explicit RootSchemaNodeImpl(struct ly_ctx* ctx, const std::shared_ptr<RepositoryPtr> & repo,
                                                   const std::unordered_map<std::string, path::Capability>& lookup_table);

            ~RootSchemaNodeImpl();

            std::vector<SchemaNode*> find(const std::string& path);

            const std::vector<std::unique_ptr<SchemaNode>>& get_children() const;

            DataNode& create_datanode(const std::string& path);

            DataNode& create_datanode(const std::string& path, const std::string& value);

            std::shared_ptr<Rpc> create_rpc(const std::string& path);

            void populate_new_schemas_from_path(const std::string& path);
            void populate_new_schemas_from_payload(const std::string& payload, ydk::EncodingFormat format);


            struct ly_ctx* m_ctx;
            std::vector<std::unique_ptr<DataNode>> m_root_data_nodes;
            std::vector<std::unique_ptr<SchemaNode>> m_children;

        private:

            void populate_all_module_schemas();
            void populate_module_schema(const struct lys_module*);
            void populate_augmented_schema_nodes(const struct lys_module* module);
            void populate_augmented_schema_node(std::vector<lys_node*>& ancestors, struct lys_node* target);
            void populate_new_schemas(std::vector<const lys_module*>& new_modules);

            const std::shared_ptr<RepositoryPtr> m_priv_repo;
            const std::unordered_map<std::string, path::Capability> m_name_namespace_lookup;
        };


        class DataNodeImpl : public DataNode{

        public:
            DataNodeImpl(DataNode* parent, struct lyd_node* node, const std::shared_ptr<RepositoryPtr> & repo);

            //no copy constructor
            DataNodeImpl(const DataNodeImpl& dn) = delete;

            DataNodeImpl(DataNodeImpl&& dn) = delete;

            virtual ~DataNodeImpl();

            virtual const SchemaNode& get_schema_node() const;

            virtual std::string get_path() const;

            // Create a new data node based on a simple XPath
            // The new node is normally inserted at the end, either as the last child of a parent.
            //
            // If path points to a list key and the list does not exist, the key value from the predicate is used
            // and value is ignored
            //
            // returns the first created or updated node

            virtual DataNode& create_datanode(const std::string& path, const std::string& value);

            void set_value(const std::string& value);

            virtual std::string get_value() const;

            virtual std::vector<std::shared_ptr<DataNode>> find(const std::string& path);

            DataNode* get_parent() const;

            virtual std::vector<std::shared_ptr<DataNode>> get_children() const;

            virtual const DataNode& get_root() const;

            void add_annotation(const Annotation& an);

            bool remove_annotation(const Annotation& an);

            std::vector<Annotation> annotations();

            std::string xml() const;

            std::shared_ptr<DataNode> get_dn_for_desc_node(struct lyd_node* desc_node) const;

        private:

            DataNode& create_helper(const std::string& path, const std::string& value);

            void populate_new_schemas_from_path(const std::string& path);

        public:

            DataNode* m_parent;
            struct lyd_node* m_node;
            std::map<struct lyd_node*, std::shared_ptr<DataNode>> child_map;

        private:
            const std::shared_ptr<RepositoryPtr> m_priv_repo;

        };


        class RootDataImpl : public DataNodeImpl {
        public:
            RootDataImpl(const SchemaNode& schema, struct ly_ctx* ctx, const std::string & path);
            RootDataImpl(const SchemaNode& schema, struct ly_ctx* ctx, const std::string & path, const std::shared_ptr<RepositoryPtr> & repo);

            ~RootDataImpl();

            const SchemaNode& get_schema_node() const;

            std::string get_path() const;

            DataNode& create_datanode(const std::string& path, const std::string& value);

            void set_value(const std::string& value);

            std::string get_value() const;

            std::vector<std::shared_ptr<DataNode>> get_children() const;

            const DataNode& get_root() const;

            std::vector<std::shared_ptr<DataNode>> find(const std::string& path);

        public:
            const SchemaNode& m_schema;
            struct ly_ctx* m_ctx;
            std::string m_path;

        private:
            void populate_new_schemas_from_path(const std::string& path);

        private:
            const std::shared_ptr<RepositoryPtr> m_priv_repo;
        };


        class RpcImpl : public Rpc {
        public:

            RpcImpl(SchemaNodeImpl& sn, struct ly_ctx* ctx, const std::shared_ptr<RepositoryPtr> & repo);

            ~RpcImpl();

            std::shared_ptr<DataNode> operator()(const Session& session);

            DataNode& get_input_node() const;

            SchemaNode& get_schema_node() const;

            bool has_output_node() const;


            SchemaNodeImpl& schema_node;
            std::unique_ptr<DataNodeImpl> data_node;

        private:
            const std::shared_ptr<RepositoryPtr> m_priv_repo;

        };

    }

}




#endif /* YDK_PRIVATE_HPP */
