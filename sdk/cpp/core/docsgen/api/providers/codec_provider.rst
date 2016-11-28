CodecServiceProvider
======================

.. cpp:namespace:: ydk

.. cpp:enum:: EncodingFormat

        Type of encoding of the payload

        .. cpp:enumerator:: XML

        .. cpp:enumerator:: JSON

.. cpp:class:: CodecServiceProvider : public path::ServiceProvider

Implementation of :cpp:class:`ServiceProvider<path::ServiceProvider>` for performing encoding and decoding.

    .. cpp:function:: CodecServiceProvider(const path::Repository* repo, EncodingFormat encoding)

        Constructs an instance of the ``CodecServiceProvider``

        :param repository: Pointer to an instance of :cpp:class:`path::Repository<ydk::path::Repository>`
        :param encoding: Indicates type of encoding (currently, either :cpp:enumerator:`JSON<EncodingFormat::JSON>` or :cpp:enumerator:`XML<EncodingFormat::XML>`)        

    .. cpp:function:: ~CodecServiceProvider()
