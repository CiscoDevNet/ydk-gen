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

namespace ydk
{
	namespace path
	{
		static void check_ly_schema_node_for_path(lyd_node* node, const std::string & path)
		{
			if(node == nullptr || node->schema == nullptr || node->schema->priv == nullptr)
			{
				YLOG_ERROR("Could not fetch schema node '{}'", path);
				throw(YCoreError{"Could not fetch schema node: " + path});
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
/// DataNode
////////////////////////////////////////////////////////////////////////

ydk::path::DataNode::~DataNode()
{

}

ydk::path::DataNode&
ydk::path::DataNode::create_datanode(const std::string& path)
{
    return create_datanode(path, "");
}

////////////////////////////////////////////////////////////////////////////
// class ydk::DataNodeImpl
//////////////////////////////////////////////////////////////////////////
ydk::path::DataNodeImpl::DataNodeImpl(DataNode* parent, lyd_node* node, const std::shared_ptr<RepositoryPtr> & repo): m_parent{parent}, m_node{node}, m_priv_repo{repo}
{
    //add the children
    if(m_node && m_node->child && !(m_node->schema->nodetype == LYS_LEAF ||
                          m_node->schema->nodetype == LYS_LEAFLIST ||
                          m_node->schema->nodetype == LYS_ANYXML))
    {
        lyd_node *iter = nullptr;
        LY_TREE_FOR(m_node->child, iter) {
            child_map.insert(std::make_pair(iter, std::make_shared<DataNodeImpl>(this, iter, m_priv_repo)));
        }
    }

}

ydk::path::DataNodeImpl::~DataNodeImpl()
{
    if (!m_parent)
    {
        lyd_free_withsiblings(m_node);
    }
}

const ydk::path::SchemaNode&
ydk::path::DataNodeImpl::get_schema_node() const
{
    check_ly_schema_node_for_path(m_node, get_path());
    auto schema_ptr = reinterpret_cast<const SchemaNode*>(m_node->schema->priv);
    return *schema_ptr;
}

std::string
ydk::path::DataNodeImpl::get_path() const
{
    char* path = lyd_path(m_node);
    if (!path) {
        return std::string{};
    }
    std::string str{path};
    std::free(path);
    return str;
}

void
ydk::path::DataNodeImpl::populate_new_schemas_from_path(const std::string& path)
{
    check_ly_schema_node_for_path(m_node, path);
    auto snode = reinterpret_cast<SchemaNodeImpl*>(m_node->schema->priv);
    snode->populate_new_schemas_from_path(path);
}

std::shared_ptr<ydk::path::DataNode>
ydk::path::DataNodeImpl::operator()(const ydk::path::Session& session)
{
    return session.invoke(*this);
}

ydk::path::DataNode&
ydk::path::DataNodeImpl::create_action(const std::string& path)
{
    return create_datanode(path, "");
}

ydk::path::DataNode&
ydk::path::DataNodeImpl::create_datanode(const std::string& path, const std::string& value)
{
    std::string v = value;
    if(value.substr(0, 5) == "<?xml") {
        for(unsigned int i = 5; i<value.length(); i++) {
            if (value[i] == '>'){
                v = value.substr(i+1);
                break;
            }
        }

        if(v != value){
            YLOG_DEBUG("Replacing 'value' with '{}'", v);
        }
    }

    YLOG_DEBUG("Populating schemas for {}", path);
    populate_new_schemas_from_path(path);
    YLOG_DEBUG("Populating schemas for {}", v);
    populate_new_schemas_from_path(v);
    return create_helper(path, v);
}

ydk::path::DataNode&
ydk::path::DataNodeImpl::create_helper(const std::string& path, const std::string& value)
{
    if(path.empty())
    {
        YLOG_ERROR("Path is empty.");
        throw(YInvalidArgumentError{"Path is empty."});
    }

    YLOG_DEBUG("Creating node '{}' with value '{}'", path, value);

    std::vector<std::string> segments = segmentalize(path);

    DataNodeImpl* dn = this;

    lyd_node* root_node = m_node;
    while(root_node->parent)
    {
        root_node = root_node->parent;
    }
    std::ostringstream os;
    os << root_node->schema->module->name << ":" << root_node->schema->name;
    std::string top_container_path = os.str();

    size_t start_index = 0;
    auto iter = segments.begin();

    YLOG_DEBUG("Current path: {}", get_path());
    YLOG_DEBUG("Top container path: {}", top_container_path);

    while (iter != segments.end())
    {
        if((*iter) == top_container_path)
        {
            YLOG_DEBUG("Skipping segment same as {}", top_container_path);
            ++iter;
            continue;
        }
        auto r = dn->find(*iter);
        if(r.empty())
        {
            break;
        }
        else if(r.size() != 1)
        {
            YLOG_ERROR("Path {} is ambiguous", path);
            throw(YPathError{YPathError::Error::PATH_AMBIGUOUS});
        }
        else
        {
            if (r[0] == nullptr)
            {
                YLOG_ERROR("Invalid data node: {}", path);
                throw(YCoreError{"Invalid data node"});
            }
            dn = dynamic_cast<DataNodeImpl*>(r[0].get());
            ++iter;
            start_index++;
            YLOG_DEBUG("Found existing datanode with path '{}'", dn->get_path());

            auto s = dn->m_node->schema;
            if (s->nodetype == LYS_LEAFLIST)
            {
                YLOG_ERROR("Duplicate leaf-list item detected: {}", dn->get_path());
                throw(YModelError{"Duplicate leaf-list item detected: " + dn->get_path()});
            }
        }
    }

    if (segments.empty())
    {
        YLOG_ERROR("Path points to existing node: {}", path);
        throw(YInvalidArgumentError{"Path points to existing node: " + path});
    }

    std::vector<lyd_node*> nodes_created;
    lyd_node* first_node_created = nullptr;
    lyd_node* cn = dn->m_node;

    for(size_t i=start_index; i< segments.size(); i++)
    {
        if(segments[i] == top_container_path)
        {
            YLOG_DEBUG("Skipping segment same as {}", top_container_path);
            continue;
        }

        auto child_segment = segments[i];
        if (i != segments.size() - 1)
        {
            YLOG_DEBUG("Creating new data path '{}' in '{}'", child_segment, cn->schema->name);
            cn = lyd_new_path(cn, nullptr, child_segment.c_str(), nullptr, LYD_ANYDATA_SXML, 0);
        }
        else
        {
            YLOG_DEBUG("Creating new data path '{}' with value '{}' in '{}'", child_segment, value, cn->schema->name);
            cn = lyd_new_path(cn, nullptr, child_segment.c_str(), (void*)value.c_str(), LYD_ANYDATA_SXML, 0);
        }

        if (cn == nullptr)
        {
            if(first_node_created)
            {
                lyd_unlink(first_node_created);
                lyd_free(first_node_created);
            }
            YLOG_ERROR("Could not create data node: {} for path: '{}', value: '{}'", segments[i], path, value);
            throw(YModelError{"Could not create data node: " + segments[i]});
        }
        else if (!first_node_created)
        {
            first_node_created = cn;
        }
    }

    if (first_node_created)
    {
        dn->child_map.insert(std::make_pair(first_node_created, std::make_shared<DataNodeImpl>(dn, first_node_created, m_priv_repo)));

        DataNodeImpl* rdn = dynamic_cast<DataNodeImpl*>(dn->child_map[first_node_created].get());

        while(!rdn->get_children().empty() && rdn->m_node != cn)
        {
            rdn = dynamic_cast<DataNodeImpl*>(rdn->get_children()[0].get());
        }

        return *rdn;
    }
    else
    {
        return *dn;
    }
}

void
ydk::path::DataNodeImpl::set_value(const std::string& value)
{
    //set depends on the kind of the node
    lys_node* s_node = m_node->schema;

    if (s_node->nodetype == LYS_LEAF || s_node->nodetype == LYS_LEAFLIST)
    {
        lyd_node_leaf_list* leaf= reinterpret_cast<lyd_node_leaf_list *>(m_node);
        YLOG_DEBUG("Setting leaf value '{}'", value);
        if(lyd_change_leaf(leaf, value.c_str()))
        {
            YLOG_ERROR("Invalid value {}", value);
            throw(YInvalidArgumentError{"Invalid value"});
        }
    }
    else if (s_node->nodetype == LYS_ANYXML)
    {
        lyd_node_anydata* anyxml = reinterpret_cast<lyd_node_anydata *>(m_node);
        anyxml->value.str = value.c_str();
    }
    else
    {
        YLOG_ERROR("Trying to set value {} for a non leaf non anyxml node.", value);
        throw(YInvalidArgumentError{"Cannot set value for this Data Node"});
    }
}

std::string
ydk::path::DataNodeImpl::get_value() const
{
    lys_node* s_node = m_node->schema;
    std::string ret {};
    if (s_node->nodetype == LYS_LEAF || s_node->nodetype == LYS_LEAFLIST) {
        lyd_node_leaf_list* leaf= reinterpret_cast<lyd_node_leaf_list *>(m_node);
        return leaf->value_str;
    } else if (s_node->nodetype == LYS_ANYXML ){
        lyd_node_anydata* anyxml = reinterpret_cast<lyd_node_anydata *>(m_node);
        return anyxml->value.str;
    }
    return ret;
}

std::vector<std::shared_ptr<ydk::path::DataNode>>
ydk::path::DataNodeImpl::find(const std::string& path)
{
    populate_new_schemas_from_path(path);

    std::vector<std::shared_ptr<DataNode>> results;

    if(m_node == nullptr) {
        return results;
    }

    YLOG_DEBUG("Getting child schema with path '{}' in {}", path, get_path());
    const lys_node* found_snode =
        ly_ctx_get_node(m_node->schema->module->ctx, m_node->schema, path.c_str(), 0);

    if(found_snode)
    {
        YLOG_DEBUG("Getting data nodes with path '{}'", path);
        ly_set* result_set = lyd_find_path(m_node, path.c_str());
        if( result_set )
        {
            if (result_set->number > 0)
            {
                for(size_t i=0; i < result_set->number; i++)
                {
                    lyd_node* node_result = result_set->set.d[i];
                    results.push_back(get_dn_for_desc_node(node_result));
                }
            }
            ly_set_free(result_set);
        }
    }

    return results;
}

ydk::path::DataNode*
ydk::path::DataNodeImpl::get_parent() const
{
    return m_parent;
}

std::vector<std::shared_ptr<ydk::path::DataNode>>
ydk::path::DataNodeImpl::get_children() const
{
    std::vector<std::shared_ptr<DataNode>> ret{};
    //the ordering should be determined by the lyd_node
    lyd_node *iter;
    if(m_node && m_node->child && !(m_node->schema->nodetype == LYS_LEAF ||
                          m_node->schema->nodetype == LYS_LEAFLIST ||
                          m_node->schema->nodetype == LYS_ANYXML))
    {
        LY_TREE_FOR(m_node->child, iter){
            auto p = child_map.find(iter);
            if (p != child_map.end()) {
                ret.push_back(p->second);
            }

        }
    }

    return ret;
}

const ydk::path::DataNode&
ydk::path::DataNodeImpl::get_root() const
{
    if(m_parent){
        return m_parent->get_root();
    }
    return *this;
}

std::string
ydk::path::DataNodeImpl::xml() const
{
    std::string ret;
    char* xml = nullptr;
    if(!lyd_print_mem(&xml, m_node,LYD_XML, LYP_FORMAT)) {
        ret = xml;
        std::free(xml);
    }
    return ret;
}

std::shared_ptr<ydk::path::DataNode>
ydk::path::DataNodeImpl::get_dn_for_desc_node(lyd_node* desc_node) const
{
    std::shared_ptr<DataNode> dn;

    //create DataNode wrappers
    std::vector<lyd_node*> nodes{};
    lyd_node* node = desc_node;

    while (node != nullptr && node != m_node)
    {
        nodes.push_back(node);
        node = node->parent;
    }

    //reverse
    std::reverse(nodes.begin(), nodes.end());

    const DataNodeImpl* parent = this;

    if(nodes[0] == m_node)
    {
        nodes.erase(nodes.begin());
    }

    for( auto p : nodes)
    {
       auto res = parent->child_map.find(p);
       if(res != parent->child_map.end())
       {
           //DataNode is already present
           dn = res->second;
       }
       else
       {
           if(!m_node->parent)
           {
               //special case the root is the first node
               parent = dynamic_cast<DataNodeImpl*>(child_map.begin()->second.get());
               auto r = parent->child_map.find(p);

               if(r != parent->child_map.end())
               {
                   res = r;
                   dn = res->second;
               }
               else
               {
                   YLOG_ERROR("Cannot find child DataNode");
                   throw(YCoreError{"Cannot find child!"});
               }
           }
           else
           {
               YLOG_ERROR("Parent is nullptr");
               throw(YCoreError{"Parent is nullptr"});
           }
       }
       parent = dynamic_cast<DataNodeImpl*>(dn.get());
    }

    return dn;
}


void ydk::path::DataNodeImpl::add_annotation(const ydk::path::Annotation& an)
{
    if(!m_node)
    {
        YLOG_ERROR("Cannot annotate uninitialized node");
        throw(YIllegalStateError{"Cannot annotate node"});
    }

    std::string name { an.m_ns + ":" + an.m_name };
    YLOG_DEBUG("Adding annotation '{}' = {} to {}", name, an.m_val, get_path());

    lyd_attr* attr = lyd_insert_attr(m_node, nullptr, name.c_str(), an.m_val.c_str());

    if(attr == nullptr)
    {
        YLOG_ERROR("Cannot find module {}", name.c_str());
        throw(YInvalidArgumentError("Cannot find module with given namespace."));
    }
}


bool
ydk::path::DataNodeImpl::remove_annotation(const ydk::path::Annotation& an)
{
    if(!m_node) {
        return false;
    }

    lyd_attr* attr = m_node->attr;
    while(attr){
        lyd_node* node = attr->parent;
        if (node && node->schema) {
			lys_module* module = node->schema->module;
			if(module){
				Annotation an1{module->ns, attr->name, attr->value_str};
				if (an == an1){
					lyd_free_attr(m_node->schema->module->ctx, m_node, attr, 0);
					return true;
				}
			}
        }
    }

    return false;
}

std::vector<ydk::path::Annotation>
ydk::path::DataNodeImpl::annotations()
{
    std::vector<ydk::path::Annotation> ann {};

    if(m_node) {
        lyd_attr* attr = m_node->attr;
        while(attr) {
            lyd_node* node = attr->parent;
            if (node && node->schema) {
    			lys_module* module = node->schema->module;
				if(module) {
					ann.emplace_back(module->ns, attr->name, attr->value_str);
				}
            }
            attr = attr->next;
        }
    }

    return ann;
}

static bool statement_is_action(const ydk::path::Statement & s)
{
    return (s.keyword == "action");
}

std::string ydk::path::DataNodeImpl::get_action_node_path() const
{
    if(statement_is_action(get_schema_node().get_statement()))
    {
        return get_path();
    }

    for(auto & child_data_node : get_children())
    {
        auto p = child_data_node->get_action_node_path();
        if(p.size() > 0)
        {
            return p;
        }
    }
    return "";
}

bool ydk::path::DataNodeImpl::has_action_node() const
{
    if(statement_is_action(get_schema_node().get_statement()))
    {
        return true;
    }
    for(auto & child_data_node : get_children())
    {
        if(child_data_node->has_action_node())
        {
            return true;
        }
    }
    return false;
}