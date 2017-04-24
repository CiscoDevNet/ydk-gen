SchemaNode
==========

.. module:: ydk.path
    :synopsis: Path API' SchemaNode


.. py:class:: SchemaNode

    Class represents a Node in the SchemaTree.

    .. note::

        The Python SchemaNode is wrapper for YDK C++ SchemaNode implementation. No constructor is defined and the user could not instantiate a ``SchemaNode`` instance. However, the user could get an instance of ``SchemaNode`` through :py:class:`DataNode<ydk.path.DataNode>`:

        .. code-block:: python

            >>> from ydk.providers import NetconfServiceProvider
            >>> provider = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 830)
            >>> root_schema = provider.get_root_schema()                               # <-- root_schema is an instance of RootSchemaNode
            >>> bgp = root_schema.create('openconfig-bgp:bgp')                         # <-- bgp is an instance of DataNode
            >>> schema_node = bgp.schema()                                             # <-- schema node for bgp

    .. py:method:: find(path)

        Finds descendant nodes that match the given xpath expression. This API finds descendant nodes in the schema node tree that satisfy the given path expression. See :ref:`howto-path`.

        :param path: (``str``) The path expression.
        :return: List of schema node satisfies the criterion.
        :rtype: list of :py:class:`SchemaNode`
        :raises RuntimeError: With ``YCPPPathError`` prefix if the path expression in invalid, see error code for details.
        :raises RuntimeError: With ``YCPPInvalidArgumentError`` if the argument is invalid.

    .. py:method:: parent()

        Get the parent node of this schema node in the tree.

        :return: Parent schema node.
        :rtype: :py:class:`SchemaNode`

    .. py:method:: path()

        Get the path expression representing this schema node in in the schema node tree.

        :return: Path to this schema node.
        :rtype: A Python string

    .. py:method:: root()

        Get the root schema node of current schema node.

        :return: Root schema node of current schema node.
        :rtype: :py:class:`SchemaNode`

    .. py:method:: statement()

        Get current schema node's YANG statement.

        :return: YANG statements for this schema node.
        :rtype: :py:class:`Statement`
