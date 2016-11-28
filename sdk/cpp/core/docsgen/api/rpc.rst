.. _ref-rpc:

Rpc
===


.. cpp:namespace:: ydk::path

.. cpp:class:: Rpc

An instance of the YANG schmea rpc node.

Instances of this class represent a YANG rpc and are modelled as Callables.
The input data node tree is used to populate the input parameters to the rpc
if any. The Callable takes as a parameter the :cpp:class:`ServiceProvider<ServiceProvider>` that can execute this rpc as its parameter returning a pointer to a :cpp:class:`DataNode<DataNode>` tree if output is available.

    .. cpp:function:: virtual ~Rpc()

    .. cpp:function:: virtual DataNode* operator()(const ServiceProvider& provider)

        Execute/Invoke the rpc through the given service provider.

        :param sp: The Service provider.
        :return: Pointer to the :cpp:class:`DataNode<DataNode>` or ``nullptr`` if none exists.

    .. cpp:function:: virtual DataNode* input() const

        Get the input data tree.

        :return: Pointer to the input :cpp:class:`DataNode<DataNode>` or ``nullptr`` if the rpc does not have an input element in the schema.

    .. cpp:function:: virtual SchemaNode* schema() const

        :return: Pointer to the :cpp:class:`SchemaNode<SchemaNode>` associated with this rpc.
