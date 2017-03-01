
#include <sstream>
#include <iostream>
#include "entity_util.hpp"
#include "network_topology.hpp"

namespace ydk {
namespace network_topology {

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
    return is_set(operation);
}

std::string NetworkTopology::Topology::TopologyTypes::TopologyNetconf::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:topology-netconf";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::TopologyTypes::TopologyNetconf::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::TopologyTypes::TopologyNetconf::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::TopologyTypes::TopologyNetconf::get_children()
{
    return children;
}

void NetworkTopology::Topology::TopologyTypes::TopologyNetconf::set_value(const std::string & value_path, std::string value)
{
}

NetworkTopology::Topology::TopologyTypes::TopologyTypes()
    :
    topology_netconf(std::make_unique<NetworkTopology::Topology::TopologyTypes::TopologyNetconf>())
{
    topology_netconf->parent = this;
    children["topology-netconf"] = topology_netconf.get();

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
    return is_set(operation)
	|| (topology_netconf !=  nullptr && topology_netconf->has_operation());
}

std::string NetworkTopology::Topology::TopologyTypes::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "topology-types";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::TopologyTypes::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::TopologyTypes::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "topology-netconf")
    {
        if(topology_netconf != nullptr)
        {
            children["topology-netconf"] = topology_netconf.get();
        }
        else
        {
            topology_netconf = std::make_unique<NetworkTopology::Topology::TopologyTypes::TopologyNetconf>();
            topology_netconf->parent = this;
            children["topology-netconf"] = topology_netconf.get();
        }
        return children.at("topology-netconf");
    }

    return nullptr;
}

std::map<std::string, Entity*> & NetworkTopology::Topology::TopologyTypes::get_children()
{
    if(children.find("topology-netconf") == children.end())
    {
        if(topology_netconf != nullptr)
        {
            children["topology-netconf"] = topology_netconf.get();
        }
    }

    return children;
}

void NetworkTopology::Topology::TopologyTypes::set_value(const std::string & value_path, std::string value)
{
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
    return is_set(operation)
	|| is_set(topology_ref.operation);
}

std::string NetworkTopology::Topology::UnderlayTopology::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "underlay-topology" <<"[topology-ref='" <<topology_ref.get() <<"']";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::UnderlayTopology::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (topology_ref.is_set || is_set(topology_ref.operation)) leaf_name_data.push_back(topology_ref.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::UnderlayTopology::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::UnderlayTopology::get_children()
{
    return children;
}

void NetworkTopology::Topology::UnderlayTopology::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "topology-ref")
    {
        topology_ref = value;
    }
}

NetworkTopology::Topology::Node::SupportingNode::SupportingNode()
    :
    	node_ref{YType::str, "node-ref"},
	 topology_ref{YType::str, "topology-ref"}
{
    yang_name = "supporting-node"; yang_parent_name = "node";
}

NetworkTopology::Topology::Node::SupportingNode::~SupportingNode()
{
}

bool NetworkTopology::Topology::Node::SupportingNode::has_data() const
{
    return node_ref.is_set
	|| topology_ref.is_set;
}

bool NetworkTopology::Topology::Node::SupportingNode::has_operation() const
{
    return is_set(operation)
	|| is_set(node_ref.operation)
	|| is_set(topology_ref.operation);
}

std::string NetworkTopology::Topology::Node::SupportingNode::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "supporting-node" <<"[node-ref='" <<node_ref.get() <<"']" <<"[topology-ref='" <<topology_ref.get() <<"']";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::SupportingNode::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (node_ref.is_set || is_set(node_ref.operation)) leaf_name_data.push_back(node_ref.get_name_leafdata());
    if (topology_ref.is_set || is_set(topology_ref.operation)) leaf_name_data.push_back(topology_ref.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::SupportingNode::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::SupportingNode::get_children()
{
    return children;
}

void NetworkTopology::Topology::Node::SupportingNode::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "node-ref")
    {
        node_ref = value;
    }
    if(value_path == "topology-ref")
    {
        topology_ref = value;
    }
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
        if(is_set(leaf.operation))
            return true;
    }
    return is_set(operation)
	|| is_set(tp_id.operation)
	|| is_set(tp_ref.operation);
}

std::string NetworkTopology::Topology::Node::TerminationPoint::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "termination-point" <<"[tp-id='" <<tp_id.get() <<"']";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::TerminationPoint::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (tp_id.is_set || is_set(tp_id.operation)) leaf_name_data.push_back(tp_id.get_name_leafdata());

    auto tp_ref_name_datas = tp_ref.get_name_leafdata();
    leaf_name_data.insert(leaf_name_data.end(), tp_ref_name_datas.begin(), tp_ref_name_datas.end());

    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::TerminationPoint::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::TerminationPoint::get_children()
{
    return children;
}

void NetworkTopology::Topology::Node::TerminationPoint::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "tp-id")
    {
        tp_id = value;
    }
    if(value_path == "tp-ref")
    {
        tp_ref.append(value);
    }
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
        if(is_set(leaf.operation))
            return true;
    }
    return is_set(operation)
	|| is_set(capability.operation)
	|| is_set(override.operation);
}

std::string NetworkTopology::Topology::Node::YangModuleCapabilities::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:yang-module-capabilities";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::YangModuleCapabilities::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (override.is_set || is_set(override.operation)) leaf_name_data.push_back(override.get_name_leafdata());

    auto capability_name_datas = capability.get_name_leafdata();
    leaf_name_data.insert(leaf_name_data.end(), capability_name_datas.begin(), capability_name_datas.end());

    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::YangModuleCapabilities::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::YangModuleCapabilities::get_children()
{
    return children;
}

void NetworkTopology::Topology::Node::YangModuleCapabilities::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "capability")
    {
        capability.append(value);
    }
    if(value_path == "override")
    {
        override = value;
    }
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
    return is_set(operation)
	|| is_set(node.operation)
	|| is_set(status.operation);
}

std::string NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "node-status";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (node.is_set || is_set(node.operation)) leaf_name_data.push_back(node.get_name_leafdata());
    if (status.is_set || is_set(status.operation)) leaf_name_data.push_back(status.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::get_children()
{
    return children;
}

void NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "node")
    {
        node = value;
    }
    if(value_path == "status")
    {
        status = value;
    }
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
    return is_set(operation)
	|| is_set(netconf_master_node.operation);
}

std::string NetworkTopology::Topology::Node::ClusteredConnectionStatus::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:clustered-connection-status";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::ClusteredConnectionStatus::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (netconf_master_node.is_set || is_set(netconf_master_node.operation)) leaf_name_data.push_back(netconf_master_node.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::ClusteredConnectionStatus::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "node-status")
    {
        for(auto const & c : node_status)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus>();
        c->parent = this;
        node_status.push_back(std::move(c));
        children[segment_path] = node_status.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::ClusteredConnectionStatus::get_children()
{
    for (auto const & c : node_status)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void NetworkTopology::Topology::Node::ClusteredConnectionStatus::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "netconf-master-node")
    {
        netconf_master_node = value;
    }
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
    return is_set(operation)
	|| is_set(capability.operation)
	|| is_set(capability_origin.operation);
}

std::string NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "available-capability";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (capability.is_set || is_set(capability.operation)) leaf_name_data.push_back(capability.get_name_leafdata());
    if (capability_origin.is_set || is_set(capability_origin.operation)) leaf_name_data.push_back(capability_origin.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::get_children()
{
    return children;
}

void NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "capability")
    {
        capability = value;
    }
    if(value_path == "capability-origin")
    {
        capability_origin = value;
    }
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
    return is_set(operation);
}

std::string NetworkTopology::Topology::Node::AvailableCapabilities::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:available-capabilities";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::AvailableCapabilities::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::AvailableCapabilities::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "available-capability")
    {
        for(auto const & c : available_capability)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability>();
        c->parent = this;
        available_capability.push_back(std::move(c));
        children[segment_path] = available_capability.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::AvailableCapabilities::get_children()
{
    for (auto const & c : available_capability)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void NetworkTopology::Topology::Node::AvailableCapabilities::set_value(const std::string & value_path, std::string value)
{
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
    return is_set(operation)
	|| is_set(capability.operation)
	|| is_set(failure_reason.operation);
}

std::string NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "unavailable-capability";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (capability.is_set || is_set(capability.operation)) leaf_name_data.push_back(capability.get_name_leafdata());
    if (failure_reason.is_set || is_set(failure_reason.operation)) leaf_name_data.push_back(failure_reason.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::get_children()
{
    return children;
}

void NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "capability")
    {
        capability = value;
    }
    if(value_path == "failure-reason")
    {
        failure_reason = value;
    }
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
    return is_set(operation);
}

std::string NetworkTopology::Topology::Node::UnavailableCapabilities::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:unavailable-capabilities";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::UnavailableCapabilities::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::UnavailableCapabilities::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "unavailable-capability")
    {
        for(auto const & c : unavailable_capability)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability>();
        c->parent = this;
        unavailable_capability.push_back(std::move(c));
        children[segment_path] = unavailable_capability.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::UnavailableCapabilities::get_children()
{
    for (auto const & c : unavailable_capability)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void NetworkTopology::Topology::Node::UnavailableCapabilities::set_value(const std::string & value_path, std::string value)
{
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
    return is_set(operation);
}

std::string NetworkTopology::Topology::Node::PassThrough::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:pass-through";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::PassThrough::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::PassThrough::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::PassThrough::get_children()
{
    return children;
}

void NetworkTopology::Topology::Node::PassThrough::set_value(const std::string & value_path, std::string value)
{
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
    return is_set(operation)
	|| is_set(password.operation)
	|| is_set(username.operation)
	|| is_set(yang_library_url.operation);
}

std::string NetworkTopology::Topology::Node::YangLibrary::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "netconf-node-topology:yang-library";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::YangLibrary::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (password.is_set || is_set(password.operation)) leaf_name_data.push_back(password.get_name_leafdata());
    if (username.is_set || is_set(username.operation)) leaf_name_data.push_back(username.get_name_leafdata());
    if (yang_library_url.is_set || is_set(yang_library_url.operation)) leaf_name_data.push_back(yang_library_url.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::YangLibrary::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::YangLibrary::get_children()
{
    return children;
}

void NetworkTopology::Topology::Node::YangLibrary::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "password")
    {
        password = value;
    }
    if(value_path == "username")
    {
        username = value;
    }
    if(value_path == "yang-library-url")
    {
        yang_library_url = value;
    }
}

NetworkTopology::Topology::Node::Node()
    :
    	node_id{YType::str, "node-id"},
	 between_attempts_timeout_millis{YType::uint16, "between-attempts-timeout-millis"},
	 concurrent_rpc_limit{YType::uint16, "concurrent-rpc-limit"},
	 connected_message{YType::str, "connected-message"},
	 connection_status{YType::enumeration, "connection-status"},
	 connection_timeout_millis{YType::uint32, "connection-timeout-millis"},
	 default_request_timeout_millis{YType::uint32, "default-request-timeout-millis"},
	 host{YType::str, "host"},
	 keepalive_delay{YType::uint32, "keepalive-delay"},
	 max_connection_attempts{YType::uint32, "max-connection-attempts"},
	 password{YType::str, "password"},
	 port{YType::uint16, "port"},
	 reconnect_on_changed_schema{YType::boolean, "reconnect-on-changed-schema"},
	 schema_cache_directory{YType::str, "schema-cache-directory"},
	 schemaless{YType::boolean, "schemaless"},
	 sleep_factor{YType::str, "sleep-factor"},
	 tcp_only{YType::boolean, "tcp-only"},
	 username{YType::str, "username"}
    	,
    available_capabilities(std::make_unique<NetworkTopology::Topology::Node::AvailableCapabilities>())
	,clustered_connection_status(std::make_unique<NetworkTopology::Topology::Node::ClusteredConnectionStatus>())
	,pass_through(std::make_unique<NetworkTopology::Topology::Node::PassThrough>())
	,unavailable_capabilities(std::make_unique<NetworkTopology::Topology::Node::UnavailableCapabilities>())
	,yang_library(std::make_unique<NetworkTopology::Topology::Node::YangLibrary>())
	,yang_module_capabilities(std::make_unique<NetworkTopology::Topology::Node::YangModuleCapabilities>())
{
    available_capabilities->parent = this;
    children["available-capabilities"] = available_capabilities.get();

    clustered_connection_status->parent = this;
    children["clustered-connection-status"] = clustered_connection_status.get();

    pass_through->parent = this;
    children["pass-through"] = pass_through.get();

    unavailable_capabilities->parent = this;
    children["unavailable-capabilities"] = unavailable_capabilities.get();

    yang_library->parent = this;
    children["yang-library"] = yang_library.get();

    yang_module_capabilities->parent = this;
    children["yang-module-capabilities"] = yang_module_capabilities.get();

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
    return is_set(operation)
	|| is_set(node_id.operation)
	|| is_set(between_attempts_timeout_millis.operation)
	|| is_set(concurrent_rpc_limit.operation)
	|| is_set(connected_message.operation)
	|| is_set(connection_status.operation)
	|| is_set(connection_timeout_millis.operation)
	|| is_set(default_request_timeout_millis.operation)
	|| is_set(host.operation)
	|| is_set(keepalive_delay.operation)
	|| is_set(max_connection_attempts.operation)
	|| is_set(password.operation)
	|| is_set(port.operation)
	|| is_set(reconnect_on_changed_schema.operation)
	|| is_set(schema_cache_directory.operation)
	|| is_set(schemaless.operation)
	|| is_set(sleep_factor.operation)
	|| is_set(tcp_only.operation)
	|| is_set(username.operation)
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
    path_buffer << "node" <<"[node-id='" <<node_id.get() <<"']";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Node::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (node_id.is_set || is_set(node_id.operation)) leaf_name_data.push_back(node_id.get_name_leafdata());
    if (between_attempts_timeout_millis.is_set || is_set(between_attempts_timeout_millis.operation)) leaf_name_data.push_back(between_attempts_timeout_millis.get_name_leafdata());
    if (concurrent_rpc_limit.is_set || is_set(concurrent_rpc_limit.operation)) leaf_name_data.push_back(concurrent_rpc_limit.get_name_leafdata());
    if (connected_message.is_set || is_set(connected_message.operation)) leaf_name_data.push_back(connected_message.get_name_leafdata());
    if (connection_status.is_set || is_set(connection_status.operation)) leaf_name_data.push_back(connection_status.get_name_leafdata());
    if (connection_timeout_millis.is_set || is_set(connection_timeout_millis.operation)) leaf_name_data.push_back(connection_timeout_millis.get_name_leafdata());
    if (default_request_timeout_millis.is_set || is_set(default_request_timeout_millis.operation)) leaf_name_data.push_back(default_request_timeout_millis.get_name_leafdata());
    if (host.is_set || is_set(host.operation)) leaf_name_data.push_back(host.get_name_leafdata());
    if (keepalive_delay.is_set || is_set(keepalive_delay.operation)) leaf_name_data.push_back(keepalive_delay.get_name_leafdata());
    if (max_connection_attempts.is_set || is_set(max_connection_attempts.operation)) leaf_name_data.push_back(max_connection_attempts.get_name_leafdata());
    if (password.is_set || is_set(password.operation)) leaf_name_data.push_back(password.get_name_leafdata());
    if (port.is_set || is_set(port.operation)) leaf_name_data.push_back(port.get_name_leafdata());
    if (reconnect_on_changed_schema.is_set || is_set(reconnect_on_changed_schema.operation)) leaf_name_data.push_back(reconnect_on_changed_schema.get_name_leafdata());
    if (schema_cache_directory.is_set || is_set(schema_cache_directory.operation)) leaf_name_data.push_back(schema_cache_directory.get_name_leafdata());
    if (schemaless.is_set || is_set(schemaless.operation)) leaf_name_data.push_back(schemaless.get_name_leafdata());
    if (sleep_factor.is_set || is_set(sleep_factor.operation)) leaf_name_data.push_back(sleep_factor.get_name_leafdata());
    if (tcp_only.is_set || is_set(tcp_only.operation)) leaf_name_data.push_back(tcp_only.get_name_leafdata());
    if (username.is_set || is_set(username.operation)) leaf_name_data.push_back(username.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Node::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "available-capabilities")
    {
        if(available_capabilities != nullptr)
        {
            children["available-capabilities"] = available_capabilities.get();
        }
        else
        {
            available_capabilities = std::make_unique<NetworkTopology::Topology::Node::AvailableCapabilities>();
            available_capabilities->parent = this;
            children["available-capabilities"] = available_capabilities.get();
        }
        return children.at("available-capabilities");
    }

    if(child_yang_name == "clustered-connection-status")
    {
        if(clustered_connection_status != nullptr)
        {
            children["clustered-connection-status"] = clustered_connection_status.get();
        }
        else
        {
            clustered_connection_status = std::make_unique<NetworkTopology::Topology::Node::ClusteredConnectionStatus>();
            clustered_connection_status->parent = this;
            children["clustered-connection-status"] = clustered_connection_status.get();
        }
        return children.at("clustered-connection-status");
    }

    if(child_yang_name == "pass-through")
    {
        if(pass_through != nullptr)
        {
            children["pass-through"] = pass_through.get();
        }
        else
        {
            pass_through = std::make_unique<NetworkTopology::Topology::Node::PassThrough>();
            pass_through->parent = this;
            children["pass-through"] = pass_through.get();
        }
        return children.at("pass-through");
    }

    if(child_yang_name == "supporting-node")
    {
        for(auto const & c : supporting_node)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::Node::SupportingNode>();
        c->parent = this;
        supporting_node.push_back(std::move(c));
        children[segment_path] = supporting_node.back().get();
        return children.at(segment_path);
    }

    if(child_yang_name == "termination-point")
    {
        for(auto const & c : termination_point)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::Node::TerminationPoint>();
        c->parent = this;
        termination_point.push_back(std::move(c));
        children[segment_path] = termination_point.back().get();
        return children.at(segment_path);
    }

    if(child_yang_name == "unavailable-capabilities")
    {
        if(unavailable_capabilities != nullptr)
        {
            children["unavailable-capabilities"] = unavailable_capabilities.get();
        }
        else
        {
            unavailable_capabilities = std::make_unique<NetworkTopology::Topology::Node::UnavailableCapabilities>();
            unavailable_capabilities->parent = this;
            children["unavailable-capabilities"] = unavailable_capabilities.get();
        }
        return children.at("unavailable-capabilities");
    }

    if(child_yang_name == "yang-library")
    {
        if(yang_library != nullptr)
        {
            children["yang-library"] = yang_library.get();
        }
        else
        {
            yang_library = std::make_unique<NetworkTopology::Topology::Node::YangLibrary>();
            yang_library->parent = this;
            children["yang-library"] = yang_library.get();
        }
        return children.at("yang-library");
    }

    if(child_yang_name == "yang-module-capabilities")
    {
        if(yang_module_capabilities != nullptr)
        {
            children["yang-module-capabilities"] = yang_module_capabilities.get();
        }
        else
        {
            yang_module_capabilities = std::make_unique<NetworkTopology::Topology::Node::YangModuleCapabilities>();
            yang_module_capabilities->parent = this;
            children["yang-module-capabilities"] = yang_module_capabilities.get();
        }
        return children.at("yang-module-capabilities");
    }

    return nullptr;
}

std::map<std::string, Entity*> & NetworkTopology::Topology::Node::get_children()
{
    if(children.find("available-capabilities") == children.end())
    {
        if(available_capabilities != nullptr)
        {
            children["available-capabilities"] = available_capabilities.get();
        }
    }

    if(children.find("clustered-connection-status") == children.end())
    {
        if(clustered_connection_status != nullptr)
        {
            children["clustered-connection-status"] = clustered_connection_status.get();
        }
    }

    if(children.find("pass-through") == children.end())
    {
        if(pass_through != nullptr)
        {
            children["pass-through"] = pass_through.get();
        }
    }

    for (auto const & c : supporting_node)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    for (auto const & c : termination_point)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    if(children.find("unavailable-capabilities") == children.end())
    {
        if(unavailable_capabilities != nullptr)
        {
            children["unavailable-capabilities"] = unavailable_capabilities.get();
        }
    }

    if(children.find("yang-library") == children.end())
    {
        if(yang_library != nullptr)
        {
            children["yang-library"] = yang_library.get();
        }
    }

    if(children.find("yang-module-capabilities") == children.end())
    {
        if(yang_module_capabilities != nullptr)
        {
            children["yang-module-capabilities"] = yang_module_capabilities.get();
        }
    }

    return children;
}

void NetworkTopology::Topology::Node::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "node-id")
    {
        node_id = value;
    }
    if(value_path == "between-attempts-timeout-millis")
    {
        between_attempts_timeout_millis = value;
    }
    if(value_path == "concurrent-rpc-limit")
    {
        concurrent_rpc_limit = value;
    }
    if(value_path == "connected-message")
    {
        connected_message = value;
    }
    if(value_path == "connection-status")
    {
        connection_status = value;
    }
    if(value_path == "connection-timeout-millis")
    {
        connection_timeout_millis = value;
    }
    if(value_path == "default-request-timeout-millis")
    {
        default_request_timeout_millis = value;
    }
    if(value_path == "host")
    {
        host = value;
    }
    if(value_path == "keepalive-delay")
    {
        keepalive_delay = value;
    }
    if(value_path == "max-connection-attempts")
    {
        max_connection_attempts = value;
    }
    if(value_path == "password")
    {
        password = value;
    }
    if(value_path == "port")
    {
        port = value;
    }
    if(value_path == "reconnect-on-changed-schema")
    {
        reconnect_on_changed_schema = value;
    }
    if(value_path == "schema-cache-directory")
    {
        schema_cache_directory = value;
    }
    if(value_path == "schemaless")
    {
        schemaless = value;
    }
    if(value_path == "sleep-factor")
    {
        sleep_factor = value;
    }
    if(value_path == "tcp-only")
    {
        tcp_only = value;
    }
    if(value_path == "username")
    {
        username = value;
    }
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
    return is_set(operation)
	|| is_set(source_node.operation)
	|| is_set(source_tp.operation);
}

std::string NetworkTopology::Topology::Link::Source::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "source";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Link::Source::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (source_node.is_set || is_set(source_node.operation)) leaf_name_data.push_back(source_node.get_name_leafdata());
    if (source_tp.is_set || is_set(source_tp.operation)) leaf_name_data.push_back(source_tp.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Link::Source::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Link::Source::get_children()
{
    return children;
}

void NetworkTopology::Topology::Link::Source::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "source-node")
    {
        source_node = value;
    }
    if(value_path == "source-tp")
    {
        source_tp = value;
    }
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
    return is_set(operation)
	|| is_set(dest_node.operation)
	|| is_set(dest_tp.operation);
}

std::string NetworkTopology::Topology::Link::Destination::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "destination";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Link::Destination::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (dest_node.is_set || is_set(dest_node.operation)) leaf_name_data.push_back(dest_node.get_name_leafdata());
    if (dest_tp.is_set || is_set(dest_tp.operation)) leaf_name_data.push_back(dest_tp.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Link::Destination::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Link::Destination::get_children()
{
    return children;
}

void NetworkTopology::Topology::Link::Destination::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "dest-node")
    {
        dest_node = value;
    }
    if(value_path == "dest-tp")
    {
        dest_tp = value;
    }
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
    return is_set(operation)
	|| is_set(link_ref.operation);
}

std::string NetworkTopology::Topology::Link::SupportingLink::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "supporting-link" <<"[link-ref='" <<link_ref.get() <<"']";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Link::SupportingLink::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (link_ref.is_set || is_set(link_ref.operation)) leaf_name_data.push_back(link_ref.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Link::SupportingLink::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
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

std::map<std::string, Entity*> & NetworkTopology::Topology::Link::SupportingLink::get_children()
{
    return children;
}

void NetworkTopology::Topology::Link::SupportingLink::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "link-ref")
    {
        link_ref = value;
    }
}

NetworkTopology::Topology::Link::Link()
    :
    	link_id{YType::str, "link-id"}
    	,
    destination(std::make_unique<NetworkTopology::Topology::Link::Destination>())
	,source(std::make_unique<NetworkTopology::Topology::Link::Source>())
{
    destination->parent = this;
    children["destination"] = destination.get();

    source->parent = this;
    children["source"] = source.get();

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
    return is_set(operation)
	|| is_set(link_id.operation)
	|| (destination !=  nullptr && destination->has_operation())
	|| (source !=  nullptr && source->has_operation());
}

std::string NetworkTopology::Topology::Link::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "link" <<"[link-id='" <<link_id.get() <<"']";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::Link::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (link_id.is_set || is_set(link_id.operation)) leaf_name_data.push_back(link_id.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::Link::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "destination")
    {
        if(destination != nullptr)
        {
            children["destination"] = destination.get();
        }
        else
        {
            destination = std::make_unique<NetworkTopology::Topology::Link::Destination>();
            destination->parent = this;
            children["destination"] = destination.get();
        }
        return children.at("destination");
    }

    if(child_yang_name == "source")
    {
        if(source != nullptr)
        {
            children["source"] = source.get();
        }
        else
        {
            source = std::make_unique<NetworkTopology::Topology::Link::Source>();
            source->parent = this;
            children["source"] = source.get();
        }
        return children.at("source");
    }

    if(child_yang_name == "supporting-link")
    {
        for(auto const & c : supporting_link)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::Link::SupportingLink>();
        c->parent = this;
        supporting_link.push_back(std::move(c));
        children[segment_path] = supporting_link.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & NetworkTopology::Topology::Link::get_children()
{
    if(children.find("destination") == children.end())
    {
        if(destination != nullptr)
        {
            children["destination"] = destination.get();
        }
    }

    if(children.find("source") == children.end())
    {
        if(source != nullptr)
        {
            children["source"] = source.get();
        }
    }

    for (auto const & c : supporting_link)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void NetworkTopology::Topology::Link::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "link-id")
    {
        link_id = value;
    }
}

NetworkTopology::Topology::Topology()
    :
    	topology_id{YType::str, "topology-id"},
	 server_provided{YType::boolean, "server-provided"}
    	,
    topology_types(std::make_unique<NetworkTopology::Topology::TopologyTypes>())
{
    topology_types->parent = this;
    children["topology-types"] = topology_types.get();

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
    return is_set(operation)
	|| is_set(topology_id.operation)
	|| is_set(server_provided.operation)
	|| (topology_types !=  nullptr && topology_types->has_operation());
}

std::string NetworkTopology::Topology::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "topology" <<"[topology-id='" <<topology_id.get() <<"']";

    return path_buffer.str();

}

EntityPath NetworkTopology::Topology::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor == nullptr)
    {
        path_buffer << "network-topology:network-topology/" << get_segment_path();
    }
    else
    {
        path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());
    }

    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};

    if (topology_id.is_set || is_set(topology_id.operation)) leaf_name_data.push_back(topology_id.get_name_leafdata());
    if (server_provided.is_set || is_set(server_provided.operation)) leaf_name_data.push_back(server_provided.get_name_leafdata());


    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::Topology::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "link")
    {
        for(auto const & c : link)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::Link>();
        c->parent = this;
        link.push_back(std::move(c));
        children[segment_path] = link.back().get();
        return children.at(segment_path);
    }

    if(child_yang_name == "node")
    {
        for(auto const & c : node)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::Node>();
        c->parent = this;
        node.push_back(std::move(c));
        children[segment_path] = node.back().get();
        return children.at(segment_path);
    }

    if(child_yang_name == "topology-types")
    {
        if(topology_types != nullptr)
        {
            children["topology-types"] = topology_types.get();
        }
        else
        {
            topology_types = std::make_unique<NetworkTopology::Topology::TopologyTypes>();
            topology_types->parent = this;
            children["topology-types"] = topology_types.get();
        }
        return children.at("topology-types");
    }

    if(child_yang_name == "underlay-topology")
    {
        for(auto const & c : underlay_topology)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology::UnderlayTopology>();
        c->parent = this;
        underlay_topology.push_back(std::move(c));
        children[segment_path] = underlay_topology.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & NetworkTopology::Topology::get_children()
{
    for (auto const & c : link)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    for (auto const & c : node)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    if(children.find("topology-types") == children.end())
    {
        if(topology_types != nullptr)
        {
            children["topology-types"] = topology_types.get();
        }
    }

    for (auto const & c : underlay_topology)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void NetworkTopology::Topology::set_value(const std::string & value_path, std::string value)
{
    if(value_path == "topology-id")
    {
        topology_id = value;
    }
    if(value_path == "server-provided")
    {
        server_provided = value;
    }
}

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
    return is_set(operation);
}

std::string NetworkTopology::get_segment_path() const
{
    std::ostringstream path_buffer;
    path_buffer << "network-topology:network-topology";

    return path_buffer.str();

}

EntityPath NetworkTopology::get_entity_path(Entity* ancestor) const
{
    std::ostringstream path_buffer;
    if (ancestor != nullptr)
    {
        throw(YCPPInvalidArgumentError{"ancestor has to be nullptr for top-level node"});
    }

    path_buffer << get_segment_path();
    std::vector<std::pair<std::string, LeafData> > leaf_name_data {};



    EntityPath entity_path {path_buffer.str(), leaf_name_data};
    return entity_path;

}

Entity* NetworkTopology::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)
{
    if(children.find(child_yang_name) != children.end())
    {
        return children.at(child_yang_name);
    }
    else if(children.find(segment_path) != children.end())
    {
        return children.at(segment_path);
    }

    if(child_yang_name == "topology")
    {
        for(auto const & c : topology)
        {
            std::string segment = c->get_segment_path();
            if(segment_path == segment)
            {
                children[segment_path] = c.get();
                return children.at(segment_path);
            }
        }
        auto c = std::make_unique<NetworkTopology::Topology>();
        c->parent = this;
        topology.push_back(std::move(c));
        children[segment_path] = topology.back().get();
        return children.at(segment_path);
    }

    return nullptr;
}

std::map<std::string, Entity*> & NetworkTopology::get_children()
{
    for (auto const & c : topology)
    {
        if(children.find(c->get_segment_path()) == children.end())
        {
            children[c->get_segment_path()] = c.get();
        }
    }

    return children;
}

void NetworkTopology::set_value(const std::string & value_path, std::string value)
{
}

std::shared_ptr<Entity> NetworkTopology::clone_ptr() const
{
    return std::make_unique<NetworkTopology>();
}

const Enum::YLeaf NetworkTopology::Topology::Node::ConnectionStatusEnum::connecting {0, "connecting"};
const Enum::YLeaf NetworkTopology::Topology::Node::ConnectionStatusEnum::connected {1, "connected"};
const Enum::YLeaf NetworkTopology::Topology::Node::ConnectionStatusEnum::unable_to_connect {2, "unable-to-connect"};

const Enum::YLeaf NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::StatusEnum::connected {0, "connected"};
const Enum::YLeaf NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::StatusEnum::unavailable {1, "unavailable"};
const Enum::YLeaf NetworkTopology::Topology::Node::ClusteredConnectionStatus::NodeStatus::StatusEnum::failed {2, "failed"};

const Enum::YLeaf NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::CapabilityOriginEnum::user_defined {0, "user-defined"};
const Enum::YLeaf NetworkTopology::Topology::Node::AvailableCapabilities::AvailableCapability::CapabilityOriginEnum::device_advertised {1, "device-advertised"};

const Enum::YLeaf NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::FailureReasonEnum::missing_source {0, "missing-source"};
const Enum::YLeaf NetworkTopology::Topology::Node::UnavailableCapabilities::UnavailableCapability::FailureReasonEnum::unable_to_resolve {1, "unable-to-resolve"};


}
}

