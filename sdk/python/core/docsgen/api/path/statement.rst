Statement
=========

.. module:: ydk.path
    :synopsis: Path API' Statement

.. py:class:: Statement(keyword, arg)

    Represents the YANG Statement.

    :param keyword: (``str``) YANG keyword.
    :param arg: (``str``) YANG argument.

    .. py:attribute:: keyword

        Read only attribute for YANG keyword.

    .. py:attribute:: arg

        Read only attribute for YANG argument.

    Example usage for creating a statement:

    .. code-block:: python

        >>> from ydk.path import Statement
        >>> s = Statement('container', 'bgp')

    Example usage for getting statement from schema node:

    .. code-block:: python

        >>> from ydk.providers import NetconfServiceProvider
        >>> provider = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 830)
        >>> root_schema = provider.get_root_schema()                               # <-- root_schema is an instance of RootSchemaNode
        >>> bgp = root_schema.create('openconfig-bgp:bgp')                         # <-- bgp is an instance of DataNode
        >>> schema_node = bgp.schema()                                             # <-- schema node for bgp
        >>> statement = schema_node.statement()                                    # <-- YANG statement for this schema node
        >>> statement.keyword
        'container'
        >>> statement.arg
        'bgp'
