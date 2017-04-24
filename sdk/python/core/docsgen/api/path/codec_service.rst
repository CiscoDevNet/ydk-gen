Codec Service
=============

.. module:: ydk.path
    :synopsis: Path API' CodecService

.. py:class:: CodecService()

    YDK Path CodecService, used by :py:class:`CodecService<ydk.services.CodecService>` internally.

    .. py:method:: encode(data_node, encoding, pretty)

        Encoding data in ``data_node`` to string payload.

        :param data_node: (:py:class:`DataNode<ydk.path.DataNode>`) Path ``DataNode`` to encode.
        :param encoding: (:py:class:`EncodingFormat<ydk.types.EncodingFormat>`) Encoding format.
        :param pretty: (``bool``) Pretty format.

    .. py:method:: decode(root_schema_node, payload, encoding)

        :param root_schema_node: (:py:class:`RootSchemaNode<ydk.path.RootSchemaNode>`) A Path ``RootSchemaNode``
        :param payload: (``str``) Payload to decode.
        :param encoding: (:py:class:`EncodingFormat<ydk.types.EncodingFormat>`) Encoding format.
