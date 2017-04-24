NETCONF Service Provider
========================

.. module:: ydk.providers
    :synopsis: NETCONF Service provider

.. py:class:: NetconfServiceProvider(address, username, password, port=830, protocol='ssh', repo=None)

    Constructs an instance of the ``NetconfServiceProvider`` to connect to a server which **has** to support model download. Since the class is a Python wrapper for C++ ``NetconfServiceProvider`` class, which has clean up methods implemented in its destructor. The user does not need to worry about close NETCONF session.
\
    :param address: (``str``) IP address of the device supporting a netconf interface
    :param port: (``int``) The port to use, defaults to 830
    :param username: (``str``) Username to log in to the device
    :param password: (``str``) Password to log in to the device
    :param protocol: (``str``) Defaults to ``ssh``, currently support ``ssh``
    :param repo: User provided repository stores cached models
    :type repo: :py:class:`Repository<ydk.path.Repository>`

    .. py:method:: get_root_schema()

        Returns the :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>` tree supported by this instance.

        :return: A :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>` instance or ``None`` if it such root schema could not be created.

    .. py:method:: invoke(rpc)

        Invokes or executes the given rpc and returns a :py:class:`DataNode<ydk.path.DataNode>` instance if the Rpc has an output modelled in YANG.

        :param rpc: (:py:class:`Rpc<ydk.path.Rpc>`) Targeted Rpc.
        :return: A :py:class:`Datanode<ydk.path.DataNode>` instance.
