#ifndef _IETF_NETCONF_ACM_
#define _IETF_NETCONF_ACM_

#include <memory>
#include <vector>
#include <string>
#include "types.hpp"
#include "errors.hpp"

namespace ydk {
namespace ietf_netconf_acm {

class Nacm : public Entity
{
    public:
        Nacm();
        ~Nacm();

        bool has_data() const;
        EntityPath get_entity_path(Entity* parent) const;
        std::string get_segment_path() const;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path);
        void set_value(const std::string & value_path, std::string value);
        std::unique_ptr<Entity> clone_ptr();
        std::map<std::string, Entity*> & get_children();
        Value enable_nacm;
        Value read_default;
        Value write_default;
        Value exec_default;
        Value enable_external_groups;
        Value denied_operations;
        Value denied_data_writes;
        Value denied_notifications;


    class Groups : public Entity
    {
        public:
            Groups();
            ~Groups();

            bool has_data() const;
            EntityPath get_entity_path(Entity* parent) const;
            std::string get_segment_path() const;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path);
            void set_value(const std::string & value_path, std::string value);
            std::map<std::string, Entity*> & get_children();


        class Group : public Entity
        {
            public:
                Group();
                ~Group();

                bool has_data() const;
                EntityPath get_entity_path(Entity* parent) const;
                std::string get_segment_path() const;
                Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path);
                void set_value(const std::string & value_path, std::string value);
                std::map<std::string, Entity*> & get_children();
                Value name;
                ValueList user_name;




        }; // Nacm::Groups::Group


            std::vector<std::unique_ptr<ietf_netconf_acm::Nacm::Groups::Group> > group;


    }; // Nacm::Groups


    class RuleList : public Entity
    {
        public:
            RuleList();
            ~RuleList();

            bool has_data() const;
            EntityPath get_entity_path(Entity* parent) const;
            std::string get_segment_path() const;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path);
            void set_value(const std::string & value_path, std::string value);
            std::map<std::string, Entity*> & get_children();
            Value name;
            ValueList group;


        class Rule : public Entity
        {
            public:
                Rule();
                ~Rule();

                bool has_data() const;
                EntityPath get_entity_path(Entity* parent) const;
                std::string get_segment_path() const;
                Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path);
                void set_value(const std::string & value_path, std::string value);
                std::map<std::string, Entity*> & get_children();
                Value name;
                Value module_name;
                Value rpc_name;
                Value notification_name;
                Value path;
                Value access_operations;
                Value action;
                Value comment;


                class ActionTypeEnum;


        }; // Nacm::RuleList::Rule


            std::vector<std::unique_ptr<ietf_netconf_acm::Nacm::RuleList::Rule> > rule;


    }; // Nacm::RuleList


        std::unique_ptr<ietf_netconf_acm::Nacm::Groups> groups;
        std::vector<std::unique_ptr<ietf_netconf_acm::Nacm::RuleList> > rule_list;
        class ActionTypeEnum;
        class ActionTypeEnum;
        class ActionTypeEnum;


}; // Nacm


class ActionTypeEnum : public Enum
{
    public:
        static const Enum::Value permit;
        static const Enum::Value deny;

};


}
}

#endif /* _IETF_NETCONF_ACM_ */

