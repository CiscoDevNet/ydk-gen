NetconfSession
==============

.. module:: ydk.path
    :synopsis: NetconfSession


.. py:class:: NetconfSession(address, usename, password, port=830, on_demand=True, common_cache=False, int timeout=None, repo=None, private_key_path="", public_key_path="")

    :param address: (``str``) IP address of the device supporting a netconf interface.
    :param username: (``str``) Username to log in to the device.
    :param password: (``str``) Password to log in to the device
    :param port: (``int``) Device port used to access the netconf interface. Default value is 830.
    :param on_demand: (``bool``) On demand model downloading by default.
    :param common_cache: (``bool``) Use common cache directory if enabled.
    :param timeout: (``int``) The timeout in microseconds: None or -1 for infinite timeout, 0 - for non-blocking
    :param repo: (:py:class:`Repository<Repository>`) User customized repository.
    :param private_key_path: (``str``) Path to private key file. Requires public_key_path field. Does not allow password field.
    :param public_key_path: (``str``) Path to public key file. Requires private_key_path field. Does not allow password field.

    .. py:method:: get_root_schema()

        Return :py:class:`RootSchemaNode<RootSchemaNode>` for this NETCONF session.

        :returns: :py:class:`RootSchemaNode<RootSchemaNode>` for this NETCONF session.

    .. py:method:: invoke(rpc)

        :param rpc: (:py:class:`Rpc<ydk.path.Rpc>`) Given Rpc to be executed.

        Invokes or executes the given rpc and returns a :py:class:`DataNode<DataNode>` pointer if the Rpc has an output modelled in YANG.

        :returns: :py:class:`DataNode<DataNode>`.

    .. py:method:: invoke(datanode)

        :param datanode: (:py:class:`Rpc<ydk.path.DataNode>`) Given DataNode containing YANG 1.1 action to be executed.

        Invokes or executes the given DataNode and returns a :py:class:`DataNode<DataNode>` pointer if the action has an output modelled in YANG.

        :returns: :py:class:`DataNode<DataNode>`.

    .. py:method:: get_capabilities()

        Returns a list of capabilities of the client

        :returns: A list of ``str`` representing the client's/server's capabilities