.. _howto-path:

How do I use the Path API?
==========================

.. contents:: Table of Contents

The :ref:`Path API<path-api-guide>` (part of the `YDK core <https://github.com/CiscoDevNet/ydk-py/tree/master/core>`_) is a generic API which can be used to create and access YANG data nodes without having to use the model bundle APIs (for example, `openconfig <https://github.com/CiscoDevNet/ydk-py/tree/master/openconfig>`_). Apps can be written using xpath-like path expressions as illustrated below.

Creating a configuration
------------------------

An example for using Path API to create a ``bgp`` configuration using the ``openconfig-bgp`` model in conjunction with the standard `edit-config RPC <https://github.com/YangModels/yang/blob/4b12d5017eb94a0760746d72c6fd93cb02943d45/standard/ietf/RFC/ietf-netconf%402011-06-01.yang#L416>`_ is shown below:

.. code-block:: python
    :linenos:

    from ydk.path import NetconfSession
    from ydk.path import Codec
    from ydk.types import EncodingFormat

    # Create a NetconfSession instance to connect to the device
    session = NetconfSession('10.0.0.1', 'admin', 'admin')

    # Get the root schema node
    root_schema = session.get_root_schema()

    # Create the bgp configuration
    bgp = root_schema.create_datanode("openconfig-bgp:bgp", "")
    bgp.create_datanode("global/config/as", "65172")

    # Create a list instance of afi-safi. Note how the key (afi-safi-name)  of the list is specified
    l3vpn_ipv4_unicast = bgp.create_datanode("global/afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "")
    l3vpn_ipv4_unicast.create_datanode("config/afi-safi-name", "openconfig-bgp-types:L3VPN_IPV4_UNICAST")
    l3vpn_ipv4_unicast.create_datanode("config/enabled","true")

    # Create a list instance of neighbor. Note how the key (neighbor-address)  of the list is specified
    neighbor = bgp.create_datanode("neighbors/neighbor[neighbor-address='172.16.255.2']", "")
    neighbor.create_datanode("config/neighbor-address", "172.16.255.2")
    neighbor.create_datanode("config/peer-as","65172")
    neighbor_af = neighbor.create_datanode("afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "")
    neighbor_af.create_datanode("config/afi-safi-name" , "openconfig-bgp-types:L3VPN_IPV4_UNICAST")
    neighbor_af.create_datanode("config/enabled","true")

    codec_service = Codec()

    # Encode the bgp object to JSON string to examine the data
    json = codec_service.encode(bgp, EncodingFormat.JSON, True)
    print(json)

    # Encode the bgp object to XML string
    xml = codec_service.encode(bgp, EncodingFormat.XML, True)

    # Create the 'ietf-netconf:edit-config' RPC object
    edit_rpc = root_schema.create_rpc('ietf-netconf:edit-config')

    # Set the target to the candidate datastore
    edit_rpc.get_input_node().create_datanode('target/candidate')
    # Set the xml string to the 'config' field
    edit_rpc.get_input_node().create_datanode('config', xml)

    # Invoke the RPC
    edit_rpc(session)


Calling a non-standard RPC using path API
-----------------------------------------

To invoke the `get-schema RPC <https://github.com/YangModels/yang/blob/4b12d5017eb94a0760746d72c6fd93cb02943d45/standard/ietf/RFC/ietf-netconf-monitoring%402010-10-04.yang#L512>`_ to download the ``Cisco-IOS-XR-aaa-lib-cfg.yang`` yang model from a netconf server using the path API, the below approach can be used.

.. code-block:: python
    :linenos:

    from ydk.path import NetconfSession
    from ydk.path import Codec
    from ydk.types import EncodingFormat

    # Create a NetconfSession instance to connect to the device
    netconf_session = NetconfSession(address='10.0.0.1' , username='admin', password='admin')

    c = Codec()

    # Get the root schema node
    root = netconf_session.get_root_schema()

    # Create the 'ietf-netconf-monitoring:get-schema' RPC object
    get_schema = root.create_rpc('ietf-netconf-monitoring:get-schema')

    # Set the 'identifier' to 'Cisco-IOS-XR-aaa-lib-cfg'
    get_schema.get_input_node().create_datanode('identifier','Cisco-IOS-XR-aaa-lib-cfg')

    # Invoke the RPC
    output_data = get_schema(netconf_session)

    # Encode the RPC reply to XML
    output_xml =  c.encode(output_data, EncodingFormat.XML, True)

    # Print the XML
    print(output_xml)

Path syntax
-----------

Full XPath notation is supported for find operations on :py:class:`DataNode<DataNode>`\(s\). This XPath conforms to the YANG specification \(`RFC 6020 section 6.4 <https://tools.ietf.org/html/rfc6020#section-6.4>`_\). Some useful examples:

- Get ``list`` instance with ``key1`` of value ``1`` and ``key2`` of value ``2`` \(this can return more ``list`` instances if there are more keys than ``key1`` and ``key2``\)

.. code-block:: bash

    /module-name:container/list[key1='1'][key2='2']

- Get ``leaf-list`` instance with the value ``val``

.. code-block:: bash

    /module-name:container/leaf-list[.='val']

- Get ``aug-leaf``, which was added to ``module-name`` from an augment module ``augment-module``

.. code-block:: bash

    /module-name:container/container2/augment-module:aug-cont/aug-leaf

A very small subset of this full XPath is recognized by :py:meth:`DataNode::create<DataNode.create>`. Basically, only a relative or absolute path can be specified to identify a new data node. However, lists must be identified by all their keys and created with all of them, so for those cases predicates are allowed. Predicates must be ordered the way the keys are ordered and all the keys must be specified. Every predicate includes a single key with its value. Optionally, leaves and leaf-lists can have predicates specifying their value in the path itself. All these paths are valid XPath expressions. Example: (Relative to Root Data or :py:class:`RootSchemaNode`)

.. code-block:: bash

    ietf-yang-library:modules-state/module[name='ietf-yang-library'][revision='']/conformance[.='implement']

Almost the same XPath is accepted by :py:class:`SchemaNode<SchemaNode>` methods. The difference is that it is not used on data, but schema, which means there are no key values and only one node matches one path. In effect, lists do not have to have any predicates. If they do, they do not need to have all the keys specified and if values are included, they are ignored. Nevertheless, any such expression is still a valid XPath, but can return more nodes if executed on a data tree. Examples (all returning the same node):

.. code-block:: bash

    ietf-yang-library:modules-state/module/submodules
    ietf-yang-library:modules-state/module[name]/submodules
    ietf-yang-library:modules-state/module[name][revision]/submodules
    ietf-yang-library:modules-state/module[name='ietf-yang-library'][revision]/submodules


.. note::

    In all cases the node's prefix is specified as the name of the appropriate YANG schema. Any node can be prefixed by the module name. However, if the prefix is omitted, the module name is inherited from the previous (parent) node. It means, that the first node in the path is always supposed to have a prefix.
