CodecService
==============

.. cpp:namespace:: ydk

.. cpp:class:: CodecService : public Service

Codec Service class for supporting encoding and decoding C++ model API objects of type :cpp:class:`Entity<Entity>`

    .. cpp:function:: CodecService()

        Constructs an instance of CodecService

    .. cpp:function:: std::string encode(CodecServiceProvider & provider, Entity & entity, bool pretty=false)

        Perform encoding

        :param provider: An instance of :cpp:class:`CodecServiceProvider<ydk::CodecServiceProvider>`
        :param entity: An instance of :cpp:class:`Entity<ydk::Entity>` class defined under a bundle
        :param pretty: Optionally produce formatted output
        :return: Encoded payload in the form of ``std::string``
        :raises YCPPError: If an error has occurred        

    .. cpp:function:: std::unique_ptr<ydk::Entity> decode(path::ServiceProvider & provider, const std::string & payload)

        Decode the payload to produce an instance of `Entity`

        :param provider: An instance of :cpp:class:`CodecServiceProvider<ydk::CodecServiceProvider>`
        :param payload: Payload to be decoded
        :return: A pointer to an instance of the decoded :cpp:class:`Entity<ydk::Entity>`
        :raises YCPPError: If an error has occurred
