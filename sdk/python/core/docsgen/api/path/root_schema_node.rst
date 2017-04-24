RootSchemaNode
==============

.. module:: ydk.path
    :synopsis: Path API' RootSchemaNode


.. py:class:: RootSchemaNode

    Instances of this class represent the ``Root`` of the ``SchemaTree``. A ``RootSchemaNode`` can be used to instantiate a ``DataNode`` tree or an ``Rpc`` object. The children of the ``RootSchemaNode`` represent the top level ``SchemaNode`` in the YANG module submodules.

    .. py:method:: create(path, value=None)

        Create data node with path and value.
        This methods creates a :py:class:`DataNode` tree based on the path passed in. The path expression must identify a single node. If the last node created is of schema type ``list``, ``leaf-list`` or ``anyxml`` that value is also set in the node.

        The returned ``DataNode`` is the last node created (the terminal part of the path).

        Note in the case of YANG ``list`` nodes the keys must be present in the path expression in the form of predicates.

        :param path: (``str``) The XPath expression identifying the node relative to the root of the schema tree.
        :param value: The string representation of the value to set.

    .. py:method:: find(path)

        Finds descendant nodes that match the given xpath expression.

        This API finds descendant nodes in the ``SchemaNode`` tree that satisfy the given path expression. See :ref:`howto-path`.

        :param path: (``str``) The path expression.
        :return: List of schema node satisfies the criterion.
        :rtype: list of :py:class:`SchemaNode`

    .. py:method:: parent()

        Get parent.

        :return: ``RootSchemaNode``'s parent, which is ``None``.
        :rtype: None

    .. py:method:: path()

        Get path.

        :return: ``RootSchemaNode``'s path, which is ``\``.
        :rtype: A Python string

    .. py:method:: root()

        Get the root schema node for ``RootSchemaNode``.

        :return: ``RootSchemaNode``'s Root schema node.
        :rtype: :py:class:`SchemaNode<ydk.path.SchemaNode>`

    .. py:method:: rpc(path)

        Create an Rpc instance.

        The path expression should point to a :py:class:`SchemaNode` that represents the :py:class:`Rpc`.

        :param path: (``str``) The path to the rpc schema node.
        :return: Rpc instance if it could be created.
        :rtype: None or :py:class:`Rpc`

        Excample usage for creating a ``ydk:create`` rpc:

        .. code-block:: python
            :linenos:

            from ydk.providers import NetconfServiceProvider
            provider = NetconfServiceProvider('127.0.0.1', 'admin', 'admin')
            root_schema = provider.get_root_schema()
            root_schema.rpc('ydk:create')
