.. _howto-path:

How To Path
===========

.. module:: ydk.path

.. contents:: Table of Contents

Path Syntax
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

Example
-------

Example for using Path API is shown below(assuming you have openconfig-bgp avaiable in device capability):

.. code-block:: python
    :linenos:

    import logging
    log = logging.getLogger('ydk')
    log.setLevel(logging.INFO)
    ch = logging.StreamHandler()
    log.addHandler(ch)                                                      # enable logging

    from ydk.providers import NetconfServiceProvider
    from ydk.path import CodecService
    from ydk.types import EncodingFormat

    provider = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 12022)
    root_schema = provider.get_root_schema()                                # get root schema node

    bgp = root_schema.create("openconfig-bgp:bgp", "")
    bgp.create("global/config/as", "65172")
    l3vpn_ipv4_unicast = bgp.create("global/afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "")
    l3vpn_ipv4_unicast.create("config/afi-safi-name", "openconfig-bgp-types:L3VPN_IPV4_UNICAST")
    l3vpn_ipv4_unicast.create("config/enabled","true")
    neighbor = bgp.create("neighbors/neighbor[neighbor-address='172.16.255.2']", "")
    neighbor.create("config/neighbor-address", "172.16.255.2")
    neighbor.create("config/peer-as","65172")
    neighbor_af = neighbor.create("afi-safis/afi-safi[afi-safi-name='openconfig-bgp-types:L3VPN_IPV4_UNICAST']", "")
    neighbor_af.create("config/afi-safi-name" , "openconfig-bgp-types:L3VPN_IPV4_UNICAST")
    neighbor_af.create("config/enabled","true")

    codec_service = CodecService()
    xml = codec_service.encode(bgp, EncodingFormat.XML, True)               # get XML encoding
    create_rpc = root_schema.rpc('ydk:create')
    create_rpc.input().create('entity', xml)
    create_rpc(provider)                                                    # create bgp configuration

    json = codec_service.encode(bgp, EncodingFormat.JSON, True)             # get JSON encoding
    print(json)
