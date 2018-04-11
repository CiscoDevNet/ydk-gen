Codec Service
=============


YDK CodecService class provides API for encoding and decoding of payload strings in XML or JSON format to/from instances of :py:class:`Entity<ydk.types.Entity>`, which represent containers in the device supported YANG models.

.. py:class:: ydk.services.CodecService()

    .. py:method:: encode(provider, entity, pretty=True, subtree=False)

        Encodes :py:class:`Entity<ydk.types.Entity>` into payload string in XML or JSON format.

        :param provider: :py:class:`CodecServiceProvider<ydk.providers.CodecServiceProvider>` - Codec Provider instance.
        :param entity: :py:class:`Entity<ydk.types.Entity>` instance or collection of :py:class:`Entity<ydk.types.Entity>` instances of type ``list`` or ``dict``.
        :param pretty: ``bool`` flag, which specifies if resulting string must be in human readable way with indentation.
        :param subtree: ``bool`` flag, which directs to encode entity to XML subtree.
        :return: Type of returned object corresponds to the type of **entity**: single payload ``str``, or ``list`` of ``str``, or a ``dictionary`` of ``str``.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: decode(provider, payload, subtree=False)

        Decodes **payload** string in XML or JSON format to instances of :py:class:`Entity<ydk.types.Entity>` class.

        :param provider: :py:class:`CodecServiceProvider<ydk.providers.CodecServiceProvider>` - Codec Provider instance.
        :param payload: ``str`` or collection of ``str`` Either a single encoded payload or a collection of payloads encapsulated to ``list`` or ``dict``.
        :param subtree: ``bool`` flag, which directs to encode entity to XML subtree.
        :return: Type of returned object corresponds to the type of **payload**. It is either an instance of :py:class:`Entity<ydk.types.Entity>`, or a collection of :py:class:`Entity<ydk.types.Entity>` instances of type ``list`` or ``dict``.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.
