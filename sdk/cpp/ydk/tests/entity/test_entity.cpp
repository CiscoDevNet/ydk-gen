//
// @file types.hpp
// @brief Header for ydk entity
//
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

#define BOOST_TEST_MODULE EntityTest
#include <boost/test/unit_test.hpp>
#include "../../src/types.hpp"

using namespace ydk;
using namespace std;

class TestEntity:public Entity
{
  public:
	TestEntity()
		: name{YType::str, "name"}, enabled{YType::boolean, "enabled"}, bits_field{YType::bits, "bits-field"}, child(make_unique<TestEntity::Child>())
	{
		yang_name = "test"; yang_parent_name = "";
	}

	~TestEntity()
	{
	}

  public:
	bool has_data() const
	{
		return name.is_set || enabled.is_set;
	}

	bool has_operation() const
	{
		return false;
	}

	std::string get_segment_path() const
	{
		return "test";
	}

	EntityPath get_entity_path(Entity* parent) const
	{
		return {{"test"}, {name.get_name_leafdata(), enabled.get_name_leafdata(), bits_field.get_name_leafdata()}};
	}

	Entity* get_child_by_name(const std::string & child_name, const std::string & child_path)
	{
		if(child_name == child->yang_name)
		{
			if(children.find(child_name) != children.end())
			{
				return children.at(child_name);
			}
			else
			{
				child = make_unique<TestEntity::Child>();
				child->parent = this;
				children[child_name] = child.get();
				return child.get();
			}
		}
		return nullptr;
	}

	void set_value(const std::string & name1, std::string value)
	{
		if(name1 == "name")
		{
			name = value;
		}
		else if(name1 == "enabled")
		{
			enabled = value;
		}
		else if(name1 == "bits-field")
		{
			bits_field[value] = true;
		}
	}

	std::map<std::string, Entity*> & get_children()
	{
		return children;
	}

	class Child:public Entity
	{
	  public:
		Child ()
	  	  : child_val{YType::int32, "child-val"}
		  {
			yang_name = "child"; yang_parent_name = "test";
		  }

		bool has_data() const
		{
			return child_val.is_set;
		}

		bool has_operation() const
		{
			return false;
		}

		std::string get_segment_path() const
		{
			return "child";
		}

		EntityPath get_entity_path(Entity* parent) const
		{
			return {{"child"}, {child_val.get_name_leafdata()}};
		}

		Entity* get_child_by_name(const std::string & child_name, const std::string & child_path)
		{
			if(child_name == "multi-child")
			{
				for(auto const& ch : multi_child)
				{
					string segment = ch->get_segment_path();
					if(child_path == segment)
					{
						if(children.find(child_path)==children.end())
						{
							children[child_path] = ch.get();
						}
						return children.at(child_path);
					}
				}
				auto ch = make_unique<TestEntity::Child::MultiChild>();
				ch->parent = this;
				multi_child.push_back(move(ch));
				children[child_path] = multi_child.back().get();
				return multi_child.back().get();
			}
			return nullptr;
		}

		std::map<std::string, Entity*> & get_children()
		{
			return children;
		}

		void set_value(const std::string & leaf_name, std::string value)
		{
			if(leaf_name == "child-val")
			{
				child_val = value;
			}
		}

		Value child_val;


		class MultiChild : public Entity
		{
		  public:
			MultiChild ()
		  	  : child_key{YType::str, "child-key"}
			  {
				yang_name = "multi-child"; yang_parent_name = "child";
			  }

			bool has_data() const
			{
				return child_key.is_set;
			}

			bool has_operation() const
			{
				return false;
			}

			std::string get_segment_path() const
			{
				return "multi-child[multi-key='"+child_key.get()+"']";
			}

			EntityPath get_entity_path(Entity* parent) const
			{
				return {{"multi-child[multi-key='"+child_key.get()+"']"}, {child_key.get_name_leafdata()}};
			}

			Entity* get_child_by_name(const std::string & child_name, const std::string & child_path)
			{
				return nullptr;
			}

			std::map<std::string, Entity*> & get_children()
			{
				return children;
			}

			void set_value(const std::string& leaf_name, std::string value)
			{
				if(leaf_name == "child-key")
				{
					child_key = value;
				}
			}

			Value child_key;
		};

		vector<unique_ptr<TestEntity::Child::MultiChild> > multi_child;
	};

  Value name;
  Value enabled;
  Value bits_field;
  unique_ptr<TestEntity::Child> child;
};

BOOST_AUTO_TEST_CASE(test_create)
{
	TestEntity test{};
	string test_value = "value for test";
	EntityPath expected {"test",
							{{"name", {test_value, EditOperation::not_set}},
							 {"enabled", {"true", EditOperation::not_set}},
							 {"bits-field", {"bit1 bit2", EditOperation::not_set}}}};

	BOOST_REQUIRE(test.get_entity_path(nullptr).path == "test");
	BOOST_REQUIRE(test.has_data() == false);

	test.name = test_value;
	test.enabled = true;

	test.bits_field["bit1"] = true;
	test.bits_field["bit2"] = true;

	BOOST_REQUIRE(test.has_data() == true);
	BOOST_REQUIRE(test.get_entity_path(nullptr) == expected);
}

BOOST_AUTO_TEST_CASE(test_read)
{
	TestEntity test{};

	BOOST_REQUIRE(test.get_entity_path(nullptr).path == "test");
	BOOST_REQUIRE(test.has_data() == false);

	test.set_value("name", "test test");
	BOOST_REQUIRE(test.has_data() == true);

	auto child = test.get_child_by_name("child", "");
	BOOST_REQUIRE(child != nullptr);

	child->set_value("child-val", "123");
	BOOST_REQUIRE(child->has_data() == true);
}

BOOST_AUTO_TEST_CASE(test_multi_create)
{
	TestEntity test{};

	BOOST_REQUIRE(test.get_entity_path(nullptr).path == "test");
	BOOST_REQUIRE(test.has_data() == false);

	test.set_value("name", "test test");
	BOOST_REQUIRE(test.has_data() == true);

	auto mchild = test.child->get_child_by_name("multi-child", "");
	BOOST_REQUIRE(mchild != nullptr);
}

BOOST_AUTO_TEST_CASE(test_multi_read)
{
	TestEntity test{};

	BOOST_REQUIRE(test.get_entity_path(nullptr).path == "test");
	BOOST_REQUIRE(test.has_data() == false);

	test.set_value("name", "test test");
	BOOST_REQUIRE(test.has_data() == true);

	auto mchild = make_unique<TestEntity::Child::MultiChild>();
	mchild->parent = test.child.get();
	test.child->multi_child.push_back(move(mchild));

	auto m = test.child->get_child_by_name("multi-child", "multi-child[multi-key='abc']");
	BOOST_REQUIRE(m != nullptr);

}
