RestconfSession
===============

.. module:: ydk.path
    :synopsis: RestconfSession


.. py:class:: RestconfSession(\
            repo, address, username, password, port=80, \
            encoding=EncodingFormat.JSON, \
            config_url_root="/data", state_url_root="/data")

        :param repo: (:py:class:`Repository`) Reference to an instance of :py:class:`Repository<Repository>`
        :param address: (``str``) IP address or DNC name of the device supporting Netconf interface.
        :param username: (``str``) Username to log in to the device.
        :param password: (``str``) Password to log in to the device.
        :param port: (``int``) Type of encoding to be used for the payload. Default is 80.
        :param encoding: (:py:class:`EncodingFormat<ydk.types.EncodingFormat>`) Encoding Format, default is :py:attr:`JSON<ydk.types.EncodingFormat.JSON>`.
        :param config_url_root: (``str``) To provider backwards compatibility with older drafts of Restconf RFC, this can be ``/config`` or ``/data`` (which is the default).
        :param state_url_root: (``str``) To provider backwards compatibility with older drafts of Restconf RFC, this can be ``/operational`` or ``/data`` (which is the default)

    .. py:method:: get_root_schema()

        Returns the :py:class:`RootSchemaNode<RootSchemaNode>` tree supported by this instance of the :py:class:`RestconfSession<RestconfSession>`.

        :returns: Pointer to the :py:class:`RootSchemaNode<RootSchemaNode>` or ``nullptr`` if one could not be created.

    .. py:method:: invoke(rpc)

        Invokes or executes the given rpc and returns a :py:class:`DataNode<DataNode>` pointer if the :py:class:`Rpc<Rpc>` has an output modelled in YANG.

        :param rpc: (:py:class:`Rpc<Rpc>`)
        :returns: Pointer to the :py:class:`DataNode<DataNode>` representing the output.
