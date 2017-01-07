NetconfServiceProvider
======================

.. cpp:namespace:: ydk

.. cpp:class:: NetconfServiceProvider : public path::ServiceProvider

Implementation of :cpp:class:`ServiceProvider<path::ServiceProvider>` for the `netconf <https://tools.ietf.org/html/rfc6241>`_ protocol.

    .. cpp:function:: NetconfServiceProvider(std::string address,\
                             std::string username,\
                                 std::string password, int port = 830)

        Constructs an instance of the ``NetconfServiceProvider`` to connect to a server which **has** to support model download
        
        :param address: IP address of the device supporting a netconf interface
        :param username: Username to log in to the device
        :param password: Password to log in to the device
        :param port: Device port used to access the netconf interface. Default value is 830

    .. cpp:function:: NetconfServiceProvider(const path::Repository& repo,\
                         std::string address,\
                             std::string username,\
                                 std::string password, int port = 830)

        Constructs an instance of the ``NetconfServiceProvider`` using the provided :cpp:class:`repository<path::Repository>`

        :param repository: Reference to an instance of :cpp:class:`path::Repository<ydk::path::Repository>`
        :param address: IP address of the device supporting a netconf interface
        :param username: Username to log in to the device
        :param password: Password to log in to the device
        :param port: Device port used to access the netconf interface. Default value is 830

    .. cpp:function:: path::RootSchemaNode* get_root_schema() const

        Returns the :cpp:class:`RootSchemaNode<path::RootSchemaNode>` tree supported by this instance of the :cpp:class:`ServiceProvider<ServiceProvider>`.

        :return: Pointer to the :cpp:class:`RootSchemaNode<path::RootSchemaNode>` or ``nullptr`` if one could not be created.

    .. cpp:function:: path::DataNode* invoke(path::Rpc* rpc) const

         Invokes or executes the given rpc and returns a :cpp:class:`DataNode<DataNode>` pointer if the Rpc has an output modelled in YANG.

        :param rpc: Pointer to the :cpp:class:`Rpc<Rpc>` node.
        :return: Pointer to the :cpp:class:`DataNode<DataNode>` representing the output.

    .. cpp:function:: EncodingFormat get_encoding()

        Returns the type of encoding supported by the service provider. In the case of netconf service provider, :cpp:enum:`EncodingFormat::XML<EncodingFormat>` is returned.

    .. cpp:function:: ~NetconfServiceProvider()
