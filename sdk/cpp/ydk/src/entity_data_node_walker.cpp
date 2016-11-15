#include "entity_data_node_walker.hpp"

#include <assert.h>
#include <boost/log/trivial.hpp>
#include <iostream>

#include "path/core_private.hpp"
#include "core.hpp"
#include "types.hpp"

using namespace std;

namespace ydk
{

static void populate_data_node(Entity & entity, path::DataNode* data_node);
static EntityPath get_top_entity_path(Entity & entity);
static void walk_children(Entity & entity, path::DataNode* data_node);
static void populate_name_values(path::DataNode* parent_data_node, EntityPath & path);
static bool data_node_is_leaf(path::DataNode* data_node);
static bool data_node_is_list(path::DataNode* data_node);
static string get_segment_path(const string & path);

//////////////////////////////////////////////////////////////////////////
// DataNode* from Entity
//////////////////////////////////////////////////////////////////////////
path::DataNode* get_data_node_from_entity(Entity & entity, const ydk::path::RootSchemaNode & root_schema)
{
	EntityPath root_path = get_top_entity_path(entity);
	auto root_data_node = root_schema.create(root_path.path);
	BOOST_LOG_TRIVIAL(trace) <<"Root entity: "<<root_path.path;
	populate_name_values(root_data_node, root_path);
	walk_children(entity, root_data_node);
	return root_data_node;
}

static void populate_data_node(Entity & entity, path::DataNode* parent_data_node)
{
	EntityPath path = entity.get_entity_path(entity.parent);
	auto data_node = parent_data_node->create(path.path);

	populate_name_values(data_node, path);
	walk_children(entity, data_node);
}

static void walk_children(Entity & entity, path::DataNode* data_node)
{
	std::map<string, Entity*> & children = entity.get_children();
	BOOST_LOG_TRIVIAL(trace) <<"Children count for: " <<entity.get_entity_path(entity.parent).path<<": "<<children.size();
	for(auto const& child : children)
	{
		BOOST_LOG_TRIVIAL(trace) <<"=================="<<endl;
		BOOST_LOG_TRIVIAL(trace) <<"Looking at child "<<child.second->get_entity_path(child.second->parent).path;
		if(child.second->has_data())
			populate_data_node(*(child.second), data_node);
		else
			BOOST_LOG_TRIVIAL(trace)  <<"Child has no data";
	}
}

static void populate_name_values(path::DataNode* data_node, EntityPath & path)
{
	BOOST_LOG_TRIVIAL(trace) <<"Leaf count: "<<path.value_paths.size();
	for(const std::pair<std::string, std::string> & name_value : path.value_paths)
	{
		auto result = data_node->create(name_value.first, name_value.second);
		BOOST_LOG_TRIVIAL(trace)  <<"Creating child "<<name_value.first<<" of "<<data_node->path()
				<<" with value: \""<<name_value.second<<"\" . Result: "<<(result?"success":"failure");
	}
}

static EntityPath get_top_entity_path(Entity & entity)
{
	if (entity.parent == nullptr)
	{
		return std::move(entity.get_entity_path(nullptr));
	}

	return get_top_entity_path(*entity.parent);
}

//////////////////////////////////////////////////////////////////////////
// Entity from DataNode*
//////////////////////////////////////////////////////////////////////////
void get_entity_from_data_node(path::DataNode * node, Entity* entity)
{
	if (entity == nullptr || node == nullptr)
		return;

	for(path::DataNode* child_data_node:node->children())
	{
		std::string child_name = child_data_node->schema()->statement().arg;
		if(data_node_is_leaf(child_data_node))
		{
			BOOST_LOG_TRIVIAL(trace)  << "Creating leaf "<<child_name << " of value '"
					<< child_data_node->get() <<"' in parent " << node->path();
			entity->set_value(child_name, child_data_node->get());
		}
		else
		{
			BOOST_LOG_TRIVIAL(trace)  << "Going into child "<<child_name <<" in parent " << node->path();
			Entity * child_entity;
			if(data_node_is_list(child_data_node))
			{
				child_entity = entity->get_child_by_name(child_name, get_segment_path(child_data_node->path()));
			}
			else
			{
				child_entity = entity->get_child_by_name(child_name);
			}

			if(child_entity == nullptr)
			{
				BOOST_LOG_TRIVIAL(error)  << "Couldn't fetch child entity "<<child_name<< " in parent "<<node->path() <<"!";
			}
			get_entity_from_data_node(child_data_node, child_entity);
		}
	}
}

static bool data_node_is_leaf(path::DataNode* data_node)
{
	return (data_node->schema()->statement().keyword == "leaf"
			|| data_node->schema()->statement().keyword == "leaf-list");
}

static bool data_node_is_list(path::DataNode* data_node)
{
	return (data_node->schema()->statement().keyword == "list");
}

static string get_segment_path(const string & path)
{
	std::vector<std::string> segments = path::segmentalize(path);
	return segments.back();
}

}
