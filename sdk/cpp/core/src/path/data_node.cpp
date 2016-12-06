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
#include <boost/log/trivial.hpp>


////////////////////////////////////////////////////////////////////////
/// DataNode
////////////////////////////////////////////////////////////////////////

ydk::path::DataNode::~DataNode()
{

}

ydk::path::DataNode*
ydk::path::DataNode::create(const std::string& path)
{
    return create(path, "");
}


ydk::path::DataNode*
ydk::path::DataNode::create_filter(const std::string& path)
{
    return create_filter(path, "");
}

////////////////////////////////////////////////////////////////////////////
// class ydk::DataNodeImpl
//////////////////////////////////////////////////////////////////////////
ydk::path::DataNodeImpl::DataNodeImpl(DataNode* parent, struct lyd_node* node): m_parent{parent}, m_node{node}
{
	//add the children
    if(m_node && m_node->child && !(m_node->schema->nodetype == LYS_LEAF ||
                          m_node->schema->nodetype == LYS_LEAFLIST ||
                          m_node->schema->nodetype == LYS_ANYXML)){
        struct lyd_node *iter = nullptr;
        LY_TREE_FOR(m_node->child, iter) {
            DataNodeImpl* dn = new DataNodeImpl{this, iter};
            child_map.insert(std::make_pair(iter, dn));
        }
    }

}

ydk::path::DataNodeImpl::~DataNodeImpl()
{
    //first destroy the children
    for (auto p : child_map) {
        delete p.second;
    }

    if(m_node){
        if(m_parent) {
            lyd_free(m_node);
        } else {
            lyd_free_withsiblings(m_node);
        }

        m_node = nullptr;
    }
}

const ydk::path::SchemaNode*
ydk::path::DataNodeImpl::schema() const
{
    return reinterpret_cast<const SchemaNode*>(m_node->schema->priv);
}

std::string
ydk::path::DataNodeImpl::path() const
{
    char* path = lyd_path(m_node);
    if (!path) {
        return std::string{};
    }
    std::string str{path};
    std::free(path);
    return str;
}


ydk::path::DataNode*
ydk::path::DataNodeImpl::create_filter(const std::string& path, const std::string& value)
{
	return create_helper(path, value, true);
}

ydk::path::DataNode*
ydk::path::DataNodeImpl::create(const std::string& path, const std::string& value)
{
	return create_helper(path, value, false);
}

ydk::path::DataNode*
ydk::path::DataNodeImpl::create_helper(const std::string& path, const std::string& value, bool is_filter)
{
    if(path.empty())
    {
        BOOST_LOG_TRIVIAL(error) << "Path is empty.";
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Path is empty."});
    }

    std::vector<std::string> segments = segmentalize(path);

    DataNodeImpl* dn = this;

    struct lyd_node* root_node = m_node;
    while(root_node->parent)
    {
    	root_node = root_node->parent;
    }
    std::ostringstream os;
    os << root_node->schema->module->name << ":" << root_node->schema->name;
    std::string top_container_path = os.str();

    size_t start_index = 0;
    auto iter = segments.begin();

    BOOST_LOG_TRIVIAL(trace) << "Current path: "<<this->schema()->path();
    BOOST_LOG_TRIVIAL(trace) << "Top container path: "<<top_container_path;

    while (iter != segments.end())
    {
    	if((*iter) == top_container_path || (*iter) == m_node->schema->name)
    	{
    		BOOST_LOG_TRIVIAL(trace) << "Skipping segment same as "<<top_container_path;
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
            BOOST_LOG_TRIVIAL(error) << "Path " << path << " is ambiguous";
            BOOST_THROW_EXCEPTION(YCPPPathError{YCPPPathError::Error::PATH_AMBIGUOUS});
        }
        else
        {
            dn = dynamic_cast<DataNodeImpl*>(r[0]);
            if (dn == nullptr)
            {
                BOOST_LOG_TRIVIAL(error) << "Invalid data node";
                BOOST_THROW_EXCEPTION(YCPPCoreError{"Invalid data node"});
            }
            ++iter;
            start_index++;
        }
    }

    if (segments.empty())
    {
        BOOST_LOG_TRIVIAL(error) << "Path " << path << " points to existing node";
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Path points to existing node: " + path});
    }

    std::vector<struct lyd_node*> nodes_created;
    struct lyd_node* first_node_created = nullptr;
    struct lyd_node* cn = dn->m_node;

    for(size_t i=start_index; i< segments.size(); i++)
    {
    	if(segments[i] == top_container_path || segments[i] == m_node->schema->name)
		{
    		BOOST_LOG_TRIVIAL(trace) << "Skipping segment same as "<<top_container_path;
			continue;
		}

    	auto child_segment = segments[i];
    	if(is_filter)
		{
			BOOST_LOG_TRIVIAL(trace) << "Creating new filter path '" << child_segment <<"' in '"<<cn->schema->name<<"'";
			cn = lyd_new_output(cn, nullptr, child_segment.c_str());
		}
    	else if (i != segments.size() - 1)
        {
			BOOST_LOG_TRIVIAL(trace) << "Creating new data path '" << child_segment <<"' in '"<<cn->schema->name<<"'";
			cn = lyd_new_path(cn, nullptr, child_segment.c_str(), nullptr, LYD_ANYDATA_SXML, 0);
		}
        else
        {
			BOOST_LOG_TRIVIAL(trace) << "Creating new data path '" << child_segment <<"', with value '"<<value<<"' in '"<<cn->schema->name<<"'";
			cn = lyd_new_path(cn, nullptr, child_segment.c_str(), (void*)value.c_str(), LYD_ANYDATA_SXML, 0);
		}

		if (cn == nullptr)
		{
            if(first_node_created)
            {
				lyd_unlink(first_node_created);
				lyd_free(first_node_created);
            }
            BOOST_LOG_TRIVIAL(error) << "Invalid path: " + segments[i];
            BOOST_THROW_EXCEPTION(YCPPModelError{"Invalid path: " + segments[i]});
        }
		else if (!first_node_created)
        {
            first_node_created = cn;
        }
    }

    if (first_node_created)
    {
        auto p = new DataNodeImpl{dn, first_node_created};
        dn->child_map.insert(std::make_pair(first_node_created, p));

        DataNodeImpl* rdn = p;

        while(!rdn->children().empty() && rdn->m_node != cn)
        {
            rdn = dynamic_cast<DataNodeImpl*>(rdn->children()[0]);
        }

        return rdn;
    }
    else
    {
        return dn;
    }
}

void
ydk::path::DataNodeImpl::set(const std::string& value)
{
    //set depends on the kind of the node
    struct lys_node* s_node = m_node->schema;

    if (s_node->nodetype == LYS_LEAF || s_node->nodetype == LYS_LEAFLIST)
    {
        struct lyd_node_leaf_list* leaf= reinterpret_cast<struct lyd_node_leaf_list *>(m_node);
        BOOST_LOG_TRIVIAL(trace) << "Setting leaf value '" << value <<"'";
        if(lyd_change_leaf(leaf, value.c_str()))
        {
            BOOST_LOG_TRIVIAL(error) << "Invalid value " << value;
            BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Invalid value"});
        }
    }
    else if (s_node->nodetype == LYS_ANYXML)
    {
        struct lyd_node_anydata* anyxml = reinterpret_cast<struct lyd_node_anydata *>(m_node);
        anyxml->value.str = value.c_str();
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Trying to set value " << value << " for a non leaf non anyxml node.";
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError{"Cannot set value for this Data Node"});
    }
}

std::string
ydk::path::DataNodeImpl::get() const
{
    struct lys_node* s_node = m_node->schema;
    std::string ret {};
    if (s_node->nodetype == LYS_LEAF || s_node->nodetype == LYS_LEAFLIST) {
        struct lyd_node_leaf_list* leaf= reinterpret_cast<struct lyd_node_leaf_list *>(m_node);
        return leaf->value_str;
    } else if (s_node->nodetype == LYS_ANYXML ){
        struct lyd_node_anydata* anyxml = reinterpret_cast<struct lyd_node_anydata *>(m_node);
        return anyxml->value.str;
    }
    return ret;
}

std::vector<ydk::path::DataNode*>
ydk::path::DataNodeImpl::find(const std::string& path) const
{
    std::vector<DataNode*> results;

    if(m_node == nullptr) {
        return results;
    }
    std::string spath{path};

    auto s = schema()->statement();
    if(s.keyword == "rpc"){
        spath="input/" + spath;
    }
    BOOST_LOG_TRIVIAL(trace) << "Getting child schema with path '" << spath <<"' in "<< m_node->schema->name;
    const struct lys_node* found_snode =
        ly_ctx_get_node(m_node->schema->module->ctx, m_node->schema, spath.c_str());

    if(found_snode)
    {
    	BOOST_LOG_TRIVIAL(trace) << "Getting data nodes with path '" << path <<"'";
        struct ly_set* result_set = lyd_find_xpath(m_node, path.c_str());
        if( result_set )
        {
            if (result_set->number > 0)
            {
                for(size_t i=0; i < result_set->number; i++)
                {
                    struct lyd_node* node_result = result_set->set.d[i];
                    results.push_back(get_dn_for_desc_node(node_result));
                }
            }
            ly_set_free(result_set);
        }

    }

    return results;
}

ydk::path::DataNode*
ydk::path::DataNodeImpl::parent() const
{
    return m_parent;
}

std::vector<ydk::path::DataNode*>
ydk::path::DataNodeImpl::children() const
{
    std::vector<DataNode*> ret{};
    //the ordering should be determined by the lyd_node
    struct lyd_node *iter;
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

const ydk::path::DataNode*
ydk::path::DataNodeImpl::root() const
{
    if(m_parent){
        return m_parent->root();
    }
    return this;
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

ydk::path::DataNodeImpl*
ydk::path::DataNodeImpl::get_dn_for_desc_node(struct lyd_node* desc_node) const
{
	DataNodeImpl* dn = nullptr;

	//create DataNode wrappers
	std::vector<struct lyd_node*> nodes{};
	struct lyd_node* node = desc_node;

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
			   parent = child_map.begin()->second;
			   res = parent->child_map.find(p);

			   if(res != parent->child_map.end())
			   {
				   dn = res->second;
			   }
			   else
			   {
				   BOOST_LOG_TRIVIAL(error) << "Cannot find child DataNode";
				   BOOST_THROW_EXCEPTION(YCPPCoreError{"Cannot find child!"});
			   }
		   }
		   else
		   {
			   BOOST_LOG_TRIVIAL(error) << "Parent is nullptr";
			   BOOST_THROW_EXCEPTION(YCPPCoreError{"Parent is nullptr"});
		   }
       }
	   parent = dn;
	}

	return dn;
}


void ydk::path::DataNodeImpl::add_annotation(const ydk::path::Annotation& an)
{
    if(!m_node)
    {
        BOOST_LOG_TRIVIAL(error) << "Cannot annotate uninitialized node";
        BOOST_THROW_EXCEPTION(YCPPIllegalStateError{"Cannot annotate node"});
    }

    std::string name { an.m_ns + ":" + an.m_name };
    BOOST_LOG_TRIVIAL(trace) << "Adding annotation '"<<name<<"="<<an.m_val<< "' to "<< m_node->schema->name;

    struct lyd_attr* attr = lyd_insert_attr(m_node, nullptr, name.c_str(), an.m_val.c_str());

    if(attr == nullptr)
    {
        BOOST_LOG_TRIVIAL(error) << "Cannot find module " << name.c_str();
        BOOST_THROW_EXCEPTION(YCPPInvalidArgumentError("Cannot find module with given namespace."));
    }
}


bool
ydk::path::DataNodeImpl::remove_annotation(const ydk::path::Annotation& an)
{
    if(!m_node) {
        return false;
    }

    struct lyd_attr* attr = m_node->attr;
    while(attr){
        struct lys_module *module = attr->module;
        if(module){
            Annotation an1{module->ns, attr->name, attr->value};
            if (an == an1){
                lyd_free_attr(m_node->schema->module->ctx, m_node, attr, 0);
                return true;
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
        struct lyd_attr* attr = m_node->attr;
        while(attr) {
            struct lys_module *module = attr->module;
            if(module) {
                ann.emplace_back(module->ns, attr->name, attr->value);

            }
            attr = attr->next;
        }
    }


    return ann;
}

