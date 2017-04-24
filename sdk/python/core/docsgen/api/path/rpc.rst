Rpc
===

.. module:: ydk.path
    :synopsis: Path API' Rpc


.. py:class:: Rpc

    Instances of this class represent a YANG rpc and are modelled as Callables. The input data node tree is used to populate the input parameters to the rpc if any. The Callable takes as a parameter the :py:class:`ServiceProvider<ServiceProvider>` that can execute this rpc as its parameter returning a :py:class:`DataNode<DataNode>` instance if output is available.


    .. py:method:: __call__(service_provider)

        Execute/Invoke the rpc through the given service provider.

        :param service_provider: (:py:class:`ServiceProvider`) The Service provider.
        :return: :py:class:`DataNode` instance if succeed.
        :rtype: None or :py:class:`DataNode`

    .. py:method:: input()

        Get the input data tree.

        :return: :py:class:`DataNode` representing the input data tree or ``None`` if the rpc does not have an input element in the schema.
        :rtype: :py:class:`DataNode` or ``None``

    .. py:method:: schema()

        Get schema node for this rpc.

        :return: Schema node associated with this rpc.
        :rtype: :py:class:`SchemaNode`
