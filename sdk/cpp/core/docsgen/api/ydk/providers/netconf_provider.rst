NetconfServiceProvider
======================


.. cpp:class:: ydk::NetconfServiceProvider : public ydk::ServiceProvider

    Implementation of :cpp:class:`ServiceProvider<ydk::ServiceProvider>` for the `netconf <https://tools.ietf.org/html/rfc6241>`_ protocol.

    .. cpp:function:: NetconfServiceProvider(\
        std::string address, \
        std::string username, \
        std::string password, \
        int port = 830, \
        const std::string& protocol = "ssh", \
        bool on_demand = true, \
        bool common_cache = false,
        int timeout = -1)

        Constructs an instance of ``NetconfServiceProvider`` connect to a server which **has** to support model download

        :param address: IP address of the device supporting a netconf interface.
        :param username: Username to log in to the device.
        :param password: Password to log in to the device.
        :param port: Device port used to access the netconf interface. Default value is 830.
        :param protocol: ``ssh`` or ``tcp``.
        :param on_demand: On demand downloading by default.
        :param common_cache: Use common caching directory if enabled.
        :param timeout: The timeout in microseconds, -1 for infinite timeout, 0 for non-blocking

    .. cpp:function:: NetconfServiceProvider(\
        const path::Repository& repo, \
        std::string address, \
        std::string username, \
        std::string password, \
        int port = 830, \
        const std::string& protocol = "ssh", \
        bool on_demand = true,
        int timeout = -1)

        Constructs an instance of ``NetconfServiceProvider`` using the provided :cpp:class:`repository<path::Repository>`

        :param repository: Reference to an instance of :cpp:class:`Repository<ydk::path::Repository>`
        :param address: IP address of the device supporting a netconf interface
        :param username: Username to log in to the device
        :param password: Password to log in to the device
        :param port: Device port used to access the netconf interface. Default value is 830
        :param timeout: The timeout in microseconds, -1 for infinite timeout, 0 for non-blocking

    .. cpp:function:: EncodingFormat get_encoding() const

        Returns the type of encoding supported by the service provider. In the case of netconf service provider, :cpp:enum:`EncodingFormat::XML<EncodingFormat>` is returned.

    .. cpp:function:: const NetconfSession get_session() const

        Returns a reference of the :cpp:class:`NetconfSession<path::NetconfSession>` used to connect to the netconf server.

    .. cpp:function:: std::vector<std::string> get_capabilities() const

        Returns a vector of the client's capabilities

        :return: A vector of ``std::string`` representing the client/server capabilities

    .. cpp:function:: ~NetconfServiceProvider()
