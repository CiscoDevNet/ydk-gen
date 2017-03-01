.. _ref-datanode:

DataNode
========


.. cpp:namespace:: ydk::path

.. cpp:class:: DataNode

Class represents the :cpp:class:`DataNode<DataNode>`.

    .. cpp:function:: virtual ~DataNode()

        The destructor.

        .. note::

            A :cpp:class:`DataNode<DataNode>` represents a containment hierarchy. So invocation of the destructor will lead to the children of this node being destroyed.

    .. cpp:function:: virtual const SchemaNode& schema() const

        Returns the :cpp:class:`SchemaNode<SchemaNode>` associated with this :cpp:class:`DataNode<DataNode>`.

        :return: :cpp:class:`SchemaNode<SchemaNode>` associated with this :cpp:class:`DataNode<DataNode>`.

    .. cpp:function:: virtual std::string path() const

        Returns the path expression representing this Node in in the NodeTree.

        :return: ``std::string`` representing the path to this Node.

    .. cpp:function:: virtual DataNode& create(const std::string& path)

        Creates a :cpp:class:`DataNode<DataNode>` corresponding to the path and set its value.

        This methods creates a :cpp:class:`DataNode<DataNode>` tree based on the path passed in. The path expression must identify a single node. If the last node created is of schema type ``list``, ``leaf-list`` or ``anyxml`` that value is also set in the node.

        The returned :cpp:class:`DataNode<DataNode>` is the last node created (the terminal part of the path).

        The user is responsible for managing the memory of this returned tree. Use :cpp:func:`root` to get the root element of the this tree and use that pointer to dispose of the entire tree.

        .. note::

            In the case of ``list`` nodes the keys must be present in the path expression in the form of predicates.

        :param path: The XPath expression identifying the node.
        :param value: The string representation of the value to set.
        :return: Pointer to :cpp:class:`DataNode<DataNode>` created.
        :raises: :cpp:class:`YCPPInvalidArgumentError<YCPPInvalidArgumentError>` In case the argument is invalid.
        :raises: :cpp:class:`YCPPPathError<YCPPPathError>` In case the path is invalid.

    .. cpp:function:: virtual DataNode& create(const std::string& path, const std::string& value)

        Create a :cpp:class:`DataNode<DataNode>` corresponding to the path and set its value.

        This methods creates a :cpp:class:`DataNode<DataNode>` tree based on the path passed in. The path expression must identify a single node. If the last node created is of schema type ``list``, ``leaf-list`` or ``anyxml`` that value is also set in the node.

        The returned :cpp:class:`DataNode<DataNode>` is the last node created (the terminal part of the path).

        The user is responsible for managing the memory of this returned tree. Use :cpp:func:`root` to get the root element of the this tree and use that pointer to dispose of the entire tree.

        .. note::

            In the case of ``list`` nodes the keys must be present in the path expression in the form of predicates.

        :param path: The XPath expression identifying the node.
        :return: Pointer to :cpp:class:`DataNode<DataNode>` created.
        :raises: :cpp:class:`YCPPInvalidArgumentError<YCPPInvalidArgumentError>` In case the argument is invalid.
        :raises: :cpp:class:`YCPPPathError<YCPPPathError>` In case the path is invalid.

    .. cpp:function:: virtual void set(const std::string& value)

        Set the value of this :cpp:class:`DataNode<DataNode>`.

        .. note::

            - The :cpp:class:`DataNode<DataNode>` should represent a ``leaf`` , ``leaf-list`` or ``anyxml`` element for this to work. The value should be the string representation of the type of according to the schema.
            - This method does not validate the value being set. To validate please see the :cpp:class:`ValidationService<ValidationService>`.

        :param value: The value to set. This should be the string representation of the YANG type.
        :raises: :cpp:class:`YCPPInvalidArgumentError<YCPPInvalidArgumentError>` if the :cpp:class:`DataNode<DataNode>`'s value cannot be set (for example it represents a container)

    .. cpp:function:: virtual std::string get() const

        Returns a copy of the value of this :cpp:class:`DataNode<DataNode>`.

        :return: A ``std::string`` representation of the value.

    .. cpp:function:: virtual std::vector<std::shared_ptr<DataNode>> find(const std::string& path) const

        Finds nodes that satisfy the given path expression. For details about the path expression see :ref:`how to path <ref-howtopath>`.

        :param path: The path expression.
        :return: Vector of :cpp:class:`DataNode<DataNode>` that satisfy the path expression supplied.

    .. cpp:function:: virtual DataNode* parent() const

        :return: Pointer to the parent of this :cpp:class:`DataNode<DataNode>` or ``nullptr`` if None exist.

    .. cpp:function:: virtual std::vector<std::shared_ptr<DataNode>> children() const

        :return: Pointer to the children of this :cpp:class:`DataNode<DataNode>`.

    .. cpp:function:: virtual const DataNode& root() const

        :return: Pointer to the root :cpp:class:`DataNode<DataNode>` of this tree.


    .. cpp:function:: virtual void add_annotation(const Annotation& an)

        This method adds the annotation to this :cpp:class:`Datanode<DataNode>`.

        :param an: The annotation to add to this :cpp:class:`DataNode<DataNode>`.
        :raises: :cpp:class:`YCPPInvalidArgumentError<YCPPInvalidArgumentError>` in case the argument is invalid.

    .. cpp:function:: virtual bool remove_annotation(const Annotation& an)

        This method will remove the annotation from the given node.

        .. note::

            The ``m_val`` for annotation is ignored.

        :param an: The reference to the annotation.
        :return: ``bool`` If true the annotation was found and removed, false otherwise.

    .. cpp:function:: virtual std::vector<Annotation> annotations()

        Get the annotations associated with this data node.

        :return: Vector of :cpp:class:`Annotation<Annotation>` for this node.
