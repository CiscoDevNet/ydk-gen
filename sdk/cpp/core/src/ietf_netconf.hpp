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
        Value with_defaults; //type: WithDefaultsModeEnum


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
            Value candidate; //type: empty
            Value running; //type: empty
            Value startup; //type: empty




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
        Value default_operation; //type: DefaultOperationEnum
        Value test_option; //type: TestOptionEnum
        Value error_option; //type: ErrorOptionEnum
        Value url; //type: string


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
            Value candidate; //type: empty
            Value running; //type: empty




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
        Value with_defaults; //type: WithDefaultsModeEnum


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
            Value candidate; //type: empty
            Value running; //type: empty
            Value startup; //type: empty
            Value url; //type: string




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
            Value candidate; //type: empty
            Value running; //type: empty
            Value startup; //type: empty
            Value url; //type: string




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
            Value startup; //type: empty
            Value url; //type: string




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
            Value candidate; //type: empty
            Value running; //type: empty
            Value startup; //type: empty




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
            Value candidate; //type: empty
            Value running; //type: empty
            Value startup; //type: empty




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
        Value with_defaults; //type: WithDefaultsModeEnum


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
        Value session_id; //type: uint32




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
        Value confirmed; //type: empty
        Value confirm_timeout; //type: uint32
        Value persist; //type: string
        Value persist_id; //type: string




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
        Value persist_id; //type: string




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
            Value candidate; //type: empty
            Value running; //type: empty
            Value startup; //type: empty
            Value url; //type: string




    }; // ValidateRpc::Source


        std::unique_ptr<ietf_netconf::ValidateRpc::Source> source;


}; // ValidateRpc


class ErrorSeverityTypeEnum : public Enum
{
    public:
        static const Enum::Value error;
        static const Enum::Value warning;

};

class ErrorTagTypeEnum : public Enum
{
    public:
        static const Enum::Value in_use;
        static const Enum::Value invalid_value;
        static const Enum::Value too_big;
        static const Enum::Value missing_attribute;
        static const Enum::Value bad_attribute;
        static const Enum::Value unknown_attribute;
        static const Enum::Value missing_element;
        static const Enum::Value bad_element;
        static const Enum::Value unknown_element;
        static const Enum::Value unknown_namespace;
        static const Enum::Value access_denied;
        static const Enum::Value lock_denied;
        static const Enum::Value resource_denied;
        static const Enum::Value rollback_failed;
        static const Enum::Value data_exists;
        static const Enum::Value data_missing;
        static const Enum::Value operation_not_supported;
        static const Enum::Value operation_failed;
        static const Enum::Value partial_operation;
        static const Enum::Value malformed_message;

};

class EditOperationTypeEnum : public Enum
{
    public:
        static const Enum::Value merge;
        static const Enum::Value replace;
        static const Enum::Value create;
        static const Enum::Value delete_;
        static const Enum::Value remove;

};

class EditConfigRpc::DefaultOperationEnum : public Enum
{
    public:
        static const Enum::Value merge;
        static const Enum::Value replace;
        static const Enum::Value none;

};

class EditConfigRpc::TestOptionEnum : public Enum
{
    public:
        static const Enum::Value test_then_set;
        static const Enum::Value set;
        static const Enum::Value test_only;

};

class EditConfigRpc::ErrorOptionEnum : public Enum
{
    public:
        static const Enum::Value stop_on_error;
        static const Enum::Value continue_on_error;
        static const Enum::Value rollback_on_error;

};


}
}

#endif /* _IETF_NETCONF_ */

