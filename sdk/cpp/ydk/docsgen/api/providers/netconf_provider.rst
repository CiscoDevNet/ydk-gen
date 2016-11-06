NetconfServiceProvider
======================

.. cpp:namespace:: ydk

.. cpp:class:: NetconfServiceProvider : public path::ServiceProvider

Implementation of :cpp:class:`ServiceProvider<path::ServiceProvider>` for the netconf protocol.

    .. cpp:function:: NetconfServiceProvider(std::string address,\
                             std::string username,\
                                 std::string password, int port)

        Constructs an instance of the ``NetconfServiceProvider``
        
        :param address: IP address of the device supporting a netconf interface
        :param username: Username to log in to the device
        :param password: Password to log in to the device
        :param port: Device port used to access the netconf interface

    .. cpp:function:: NetconfServiceProvider(const path::Repository* repo,\
                         std::string address,\
                             std::string username,\
                                 std::string password, int port)

        Constructs an instance of the ``NetconfServiceProvider``

        :param repository: Pointer to an instance of :cpp:class:`path::Repository<ydk::path::Repository>`
        :param address: IP address of the device supporting a netconf interface
        :param username: Username to log in to the device
        :param password: Password to log in to the device
        :param port: Device port used to access the netconf interface

    .. cpp:function:: virtual path::RootSchemaNode* get_root_schema() const

        Returns the :cpp:class:`SchemaNode<SchemaNode>` tree supported by this instance of the :cpp:class:`ServiceProvider<ServiceProvider>`.

        :return: Pointer to the :cpp:class:`RootSchemaNode<RootSchemaNode>` or ``nullptr`` if one could not be created.

    .. cpp:function:: virtual path::DataNode* invoke(path::Rpc* rpc) const

         Invokes or executes the given rpc and returns a :cpp:class:`DataNode<DataNode>` pointer if the Rpc has an output modelled in YANG.

        :param rpc: Pointer to the :cpp:class:`Rpc<Rpc>` node.
        :return: Pointer to the :cpp:class:`DataNode<DataNode>` representing the output.

    .. cpp:function:: ~NetconfServiceProvider()
