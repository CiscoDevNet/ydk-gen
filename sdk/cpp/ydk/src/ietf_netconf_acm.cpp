
#include <sstream>
#include <iostream>
#include "entity_util.hpp"
#include "ietf_netconf_acm.hpp"

namespace ydk {
namespace ietf_netconf_acm {

Nacm::Groups::Group::Group()
    : 
	name{YType::str, "name"},
	 user_name{YType::str, "user-name"}
{
    yang_name = "group"; yang_parent_name = "groups";
}

Nacm::Groups::Group::~Group()
{
}

bool Nacm::Groups::Group::has_data() const
{
    for (auto const & leaf : user_name.getValues())
    {
        if(leaf.is_set)
            return true;
    }
    return name.is_set;
}

std::string Nacm::Groups::Group::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "group" <<"[name='" <<name.get() <<"']";

    return path_buffer.str();

}

EntityPath Nacm::Groups::Group::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf-acm:nacm/groups/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, std::string> > leaf_name_values {};
    if (name.is_set) leaf_name_values.push_back(name.get_name_value());
    auto user_name_name_values = user_name.get_name_values();
    leaf_name_values.insert(leaf_name_values.end(), user_name_name_values.begin(), user_name_name_values.end());

    EntityPath entity_path {path_buffer.str(), leaf_name_values};
    return entity_path;

}

Entity* Nacm::Groups::Group::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & Nacm::Groups::Group::get_children()
{
    return children;
}

void Nacm::Groups::Group::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "name")
    {
        name = value;
    }
    if(value_path == "user-name")
    {
        user_name.append(value);
    }
}

Nacm::Groups::Groups()
{
    yang_name = "groups"; yang_parent_name = "nacm";
}

Nacm::Groups::~Groups()
{
}

bool Nacm::Groups::has_data() const
{
    for (std::size_t index=0; index<group.size(); index++)
    {
        if(group[index]->has_data())
            return true;
    }
    return false;
}

std::string Nacm::Groups::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "groups";

    return path_buffer.str();

}

EntityPath Nacm::Groups::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf-acm:nacm/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, std::string> > leaf_name_values {};

    EntityPath entity_path {path_buffer.str(), leaf_name_values};
    return entity_path;

}

Entity* Nacm::Groups::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "group")
    {
        for(auto const & c : group)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<Nacm::Groups::Group>();
        c->parent = this;
        group.push_back(std::move(c));
        children[segment_path] = group.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & Nacm::Groups::get_children()
{
    for (auto const & c : group)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void Nacm::Groups::set_value(const std::string & value_path, std::string value)
{
}

Nacm::RuleList::Rule::Rule()
    : 
	name{YType::str, "name"},
	 access_operations{YType::str, "access-operations"},
	 action{YType::enumeration, "action"},
	 comment{YType::str, "comment"},
	 module_name{YType::str, "module-name"},
	 notification_name{YType::str, "notification-name"},
	 path{YType::str, "path"},
	 rpc_name{YType::str, "rpc-name"}
{
    yang_name = "rule"; yang_parent_name = "rule-list";
}

Nacm::RuleList::Rule::~Rule()
{
}

bool Nacm::RuleList::Rule::has_data() const
{
    return name.is_set || access_operations.is_set || action.is_set || comment.is_set || module_name.is_set || notification_name.is_set || path.is_set || rpc_name.is_set;
}

std::string Nacm::RuleList::Rule::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "rule" <<"[name='" <<name.get() <<"']";

    return path_buffer.str();

}

EntityPath Nacm::RuleList::Rule::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, std::string> > leaf_name_values {};
    if (name.is_set) leaf_name_values.push_back(name.get_name_value());
    if (access_operations.is_set) leaf_name_values.push_back(access_operations.get_name_value());
    if (action.is_set) leaf_name_values.push_back(action.get_name_value());
    if (comment.is_set) leaf_name_values.push_back(comment.get_name_value());
    if (module_name.is_set) leaf_name_values.push_back(module_name.get_name_value());
    if (notification_name.is_set) leaf_name_values.push_back(notification_name.get_name_value());
    if (path.is_set) leaf_name_values.push_back(path.get_name_value());
    if (rpc_name.is_set) leaf_name_values.push_back(rpc_name.get_name_value());

    EntityPath entity_path {path_buffer.str(), leaf_name_values};
    return entity_path;

}

Entity* Nacm::RuleList::Rule::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & Nacm::RuleList::Rule::get_children()
{
    return children;
}

void Nacm::RuleList::Rule::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "name")
    {
        name = value;
    }
    if(value_path == "access-operations")
    {
        access_operations = value;
    }
    if(value_path == "action")
    {
        action = value;
    }
    if(value_path == "comment")
    {
        comment = value;
    }
    if(value_path == "module-name")
    {
        module_name = value;
    }
    if(value_path == "notification-name")
    {
        notification_name = value;
    }
    if(value_path == "path")
    {
        path = value;
    }
    if(value_path == "rpc-name")
    {
        rpc_name = value;
    }
}

Nacm::RuleList::RuleList()
    : 
	name{YType::str, "name"},
	 group{YType::str, "group"}
{
    yang_name = "rule-list"; yang_parent_name = "nacm";
}

Nacm::RuleList::~RuleList()
{
}

bool Nacm::RuleList::has_data() const
{
    for (std::size_t index=0; index<rule.size(); index++)
    {
        if(rule[index]->has_data())
            return true;
    }
    for (auto const & leaf : group.getValues())
    {
        if(leaf.is_set)
            return true;
    }
    return name.is_set;
}

std::string Nacm::RuleList::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "rule-list" <<"[name='" <<name.get() <<"']";

    return path_buffer.str();

}

EntityPath Nacm::RuleList::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "ietf-netconf-acm:nacm/" << get_segment_path();
    }
    else
    {
        get_relative_entity_path(this, ancestor, path_buffer);
    }

    std::vector<std::pair<std::string, std::string> > leaf_name_values {};
    if (name.is_set) leaf_name_values.push_back(name.get_name_value());
    auto group_name_values = group.get_name_values();
    leaf_name_values.insert(leaf_name_values.end(), group_name_values.begin(), group_name_values.end());

    EntityPath entity_path {path_buffer.str(), leaf_name_values};
    return entity_path;

}

Entity* Nacm::RuleList::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "rule")
    {
        for(auto const & c : rule)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<Nacm::RuleList::Rule>();
        c->parent = this;
        rule.push_back(std::move(c));
        children[segment_path] = rule.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & Nacm::RuleList::get_children()
{
    for (auto const & c : rule)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void Nacm::RuleList::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "name")
    {
        name = value;
    }
    if(value_path == "group")
    {
        group.append(value);
    }
}

Nacm::Nacm()
    : 
	denied_data_writes{YType::uint32, "denied-data-writes"},
	 denied_notifications{YType::uint32, "denied-notifications"},
	 denied_operations{YType::uint32, "denied-operations"},
	 enable_external_groups{YType::boolean, "enable-external-groups"},
	 enable_nacm{YType::boolean, "enable-nacm"},
	 exec_default{YType::enumeration, "exec-default"},
	 read_default{YType::enumeration, "read-default"},
	 write_default{YType::enumeration, "write-default"}
    , 		groups(std::make_unique<Nacm::Groups>())
{
    groups->parent = this;
    children["groups"] = groups.get();

    yang_name = "nacm"; yang_parent_name = "ietf-netconf-acm";
}

Nacm::~Nacm()
{
}

bool Nacm::has_data() const
{
    for (std::size_t index=0; index<rule_list.size(); index++)
    {
        if(rule_list[index]->has_data())
            return true;
    }
    return denied_data_writes.is_set || denied_notifications.is_set || denied_operations.is_set || enable_external_groups.is_set || enable_nacm.is_set || exec_default.is_set || read_default.is_set || write_default.is_set || groups->has_data();
}

std::string Nacm::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "ietf-netconf-acm:nacm";

    return path_buffer.str();

}

EntityPath Nacm::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        BOOST_THROW_EXCEPTION(YDKInvalidArgumentException{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, std::string> > leaf_name_values {};
    if (denied_data_writes.is_set) leaf_name_values.push_back(denied_data_writes.get_name_value());
    if (denied_notifications.is_set) leaf_name_values.push_back(denied_notifications.get_name_value());
    if (denied_operations.is_set) leaf_name_values.push_back(denied_operations.get_name_value());
    if (enable_external_groups.is_set) leaf_name_values.push_back(enable_external_groups.get_name_value());
    if (enable_nacm.is_set) leaf_name_values.push_back(enable_nacm.get_name_value());
    if (exec_default.is_set) leaf_name_values.push_back(exec_default.get_name_value());
    if (read_default.is_set) leaf_name_values.push_back(read_default.get_name_value());
    if (write_default.is_set) leaf_name_values.push_back(write_default.get_name_value());

    EntityPath entity_path {path_buffer.str(), leaf_name_values};
    return entity_path;

}

Entity* Nacm::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "groups")
    {
        if(groups != nullptr)
        {
            children["groups"] = groups.get();
        }
        else
        {
            groups = std::make_unique<Nacm::Groups>();
            groups->parent = this;
            children["groups"] = groups.get();
        }
        return children.at("groups");
    }

    if(child_yang_name == "rule-list")
    {
        for(auto const & c : rule_list)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<Nacm::RuleList>();
        c->parent = this;
        rule_list.push_back(std::move(c));
        children[segment_path] = rule_list.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & Nacm::get_children()
{
    if(children.find("groups") == children.end())
    {
        if(groups != nullptr)
        {
            children["groups"] = groups.get();
        }
    }

    for (auto const & c : rule_list)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void Nacm::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "denied-data-writes")
    {
        denied_data_writes = value;
    }
    if(value_path == "denied-notifications")
    {
        denied_notifications = value;
    }
    if(value_path == "denied-operations")
    {
        denied_operations = value;
    }
    if(value_path == "enable-external-groups")
    {
        enable_external_groups = value;
    }
    if(value_path == "enable-nacm")
    {
        enable_nacm = value;
    }
    if(value_path == "exec-default")
    {
        exec_default = value;
    }
    if(value_path == "read-default")
    {
        read_default = value;
    }
    if(value_path == "write-default")
    {
        write_default = value;
    }
}

std::unique_ptr<Entity> Nacm::clone_ptr()
{
    return std::make_unique<Nacm>();
}

const Enum::Value ActionTypeEnum::permit {0, "permit"};
const Enum::Value ActionTypeEnum::deny {1, "deny"};


}
}

