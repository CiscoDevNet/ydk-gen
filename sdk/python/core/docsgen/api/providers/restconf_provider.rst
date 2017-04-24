RESTCONF Service Provider
=========================

.. module:: ydk.providers
    :synopsis: YDK RESTCONF Service provider

.. py:class:: RestconfServiceProvider(repo, address, username, password, port, encoding)

    Constructs an instance of the ``RestconfServiceProvider`` to connect to a server which has to support model download. Since the class is a Python wrapper for C++ ``RestconfServiceProvider`` class, which has clean up methods implemented in its destructor. The user does not need to worry about close RESTCONF session.
\
    :param repo: (:py:class:`Repository<ydk.path.Repository>`) User provided repository stores cached models
    :param address: (``str``) IP address of the device supporting a restconf interface
    :param username: (``str``) Username to log in to the device
    :param password: (``str``) Password to log in to the device
    :param port: (``int``) Device port used to access the restconf interface, the default value being 80
    :param encoding: (:py:class:`EncodingFormat<ydk.types.EncodingFormat>`) Type of encoding to be used for the payload, the default being :py:attr:`JSON<ydk.types.EncodingFormat.JSON>`

    .. py:method:: get_root_schema()

        Returns the :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>` tree supported by this instance.

        :return: A :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>` instance or ``None`` if it such root schema could not be created.

    .. py:method:: invoke(rpc)

        Invokes or executes the given rpc and returns a :py:class:`DataNode<ydk.path.DataNode>` instance if the Rpc has an output modelled in YANG

        :param rpc: (:py:class:`Rpc<ydk.path.Rpc>`) Targeted Rpc
        :return: A :py:class:`Datanode<ydk.path.DataNode>` instance
