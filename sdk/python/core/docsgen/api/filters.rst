.. _filters-ydk:

YDK Filters
===========

.. class:: YOperation

    Represents edit operation for YDK objects as specified in
    `NETCONF RFC 6241 <https://tools.ietf.org/html/rfc6241#section-7.2>`_,
    defaults to ``not_set``, and  ``read`` operation providing functionality
    to read a singal leaf.
    Operations as defined under netconf edit-config operation attribute in
    `RFC 6241 <https://tools.ietf.org/html/rfc6241#section-7.2>`_ and for
    filtering read operations by leaf to be used with various :py:class:`YDK services<ydk.services>` and :py:class:`entities<ydk.path.Entity>`.
\
    .. attribute:: create

        The configuration data identified by the element
        containing this attribute is added to the configuration if
        and only if the configuration data does not already exist in
        the configuration datastore. If the configuration data
        exists, an :class:`ydk.errors.YPYServiceProviderError` will be thrown
        with XML error message.

    .. attribute:: delete

        The configuration data identified by the element
        containing this attribute is deleted from the configuration
        if and only if the configuration data currently exists in
        the configuration datastore. If the configuration data does
        not exist, an :class:`ydk.errors.YPYServiceProviderError` will be
        thrown with XML error message.

    .. attribute:: merge

        The configuration data identified by the element
        containing this attribute is merged with the configuration
        at the corresponding level in the configuration datastore
        identified by the target.

    .. attribute:: not_set

        This is the default behavior. No operation tag is attached to the
        payload.

    .. attribute:: remove

        The configuration data identified by the element
        containing this attribute is deleted from the configuration
        if the configuration data currently exists in the
        configuration datastore.  If the configuration data does not
        exist, the ``remove`` operation is silently ignored by the server.

    .. attribute:: replace

        The configuration data identified by the element
        containing this attribute replaces any related configuration
        in the configuration datastore identified by the target
        parameter.  If no such configuration data exists in the
        configuration datastore, it is created.

    .. attribute:: read

        When reading configuration or operational data from a network
        device and a specific leaf is desired to be read, the operation can
        be set to ``read`` on that leaf. See example :ref:`below<read-filter>`.
