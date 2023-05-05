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
#include "../logger.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
/// SchemaNode
///////////////////////////////////////////////////////////////////////////////
ydk::path::SchemaNode::~SchemaNode()
{

}


/////////////////////////////////////////////////////////////////////
// ydk::SchemaNodeImpl
////////////////////////////////////////////////////////////////////
ydk::path::SchemaNodeImpl::SchemaNodeImpl(const SchemaNode* parent, struct lys_node* node):m_parent{parent}, m_node{node}, m_children{}
{
    node->priv = this;
    if(node->nodetype != LYS_LEAF && node->nodetype != LYS_LEAFLIST) {

        const struct lys_node *last = nullptr;

        while( auto q = lys_getnext(last, node, nullptr, 0)) {
            m_children.emplace_back(make_unique<SchemaNodeImpl>(this, const_cast<struct lys_node*>(q)));
            last = q;
        }
    }
}

void
ydk::path::SchemaNodeImpl::populate_augmented_schema_node(vector<lys_node*>& ancestors, struct lys_node* node) {
    if (!ancestors.empty()) {
        auto curr = ancestors.back();
        ancestors.pop_back();
        for (auto &c: m_children) {
            if (c->get_statement().arg == curr->name) {
                reinterpret_cast<SchemaNodeImpl*>(c.get())->populate_augmented_schema_node(ancestors, node);
            }
        }
    }
    else {
        const struct lys_node *last = nullptr;
        while (auto p = lys_getnext(last, node->parent, nullptr, 0)) {
        	last = p;
            while (p->nodetype == LYS_USES) {
                p = p->child;
            }
            if (p) {
                YLOG_DEBUG("Populating new schema node '{}'", string(p->name));
                m_children.emplace_back(make_unique<SchemaNodeImpl>(this, const_cast<struct lys_node*>(p)));
            }
        }
    }
}

ydk::path::SchemaNodeImpl::~SchemaNodeImpl()
{
}

string
ydk::path::SchemaNodeImpl::get_path() const
{
    string ret{};

    vector<string> segments;

    struct lys_node* cur_node = m_node;
    struct lys_module* module = nullptr;

    while(cur_node != nullptr){
    module = cur_node->module;
    if (!cur_node->parent || cur_node->parent->module != module) {
        //qualify with module name

        string segname {module->name};
        segname+=':';
        segname+=cur_node->name;
        segments.push_back(segname);
    } else {
        segments.push_back(cur_node->name);
    }
    cur_node = cur_node->parent;
    }

    reverse(segments.begin(), segments.end());
    for ( auto seg : segments ) {
    ret+="/";
    ret+=seg;

    }

    return ret;
}

vector<ydk::path::SchemaNode*>
ydk::path::SchemaNodeImpl::find(const string& path)
{
    populate_new_schemas_from_path(path);

    if(path.empty())
    {
        YLOG_ERROR("Path is empty");
        throw(YInvalidArgumentError{"path is empty"});
    }

    //has to be a relative path
    if(path.at(0) == '/')
    {
        YLOG_ERROR("Path must be a relative path");
        throw(YInvalidArgumentError{"path must be a relative path"});
    }

    vector<SchemaNode*> ret;
    struct ly_ctx* ctx = m_node->module->ctx;

    const struct lys_node* found_node = ly_ctx_get_node(ctx, m_node, path.c_str(), 0);

    if (found_node)
    {
        auto p = reinterpret_cast<SchemaNode*>(found_node->priv);
        if(p)
        {
            ret.push_back(p);
        }
    }

    return ret;
}

const ydk::path::SchemaNode*
ydk::path::SchemaNodeImpl::get_parent() const noexcept
{
    return m_parent;
}

const vector<unique_ptr<ydk::path::SchemaNode>> &
ydk::path::SchemaNodeImpl::get_children() const
{

    return m_children;
}

const ydk::path::SchemaNode&
ydk::path::SchemaNodeImpl::get_root() const noexcept
{
    if(m_parent == nullptr)
    {
        return *this;
    }
    else
    {
        return m_parent->get_root();
    }
}

void
ydk::path::SchemaNodeImpl::populate_new_schemas_from_path(const string& path) {
	if (path.empty()) return;
    YLOG_DEBUG("Looking to populate schemas for '{}'", path);
    auto snode = const_cast<SchemaNode*>(&get_root());
    auto rsnode = reinterpret_cast<RootSchemaNodeImpl*>(snode);
    rsnode->populate_new_schemas_from_path(path);
}

static bool is_submodule(lys_node* node)
{
    return node->module->type;
}

ydk::path::Statement
ydk::path::SchemaNodeImpl::get_statement() const
{
    Statement s{};
    s.arg = m_node->name;
    s.module_name = m_node->module?m_node->module->name:"";
    if(is_submodule(m_node))
    {
        s.name_space = ((lys_submodule*)m_node->module)->belongsto->ns;
    }
    else
    {
        s.name_space = m_node->module->ns;
    }

    switch(m_node->nodetype) {
    case LYS_CONTAINER:
        s.keyword = "container";
        break;
    case LYS_CHOICE:
    s.keyword = "choice";
    break;
    case LYS_LEAF:
    s.keyword = "leaf";
    break;
    case LYS_LEAFLIST:
    s.keyword = "leaf-list";
    break;
    case LYS_LIST:
        if(m_node->nodetype & LYS_LEAFLIST)
        {
            s.keyword = "leaf-list";
        }
        else
        {
            s.keyword = "list";
        }
        break;
    case LYS_CASE:
        s.keyword = "case";
        break;
    case LYS_NOTIF:
        s.keyword = "notification";
        break;
    case LYS_RPC:
        s.keyword = "rpc";
        break;
    case LYS_INPUT:
        s.keyword = "input";
        break;
    case LYS_OUTPUT:
        s.keyword = "output";
        break;
    case LYS_GROUPING:
        s.keyword = "grouping";
        break;
    case LYS_USES:
        s.keyword = "uses";
        break;
    case LYS_AUGMENT:
        s.keyword = "augment";
        break;
    case LYS_ANYXML:
        s.keyword = "anyxml";
        break;
    case LYS_ACTION:
        s.keyword = "action";
        break;
    case LYS_EXT:
        s.keyword = "extension";
        break;
    case LYS_ANYDATA:
    case LYS_UNKNOWN:
        break;
    }
    return s;
}

///
/// @brief return YANG statement corresponding the the keys
///
/// Returns the vector of Statement keys
/// @return vector of Statement that represent keys
///
vector<ydk::path::Statement>
ydk::path::SchemaNodeImpl::get_keys() const
{
    vector<Statement> stmts{};

    Statement stmt = get_statement();
    if(stmt.keyword == "list") {
        //sanity check
        if(m_node->nodetype != LYS_LIST) {
            YLOG_ERROR("Mismatch in schema");
            throw(YIllegalStateError{"Mismatch in schema"});
        }
        struct lys_node_list *slist = (struct lys_node_list *)m_node;
        for(uint8_t i=0; i < slist->keys_size; ++i) {
            SchemaNode* sn = reinterpret_cast<SchemaNode*>(slist->keys[i]->priv);
            if(sn != nullptr){
                stmts.push_back(sn->get_statement());
            }
        }
    }

    return stmts;
}
