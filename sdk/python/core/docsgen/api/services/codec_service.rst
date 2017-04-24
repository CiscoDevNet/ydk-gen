Codec Service
=============

.. module:: ydk.services
    :synopsis: YDK Codec service

.. py:class:: CodecService()

    Supports encoding and decoding Python model API objects of type :py:class:`Entity<ydk.types.Entity>`.
\
    .. py:method:: encode(provider, entity)

        Encodes :py:class:`Entity<ydk.types.Entity>` in ``entity`` and returns the payload.

        :param provider: An instance of :py:class:`CodecServiceProvider<ydk.providers.CodecServiceProvider>`.
        :param entity: Either an instance of :py:class:`Entity<ydk.types.Entity>` or a dictionary of :py:class:`Entity<ydk.types.Entity>`
        :return: Either a single ``str`` for a single encoded payload or a dictionary of ``str``
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if error has occurred.

    .. py:method:: decode(provider, payload)

        Decodes payload in ``payload`` and returns :py:class:`Entity<ydk.types.Entity>`.

        :param provider: An instance of :py:class:`CodecServiceProvider<ydk.providers.CodecServiceProvider>`.
        :param payload: Either a single ``str`` for a single encoded payload or a dictionary of ``str``
        :return: Either a single decoded instance of :py:class:`Entity<ydk.types.Entity>` or a dictionary of decoded :py:class:`Entity<ydk.types.Entity>`
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if error has occurred.
