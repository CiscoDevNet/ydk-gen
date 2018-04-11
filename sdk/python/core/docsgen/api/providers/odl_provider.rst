Opendaylight Service Provider
=============================


.. py:class:: ydk.providers.OpendaylightServiceProvider(repo, address, username, password, port, encoding)

    A service provider to be used to communicate with an OpenDaylight instance.

    :param repo: (:py:class:`Repository<ydk.path.Repository>`) User provided repository stores cached models.
    :param address: (``str``) IP address of the ODL instance
    :param username: (``str``) Username to log in to the instance
    :param password: (``str``) Password to log in to the instance
    :param port: (``int``) Device port used to access the ODL instance.
    :param encoding: (:py:class:`EncodingFormat<ydk.types.EncodingFormat>`) Type of encoding to be used for the payload. Default is :py:attr:`JSON<ydk.types.EncodingFormat.JSON>`

    .. py:method:: get_node_provider(node_id)

        Returns the ServiceProvider instance corresponding to the device being controlled by the OpenDaylight instance, indicated by ``node_id``

        :param node_id: (``str``) The name of the device being controlled by the OpenDaylight instance.
        :return: One of supported service provider instance.
        :raises: :py:exc:`YError<ydk.errors.YServiceProviderError>` if no such service provider could be found.

    .. py:method:: get_node_ids()

        Returns a list of node ID’s of the devices being controlled by this OpenDaylight instance.

        :return: List of node ID’s of the devices being controlled by this OpenDaylight instance.
