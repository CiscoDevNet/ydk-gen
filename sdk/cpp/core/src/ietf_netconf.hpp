#ifndef _IETF_NETCONF_
#define _IETF_NETCONF_

#include <memory>
#include <vector>
#include <string>
#include "types.hpp"
#include "errors.hpp"

#include "ietf_netconf_with_defaults.hpp"

namespace ydk {
namespace ietf_netconf {

class GetConfigRpc : public Entity
{
    public:
        GetConfigRpc();
        ~GetConfigRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;
        YLeaf with_defaults; //type: WithDefaultsModeEnum


    class Output : public Entity
    {
        public:
            Output();
            ~Output();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;




    }; // GetConfigRpc::Output


    class Source : public Entity
    {
        public:
            Source();
            ~Source();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;
            YLeaf candidate; //type: empty
            YLeaf running; //type: empty
            YLeaf startup; //type: empty




    }; // GetConfigRpc::Source


        std::unique_ptr<ietf_netconf::GetConfigRpc::Output> output;
        std::unique_ptr<ietf_netconf::GetConfigRpc::Source> source;
        class WithDefaultsModeEnum;


}; // GetConfigRpc

class EditConfigRpc : public Entity
{
    public:
        EditConfigRpc();
        ~EditConfigRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;
        YLeaf default_operation; //type: DefaultOperationEnum
        YLeaf test_option; //type: TestOptionEnum
        YLeaf error_option; //type: ErrorOptionEnum
        YLeaf url; //type: string


    class Target : public Entity
    {
        public:
            Target();
            ~Target();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;
            YLeaf candidate; //type: empty
            YLeaf running; //type: empty




    }; // EditConfigRpc::Target


        std::unique_ptr<ietf_netconf::EditConfigRpc::Target> target;
        class DefaultOperationEnum;
        class ErrorOptionEnum;
        class TestOptionEnum;


}; // EditConfigRpc

class CopyConfigRpc : public Entity
{
    public:
        CopyConfigRpc();
        ~CopyConfigRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;
        YLeaf with_defaults; //type: WithDefaultsModeEnum


    class Target : public Entity
    {
        public:
            Target();
            ~Target();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;
            YLeaf candidate; //type: empty
            YLeaf running; //type: empty
            YLeaf startup; //type: empty
            YLeaf url; //type: string




    }; // CopyConfigRpc::Target


    class Source : public Entity
    {
        public:
            Source();
            ~Source();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;
            YLeaf candidate; //type: empty
            YLeaf running; //type: empty
            YLeaf startup; //type: empty
            YLeaf url; //type: string




    }; // CopyConfigRpc::Source


        std::unique_ptr<ietf_netconf::CopyConfigRpc::Source> source;
        std::unique_ptr<ietf_netconf::CopyConfigRpc::Target> target;
        class WithDefaultsModeEnum;


}; // CopyConfigRpc

class DeleteConfigRpc : public Entity
{
    public:
        DeleteConfigRpc();
        ~DeleteConfigRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;


    class Target : public Entity
    {
        public:
            Target();
            ~Target();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;
            YLeaf startup; //type: empty
            YLeaf url; //type: string




    }; // DeleteConfigRpc::Target


        std::unique_ptr<ietf_netconf::DeleteConfigRpc::Target> target;


}; // DeleteConfigRpc

class LockRpc : public Entity
{
    public:
        LockRpc();
        ~LockRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;


    class Target : public Entity
    {
        public:
            Target();
            ~Target();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;
            YLeaf candidate; //type: empty
            YLeaf running; //type: empty
            YLeaf startup; //type: empty




    }; // LockRpc::Target


        std::unique_ptr<ietf_netconf::LockRpc::Target> target;


}; // LockRpc

class UnlockRpc : public Entity
{
    public:
        UnlockRpc();
        ~UnlockRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;


    class Target : public Entity
    {
        public:
            Target();
            ~Target();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;
            YLeaf candidate; //type: empty
            YLeaf running; //type: empty
            YLeaf startup; //type: empty




    }; // UnlockRpc::Target


        std::unique_ptr<ietf_netconf::UnlockRpc::Target> target;


}; // UnlockRpc

class GetRpc : public Entity
{
    public:
        GetRpc();
        ~GetRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;
        YLeaf with_defaults; //type: WithDefaultsModeEnum


    class Output : public Entity
    {
        public:
            Output();
            ~Output();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;




    }; // GetRpc::Output


        std::unique_ptr<ietf_netconf::GetRpc::Output> output;
        class WithDefaultsModeEnum;


}; // GetRpc

class CloseSessionRpc : public Entity
{
    public:
        CloseSessionRpc();
        ~CloseSessionRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;




}; // CloseSessionRpc

class KillSessionRpc : public Entity
{
    public:
        KillSessionRpc();
        ~KillSessionRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;
        YLeaf session_id; //type: uint32




}; // KillSessionRpc

class CommitRpc : public Entity
{
    public:
        CommitRpc();
        ~CommitRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;
        YLeaf confirmed; //type: empty
        YLeaf confirm_timeout; //type: uint32
        YLeaf persist; //type: string
        YLeaf persist_id; //type: string




}; // CommitRpc

class DiscardChangesRpc : public Entity
{
    public:
        DiscardChangesRpc();
        ~DiscardChangesRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;




}; // DiscardChangesRpc

class CancelCommitRpc : public Entity
{
    public:
        CancelCommitRpc();
        ~CancelCommitRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;
        YLeaf persist_id; //type: string




}; // CancelCommitRpc

class ValidateRpc : public Entity
{
    public:
        ValidateRpc();
        ~ValidateRpc();

        bool has_data() const override;
        bool has_operation() const override;
        EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, Entity*> & get_children() override;
        std::unique_ptr<Entity> clone_ptr() override;


    class Source : public Entity
    {
        public:
            Source();
            ~Source();

            bool has_data() const override;
            bool has_operation() const override;
            EntityPath get_entity_path(Entity* parent) const override;
            std::string get_segment_path() const override;
            Entity* get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
            void set_value(const std::string & value_path, std::string value) override;
            std::map<std::string, Entity*> & get_children() override;
            YLeaf candidate; //type: empty
            YLeaf running; //type: empty
            YLeaf startup; //type: empty
            YLeaf url; //type: string




    }; // ValidateRpc::Source


        std::unique_ptr<ietf_netconf::ValidateRpc::Source> source;


}; // ValidateRpc


class ErrorSeverityTypeEnum : public Enum
{
    public:
        static const Enum::YLeaf error;
        static const Enum::YLeaf warning;

};

class ErrorTagTypeEnum : public Enum
{
    public:
        static const Enum::YLeaf in_use;
        static const Enum::YLeaf invalid_value;
        static const Enum::YLeaf too_big;
        static const Enum::YLeaf missing_attribute;
        static const Enum::YLeaf bad_attribute;
        static const Enum::YLeaf unknown_attribute;
        static const Enum::YLeaf missing_element;
        static const Enum::YLeaf bad_element;
        static const Enum::YLeaf unknown_element;
        static const Enum::YLeaf unknown_namespace;
        static const Enum::YLeaf access_denied;
        static const Enum::YLeaf lock_denied;
        static const Enum::YLeaf resource_denied;
        static const Enum::YLeaf rollback_failed;
        static const Enum::YLeaf data_exists;
        static const Enum::YLeaf data_missing;
        static const Enum::YLeaf operation_not_supported;
        static const Enum::YLeaf operation_failed;
        static const Enum::YLeaf partial_operation;
        static const Enum::YLeaf malformed_message;

};

class EditOperationTypeEnum : public Enum
{
    public:
        static const Enum::YLeaf merge;
        static const Enum::YLeaf replace;
        static const Enum::YLeaf create;
        static const Enum::YLeaf delete_;
        static const Enum::YLeaf remove;

};

class EditConfigRpc::DefaultOperationEnum : public Enum
{
    public:
        static const Enum::YLeaf merge;
        static const Enum::YLeaf replace;
        static const Enum::YLeaf none;

};

class EditConfigRpc::TestOptionEnum : public Enum
{
    public:
        static const Enum::YLeaf test_then_set;
        static const Enum::YLeaf set;
        static const Enum::YLeaf test_only;

};

class EditConfigRpc::ErrorOptionEnum : public Enum
{
    public:
        static const Enum::YLeaf stop_on_error;
        static const Enum::YLeaf continue_on_error;
        static const Enum::YLeaf rollback_on_error;

};


}
}

#endif /* _IETF_NETCONF_ */

