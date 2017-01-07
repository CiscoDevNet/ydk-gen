/*  ----------------------------------------------------------------
 Copyright 2016 Cisco Systems

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ------------------------------------------------------------------*/
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include "ydk/path_api.hpp"
#include "ydk/netconf_provider.hpp"
#include "ydk/crud_service.hpp"
#include "ydk/types.hpp"

using namespace std;
using namespace pybind11;

PYBIND11_PLUGIN(path)
{
	module m("path", "YDK path module");

	class_<ydk::path::Capability>(m, "Capability")
			.def(init<const string &, const string &>());

	class_<ydk::path::Annotation>(m, "Annotation")
			.def(init<const string &, const string &, const string &>());

	class_<ydk::path::Statement>(m, "Statement")
		.def(init<const string &, const string &>())
		.def(init<>());

	class_<ydk::path::SchemaNode>(m, "SchemaNode")
		.def("path", &ydk::path::SchemaNode::path, return_value_policy::reference)
		.def("parent", &ydk::path::SchemaNode::parent, return_value_policy::reference)
		.def("root", &ydk::path::SchemaNode::root, return_value_policy::reference)
		.def("statement", &ydk::path::SchemaNode::statement, return_value_policy::reference)
		.def("find", &ydk::path::SchemaNode::find, return_value_policy::reference)
		.def("keys", &ydk::path::SchemaNode::keys, return_value_policy::reference);
//		.def("children", &ydk::path::SchemaNode::children);

	class_<ydk::path::DataNode>(m, "DataNode")
		.def("schema", &ydk::path::DataNode::schema, return_value_policy::reference)
		.def("path", &ydk::path::DataNode::path, return_value_policy::reference)
		.def("create", (ydk::path::DataNode* (ydk::path::DataNode::*)(const string&)) &ydk::path::DataNode::create, return_value_policy::reference)
		.def("create_filter", (ydk::path::DataNode* (ydk::path::DataNode::*)(const string&)) &ydk::path::DataNode::create_filter, return_value_policy::reference)
		.def("create", (ydk::path::DataNode* (ydk::path::DataNode::*)(const string&, const string&)) &ydk::path::DataNode::create, return_value_policy::reference)
		.def("create_filter", (ydk::path::DataNode* (ydk::path::DataNode::*)(const string&, const string&)) &ydk::path::DataNode::create_filter, return_value_policy::reference)
		.def("get", &ydk::path::DataNode::get, return_value_policy::reference)
		.def("set", &ydk::path::DataNode::set, return_value_policy::reference)
		.def("children", &ydk::path::DataNode::children, return_value_policy::reference)
		.def("root", &ydk::path::DataNode::root, return_value_policy::reference)
		.def("find", &ydk::path::DataNode::find, return_value_policy::reference)
		.def("add_annotation", &ydk::path::DataNode::add_annotation)
		.def("remove_annotation", &ydk::path::DataNode::remove_annotation)
		.def("annotations", &ydk::path::DataNode::annotations, return_value_policy::reference);

	class_<ydk::path::RootSchemaNode>(m, "RootSchemaNode")
		.def("path", &ydk::path::RootSchemaNode::path, return_value_policy::reference)
		.def("parent", &ydk::path::RootSchemaNode::parent, return_value_policy::reference)
		.def("find", &ydk::path::RootSchemaNode::find, return_value_policy::reference)
		.def("root", &ydk::path::RootSchemaNode::root, return_value_policy::reference)
//		.def("children", &ydk::path::RootSchemaNode::children)
		.def("statement", &ydk::path::SchemaNode::statement, return_value_policy::reference)
		.def("keys", &ydk::path::SchemaNode::keys, return_value_policy::reference)
		.def("create", (ydk::path::DataNode* (ydk::path::RootSchemaNode::*)(const string&) const) &ydk::path::RootSchemaNode::create, return_value_policy::reference)
		.def("create", (ydk::path::DataNode* (ydk::path::RootSchemaNode::*)(const string&, const string&) const) &ydk::path::RootSchemaNode::create, return_value_policy::reference)
		.def("rpc", &ydk::path::RootSchemaNode::rpc, return_value_policy::reference);

	class_<ydk::path::ServiceProvider>(m, "ServiceProvider")
		.def("invoke", &ydk::path::ServiceProvider::invoke, return_value_policy::reference)
		.def("get_root_schema", &ydk::path::ServiceProvider::get_root_schema, return_value_policy::reference);

	class_<ydk::path::Rpc>(m, "Rpc")
		.def("schema", &ydk::path::Rpc::schema, return_value_policy::reference)
		.def("input", &ydk::path::Rpc::input, return_value_policy::reference)
		.def("__call__", &ydk::path::Rpc::operator(), return_value_policy::reference);

	class_<ydk::path::Repository>(m, "Repository")
		.def(init<>())
		.def(init<const string&>())
		.def("create_root_schema", &ydk::path::Repository::create_root_schema, return_value_policy::reference);

	class_<ydk::path::CodecService> codec_service(m, "CodecService");

	codec_service
		.def(init<>())
		.def("encode", &ydk::path::CodecService::encode, return_value_policy::reference)
		.def("decode", &ydk::path::CodecService::decode, return_value_policy::reference);

	enum_<ydk::path::CodecService::Format>(codec_service, "Format")
		.value("XML", ydk::path::CodecService::Format::XML)
		.value("JSON", ydk::path::CodecService::Format::JSON);

	class_<ydk::NetconfServiceProvider>(m, "NetconfServiceProvider", base<ydk::path::ServiceProvider>())
		.def(init<ydk::path::Repository&, string, string, string, int>())
		.def(init<string, string, string, int>())
		.def("invoke", &ydk::NetconfServiceProvider::invoke, return_value_policy::reference)
		.def("get_root_schema", &ydk::NetconfServiceProvider::get_root_schema, return_value_policy::reference);

	class_<ydk::CrudService>(m, "CrudService")
		.def(init<>());

	enum_<ydk::EditOperation>(m, "EditOperation")
		.value("merge", ydk::EditOperation::merge)
		.value("create", ydk::EditOperation::create)
		.value("remove", ydk::EditOperation::remove)
		.value("delete", ydk::EditOperation::delete_)
		.value("replace", ydk::EditOperation::replace)
		.value("not_set", ydk::EditOperation::not_set)
		;

	class_<ydk::Empty>(m, "Empty");

	class_<ydk::LeafData>(m, "LeafData")
		.def(init<string, ydk::EditOperation, bool>())
		.def_readonly("value", &ydk::LeafData::value, return_value_policy::reference)
		.def_readonly("operation", &ydk::LeafData::operation, return_value_policy::reference)
		.def_readonly("is_set", &ydk::LeafData::is_set, return_value_policy::reference)
		.def(self == self);

	class_<ydk::Entity>(m, "Entity")
		.def("get_entity_path", &ydk::Entity::get_entity_path, return_value_policy::reference)
		.def("get_segment_path", &ydk::Entity::get_segment_path, return_value_policy::reference)
		.def("get_child_by_name", &ydk::Entity::get_child_by_name, return_value_policy::reference)
		.def("set_value", &ydk::Entity::set_value, return_value_policy::reference)
		.def("has_data", &ydk::Entity::has_data, return_value_policy::reference)
		.def("has_operation", &ydk::Entity::has_operation, return_value_policy::reference)
		.def("get_children", &ydk::Entity::get_children, return_value_policy::reference)
		.def("clone_ptr", &ydk::Entity::clone_ptr, return_value_policy::reference);

	class_<ydk::EntityPath>(m, "EntityPath")
		.def(init<string, vector<pair<std::string, ydk::LeafData> > >())
		.def_readonly("path", &ydk::EntityPath::path, return_value_policy::reference)
		.def_readonly("value_paths", &ydk::EntityPath::value_paths, return_value_policy::reference)
		.def(self == self);

	return m.ptr();
};
