CodecServiceProvider
======================

.. cpp:namespace:: ydk

.. cpp:class:: CodecServiceProvider : public path::ServiceProvider

Implementation of :cpp:class:`ServiceProvider<path::ServiceProvider>` for the netconf protocol.

    .. cpp:enum:: Encoding

        Type of encoding of the payload

        .. cpp:enumerator:: XML

        .. cpp:enumerator:: JSON

    .. cpp:function:: CodecServiceProvider(const path::Repository* repo, Encoding encoding)

        Constructs an instance of the ``CodecServiceProvider``

        :param repository: Pointer to an instance of :cpp:class:`path::Repository<ydk::path::Repository>`
        :param encoding: Indicates type of encoding (currently, either :cpp:enumerator:`JSON<Encoding::JSON>` or :cpp:enumerator:`XML<Encoding::XML>`)        

    .. cpp:function:: ~CodecServiceProvider()
