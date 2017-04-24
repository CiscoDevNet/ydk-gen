DataNode
========

.. module:: ydk.path
    :synopsis: Path API' DataNode


.. py:class:: DataNode

    Class represents a Path API's DataNode.

    .. note::

        The Python DataNode is wrapper for YDK C++ DataNode implementation. No constructor is defined and the user could not instantiate a ``DataNode`` instance. However, the user could get an instance of ``DataNode`` through :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>`:

        .. code-block:: python

            >>> from ydk.providers import NetconfServiceProvider
            >>> provider = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 830)
            >>> root_schema = provider.get_root_schema()                               # <-- root_schema is an instance of RootSchemaNode
            >>> bgp = root_schema.create('openconfig-bgp:bgp')                         # <-- bgp is an instance of DataNode


    .. py:method:: add_annotation(annotation)

        This method adds the annotation to this Datanode.

        :param annotation: (:py:class:`Annotation<ydk.path.Annotation>`) Annotation to be added.
        :raises RuntimeError: With ``YCPPInvalidArgumentError`` prefix in case the argument is invalid.

    .. py:method:: annotations()

        Get the annotations associated with this data node.

        :return: List of annotations for this data node.
        :rtype: list of :py:class:`Annotation<ydk.path.Annotation>`

    .. py:method:: children()

        Return list of children for this data node.

        :return: List of data node children.
        :rtype: list of :py:class:`DataNode<ydk.path.DataNode>`

    .. py:method:: create(path, value=None)

        Create a DataNode corresponding to the path and set its value, if provided.

        :param path: (``str``) The XPath expression identifying the node.
        :param value: (``str``) The value to be set.

        :raises RuntimeError: With ``YCPPPathError`` prefix in case the path is invalie.
        :raises RuntimeError: With ``YCPPInvalidArgumentError`` prefix in case the argument is invalid.

    .. py:method:: find(path)

        Finds nodes that satisfy the given path expression. For details about the path expression see :ref:`howto-path`.

        :param path: (``str``) The path expression.
        :return: Data node satisfy the path expression supplied.
        :rtype: list of :py:class:`DataNode<ydk.path.DataNode>`

    .. py:method:: get()

        Returns this data node's value.

        :return: Value of this data node.
        :rtype: A Python string

    .. py:method:: path()

        Returns the path expression representing this Node in in the NodeTree.

        :return: Path to this data node.
        :rtype: A Python string

    .. py:method:: remove_annotation(annotation)

        Remove given annotation for this node.

        :param annotation: (:py:class:`Annotation<ydk.path.Annotation>`) Annotation to be removed.
        :return: If ``True`` the annotation was found and removed, ``False`` otherwise.
        :rtype: bool

    .. py:method:: root()

        Get the root data node.

        :return: Root data node of current data node.
        :rtype: :py:class:`~DataNode`

    .. py:method:: schema()

        Get :py:class:`SchemaNode` associated with this :py:class:`DataNode`.

    .. py:method:: set(value)

        .. note::

            * The DataNode should represent a ``leaf`` , ``leaf-list`` or ``anyxml`` element for this to work. The value should be the string representation of the type of according to the schema.

            * This method does not validate the value being set.

        :param value: (``str``) The value to set. This should be the string representation of the YANG type.
        :raises RuntimeError: With ``YCPPInvalidArgumentError`` prefix if the its value cannot be set (for example it represents a container).
