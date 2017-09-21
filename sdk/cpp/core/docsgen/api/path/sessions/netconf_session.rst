NetconfSession
==============


.. cpp:class:: ydk::path::NetconfSession : public path::Session

    Implementation of :cpp:class:`Session<Session>` for the `netconf <https://tools.ietf.org/html/rfc6241>`_ protocol.

    .. cpp:function:: NetconfSession(\
        const std::string& address, \
        const std::string& username, \
        const std::string& password, \
        int port = 830, \
        const std::string& protocol = "ssh", \
        bool on_demand = true, \
        bool common_cache = false\
        int timeout = -1)

        Constructs an instance of the ``NetconfSession`` to connect to a server which **has** to support model download

        :param address: IP address of the device supporting a netconf interface
        :param username: Username to log in to the device
        :param password: Password to log in to the device
        :param port: Device port used to access the netconf interface. Default value is 830
        :param protocol: ``ssh`` or ``tcp``.
        :param on_demand: Enable on demand downloading by default.
        :param common_cache: Use different directories for different connections by default.
        :param timeout: The timeout in microseconds, -1 for infinite timeout, 0 for non-blocking

    .. cpp:function:: NetconfSession(\
        const path::Repository& repo, \
        const std::string& address, \
        const std::string& username, \
        const std::string& password, \
        int port = 830, \
        const std::string& protocol = "ssh", \
        bool on_demand = true
        int timeout = -1)

        Constructs an instance of the ``NetconfSession`` using the provided :cpp:class:`Repository<Repository>`

        :param repo: Reference to an instance of :cpp:class:`Repository<Repository>`
        :param address: IP address of the device supporting a netconf interface
        :param username: Username to log in to the device
        :param password: Password to log in to the device
        :param port: Device port used to access the netconf interface. Default value is 830
        :param protocol: ``ssh`` or ``tcp``.
        :param on_demand: Enable on demand downloading by default.
        :param timeout: The timeout in microseconds, -1 for infinite timeout, 0 for non-blocking

    .. cpp:function:: virtual path::RootSchemaNode& get_root_schema() const

        Returns the :cpp:class:`RootSchemaNode<path::RootSchemaNode>` tree supported by this instance of the ``NetconfSession``.

        :return: Pointer to the :cpp:class:`RootSchemaNode<path::RootSchemaNode>` or ``nullptr`` if one could not be created.

    .. cpp:function:: virtual std::shared_ptr<path::DataNode> invoke(path::Rpc& rpc) const

        Invokes or executes the given rpc and returns a :cpp:class:`DataNode<DataNode>` pointer if the Rpc has an output modelled in YANG.

        :param rpc: Reference to the :cpp:class:`Rpc<Rpc>` node.
        :return: Shared pointer to the :cpp:class:`DataNode<DataNode>` representing the output.

    .. cpp:function:: std::vector<std::string> get_capabilities() const

        Returns a vector of the client's capabilities

        :return: A vector of ``std::string`` representing the client/server capabilities

    .. cpp:function:: ~NetconfSession()
