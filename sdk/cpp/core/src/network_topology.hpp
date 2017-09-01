#ifndef _NETWORK_TOPOLOGY_
#define _NETWORK_TOPOLOGY_

#include <memory>
#include <vector>
#include <string>
#include "types.hpp"
#include "errors.hpp"

namespace opendaylight {
namespace network_topology {

class NetworkTopology : public ydk::Entity
{
    public:
        NetworkTopology();
        ~NetworkTopology();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;
        std::shared_ptr<ydk::Entity> clone_ptr() const override;

        class Topology; //type: NetworkTopology::Topology

        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology> > topology;

}; // NetworkTopology


class NetworkTopology::Topology : public ydk::Entity
{
    public:
        Topology();
        ~Topology();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf topology_id; //type: string
        ydk::YLeaf server_provided; //type: boolean
        class TopologyTypes; //type: NetworkTopology::Topology::TopologyTypes
        class UnderlayTopology; //type: NetworkTopology::Topology::UnderlayTopology
        class Node; //type: NetworkTopology::Topology::Node
        class Link; //type: NetworkTopology::Topology::Link

        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Link> > link;
        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node> > node;
        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::TopologyTypes> topology_types;
        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::UnderlayTopology> > underlay_topology;

}; // NetworkTopology::Topology


class NetworkTopology::Topology::TopologyTypes : public ydk::Entity
{
    public:
        TopologyTypes();
        ~TopologyTypes();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        class TopologyNetconf; //type: NetworkTopology::Topology::TopologyTypes::TopologyNetconf

        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::TopologyTypes::TopologyNetconf> topology_netconf;

}; // NetworkTopology::Topology::TopologyTypes


class NetworkTopology::Topology::TopologyTypes::TopologyNetconf : public ydk::Entity
{
    public:
        TopologyNetconf();
        ~TopologyNetconf();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;


}; // NetworkTopology::Topology::TopologyTypes::TopologyNetconf


class NetworkTopology::Topology::UnderlayTopology : public ydk::Entity
{
    public:
        UnderlayTopology();
        ~UnderlayTopology();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        //type: string (refers to opendaylight::network_topology::NetworkTopology::Topology::topology_id)
        ydk::YLeaf topology_ref;

}; // NetworkTopology::Topology::UnderlayTopology


class NetworkTopology::Topology::Node : public ydk::Entity
{
    public:
        Node();
        ~Node();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf node_id; //type: string
        ydk::YLeaf username; //type: string
        ydk::YLeaf password; //type: string
        ydk::YLeaf host; //type: one of string, union
        ydk::YLeaf port; //type: uint16
        ydk::YLeaf tcp_only; //type: boolean
        ydk::YLeaf schemaless; //type: boolean
        ydk::YLeaf reconnect_on_changed_schema; //type: boolean
        ydk::YLeaf connection_timeout_millis; //type: uint32
        ydk::YLeaf default_request_timeout_millis; //type: uint32
        ydk::YLeaf max_connection_attempts; //type: uint32
        ydk::YLeaf between_attempts_timeout_millis; //type: uint16
        ydk::YLeaf sleep_factor; //type: decimal64
        ydk::YLeaf keepalive_delay; //type: uint32
        ydk::YLeaf concurrent_rpc_limit; //type: uint16
        ydk::YLeaf connection_status; //type: ConnectionStatus
        ydk::YLeaf connected_message; //type: string
        ydk::YLeaf schema_cache_directory; //type: string
        class SupportingNode; //type: NetworkTopology::Topology::Node::SupportingNode
        class TerminationPoint; //type: NetworkTopology::Topology::Node::TerminationPoint
        class YangModuleCapabilities; //type: NetworkTopology::Topology::Node::YangModuleCapabilities
        class ClusteredConnectionStatus; //type: NetworkTopology::Topology::Node::ClusteredConnectionStatus
        class AvailableCapabilities; //type: NetworkTopology::Topology::Node::AvailableCapabilities
        class UnavailableCapabilities; //type: NetworkTopology::Topology::Node::UnavailableCapabilities
        class PassThrough; //type: NetworkTopology::Topology::Node::PassThrough
        class YangLibrary; //type: NetworkTopology::Topology::Node::YangLibrary

        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::AvailableCapabilities> available_capabilities;
        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::ClusteredConnectionStatus> clustered_connection_status;
        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::PassThrough> pass_through;
        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::SupportingNode> > supporting_node;
        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::TerminationPoint> > termination_point;
        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::UnavailableCapabilities> unavailable_capabilities;
        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::YangLibrary> yang_library;
        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::YangModuleCapabilities> yang_module_capabilities;
                class ConnectionStatus;

}; // NetworkTopology::Topology::Node


class NetworkTopology::Topology::Node::SupportingNode : public ydk::Entity
{
    public:
        SupportingNode();
        ~SupportingNode();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        //type: string (refers to opendaylight::network_topology::NetworkTopology::Topology::topology_id)
        ydk::YLeaf topology_ref;
        //type: string (refers to opendaylight::network_topology::NetworkTopology::Topology::Node::node_id)
        ydk::YLeaf node_ref;

}; // NetworkTopology::Topology::Node::SupportingNode


class NetworkTopology::Topology::Node::TerminationPoint : public ydk::Entity
{
    public:
        TerminationPoint();
        ~TerminationPoint();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf tp_id; //type: string
        //type: list of  string (refers to opendaylight::network_topology::NetworkTopology::Topology::Node::TerminationPoint::tp_id)
        ydk::YLeafList tp_ref;

}; // NetworkTopology::Topology::Node::TerminationPoint


class NetworkTopology::Topology::Node::YangModuleCapabilities : public ydk::Entity
{
    public:
        YangModuleCapabilities();
        ~YangModuleCapabilities();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf override; //type: boolean
        ydk::YLeafList capability; //type: list of  string

}; // NetworkTopology::Topology::Node::YangModuleCapabilities


class NetworkTopology::Topology::Node::ClusteredConnectionStatus : public ydk::Entity
{
    public:
        ClusteredConnectionStatus();
        ~ClusteredConnectionStatus();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf netconf_master_node; //type: string
        class NodeStatus; //type: NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus

        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus> > node_status;

}; // NetworkTopology::Topology::Node::ClusteredConnectionStatus


class NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus : public ydk::Entity
{
    public:
        NodeStatus();
        ~NodeStatus();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf node; //type: string
        ydk::YLeaf status; //type: Status
        class Status;

}; // NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus


class NetworkTopology::Topology::Node::AvailableCapabilities : public ydk::Entity
{
    public:
        AvailableCapabilities();
        ~AvailableCapabilities();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        class AvailableCapability; //type: NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability

        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability> > available_capability;

}; // NetworkTopology::Topology::Node::AvailableCapabilities


class NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability : public ydk::Entity
{
    public:
        AvailableCapability();
        ~AvailableCapability();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf capability; //type: string
        ydk::YLeaf capability_origin; //type: CapabilityOrigin
        class CapabilityOrigin;

}; // NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability


class NetworkTopology::Topology::Node::UnavailableCapabilities : public ydk::Entity
{
    public:
        UnavailableCapabilities();
        ~UnavailableCapabilities();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        class UnavailableCapability; //type: NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability

        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability> > unavailable_capability;

}; // NetworkTopology::Topology::Node::UnavailableCapabilities


class NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability : public ydk::Entity
{
    public:
        UnavailableCapability();
        ~UnavailableCapability();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf capability; //type: string
        ydk::YLeaf failure_reason; //type: FailureReason
        class FailureReason;

}; // NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability


class NetworkTopology::Topology::Node::PassThrough : public ydk::Entity
{
    public:
        PassThrough();
        ~PassThrough();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;


}; // NetworkTopology::Topology::Node::PassThrough


class NetworkTopology::Topology::Node::YangLibrary : public ydk::Entity
{
    public:
        YangLibrary();
        ~YangLibrary();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf yang_library_url; //type: string
        ydk::YLeaf username; //type: string
        ydk::YLeaf password; //type: string

}; // NetworkTopology::Topology::Node::YangLibrary


class NetworkTopology::Topology::Link : public ydk::Entity
{
    public:
        Link();
        ~Link();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        ydk::YLeaf link_id; //type: string
        class Source; //type: NetworkTopology::Topology::Link::Source
        class Destination; //type: NetworkTopology::Topology::Link::Destination
        class SupportingLink; //type: NetworkTopology::Topology::Link::SupportingLink

        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Link::Destination> destination;
        std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Link::Source> source;
        std::vector<std::shared_ptr<opendaylight::network_topology::NetworkTopology::Topology::Link::SupportingLink> > supporting_link;

}; // NetworkTopology::Topology::Link


class NetworkTopology::Topology::Link::Source : public ydk::Entity
{
    public:
        Source();
        ~Source();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        //type: string (refers to opendaylight::network_topology::NetworkTopology::Topology::Node::node_id)
        ydk::YLeaf source_node;
        //type: string (refers to opendaylight::network_topology::NetworkTopology::Topology::Node::TerminationPoint::tp_id)
        ydk::YLeaf source_tp;

}; // NetworkTopology::Topology::Link::Source


class NetworkTopology::Topology::Link::Destination : public ydk::Entity
{
    public:
        Destination();
        ~Destination();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        //type: string (refers to network_topology::NetworkTopology::Topology::Node::node_id)
        ydk::YLeaf dest_node;
        //type: string (refers to network_topology::NetworkTopology::Topology::Node::TerminationPoint::tp_id)
        ydk::YLeaf dest_tp;

}; // NetworkTopology::Topology::Link::Destination


class NetworkTopology::Topology::Link::SupportingLink : public ydk::Entity
{
    public:
        SupportingLink();
        ~SupportingLink();

        bool has_data() const override;
        bool has_operation() const override;
        std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;
        std::string get_segment_path() const override;
        std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;
        void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;
        void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;
        std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;
        bool has_leaf_or_child_of_name(const std::string & name) const override;

        //type: string (refers to opendaylight::network_topology::NetworkTopology::Topology::Link::link_id)
        ydk::YLeaf link_ref;

}; // NetworkTopology::Topology::Link::SupportingLink

class NetworkTopology::Topology::Node::ConnectionStatus : public ydk::Enum
{
    public:
        static const ydk::Enum::YLeaf connecting;
        static const ydk::Enum::YLeaf connected;
        static const ydk::Enum::YLeaf unable_to_connect;

};

class NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::Status : public ydk::Enum
{
    public:
        static const ydk::Enum::YLeaf connected;
        static const ydk::Enum::YLeaf unavailable;
        static const ydk::Enum::YLeaf failed;

};

class NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::CapabilityOrigin : public ydk::Enum
{
    public:
        static const ydk::Enum::YLeaf user_defined;
        static const ydk::Enum::YLeaf device_advertised;

};

class NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::FailureReason : public ydk::Enum
{
    public:
        static const ydk::Enum::YLeaf missing_source;
        static const ydk::Enum::YLeaf unable_to_resolve;

};


}
}

#endif /* _NETWORK_TOPOLOGY_ */

