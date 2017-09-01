
#include <sstream>
#include <iostream>
#include "entity_util.hpp"
#include "network_topology.hpp"

using namespace ydk;

namespace opendaylight {
namespace network_topology {

NetworkTopology::NetworkTopology()
{
    yang_name = "network-topology"; yang_parent_name = "network-topology";
}

NetworkTopology::~NetworkTopology()
{
}

bool NetworkTopology::has_data() const
{
    for (std::size_t index=0; index<topology.size(); index++)
    {
        if(topology[index]->has_data())
            return true;
    }
    return false;
}

bool NetworkTopology::has_operation() const
{
    for (std::size_t index=0; index<topology.size(); index++)
    {
        if(topology[index]->has_operation())
            return true;
    }
    return is_set(yfilter);
}

std::string NetworkTopology::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "network-topology:network-topology";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::get_name_leaf_data() const
{
    return {};

}

std::shared_ptr<Entity> NetworkTopology::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(child_yang_name == "topology")
    {
        for(auto const & c : topology)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology>();
        c->parent = this;
        topology.push_back(c);
        return c;
    }

    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    for (auto const & c : topology)
    {
        children[c->get_segment_path()] = c;
    }

    return children;
}

void NetworkTopology::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
}

void NetworkTopology::set_filter(const std::string & value_path, YFilter yfilter)
{
}

std::shared_ptr<Entity> NetworkTopology::clone_ptr() const
{
    return std::make_shared<NetworkTopology>();
}

bool NetworkTopology::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "topology")
        return true;
    return false;
}

NetworkTopology::Topology::Topology()
    :
    topology_id{YType::str, "topology-id"},
    server_provided{YType::boolean, "server-provided"}
    	,
    topology_types(std::make_shared<NetworkTopology::Topology::TopologyTypes>())
{
    topology_types->parent = this;

    yang_name = "topology"; yang_parent_name = "network-topology";
}

NetworkTopology::Topology::~Topology()
{
}

bool NetworkTopology::Topology::has_data() const
{
    for (std::size_t index=0; index<link.size(); index++)
    {
        if(link[index]->has_data())
            return true;
    }
    for (std::size_t index=0; index<node.size(); index++)
    {
        if(node[index]->has_data())
            return true;
    }
    for (std::size_t index=0; index<underlay_topology.size(); index++)
    {
        if(underlay_topology[index]->has_data())
            return true;
    }
    return topology_id.is_set
	|| server_provided.is_set
	|| (topology_types !=  nullptr && topology_types->has_data());
}

bool NetworkTopology::Topology::has_operation() const
{
    for (std::size_t index=0; index<link.size(); index++)
    {
        if(link[index]->has_operation())
            return true;
    }
    for (std::size_t index=0; index<node.size(); index++)
    {
        if(node[index]->has_operation())
            return true;
    }
    for (std::size_t index=0; index<underlay_topology.size(); index++)
    {
        if(underlay_topology[index]->has_operation())
            return true;
    }
    return is_set(yfilter)
	|| ydk::is_set(topology_id.yfilter)
	|| ydk::is_set(server_provided.yfilter)
	|| (topology_types !=  nullptr && topology_types->has_operation());
}

std::string NetworkTopology::Topology::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "topology" <<"[topology-id='" <<topology_id <<"']";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::get_name_leaf_data() const
{
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (topology_id.is_set || is_set(topology_id.yfilter)) leaf_name_data.push_back(topology_id.get_name_leafdata());
    if (server_provided.is_set || is_set(server_provided.yfilter)) leaf_name_data.push_back(server_provided.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(child_yang_name == "link")
    {
        for(auto const & c : link)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::Link>();
        c->parent = this;
        link.push_back(c);
        return c;
    }

    if(child_yang_name == "node")
    {
        for(auto const & c : node)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::Node>();
        c->parent = this;
        node.push_back(c);
        return c;
    }

    if(child_yang_name == "topology-types")
    {
        if(topology_types == nullptr)
        {
            topology_types = std::make_shared<NetworkTopology::Topology::TopologyTypes>();
        }
        return topology_types;
    }

    if(child_yang_name == "underlay-topology")
    {
        for(auto const & c : underlay_topology)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::UnderlayTopology>();
        c->parent = this;
        underlay_topology.push_back(c);
        return c;
    }

    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    for (auto const & c : link)
    {
        children[c->get_segment_path()] = c;
    }

    for (auto const & c : node)
    {
        children[c->get_segment_path()] = c;
    }

    if(topology_types != nullptr)
    {
        children["topology-types"] = topology_types;
    }

    for (auto const & c : underlay_topology)
    {
        children[c->get_segment_path()] = c;
    }

    return children;
}

void NetworkTopology::Topology::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "topology-id")
    {
        topology_id = value;
        topology_id.value_namespace = name_space;
        topology_id.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "server-provided")
    {
        server_provided = value;
        server_provided.value_namespace = name_space;
        server_provided.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "topology-id")
    {
        topology_id.yfilter = yfilter;
    }
    if(value_path == "server-provided")
    {
        server_provided.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "link" || name == "node" || name == "topology-types" || name == "underlay-topology" || name == "topology-id" || name == "server-provided")
        return true;
    return false;
}

NetworkTopology::Topology::TopologyTypes::TopologyTypes()
    :
    topology_netconf(std::make_shared<NetworkTopology::Topology::TopologyTypes::TopologyNetconf>())
{
    topology_netconf->parent = this;

    yang_name = "topology-types"; yang_parent_name = "topology";
}

NetworkTopology::Topology::TopologyTypes::~TopologyTypes()
{
}

bool NetworkTopology::Topology::TopologyTypes::has_data() const
{
    return (topology_netconf !=  nullptr && topology_netconf->has_data());
}

bool NetworkTopology::Topology::TopologyTypes::has_operation() const
{
    return is_set(yfilter)
	|| (topology_netconf !=  nullptr && topology_netconf->has_operation());
}

std::string NetworkTopology::Topology::TopologyTypes::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "topology-types";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::TopologyTypes::get_name_leaf_data() const
{
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    return leaf_name_data;;
 
}

std::shared_ptr<Entity> NetworkTopology::Topology::TopologyTypes::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(child_yang_name == "topology-netconf")
    {
        if(topology_netconf == nullptr)
        {
            topology_netconf = std::make_shared<NetworkTopology::Topology::TopologyTypes::TopologyNetconf>();
        }
        return topology_netconf;
    }

    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::TopologyTypes::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    if(topology_netconf != nullptr)
    {
        children["topology-netconf"] = topology_netconf;
    }

    return children;
}

void NetworkTopology::Topology::TopologyTypes::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
}

void NetworkTopology::Topology::TopologyTypes::set_filter(const std::string & value_path, YFilter yfilter)
{
}

bool NetworkTopology::Topology::TopologyTypes::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "topology-netconf")
        return true;
    return false;
}

NetworkTopology::Topology::TopologyTypes::TopologyNetconf::TopologyNetconf()
{
    yang_name = "topology-netconf"; yang_parent_name = "topology-types";
}

NetworkTopology::Topology::TopologyTypes::TopologyNetconf::~TopologyNetconf()
{
}

bool NetworkTopology::Topology::TopologyTypes::TopologyNetconf::has_data() const
{
    return false;
}

bool NetworkTopology::Topology::TopologyTypes::TopologyNetconf::has_operation() const
{
    return is_set(yfilter);
}

std::string NetworkTopology::Topology::TopologyTypes::TopologyNetconf::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:topology-netconf";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::TopologyTypes::TopologyNetconf::get_name_leaf_data() const
{
    return {};

}

std::shared_ptr<Entity> NetworkTopology::Topology::TopologyTypes::TopologyNetconf::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::TopologyTypes::TopologyNetconf::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::TopologyTypes::TopologyNetconf::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
}

void NetworkTopology::Topology::TopologyTypes::TopologyNetconf::set_filter(const std::string & value_path, YFilter yfilter)
{
}

bool NetworkTopology::Topology::TopologyTypes::TopologyNetconf::has_leaf_or_child_of_name(const std::string & name) const
{
    return false;
}

NetworkTopology::Topology::UnderlayTopology::UnderlayTopology()
    :
    topology_ref{YType::str, "topology-ref"}
{
    yang_name = "underlay-topology"; yang_parent_name = "topology";
}

NetworkTopology::Topology::UnderlayTopology::~UnderlayTopology()
{
}

bool NetworkTopology::Topology::UnderlayTopology::has_data() const
{
    return topology_ref.is_set;
}

bool NetworkTopology::Topology::UnderlayTopology::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(topology_ref.yfilter);
}

std::string NetworkTopology::Topology::UnderlayTopology::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "underlay-topology" <<"[topology-ref='" <<topology_ref <<"']";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::UnderlayTopology::get_name_leaf_data() const
{
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (topology_ref.is_set || is_set(topology_ref.yfilter)) leaf_name_data.push_back(topology_ref.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::UnderlayTopology::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::UnderlayTopology::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::UnderlayTopology::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "topology-ref")
    {
        topology_ref = value;
        topology_ref.value_namespace = name_space;
        topology_ref.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::UnderlayTopology::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "topology-ref")
    {
        topology_ref.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::UnderlayTopology::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "topology-ref")
        return true;
    return false;
}

NetworkTopology::Topology::Node::Node()
    :
    node_id{YType::str, "node-id"},
    between_attempts_timeout_millis{YType::uint16, "netconf-node-topology:between-attempts-timeout-millis"},
    concurrent_rpc_limit{YType::uint16, "netconf-node-topology:concurrent-rpc-limit"},
    connected_message{YType::str, "netconf-node-topology:connected-message"},
    connection_status{YType::enumeration, "netconf-node-topology:connection-status"},
    connection_timeout_millis{YType::uint32, "netconf-node-topology:connection-timeout-millis"},
    default_request_timeout_millis{YType::uint32, "netconf-node-topology:default-request-timeout-millis"},
    host{YType::str, "netconf-node-topology:host"},
    keepalive_delay{YType::uint32, "netconf-node-topology:keepalive-delay"},
    max_connection_attempts{YType::uint32, "netconf-node-topology:max-connection-attempts"},
    password{YType::str, "netconf-node-topology:password"},
    port{YType::uint16, "netconf-node-topology:port"},
    reconnect_on_changed_schema{YType::boolean, "netconf-node-topology:reconnect-on-changed-schema"},
    schema_cache_directory{YType::str, "netconf-node-topology:schema-cache-directory"},
    schemaless{YType::boolean, "netconf-node-topology:schemaless"},
    sleep_factor{YType::str, "netconf-node-topology:sleep-factor"},
    tcp_only{YType::boolean, "netconf-node-topology:tcp-only"},
    username{YType::str, "netconf-node-topology:username"}
    	,
    available_capabilities(std::make_shared<NetworkTopology::Topology::Node::AvailableCapabilities>())
	,clustered_connection_status(std::make_shared<NetworkTopology::Topology::Node::ClusteredConnectionStatus>())
	,pass_through(std::make_shared<NetworkTopology::Topology::Node::PassThrough>())
	,unavailable_capabilities(std::make_shared<NetworkTopology::Topology::Node::UnavailableCapabilities>())
	,yang_library(std::make_shared<NetworkTopology::Topology::Node::YangLibrary>())
	,yang_module_capabilities(std::make_shared<NetworkTopology::Topology::Node::YangModuleCapabilities>())
{
    available_capabilities->parent = this;

    clustered_connection_status->parent = this;

    pass_through->parent = this;

    unavailable_capabilities->parent = this;

    yang_library->parent = this;

    yang_module_capabilities->parent = this;

    yang_name = "node"; yang_parent_name = "topology";
}

NetworkTopology::Topology::Node::~Node()
{
}

bool NetworkTopology::Topology::Node::has_data() const
{
    for (std::size_t index=0; index<supporting_node.size(); index++)
    {
        if(supporting_node[index]->has_data())
            return true;
    }
    for (std::size_t index=0; index<termination_point.size(); index++)
    {
        if(termination_point[index]->has_data())
            return true;
    }
    return node_id.is_set
	|| between_attempts_timeout_millis.is_set
	|| concurrent_rpc_limit.is_set
	|| connected_message.is_set
	|| connection_status.is_set
	|| connection_timeout_millis.is_set
	|| default_request_timeout_millis.is_set
	|| host.is_set
	|| keepalive_delay.is_set
	|| max_connection_attempts.is_set
	|| password.is_set
	|| port.is_set
	|| reconnect_on_changed_schema.is_set
	|| schema_cache_directory.is_set
	|| schemaless.is_set
	|| sleep_factor.is_set
	|| tcp_only.is_set
	|| username.is_set
	|| (available_capabilities !=  nullptr && available_capabilities->has_data())
	|| (clustered_connection_status !=  nullptr && clustered_connection_status->has_data())
	|| (pass_through !=  nullptr && pass_through->has_data())
	|| (unavailable_capabilities !=  nullptr && unavailable_capabilities->has_data())
	|| (yang_library !=  nullptr && yang_library->has_data())
	|| (yang_module_capabilities !=  nullptr && yang_module_capabilities->has_data());
}

bool NetworkTopology::Topology::Node::has_operation() const
{
    for (std::size_t index=0; index<supporting_node.size(); index++)
    {
        if(supporting_node[index]->has_operation())
            return true;
    }
    for (std::size_t index=0; index<termination_point.size(); index++)
    {
        if(termination_point[index]->has_operation())
            return true;
    }
    return is_set(yfilter)
	|| ydk::is_set(node_id.yfilter)
	|| ydk::is_set(between_attempts_timeout_millis.yfilter)
	|| ydk::is_set(concurrent_rpc_limit.yfilter)
	|| ydk::is_set(connected_message.yfilter)
	|| ydk::is_set(connection_status.yfilter)
	|| ydk::is_set(connection_timeout_millis.yfilter)
	|| ydk::is_set(default_request_timeout_millis.yfilter)
	|| ydk::is_set(host.yfilter)
	|| ydk::is_set(keepalive_delay.yfilter)
	|| ydk::is_set(max_connection_attempts.yfilter)
	|| ydk::is_set(password.yfilter)
	|| ydk::is_set(port.yfilter)
	|| ydk::is_set(reconnect_on_changed_schema.yfilter)
	|| ydk::is_set(schema_cache_directory.yfilter)
	|| ydk::is_set(schemaless.yfilter)
	|| ydk::is_set(sleep_factor.yfilter)
	|| ydk::is_set(tcp_only.yfilter)
	|| ydk::is_set(username.yfilter)
	|| (available_capabilities !=  nullptr && available_capabilities->has_operation())
	|| (clustered_connection_status !=  nullptr && clustered_connection_status->has_operation())
	|| (pass_through !=  nullptr && pass_through->has_operation())
	|| (unavailable_capabilities !=  nullptr && unavailable_capabilities->has_operation())
	|| (yang_library !=  nullptr && yang_library->has_operation())
	|| (yang_module_capabilities !=  nullptr && yang_module_capabilities->has_operation());
}

std::string NetworkTopology::Topology::Node::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "node" <<"[node-id='" <<node_id <<"']";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (node_id.is_set || is_set(node_id.yfilter)) leaf_name_data.push_back(node_id.get_name_leafdata());
    if (between_attempts_timeout_millis.is_set || is_set(between_attempts_timeout_millis.yfilter)) leaf_name_data.push_back(between_attempts_timeout_millis.get_name_leafdata());
    if (concurrent_rpc_limit.is_set || is_set(concurrent_rpc_limit.yfilter)) leaf_name_data.push_back(concurrent_rpc_limit.get_name_leafdata());
    if (connected_message.is_set || is_set(connected_message.yfilter)) leaf_name_data.push_back(connected_message.get_name_leafdata());
    if (connection_status.is_set || is_set(connection_status.yfilter)) leaf_name_data.push_back(connection_status.get_name_leafdata());
    if (connection_timeout_millis.is_set || is_set(connection_timeout_millis.yfilter)) leaf_name_data.push_back(connection_timeout_millis.get_name_leafdata());
    if (default_request_timeout_millis.is_set || is_set(default_request_timeout_millis.yfilter)) leaf_name_data.push_back(default_request_timeout_millis.get_name_leafdata());
    if (host.is_set || is_set(host.yfilter)) leaf_name_data.push_back(host.get_name_leafdata());
    if (keepalive_delay.is_set || is_set(keepalive_delay.yfilter)) leaf_name_data.push_back(keepalive_delay.get_name_leafdata());
    if (max_connection_attempts.is_set || is_set(max_connection_attempts.yfilter)) leaf_name_data.push_back(max_connection_attempts.get_name_leafdata());
    if (password.is_set || is_set(password.yfilter)) leaf_name_data.push_back(password.get_name_leafdata());
    if (port.is_set || is_set(port.yfilter)) leaf_name_data.push_back(port.get_name_leafdata());
    if (reconnect_on_changed_schema.is_set || is_set(reconnect_on_changed_schema.yfilter)) leaf_name_data.push_back(reconnect_on_changed_schema.get_name_leafdata());
    if (schema_cache_directory.is_set || is_set(schema_cache_directory.yfilter)) leaf_name_data.push_back(schema_cache_directory.get_name_leafdata());
    if (schemaless.is_set || is_set(schemaless.yfilter)) leaf_name_data.push_back(schemaless.get_name_leafdata());
    if (sleep_factor.is_set || is_set(sleep_factor.yfilter)) leaf_name_data.push_back(sleep_factor.get_name_leafdata());
    if (tcp_only.is_set || is_set(tcp_only.yfilter)) leaf_name_data.push_back(tcp_only.get_name_leafdata());
    if (username.is_set || is_set(username.yfilter)) leaf_name_data.push_back(username.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(child_yang_name == "available-capabilities")
    {
        if(available_capabilities == nullptr)
        {
            available_capabilities = std::make_shared<NetworkTopology::Topology::Node::AvailableCapabilities>();
        }
        return available_capabilities;
    }

    if(child_yang_name == "clustered-connection-status")
    {
        if(clustered_connection_status == nullptr)
        {
            clustered_connection_status = std::make_shared<NetworkTopology::Topology::Node::ClusteredConnectionStatus>();
        }
        return clustered_connection_status;
    }

    if(child_yang_name == "pass-through")
    {
        if(pass_through == nullptr)
        {
            pass_through = std::make_shared<NetworkTopology::Topology::Node::PassThrough>();
        }
        return pass_through;
    }

    if(child_yang_name == "supporting-node")
    {
        for(auto const & c : supporting_node)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::Node::SupportingNode>();
        c->parent = this;
        supporting_node.push_back(c);
        return c;
    }

    if(child_yang_name == "termination-point")
    {
        for(auto const & c : termination_point)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::Node::TerminationPoint>();
        c->parent = this;
        termination_point.push_back(c);
        return c;
    }

    if(child_yang_name == "unavailable-capabilities")
    {
        if(unavailable_capabilities == nullptr)
        {
            unavailable_capabilities = std::make_shared<NetworkTopology::Topology::Node::UnavailableCapabilities>();
        }
        return unavailable_capabilities;
    }

    if(child_yang_name == "yang-library")
    {
        if(yang_library == nullptr)
        {
            yang_library = std::make_shared<NetworkTopology::Topology::Node::YangLibrary>();
        }
        return yang_library;
    }

    if(child_yang_name == "yang-module-capabilities")
    {
        if(yang_module_capabilities == nullptr)
        {
            yang_module_capabilities = std::make_shared<NetworkTopology::Topology::Node::YangModuleCapabilities>();
        }
        return yang_module_capabilities;
    }

    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    if(available_capabilities != nullptr)
    {
        children["available-capabilities"] = available_capabilities;
    }

    if(clustered_connection_status != nullptr)
    {
        children["clustered-connection-status"] = clustered_connection_status;
    }

    if(pass_through != nullptr)
    {
        children["pass-through"] = pass_through;
    }

    for (auto const & c : supporting_node)
    {
        children[c->get_segment_path()] = c;
    }

    for (auto const & c : termination_point)
    {
        children[c->get_segment_path()] = c;
    }

    if(unavailable_capabilities != nullptr)
    {
        children["unavailable-capabilities"] = unavailable_capabilities;
    }

    if(yang_library != nullptr)
    {
        children["yang-library"] = yang_library;
    }

    if(yang_module_capabilities != nullptr)
    {
        children["yang-module-capabilities"] = yang_module_capabilities;
    }

    return children;
}

void NetworkTopology::Topology::Node::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "node-id")
    {
        node_id = value;
        node_id.value_namespace = name_space;
        node_id.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "between-attempts-timeout-millis")
    {
        between_attempts_timeout_millis = value;
        between_attempts_timeout_millis.value_namespace = name_space;
        between_attempts_timeout_millis.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "concurrent-rpc-limit")
    {
        concurrent_rpc_limit = value;
        concurrent_rpc_limit.value_namespace = name_space;
        concurrent_rpc_limit.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "connected-message")
    {
        connected_message = value;
        connected_message.value_namespace = name_space;
        connected_message.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "connection-status")
    {
        connection_status = value;
        connection_status.value_namespace = name_space;
        connection_status.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "connection-timeout-millis")
    {
        connection_timeout_millis = value;
        connection_timeout_millis.value_namespace = name_space;
        connection_timeout_millis.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "default-request-timeout-millis")
    {
        default_request_timeout_millis = value;
        default_request_timeout_millis.value_namespace = name_space;
        default_request_timeout_millis.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "host")
    {
        host = value;
        host.value_namespace = name_space;
        host.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "keepalive-delay")
    {
        keepalive_delay = value;
        keepalive_delay.value_namespace = name_space;
        keepalive_delay.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "max-connection-attempts")
    {
        max_connection_attempts = value;
        max_connection_attempts.value_namespace = name_space;
        max_connection_attempts.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "password")
    {
        password = value;
        password.value_namespace = name_space;
        password.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "port")
    {
        port = value;
        port.value_namespace = name_space;
        port.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "reconnect-on-changed-schema")
    {
        reconnect_on_changed_schema = value;
        reconnect_on_changed_schema.value_namespace = name_space;
        reconnect_on_changed_schema.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "schema-cache-directory")
    {
        schema_cache_directory = value;
        schema_cache_directory.value_namespace = name_space;
        schema_cache_directory.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "schemaless")
    {
        schemaless = value;
        schemaless.value_namespace = name_space;
        schemaless.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "sleep-factor")
    {
        sleep_factor = value;
        sleep_factor.value_namespace = name_space;
        sleep_factor.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "tcp-only")
    {
        tcp_only = value;
        tcp_only.value_namespace = name_space;
        tcp_only.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "username")
    {
        username = value;
        username.value_namespace = name_space;
        username.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Node::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "node-id")
    {
        node_id.yfilter = yfilter;
    }
    if(value_path == "between-attempts-timeout-millis")
    {
        between_attempts_timeout_millis.yfilter = yfilter;
    }
    if(value_path == "concurrent-rpc-limit")
    {
        concurrent_rpc_limit.yfilter = yfilter;
    }
    if(value_path == "connected-message")
    {
        connected_message.yfilter = yfilter;
    }
    if(value_path == "connection-status")
    {
        connection_status.yfilter = yfilter;
    }
    if(value_path == "connection-timeout-millis")
    {
        connection_timeout_millis.yfilter = yfilter;
    }
    if(value_path == "default-request-timeout-millis")
    {
        default_request_timeout_millis.yfilter = yfilter;
    }
    if(value_path == "host")
    {
        host.yfilter = yfilter;
    }
    if(value_path == "keepalive-delay")
    {
        keepalive_delay.yfilter = yfilter;
    }
    if(value_path == "max-connection-attempts")
    {
        max_connection_attempts.yfilter = yfilter;
    }
    if(value_path == "password")
    {
        password.yfilter = yfilter;
    }
    if(value_path == "port")
    {
        port.yfilter = yfilter;
    }
    if(value_path == "reconnect-on-changed-schema")
    {
        reconnect_on_changed_schema.yfilter = yfilter;
    }
    if(value_path == "schema-cache-directory")
    {
        schema_cache_directory.yfilter = yfilter;
    }
    if(value_path == "schemaless")
    {
        schemaless.yfilter = yfilter;
    }
    if(value_path == "sleep-factor")
    {
        sleep_factor.yfilter = yfilter;
    }
    if(value_path == "tcp-only")
    {
        tcp_only.yfilter = yfilter;
    }
    if(value_path == "username")
    {
        username.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "available-capabilities" || name == "clustered-connection-status" || name == "pass-through" || name == "supporting-node" || name == "termination-point" || name == "unavailable-capabilities" || name == "yang-library" || name == "yang-module-capabilities" || name == "node-id" || name == "between-attempts-timeout-millis" || name == "concurrent-rpc-limit" || name == "connected-message" || name == "connection-status" || name == "connection-timeout-millis" || name == "default-request-timeout-millis" || name == "host" || name == "keepalive-delay" || name == "max-connection-attempts" || name == "password" || name == "port" || name == "reconnect-on-changed-schema" || name == "schema-cache-directory" || name == "schemaless" || name == "sleep-factor" || name == "tcp-only" || name == "username")
        return true;
    return false;
}

NetworkTopology::Topology::Node::SupportingNode::SupportingNode()
    :
    topology_ref{YType::str, "topology-ref"},
    node_ref{YType::str, "node-ref"}
{
    yang_name = "supporting-node"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::SupportingNode::~SupportingNode()
{
}

bool NetworkTopology::Topology::Node::SupportingNode::has_data() const
{
    return topology_ref.is_set
	|| node_ref.is_set;
}

bool NetworkTopology::Topology::Node::SupportingNode::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(topology_ref.yfilter)
	|| ydk::is_set(node_ref.yfilter);
}

std::string NetworkTopology::Topology::Node::SupportingNode::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "supporting-node" <<"[topology-ref='" <<topology_ref <<"']" <<"[node-ref='" <<node_ref <<"']";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::SupportingNode::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (topology_ref.is_set || is_set(topology_ref.yfilter)) leaf_name_data.push_back(topology_ref.get_name_leafdata());
    if (node_ref.is_set || is_set(node_ref.yfilter)) leaf_name_data.push_back(node_ref.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::SupportingNode::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::SupportingNode::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Node::SupportingNode::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "topology-ref")
    {
        topology_ref = value;
        topology_ref.value_namespace = name_space;
        topology_ref.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "node-ref")
    {
        node_ref = value;
        node_ref.value_namespace = name_space;
        node_ref.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Node::SupportingNode::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "topology-ref")
    {
        topology_ref.yfilter = yfilter;
    }
    if(value_path == "node-ref")
    {
        node_ref.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::SupportingNode::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "topology-ref" || name == "node-ref")
        return true;
    return false;
}

NetworkTopology::Topology::Node::TerminationPoint::TerminationPoint()
    :
    tp_id{YType::str, "tp-id"},
    tp_ref{YType::str, "tp-ref"}
{
    yang_name = "termination-point"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::TerminationPoint::~TerminationPoint()
{
}

bool NetworkTopology::Topology::Node::TerminationPoint::has_data() const
{
    for (auto const & leaf : tp_ref.getYLeafs())
    {
        if(leaf.is_set)
            return true;
    }
    return tp_id.is_set;
}

bool NetworkTopology::Topology::Node::TerminationPoint::has_operation() const
{
    for (auto const & leaf : tp_ref.getYLeafs())
    {
        if(is_set(leaf.yfilter))
            return true;
    }
    return is_set(yfilter)
	|| ydk::is_set(tp_id.yfilter)
	|| ydk::is_set(tp_ref.yfilter);
}

std::string NetworkTopology::Topology::Node::TerminationPoint::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "termination-point" <<"[tp-id='" <<tp_id <<"']";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::TerminationPoint::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (tp_id.is_set || is_set(tp_id.yfilter)) leaf_name_data.push_back(tp_id.get_name_leafdata());

    auto tp_ref_name_datas = tp_ref.get_name_leafdata();
    leaf_name_data.insert(leaf_name_data.end(), tp_ref_name_datas.begin(), tp_ref_name_datas.end());

    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::TerminationPoint::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::TerminationPoint::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Node::TerminationPoint::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "tp-id")
    {
        tp_id = value;
        tp_id.value_namespace = name_space;
        tp_id.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "tp-ref")
    {
        tp_ref.append(value);
    }
}

void NetworkTopology::Topology::Node::TerminationPoint::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "tp-id")
    {
        tp_id.yfilter = yfilter;
    }
    if(value_path == "tp-ref")
    {
        tp_ref.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::TerminationPoint::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "tp-id" || name == "tp-ref")
        return true;
    return false;
}

NetworkTopology::Topology::Node::YangModuleCapabilities::YangModuleCapabilities()
    :
    capability{YType::str, "capability"},
    override{YType::boolean, "override"}
{
    yang_name = "yang-module-capabilities"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::YangModuleCapabilities::~YangModuleCapabilities()
{
}

bool NetworkTopology::Topology::Node::YangModuleCapabilities::has_data() const
{
    for (auto const & leaf : capability.getYLeafs())
    {
        if(leaf.is_set)
            return true;
    }
    return override.is_set;
}

bool NetworkTopology::Topology::Node::YangModuleCapabilities::has_operation() const
{
    for (auto const & leaf : capability.getYLeafs())
    {
        if(is_set(leaf.yfilter))
            return true;
    }
    return is_set(yfilter)
	|| ydk::is_set(capability.yfilter)
	|| ydk::is_set(override.yfilter);
}

std::string NetworkTopology::Topology::Node::YangModuleCapabilities::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:yang-module-capabilities";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::YangModuleCapabilities::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (override.is_set || is_set(override.yfilter)) leaf_name_data.push_back(override.get_name_leafdata());

    auto capability_name_datas = capability.get_name_leafdata();
    leaf_name_data.insert(leaf_name_data.end(), capability_name_datas.begin(), capability_name_datas.end());

    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::YangModuleCapabilities::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::YangModuleCapabilities::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Node::YangModuleCapabilities::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "capability")
    {
        capability.append(value);
    }
    if(value_path == "override")
    {
        override = value;
        override.value_namespace = name_space;
        override.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Node::YangModuleCapabilities::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "capability")
    {
        capability.yfilter = yfilter;
    }
    if(value_path == "override")
    {
        override.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::YangModuleCapabilities::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "capability" || name == "override")
        return true;
    return false;
}

NetworkTopology::Topology::Node::ClusteredConnectionStatus::ClusteredConnectionStatus()
    :
    netconf_master_node{YType::str, "netconf-master-node"}
{
    yang_name = "clustered-connection-status"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::ClusteredConnectionStatus::~ClusteredConnectionStatus()
{
}

bool NetworkTopology::Topology::Node::ClusteredConnectionStatus::has_data() const
{
    for (std::size_t index=0; index<node_status.size(); index++)
    {
        if(node_status[index]->has_data())
            return true;
    }
    return netconf_master_node.is_set;
}

bool NetworkTopology::Topology::Node::ClusteredConnectionStatus::has_operation() const
{
    for (std::size_t index=0; index<node_status.size(); index++)
    {
        if(node_status[index]->has_operation())
            return true;
    }
    return is_set(yfilter)
	|| ydk::is_set(netconf_master_node.yfilter);
}

std::string NetworkTopology::Topology::Node::ClusteredConnectionStatus::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:clustered-connection-status";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::ClusteredConnectionStatus::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (netconf_master_node.is_set || is_set(netconf_master_node.yfilter)) leaf_name_data.push_back(netconf_master_node.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::ClusteredConnectionStatus::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(child_yang_name == "node-status")
    {
        for(auto const & c : node_status)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus>();
        c->parent = this;
        node_status.push_back(c);
        return c;
    }

    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::ClusteredConnectionStatus::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    for (auto const & c : node_status)
    {
        children[c->get_segment_path()] = c;
    }

    return children;
}

void NetworkTopology::Topology::Node::ClusteredConnectionStatus::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "netconf-master-node")
    {
        netconf_master_node = value;
        netconf_master_node.value_namespace = name_space;
        netconf_master_node.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Node::ClusteredConnectionStatus::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "netconf-master-node")
    {
        netconf_master_node.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::ClusteredConnectionStatus::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "node-status" || name == "netconf-master-node")
        return true;
    return false;
}

NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::NodeStatus()
    :
    node{YType::str, "node"},
    status{YType::enumeration, "status"}
{
    yang_name = "node-status"; yang_parent_name = "clustered-connection-status";
}

NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::~NodeStatus()
{
}

bool NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::has_data() const
{
    return node.is_set
	|| status.is_set;
}

bool NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(node.yfilter)
	|| ydk::is_set(status.yfilter);
}

std::string NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "node-status";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (node.is_set || is_set(node.yfilter)) leaf_name_data.push_back(node.get_name_leafdata());
    if (status.is_set || is_set(status.yfilter)) leaf_name_data.push_back(status.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "node")
    {
        node = value;
        node.value_namespace = name_space;
        node.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "status")
    {
        status = value;
        status.value_namespace = name_space;
        status.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "node")
    {
        node.yfilter = yfilter;
    }
    if(value_path == "status")
    {
        status.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "node" || name == "status")
        return true;
    return false;
}

NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapabilities()
{
    yang_name = "available-capabilities"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::AvailableCapabilities::~AvailableCapabilities()
{
}

bool NetworkTopology::Topology::Node::AvailableCapabilities::has_data() const
{
    for (std::size_t index=0; index<available_capability.size(); index++)
    {
        if(available_capability[index]->has_data())
            return true;
    }
    return false;
}

bool NetworkTopology::Topology::Node::AvailableCapabilities::has_operation() const
{
    for (std::size_t index=0; index<available_capability.size(); index++)
    {
        if(available_capability[index]->has_operation())
            return true;
    }
    return is_set(yfilter);
}

std::string NetworkTopology::Topology::Node::AvailableCapabilities::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:available-capabilities";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::AvailableCapabilities::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::AvailableCapabilities::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(child_yang_name == "available-capability")
    {
        for(auto const & c : available_capability)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability>();
        c->parent = this;
        available_capability.push_back(c);
        return c;
    }

    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::AvailableCapabilities::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    for (auto const & c : available_capability)
    {
        children[c->get_segment_path()] = c;
    }

    return children;
}

void NetworkTopology::Topology::Node::AvailableCapabilities::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
}

void NetworkTopology::Topology::Node::AvailableCapabilities::set_filter(const std::string & value_path, YFilter yfilter)
{
}

bool NetworkTopology::Topology::Node::AvailableCapabilities::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "available-capability")
        return true;
    return false;
}

NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::AvailableCapability()
    :
    capability{YType::str, "capability"},
    capability_origin{YType::enumeration, "capability-origin"}
{
    yang_name = "available-capability"; yang_parent_name = "available-capabilities";
}

NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::~AvailableCapability()
{
}

bool NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::has_data() const
{
    return capability.is_set
	|| capability_origin.is_set;
}

bool NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(capability.yfilter)
	|| ydk::is_set(capability_origin.yfilter);
}

std::string NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "available-capability";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (capability.is_set || is_set(capability.yfilter)) leaf_name_data.push_back(capability.get_name_leafdata());
    if (capability_origin.is_set || is_set(capability_origin.yfilter)) leaf_name_data.push_back(capability_origin.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "capability")
    {
        capability = value;
        capability.value_namespace = name_space;
        capability.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "capability-origin")
    {
        capability_origin = value;
        capability_origin.value_namespace = name_space;
        capability_origin.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "capability")
    {
        capability.yfilter = yfilter;
    }
    if(value_path == "capability-origin")
    {
        capability_origin.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "capability" || name == "capability-origin")
        return true;
    return false;
}

NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapabilities()
{
    yang_name = "unavailable-capabilities"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::UnavailableCapabilities::~UnavailableCapabilities()
{
}

bool NetworkTopology::Topology::Node::UnavailableCapabilities::has_data() const
{
    for (std::size_t index=0; index<unavailable_capability.size(); index++)
    {
        if(unavailable_capability[index]->has_data())
            return true;
    }
    return false;
}

bool NetworkTopology::Topology::Node::UnavailableCapabilities::has_operation() const
{
    for (std::size_t index=0; index<unavailable_capability.size(); index++)
    {
        if(unavailable_capability[index]->has_operation())
            return true;
    }
    return is_set(yfilter);
}

std::string NetworkTopology::Topology::Node::UnavailableCapabilities::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:unavailable-capabilities";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::UnavailableCapabilities::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::UnavailableCapabilities::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(child_yang_name == "unavailable-capability")
    {
        for(auto const & c : unavailable_capability)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability>();
        c->parent = this;
        unavailable_capability.push_back(c);
        return c;
    }

    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::UnavailableCapabilities::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    for (auto const & c : unavailable_capability)
    {
        children[c->get_segment_path()] = c;
    }

    return children;
}

void NetworkTopology::Topology::Node::UnavailableCapabilities::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
}

void NetworkTopology::Topology::Node::UnavailableCapabilities::set_filter(const std::string & value_path, YFilter yfilter)
{
}

bool NetworkTopology::Topology::Node::UnavailableCapabilities::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "unavailable-capability")
        return true;
    return false;
}

NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::UnavailableCapability()
    :
    capability{YType::str, "capability"},
    failure_reason{YType::enumeration, "failure-reason"}
{
    yang_name = "unavailable-capability"; yang_parent_name = "unavailable-capabilities";
}

NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::~UnavailableCapability()
{
}

bool NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::has_data() const
{
    return capability.is_set
	|| failure_reason.is_set;
}

bool NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(capability.yfilter)
	|| ydk::is_set(failure_reason.yfilter);
}

std::string NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "unavailable-capability";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::get_name_leaf_data() const
{
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (capability.is_set || is_set(capability.yfilter)) leaf_name_data.push_back(capability.get_name_leafdata());
    if (failure_reason.is_set || is_set(failure_reason.yfilter)) leaf_name_data.push_back(failure_reason.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "capability")
    {
        capability = value;
        capability.value_namespace = name_space;
        capability.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "failure-reason")
    {
        failure_reason = value;
        failure_reason.value_namespace = name_space;
        failure_reason.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "capability")
    {
        capability.yfilter = yfilter;
    }
    if(value_path == "failure-reason")
    {
        failure_reason.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "capability" || name == "failure-reason")
        return true;
    return false;
}

NetworkTopology::Topology::Node::PassThrough::PassThrough()
{
    yang_name = "pass-through"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::PassThrough::~PassThrough()
{
}

bool NetworkTopology::Topology::Node::PassThrough::has_data() const
{
    return false;
}

bool NetworkTopology::Topology::Node::PassThrough::has_operation() const
{
    return is_set(yfilter);
}

std::string NetworkTopology::Topology::Node::PassThrough::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:pass-through";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::PassThrough::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::PassThrough::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::PassThrough::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Node::PassThrough::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
}

void NetworkTopology::Topology::Node::PassThrough::set_filter(const std::string & value_path, YFilter yfilter)
{
}

bool NetworkTopology::Topology::Node::PassThrough::has_leaf_or_child_of_name(const std::string & name) const
{
    return false;
}

NetworkTopology::Topology::Node::YangLibrary::YangLibrary()
    :
    password{YType::str, "password"},
    username{YType::str, "username"},
    yang_library_url{YType::str, "yang-library-url"}
{
    yang_name = "yang-library"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::YangLibrary::~YangLibrary()
{
}

bool NetworkTopology::Topology::Node::YangLibrary::has_data() const
{
    return password.is_set
	|| username.is_set
	|| yang_library_url.is_set;
}

bool NetworkTopology::Topology::Node::YangLibrary::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(password.yfilter)
	|| ydk::is_set(username.yfilter)
	|| ydk::is_set(yang_library_url.yfilter);
}

std::string NetworkTopology::Topology::Node::YangLibrary::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:yang-library";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Node::YangLibrary::get_name_leaf_data() const
{


    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (password.is_set || is_set(password.yfilter)) leaf_name_data.push_back(password.get_name_leafdata());
    if (username.is_set || is_set(username.yfilter)) leaf_name_data.push_back(username.get_name_leafdata());
    if (yang_library_url.is_set || is_set(yang_library_url.yfilter)) leaf_name_data.push_back(yang_library_url.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Node::YangLibrary::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Node::YangLibrary::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Node::YangLibrary::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "password")
    {
        password = value;
        password.value_namespace = name_space;
        password.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "username")
    {
        username = value;
        username.value_namespace = name_space;
        username.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "yang-library-url")
    {
        yang_library_url = value;
        yang_library_url.value_namespace = name_space;
        yang_library_url.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Node::YangLibrary::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "password")
    {
        password.yfilter = yfilter;
    }
    if(value_path == "username")
    {
        username.yfilter = yfilter;
    }
    if(value_path == "yang-library-url")
    {
        yang_library_url.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Node::YangLibrary::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "password" || name == "username" || name == "yang-library-url")
        return true;
    return false;
}

NetworkTopology::Topology::Link::Link()
    :
    link_id{YType::str, "link-id"}
    	,
    destination(std::make_shared<NetworkTopology::Topology::Link::Destination>())
	,source(std::make_shared<NetworkTopology::Topology::Link::Source>())
{
    destination->parent = this;

    source->parent = this;

    yang_name = "link"; yang_parent_name = "topology";
}

NetworkTopology::Topology::Link::~Link()
{
}

bool NetworkTopology::Topology::Link::has_data() const
{
    for (std::size_t index=0; index<supporting_link.size(); index++)
    {
        if(supporting_link[index]->has_data())
            return true;
    }
    return link_id.is_set
	|| (destination !=  nullptr && destination->has_data())
	|| (source !=  nullptr && source->has_data());
}

bool NetworkTopology::Topology::Link::has_operation() const
{
    for (std::size_t index=0; index<supporting_link.size(); index++)
    {
        if(supporting_link[index]->has_operation())
            return true;
    }
    return is_set(yfilter)
	|| ydk::is_set(link_id.yfilter)
	|| (destination !=  nullptr && destination->has_operation())
	|| (source !=  nullptr && source->has_operation());
}

std::string NetworkTopology::Topology::Link::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "link" <<"[link-id='" <<link_id <<"']";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Link::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (link_id.is_set || is_set(link_id.yfilter)) leaf_name_data.push_back(link_id.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Link::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(child_yang_name == "destination")
    {
        if(destination == nullptr)
        {
            destination = std::make_shared<NetworkTopology::Topology::Link::Destination>();
        }
        return destination;
    }

    if(child_yang_name == "source")
    {
        if(source == nullptr)
        {
            source = std::make_shared<NetworkTopology::Topology::Link::Source>();
        }
        return source;
    }

    if(child_yang_name == "supporting-link")
    {
        for(auto const & c : supporting_link)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                return c;
            }
        }
        auto c = std::make_shared<NetworkTopology::Topology::Link::SupportingLink>();
        c->parent = this;
        supporting_link.push_back(c);
        return c;
    }

    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Link::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    if(destination != nullptr)
    {
        children["destination"] = destination;
    }

    if(source != nullptr)
    {
        children["source"] = source;
    }

    for (auto const & c : supporting_link)
    {
        children[c->get_segment_path()] = c;
    }

    return children;
}

void NetworkTopology::Topology::Link::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "link-id")
    {
        link_id = value;
        link_id.value_namespace = name_space;
        link_id.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Link::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "link-id")
    {
        link_id.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Link::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "destination" || name == "source" || name == "supporting-link" || name == "link-id")
        return true;
    return false;
}

NetworkTopology::Topology::Link::Source::Source()
    :
    source_node{YType::str, "source-node"},
    source_tp{YType::str, "source-tp"}
{
    yang_name = "source"; yang_parent_name = "link";
}

NetworkTopology::Topology::Link::Source::~Source()
{
}

bool NetworkTopology::Topology::Link::Source::has_data() const
{
    return source_node.is_set
	|| source_tp.is_set;
}

bool NetworkTopology::Topology::Link::Source::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(source_node.yfilter)
	|| ydk::is_set(source_tp.yfilter);
}

std::string NetworkTopology::Topology::Link::Source::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "source";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Link::Source::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (source_node.is_set || is_set(source_node.yfilter)) leaf_name_data.push_back(source_node.get_name_leafdata());
    if (source_tp.is_set || is_set(source_tp.yfilter)) leaf_name_data.push_back(source_tp.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Link::Source::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Link::Source::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Link::Source::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "source-node")
    {
        source_node = value;
        source_node.value_namespace = name_space;
        source_node.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "source-tp")
    {
        source_tp = value;
        source_tp.value_namespace = name_space;
        source_tp.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Link::Source::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "source-node")
    {
        source_node.yfilter = yfilter;
    }
    if(value_path == "source-tp")
    {
        source_tp.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Link::Source::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "source-node" || name == "source-tp")
        return true;
    return false;
}

NetworkTopology::Topology::Link::Destination::Destination()
    :
    dest_node{YType::str, "dest-node"},
    dest_tp{YType::str, "dest-tp"}
{
    yang_name = "destination"; yang_parent_name = "link";
}

NetworkTopology::Topology::Link::Destination::~Destination()
{
}

bool NetworkTopology::Topology::Link::Destination::has_data() const
{
    return dest_node.is_set
	|| dest_tp.is_set;
}

bool NetworkTopology::Topology::Link::Destination::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(dest_node.yfilter)
	|| ydk::is_set(dest_tp.yfilter);
}

std::string NetworkTopology::Topology::Link::Destination::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "destination";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Link::Destination::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (dest_node.is_set || is_set(dest_node.yfilter)) leaf_name_data.push_back(dest_node.get_name_leafdata());
    if (dest_tp.is_set || is_set(dest_tp.yfilter)) leaf_name_data.push_back(dest_tp.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Link::Destination::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Link::Destination::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Link::Destination::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "dest-node")
    {
        dest_node = value;
        dest_node.value_namespace = name_space;
        dest_node.value_namespace_prefix = name_space_prefix;
    }
    if(value_path == "dest-tp")
    {
        dest_tp = value;
        dest_tp.value_namespace = name_space;
        dest_tp.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Link::Destination::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "dest-node")
    {
        dest_node.yfilter = yfilter;
    }
    if(value_path == "dest-tp")
    {
        dest_tp.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Link::Destination::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "dest-node" || name == "dest-tp")
        return true;
    return false;
}

NetworkTopology::Topology::Link::SupportingLink::SupportingLink()
    :
    link_ref{YType::str, "link-ref"}
{
    yang_name = "supporting-link"; yang_parent_name = "link";
}

NetworkTopology::Topology::Link::SupportingLink::~SupportingLink()
{
}

bool NetworkTopology::Topology::Link::SupportingLink::has_data() const
{
    return link_ref.is_set;
}

bool NetworkTopology::Topology::Link::SupportingLink::has_operation() const
{
    return is_set(yfilter)
	|| ydk::is_set(link_ref.yfilter);
}

std::string NetworkTopology::Topology::Link::SupportingLink::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "supporting-link" <<"[link-ref='" <<link_ref <<"']";

    return path_buffer.str();

}

std::vector<std::pair<std::string, LeafData> > NetworkTopology::Topology::Link::SupportingLink::get_name_leaf_data() const
{

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (link_ref.is_set || is_set(link_ref.yfilter)) leaf_name_data.push_back(link_ref.get_name_leafdata());


    return leaf_name_data;

}

std::shared_ptr<Entity> NetworkTopology::Topology::Link::SupportingLink::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    return nullptr;
}

std::map<std::string, std::shared_ptr<Entity>> NetworkTopology::Topology::Link::SupportingLink::get_children() const
{
    std::map<std::string, std::shared_ptr<Entity>> children{};
    return children;
}

void NetworkTopology::Topology::Link::SupportingLink::set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix)
{
    if(value_path == "link-ref")
    {
        link_ref = value;
        link_ref.value_namespace = name_space;
        link_ref.value_namespace_prefix = name_space_prefix;
    }
}

void NetworkTopology::Topology::Link::SupportingLink::set_filter(const std::string & value_path, YFilter yfilter)
{
    if(value_path == "link-ref")
    {
        link_ref.yfilter = yfilter;
    }
}

bool NetworkTopology::Topology::Link::SupportingLink::has_leaf_or_child_of_name(const std::string & name) const
{
    if(name == "link-ref")
        return true;
    return false;
}

const Enum::YLeaf NetworkTopology::Topology::Node::ConnectionStatus::connecting {0, "connecting"};
const Enum::YLeaf NetworkTopology::Topology::Node::ConnectionStatus::connected {1, "connected"};
const Enum::YLeaf NetworkTopology::Topology::Node::ConnectionStatus::unable_to_connect {2, "unable-to-connect"};

const Enum::YLeaf NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::Status::connected {0, "connected"};
const Enum::YLeaf NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::Status::unavailable {1, "unavailable"};
const Enum::YLeaf NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::Status::failed {2, "failed"};

const Enum::YLeaf NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::CapabilityOrigin::user_defined {0, "user-defined"};
const Enum::YLeaf NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::CapabilityOrigin::device_advertised {1, "device-advertised"};

const Enum::YLeaf NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::FailureReason::missing_source {0, "missing-source"};
const Enum::YLeaf NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::FailureReason::unable_to_resolve {1, "unable-to-resolve"};


}
}

