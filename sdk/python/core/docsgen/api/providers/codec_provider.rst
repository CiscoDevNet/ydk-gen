Codec Service Provider
======================

.. module:: ydk.providers
    :synopsis: YDK Codec Service provider

.. py:class:: CodecServiceProvider(type=EncodingFormat.XML, repo=None)

    A provider to be used with :py:class:`CodecService<ydk.services.CodecService>` for performing encoding and decoding.
\
    :param type: An argument specifies encoding format, could be a Python string (``xml`` or ``json``) or an instance of :py:class:`EncodingFormat<ydk.types.EncodingFormat>`.
    :type type: ``string`` or :py:class:`EncodingFormat<ydk.types.EncodingFormat>`
    :param repo: User provided repository stores cached models.
    :type repo: :py:class:`Repository<ydk.path.Repository>`

    .. py:method:: get_root_schema(bundle_name)

        Return root_schema for bundle_name.

        :param bundle_name: (``str``) Bundle name.
        :return: :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>` for this bundle.
