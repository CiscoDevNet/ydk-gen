NetconfSession
==============

.. module:: ydk.path
    :synopsis: NetconfSession


.. py:class:: NetconfSession(address, username, password, port=830, protocol="ssh", on_demand=True, common_cache=False, int timeout=None, repo=None, private_key_path="", public_key_path="")

    Constructs an instance of the `NetconfSession` class and connects to Netconf server, which **must** support model download. 

    :param address: (``str``) IP address or DNS name of device, which supports Netconf server; required parameter.
    :param username: (``str``) Username to log in to the device; required parameter.
    :param password: (``str``) Password to log in to the device.
    :param port: (``int``) Device port used to access the Netconf interface. Default value is 830.
    :param protocol: (``str``) Currently supported `ssh` for secure connection and `tcp` for insecure connection; default - `ssh`.
    :param on_demand: (``bool``) On demand model downloading by default.
    :param common_cache: (``bool``) Use common cache directory, if enabled.
    :param timeout: (``int``) The timeout in microseconds: None or -1 for infinite timeout, 0 - for non-blocking
    :param repo: (:py:class:`Repository<Repository>`) User provided repository - directory, which stores cached Yang models.
    :param private_key_path: (``str``) Path to private key file. Requires public_key_path field.
    :param public_key_path: (``str``) Path to public key file. Does not allow `password` field, if specified.

    .. py:method:: get_root_schema()

        Return :py:class:`RootSchemaNode<RootSchemaNode>` for this Netconf session.

        :returns: :py:class:`RootSchemaNode<RootSchemaNode>` for this Netconf session.

    .. py:method:: invoke(rpc)

        Invokes or executes the given RPC and returns a :py:class:`DataNode<DataNode>` pointer, if the RPC has an output modeled in YANG.

        :param rpc: (:py:class:`Rpc<ydk.path.Rpc>`) Given RPC to be executed.

        :returns: :py:class:`DataNode<DataNode>`.

    .. py:method:: invoke(datanode)

        Invokes or executes the given DataNode and returns a :py:class:`DataNode<DataNode>` pointer if the action has an output modeled in YANG.

        :param datanode: (:py:class:`Rpc<ydk.path.DataNode>`) Given DataNode containing YANG 1.1 action to be executed.

        :returns: :py:class:`DataNode<DataNode>`.

    .. py:method:: execute_netconf_operation(rpc)

        Sends specified RPC to device (similar to `invoke` function) and returns device response as XML encoded string.

        :param rpc: (:py:class:`Rpc<ydk.path.Rpc>`) The RPC to be executed.

        :returns: ``str`` - reply from device in XML encoded string.

    .. py:method:: get_capabilities()

        Returns a list of capabilities of the client

        :returns: A list of ``str`` representing the client's/server's capabilities