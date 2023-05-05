.. _howto-path:

.. _path-api-guide:

How do I use the Path API?
==========================

.. contents:: Table of Contents

The :ref:`Path API<path-api-guide>` (part of the `YDK core <https://github.com/CiscoDevNet/ydk-py/tree/master/core>`_) is a generic API which can be used to create and access YANG data nodes without having to use the model bundle APIs (for example, `openconfig <https://github.com/CiscoDevNet/ydk-py/tree/master/openconfig>`_). Applications can be written using xpath-like path expressions as illustrated below.


Path Syntax
-----------

Full XPath notation is supported for find operations on :go:struct:`DataNode<DataNode>`\(s\). This XPath conforms to the YANG specification \(`RFC 6020 section 6.4 <https://tools.ietf.org/html/rfc6020#section-6.4>`_\). Some useful examples:

- Get ``list`` instance with ``key1`` of value ``1`` and ``key2`` of value ``2`` \(this can return more ``list`` instances if there are more keys than ``key1`` and ``key2``\)

.. code-block:: bash

    /module-name:container/list[key1='1'][key2='2']

- Get ``leaf-list`` instance with the value ``val``

.. code-block:: bash

    /module-name:container/leaf-list[.='val']

- Get ``aug-leaf``, which was added to ``module-name`` from an augment module ``augment-module``

.. code-block:: bash

    /module-name:container/container2/augment-module:aug-cont/aug-leaf

A very small subset of this full XPath is recognized by :go:func:`DataNodeCreate<DataNodeCreate>`. Basically, only a relative or absolute path can be specified to identify a new data node. However, lists must be identified by all their keys and created with all of them, so for those cases predicates are allowed. Predicates must be ordered the way the keys are ordered and all the keys must be specified. Every predicate includes a single key with its value. Optionally, leaves and leaf-lists can have predicates specifying their value in the path itself. All these paths are valid XPath expressions. Example: (Relative to Root Data or :go:struct:`RootSchemaNode`)

.. code-block:: bash

    ietf-yang-library:modules-state/module[name='ietf-yang-library'][revision='']/conformance[.='implement']

Almost the same XPath is accepted by :go:struct:`SchemaNode<SchemaNode>` methods. The difference is that it is not used on data, but schema, which means there are no key values and only one node matches one path. In effect, lists do not have to have any predicates. If they do, they do not need to have all the keys specified and if values are included, they are ignored. Nevertheless, any such expression is still a valid XPath, but can return more nodes if executed on a data tree. Examples (all returning the same node):

.. code-block:: bash

    ietf-yang-library:modules-state/module/submodules
    ietf-yang-library:modules-state/module[name]/submodules
    ietf-yang-library:modules-state/module[name][revision]/submodules
    ietf-yang-library:modules-state/module[name='ietf-yang-library'][revision]/submodules


.. note::

    In all cases the node's prefix is specified as the name of the appropriate YANG schema. Any node can be prefixed by the module name. However, if the prefix is omitted, the module name is inherited from the previous (parent) node. It means, that the first node in the path is always supposed to have a prefix.
