.. _ref-repository:

Repository
==========


.. cpp:namespace:: ydk::path

.. cpp:class:: Repository

Represents the Repository of YANG models.

A instance of the :cpp:class:`Repository<Repository>` will be used to create a :cpp:class:`RootSchemaNode<RootSchemaNode>` given a set of :cpp:class:`Capabilities<Capabilities>`.

Behind the scenes the repository is responsible for loading and parsing the YANG modules and creating the :cpp:class:`SchemaNode<SchemaNode>` tree. The :cpp:class:`ServiceProvider<ServiceProvider>` are expected to use the method ``create_root_schema`` to generate the :cpp:class:`RootSchemaNode<RootSchemaNode>`.

    .. cpp:member:: std::string m_search_dir

    .. cpp:function:: Repository(const std::string& search_dir)

        Constructor.

        :param m_search_dir: The path in the filesystem where yang files can be found.
        :raises: :cpp:class:`YDKInvalidArgumentException<YDKInvalidArgumentException>` if the search_dir is not a valid directory in the filesystem.

    .. cpp:function:: RootSchemaNode* create_root_schema(const std::vector<Capability> capabilities) const

        Creates the root schema based on the vector of capabilities passed in.

        This method verifies the said capabilities and can throw exceptions if a module is not found in the search directory or cannot be loaded.

        :param capabilities: Vector of :cpp:class:`Capability<Capability>`.
        :return: Pointer to the :cpp:class:`RootSchemaNode<RootSchemaNode>` or ``nullptr`` if one could not be created.
