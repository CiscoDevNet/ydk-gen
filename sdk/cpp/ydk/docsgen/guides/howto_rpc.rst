RPC
===

.. cpp:namespace:: ydk::path

An :cpp:class:`Rpc<Rpc>` represents an instance of the YANG RPC schema node. To invoke a rpc the user first creates an :cpp:class:`Rpc<Rpc>` using the :cpp:func:`RootSchemaNode::create_rpc` call passing in a path with the name of the rpc. For example:

.. code-block:: c++

    auto get_config = root_schema->create_rpc("ietf-netconf:get-config")

The input :cpp:class:`DataNode<DataNode>` can be obtained using :cpp:func:`Rpc::input`. This can be used to populate/create the child nodes of input as per this rpc's schema. The :cpp:class:`Rpc<Rpc>` is a callable that takes a single argument which is the :cpp:class:`ServiceProvider`. To invoke the rpc do this

.. code-block:: c++

    auto config = get_config(sp); /// sp is a service provider

The config variable above is the :cpp:class:`DataNode<DataNode>` representing the output of the rpc.
