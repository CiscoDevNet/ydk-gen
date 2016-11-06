.. _ref-codecservice:

path::CodecService
===================


.. cpp:namespace:: ydk::path

.. cpp:class:: ydk::path::CodecService

Codec Service, part of YDK path API, which deals with generic path-based YANG data nodes

    .. cpp:function:: virtual std::string encode(const DataNode* datanode, Format format, bool pretty)

        Encode the given DataNode Tree

        :param datanode: The :cpp:class:`DataNode<path::DataNode>` to encode
        :param format: :cpp:enum:`Format<Format>` to encode to
        :param pretty: The output is indented for human consumption if pretty is ``true``
        :return: The encoded string
        :raises: :cpp:class:`YDKInvalidArgumentException<YDKInvalidArgumentException>` if the arguments are invalid

    .. cpp:function:: virtual DataNode* decode(const RootSchemaNode* root_schema, const std::string& buffer, Format format)

        Decode the buffer to return a DataNode

        :param root_schema: The root schema to use
        :param buffer: The string representation of the :cpp:class:`DataNode<DataNode>`
        :param format: Decode format
        :return: The :cpp:class:`DataNode<DataNode>` instantiated or ``nullptr`` in case of error.
        :raises: :cpp:class:`YDKInvalidArgumentException<YDKInvalidArgumentException>` if the arguments are invalid.

    .. cpp:enum:: Format

        These options can be used for encoding the given tree.

        .. cpp:enumerator:: XML

            XML

        .. cpp:enumerator:: JSON

            JSON

    .. cpp:function:: virtual ~CodecService()