#ifndef _NETWORK_TOPOLOGY_
#define _NETWORK_TOPOLOGY_

#include <memory>
#include <vector>
#include <string>
#include "types.hpp"
#include "errors.hpp"

namespace ydk {
namespace network_topology {

class NetworkTopology : public Entity
{
    public:
        NetworkTopology();
        ~NetworkTopology();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;
        std::shared_ptr<Entity> clone_ptr() const override;



        class Topology; //type: NetworkTopology::Topology

        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology> > topology;


}; // NetworkTopology


class NetworkTopology::Topology : public Entity
{
    public:
        Topology();
        ~Topology();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf topology_id; //type: string
        YLeaf server_provided; //type: boolean

        class TopologyTypes; //type: NetworkTopology::Topology::TopologyTypes
        class UnderlayTopology; //type: NetworkTopology::Topology::UnderlayTopology
        class Node; //type: NetworkTopology::Topology::Node
        class Link; //type: NetworkTopology::Topology::Link

        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::Link> > link;
        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::Node> > node;
        std::shared_ptr<network_topology::NetworkTopology::Topology::TopologyTypes> topology_types;
        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::UnderlayTopology> > underlay_topology;


}; // NetworkTopology::Topology


class NetworkTopology::Topology::TopologyTypes : public Entity
{
    public:
        TopologyTypes();
        ~TopologyTypes();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;



        class TopologyNetconf; //type: NetworkTopology::Topology::TopologyTypes::TopologyNetconf

        std::shared_ptr<network_topology::NetworkTopology::Topology::TopologyTypes::TopologyNetconf> topology_netconf;


}; // NetworkTopology::Topology::TopologyTypes


class NetworkTopology::Topology::TopologyTypes::TopologyNetconf : public Entity
{
    public:
        TopologyNetconf();
        ~TopologyNetconf();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;





}; // NetworkTopology::Topology::TopologyTypes::TopologyNetconf


class NetworkTopology::Topology::UnderlayTopology : public Entity
{
    public:
        UnderlayTopology();
        ~UnderlayTopology();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        //type: string (refers to network_topology::NetworkTopology::Topology::topology_id)
        YLeaf topology_ref;



}; // NetworkTopology::Topology::UnderlayTopology


class NetworkTopology::Topology::Node : public Entity
{
    public:
        Node();
        ~Node();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf node_id; //type: string
        YLeaf username; //type: string
        YLeaf password; //type: string
        YLeaf host; //type: one of union, string
        YLeaf port; //type: uint16
        YLeaf tcp_only; //type: boolean
        YLeaf schemaless; //type: boolean
        YLeaf reconnect_on_changed_schema; //type: boolean
        YLeaf connection_timeout_millis; //type: uint32
        YLeaf default_request_timeout_millis; //type: uint32
        YLeaf max_connection_attempts; //type: uint32
        YLeaf between_attempts_timeout_millis; //type: uint16
        YLeaf sleep_factor; //type: decimal64
        YLeaf keepalive_delay; //type: uint32
        YLeaf concurrent_rpc_limit; //type: uint16
        YLeaf connection_status; //type: ConnectionStatusEnum
        YLeaf connected_message; //type: string
        YLeaf schema_cache_directory; //type: string

        class SupportingNode; //type: NetworkTopology::Topology::Node::SupportingNode
        class TerminationPoint; //type: NetworkTopology::Topology::Node::TerminationPoint
        class YangModuleCapabilities; //type: NetworkTopology::Topology::Node::YangModuleCapabilities
        class ClusteredConnectionStatus; //type: NetworkTopology::Topology::Node::ClusteredConnectionStatus
        class AvailableCapabilities; //type: NetworkTopology::Topology::Node::AvailableCapabilities
        class UnavailableCapabilities; //type: NetworkTopology::Topology::Node::UnavailableCapabilities
        class PassThrough; //type: NetworkTopology::Topology::Node::PassThrough
        class YangLibrary; //type: NetworkTopology::Topology::Node::YangLibrary

        std::shared_ptr<network_topology::NetworkTopology::Topology::Node::AvailableCapabilities> available_capabilities;
        std::shared_ptr<network_topology::NetworkTopology::Topology::Node::ClusteredConnectionStatus> clustered_connection_status;
        std::shared_ptr<network_topology::NetworkTopology::Topology::Node::PassThrough> pass_through;
        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::Node::SupportingNode> > supporting_node;
        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::Node::TerminationPoint> > termination_point;
        std::shared_ptr<network_topology::NetworkTopology::Topology::Node::UnavailableCapabilities> unavailable_capabilities;
        std::shared_ptr<network_topology::NetworkTopology::Topology::Node::YangLibrary> yang_library;
        std::shared_ptr<network_topology::NetworkTopology::Topology::Node::YangModuleCapabilities> yang_module_capabilities;
        class ConnectionStatusEnum;


}; // NetworkTopology::Topology::Node


class NetworkTopology::Topology::Node::SupportingNode : public Entity
{
    public:
        SupportingNode();
        ~SupportingNode();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        //type: string (refers to network_topology::NetworkTopology::Topology::topology_id)
        YLeaf topology_ref;
        //type: string (refers to network_topology::NetworkTopology::Topology::Node::node_id)
        YLeaf node_ref;



}; // NetworkTopology::Topology::Node::SupportingNode


class NetworkTopology::Topology::Node::TerminationPoint : public Entity
{
    public:
        TerminationPoint();
        ~TerminationPoint();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf tp_id; //type: string
        //type: list of  string (refers to network_topology::NetworkTopology::Topology::Node::TerminationPoint::tp_id)
        YLeafList tp_ref;



}; // NetworkTopology::Topology::Node::TerminationPoint


class NetworkTopology::Topology::Node::YangModuleCapabilities : public Entity
{
    public:
        YangModuleCapabilities();
        ~YangModuleCapabilities();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf override; //type: boolean
        YLeafList capability; //type: list of  string



}; // NetworkTopology::Topology::Node::YangModuleCapabilities


class NetworkTopology::Topology::Node::ClusteredConnectionStatus : public Entity
{
    public:
        ClusteredConnectionStatus();
        ~ClusteredConnectionStatus();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf netconf_master_node; //type: string

        class NodeStatus; //type: NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus

        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus> > node_status;


}; // NetworkTopology::Topology::Node::ClusteredConnectionStatus


class NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus : public Entity
{
    public:
        NodeStatus();
        ~NodeStatus();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf node; //type: string
        YLeaf status; //type: StatusEnum

        class StatusEnum;


}; // NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus


class NetworkTopology::Topology::Node::AvailableCapabilities : public Entity
{
    public:
        AvailableCapabilities();
        ~AvailableCapabilities();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;



        class AvailableCapability; //type: NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability

        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability> > available_capability;


}; // NetworkTopology::Topology::Node::AvailableCapabilities


class NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability : public Entity
{
    public:
        AvailableCapability();
        ~AvailableCapability();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf capability; //type: string
        YLeaf capability_origin; //type: CapabilityOriginEnum

        class CapabilityOriginEnum;


}; // NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability


class NetworkTopology::Topology::Node::UnavailableCapabilities : public Entity
{
    public:
        UnavailableCapabilities();
        ~UnavailableCapabilities();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;



        class UnavailableCapability; //type: NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability

        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability> > unavailable_capability;


}; // NetworkTopology::Topology::Node::UnavailableCapabilities


class NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability : public Entity
{
    public:
        UnavailableCapability();
        ~UnavailableCapability();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf capability; //type: string
        YLeaf failure_reason; //type: FailureReasonEnum

        class FailureReasonEnum;


}; // NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability


class NetworkTopology::Topology::Node::PassThrough : public Entity
{
    public:
        PassThrough();
        ~PassThrough();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;





}; // NetworkTopology::Topology::Node::PassThrough


class NetworkTopology::Topology::Node::YangLibrary : public Entity
{
    public:
        YangLibrary();
        ~YangLibrary();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf yang_library_url; //type: string
        YLeaf username; //type: string
        YLeaf password; //type: string



}; // NetworkTopology::Topology::Node::YangLibrary


class NetworkTopology::Topology::Link : public Entity
{
    public:
        Link();
        ~Link();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        YLeaf link_id; //type: string

        class Source; //type: NetworkTopology::Topology::Link::Source
        class Destination; //type: NetworkTopology::Topology::Link::Destination
        class SupportingLink; //type: NetworkTopology::Topology::Link::SupportingLink

        std::shared_ptr<network_topology::NetworkTopology::Topology::Link::Destination> destination;
        std::shared_ptr<network_topology::NetworkTopology::Topology::Link::Source> source;
        std::vector<std::shared_ptr<network_topology::NetworkTopology::Topology::Link::SupportingLink> > supporting_link;


}; // NetworkTopology::Topology::Link


class NetworkTopology::Topology::Link::Source : public Entity
{
    public:
        Source();
        ~Source();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        //type: string (refers to network_topology::NetworkTopology::Topology::Node::node_id)
        YLeaf source_node;
        //type: string (refers to network_topology::NetworkTopology::Topology::Node::TerminationPoint::tp_id)
        YLeaf source_tp;



}; // NetworkTopology::Topology::Link::Source


class NetworkTopology::Topology::Link::Destination : public Entity
{
    public:
        Destination();
        ~Destination();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        //type: string (refers to network_topology::NetworkTopology::Topology::Node::node_id)
        YLeaf dest_node;
        //type: string (refers to network_topology::NetworkTopology::Topology::Node::TerminationPoint::tp_id)
        YLeaf dest_tp;



}; // NetworkTopology::Topology::Link::Destination


class NetworkTopology::Topology::Link::SupportingLink : public Entity
{
    public:
        SupportingLink();
        ~SupportingLink();

        bool has_data() const override;
        bool has_operation() const override;
        const EntityPath get_entity_path(Entity* parent) const override;
        std::string get_segment_path() const override;
        std::shared_ptr<Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, std::string value) override;
        std::map<std::string, std::shared_ptr<Entity>> get_children() const override;


        //type: string (refers to network_topology::NetworkTopology::Topology::Link::link_id)
        YLeaf link_ref;



}; // NetworkTopology::Topology::Link::SupportingLink

class NetworkTopology::Topology::Node::ConnectionStatusEnum : public Enum
{
    public:
        static const Enum::YLeaf connecting;
        static const Enum::YLeaf connected;
        static const Enum::YLeaf unable_to_connect;

};

class NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::StatusEnum : public Enum
{
    public:
        static const Enum::YLeaf connected;
        static const Enum::YLeaf unavailable;
        static const Enum::YLeaf failed;

};

class NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::CapabilityOriginEnum : public Enum
{
    public:
        static const Enum::YLeaf user_defined;
        static const Enum::YLeaf device_advertised;

};

class NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::FailureReasonEnum : public Enum
{
    public:
        static const Enum::YLeaf missing_source;
        static const Enum::YLeaf unable_to_resolve;

};


}
}

#endif /* _NETWORK_TOPOLOGY_ */

