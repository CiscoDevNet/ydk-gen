.. _types-filters:

YDK Filters
===========

.. go:package:: ydk/yfilter
    :synopsis: YDK Go Filters

Filters represent edit operation for YDK objects as specified in
`NETCONF RFC 6241 <https://tools.ietf.org/html/rfc6241#section-7.2>`_,
defaults to ``not_set``, and  ``read`` operation providing functionality
to read a singal leaf.
Operations as defined under netconf edit-config operation attribute in
`RFC 6241 <https://tools.ietf.org/html/rfc6241#section-7.2>`_ and for
filtering read operations by leaf to be used with various :go:struct:`Services<ydk/services>` and 
:go:struct:`entities<ydk/types/Entity>`.

.. _y-filter:

.. attribute:: YFilter

    Represents the NETCONF edit config operations with underlying type ``int``

    .. attribute:: NotSet

        Represented by a value of 0.

    .. attribute:: Read

        Represented by a value of 1.

        When reading configuration or operational data from a network
        device and a specific leaf is desired to be read, the operation can
        be set to ``read`` on that leaf.

    .. attribute:: Merge

        Represented by a value of 2.

        The configuration data identified by the element
        containing this attribute is merged with the configuration
        at the corresponding level in the configuration datastore
        identified by the target.

    .. attribute:: Create

        Represented by a value of 3.

        The configuration data identified by the element
        containing this attribute is added to the configuration if
        and only if the configuration data does not already exist in
        the configuration datastore. If the configuration data
        exists, an error (:go:struct:`YGOServiceProviderError<ydk/types/YGOServiceProviderError>`) will be thrown
        with XML error message.

    .. attribute:: Remove

        Represented by a value of 4.

        The configuration data identified by the element
        containing this attribute is deleted from the configuration
        if the configuration data currently exists in the
        configuration datastore.  If the configuration data does not
        exist, the ``remove`` operation is silently ignored by the server.

    .. attribute:: Delete

        Represented by a value of 5.

        The configuration data identified by the element
        containing this attribute is deleted from the configuration
        if and only if the configuration data currently exists in
        the configuration datastore. If the configuration data does
        not exist, an :go:struct:`YGOServiceProviderError<ydk/types/YGOServiceProviderError>` will be
        thrown with XML error message.

    .. attribute:: Replace

        Represented by a value of 6.

        The configuration data identified by the element
        containing this attribute replaces any related configuration
        in the configuration datastore identified by the target
        parameter.  If no such configuration data exists in the
        configuration datastore, it is created.

.. function:: (yf YFilter) String()

    Returns the string representation of YFilter type

    :param yf: :ref:`YFilter <y-filter>`
    :return: The string representation of the given type
    :rtype: A Go ``string``
