.. _ref-serviceprovider:

ServiceProvider
===============

.. toctree::
   :maxdepth: 2

   netconf_provider.rst
   restconf_provider.rst
   codec_provider.rst
   opendaylight_provider.rst

.. cpp:namespace:: ydk::path

.. cpp:class:: ServiceProvider

Interface for all ServiceProvider implementations.

Concrete instances of ServiceProviders are expected to extend this interface.

    .. cpp:function:: virtual RootSchemaNode& get_root_schema()

        Returns the :cpp:class:`SchemaNode<SchemaNode>` tree supported by this instance of the :cpp:class:`ServiceProvider<ServiceProvider>`.

        :return: Pointer to the :cpp:class:`RootSchemaNode<RootSchemaNode>` or ``nullptr`` if one could not be created.

    .. cpp:function:: virtual std::shared_ptr<DataNode> invoke(Rpc& rpc) const

        Invokes or executes the given rpc and returns a :cpp:class:`DataNode<DataNode>` pointer if the Rpc has an output modelled in YANG.

        :param rpc: Reference to the :cpp:class:`Rpc<Rpc>` node.
        :return: Shared pointer to the :cpp:class:`DataNode<DataNode>` representing the output.

    .. cpp:function:: EncodingFormat get_encoding()

        Returns the type of encoding supported by the service provider.

    .. cpp:function:: virtual ~ServiceProvider()
