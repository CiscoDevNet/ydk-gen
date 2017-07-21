#ifndef _MOCK_SP_
#define _MOCK_SP_

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

#include "../src/path_api.hpp"

namespace mock {
class MockServiceProvider : public ydk::path::ServiceProvider
{
public:
    MockServiceProvider(const std::string searchdir, const std::vector<ydk::path::Capability> capabilities) : m_searchdir{searchdir}, m_capabilities{capabilities}
    {
        ydk::path::Repository repo{m_searchdir};
        root_schema = repo.create_root_schema(m_capabilities);
    }

    virtual ~MockServiceProvider()
    {
    }


    ydk::path::RootSchemaNode& get_root_schema() const
    {
        return *root_schema;
    }

    ydk::EncodingFormat get_encoding() const
    {
        return ydk::EncodingFormat::XML;
    }

    ydk::path::DataNodeCollection invoke(ydk::path::Rpc& rpc) const
    {
        ydk::path::Codec s{};

        std::cout << s.encode(rpc.get_input_node(), ydk::EncodingFormat::XML, true) << std::endl;

        return {};
    }
private:
    std::string m_searchdir;
    std::vector<ydk::path::Capability> m_capabilities;
    std::shared_ptr<ydk::path::RootSchemaNode> root_schema;

};
}

namespace ydk {
class TestEntity:public Entity
{
  public:
    TestEntity()
        : name{YType::str, "name"}, enabled{YType::boolean, "enabled"}, bits_field{YType::bits, "bits-field"}, child(std::make_shared<TestEntity::Child>())
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

    const EntityPath get_entity_path(Entity* parent) const
    {
        return {{"test"}, {name.get_name_leafdata(), enabled.get_name_leafdata(), bits_field.get_name_leafdata()}};
    }

    std::shared_ptr<Entity> get_child_by_name(const std::string & child_path, const std::string & u)
    {
        if(child_path == child->get_segment_path())
        {
            if(child != nullptr)
            {
                return child;
            }
            else
            {
                child = std::make_shared<TestEntity::Child>();
                child->parent = this;
                return child;
            }
        }
        return nullptr;
    }

    bool has_leaf_or_child_of_name(const std::string & name) const override
    {
        return false;
    }

    void set_filter(const std::string & name1, YFilter yfilter)
    {
    }

    void set_value(const std::string & name1, const std::string & value, const std::string & value1, const std::string & value2)
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

    std::map<std::string, std::shared_ptr<Entity>> get_children() const
    {
        std::map<std::string, std::shared_ptr<Entity>> children;
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

        bool has_leaf_or_child_of_name(const std::string & name) const override
        {
            return false;
        }

        bool has_operation() const
        {
            return false;
        }

        std::string get_segment_path() const
        {
            return "child";
        }

        const EntityPath get_entity_path(Entity* parent) const
        {
            return {{"child"}, {child_val.get_name_leafdata()}};
        }

        std::shared_ptr<Entity> get_child_by_name(const std::string & child_path, const std::string & u)
        {
            if(child_path == "multi-child")
            {
                for(auto const& ch : multi_child)
                {
                    std::string segment = ch->get_segment_path();
                    if(u == segment)
                    {
                        return ch;
                    }
                }
                auto ch = std::make_shared<TestEntity::Child::MultiChild>();
                ch->parent = this;
                multi_child.push_back(ch);
                return multi_child.back();
            }
            return nullptr;
        }

        std::map<std::string, std::shared_ptr<Entity>> get_children() const
        {
            std::map<std::string, std::shared_ptr<Entity>> children;
            return children;
        }

        void set_filter(const std::string & name1, YFilter yfilter)
        {
        }

        void set_value(const std::string & leaf_name, const std::string & value, const std::string & value1, const std::string & value2)
        {
            if(leaf_name == "child-val")
            {
                child_val = value;
            }
        }

        YLeaf child_val;


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

            bool has_leaf_or_child_of_name(const std::string & name) const override
            {
                return false;
            }

            std::string get_segment_path() const
            {
                return "multi-child[child-key='"+child_key.get()+"']";
            }

            const EntityPath get_entity_path(Entity* parent) const
            {
                return {{"multi-child[child-key='"+child_key.get()+"']"}, {child_key.get_name_leafdata()}};
            }

            std::shared_ptr<Entity> get_child_by_name(const std::string & child_path, const std::string & u)
            {
                return nullptr;
            }

            std::map<std::string, std::shared_ptr<Entity>> get_children() const
            {
                std::map<std::string, std::shared_ptr<Entity>> children;
                return children;
            }

            void set_filter(const std::string & name1, YFilter yfilter)
            {
            }

            void set_value(const std::string& leaf_name, const std::string & value, const std::string & value2, const std::string & value3)
            {
                if(leaf_name == "child-key")
                {
                    child_key = value;
                }
            }

            YLeaf child_key;
        };

        std::vector<std::shared_ptr<TestEntity::Child::MultiChild> > multi_child;
    };

  YLeaf name;
  YLeaf enabled;
  YLeaf bits_field;
  std::shared_ptr<TestEntity::Child> child;
};
}

static std::vector<ydk::path::Capability> test_openconfig {
    {"openconfig-bgp-types", "" },
    {"openconfig-bgp", ""},
    {"openconfig-extensions", ""},
    {"openconfig-interfaces", ""},
    {"openconfig-policy-types", ""},
    {"openconfig-routing-policy", ""},
    {"openconfig-types", ""},
    {"ietf-interfaces", ""},
    {"ietf-netconf", ""},
    {"ietf-netconf-monitoring", ""},
    {"ydk", ""},
    {"ydktest-sanity", ""}

};

#endif
