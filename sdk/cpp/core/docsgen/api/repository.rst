.. _ref-repository:

Repository
==========


.. cpp:namespace:: ydk::path

.. cpp:class:: Repository

Represents the Repository of YANG models.

A instance of the :cpp:class:`Repository<Repository>` will be used to create a :cpp:class:`RootSchemaNode<RootSchemaNode>` given a set of :cpp:class:`Capabilities<Capability>`.

Behind the scenes the repository is responsible for loading and parsing the YANG modules and creating the :cpp:class:`SchemaNode<SchemaNode>` tree. The :cpp:class:`ServiceProvider<ServiceProvider>` is expected to use the method :cpp:func:`create_root_schema<create_root_schema>` to generate the :cpp:class:`RootSchemaNode<RootSchemaNode>`.


    .. cpp:function:: Repository()

        Constructs an instance of the ``Repository`` class. If the server supports model download, the repo will attempt to download all models from the server using the :cpp:class:`ModelProvider<ModelProvider>` provided using the :cpp:func:`add_model_provider<add_model_provider>` method.

        :raises: :cpp:class:`YCPPInvalidArgumentError<YCPPInvalidArgumentError>` if the search_dir is not a valid directory in the filesystem.

    .. cpp:function:: Repository(const std::string& search_dir)

        Constructs an instance of the ``Repository`` class.

        :param search_dir: The path in the filesystem where yang files can be found.
        :raises: :cpp:class:`YCPPInvalidArgumentError<YCPPInvalidArgumentError>` if the search_dir is not a valid directory in the filesystem.

    .. cpp:function:: RootSchemaNode* create_root_schema(const std::vector<Capability> capabilities) const

        Creates the root schema based on the ``std::vector`` of capabilities passed in.

        This method verifies the said capabilities and can throw exceptions if a module is not found in the search directory or cannot be loaded.

        :param capabilities: ``std::vector`` of :cpp:class:`Capability<Capability>`.
        :return: Pointer to the :cpp:class:`RootSchemaNode<RootSchemaNode>` or ``nullptr`` if one could not be created.

    .. cpp:function:: void add_model_provider(ModelProvider* model_provider)

        Adds a model provider to this Repository. If the repository does not find a model while trying to create a SchemaTree it calls on the model_provider to see if the said model can be downloaded by one of them. If that fails it tries the next.
        
        :param model_provider: The model provider to add

    .. cpp:function:: void remove_model_provider(ModelProvider* model_provider)

        Removes the given model provider from this repository

        :param model_provider: The model provider to remove

    .. cpp:function:: std::vector<ModelProvider*> get_model_providers() const

        Gets all model providers registered with this repository.

        :return: ``std::vector`` of model providers associated with this repository

    .. cpp:member:: std::string path

        Location where YANG models are present and/or downloaded to

