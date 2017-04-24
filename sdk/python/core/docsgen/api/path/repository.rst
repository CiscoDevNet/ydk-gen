Repository
==========

.. module:: ydk.path
    :synopsis: Path API' Repository

.. py:class:: Repository(*args)

    Repository is used to create a :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>` given a set of Capabilities. Behind the scenes the repository is responsible for loading and parsing the YANG modules and creating the :py:class:`SchemaNode<ydk.path.SchemaNode>` tree. Service provider is expected to use the method :py:meth:`create_root_schema<ydk.path.Repository.create_root_schema>` to generate the :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>`.

    :param args: Search directory of type ``str`` or nothing.

    .. note::

        If a directory path of type ``str`` is provided, model search path will be located in this path, otherwise, default location ``~/.cache/ydk`` will be used. Example usage:

        .. code-block:: python

            >>> import os
            >>> from ydk.path import Repository
            >>> repo_path = os.path.join(os.path.expanduser('~'), 'Cisco', 'yang') # this directory should exist
            >>> default_repo = Repository()                                        # repository using default location
            >>> custom_repo = Repository(repo_path)                                # custom repository

    .. py:method:: create_root_schema(capabilities)

        Creates the root schema based on the capabilities passed in. This method verifies the said capabilities and can throw exceptions if a module is not found in the search directory or cannot be loaded.

        :param capabilities: (list of :py:class:`Capability<ydk.path.Capability>`) Enabled capabilities.
