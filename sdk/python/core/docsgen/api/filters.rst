.. _filters-ydk:

Filters
=======

.. class:: ydk.filters.YFilter

    YFilter can be used to include the various operations supported in the `Netconf protocol's edit-config RPC <https://tools.ietf.org/html/rfc6241#section-7.2>`_ in your YDK app. YFilter can also be used to mark leafs, lists or containers for reading.

    .. attribute:: create

        The configuration data identified by the element
        containing this attribute is added to the configuration if
        and only if the configuration data does not already exist in
        the configuration datastore. If the configuration data
        exists, an :class:`ydk.errors.YServiceProviderError` will be thrown
        with XML error message.

    .. attribute:: delete

        The configuration data identified by the element
        containing this attribute is deleted from the configuration
        if and only if the configuration data currently exists in
        the configuration datastore. If the configuration data does
        not exist, an :class:`ydk.errors.YServiceProviderError` will be
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
        device and a specific node, like a leaf or a class, is desired to be read, the yfilter attribute can
        be set to ``YFilter.read`` on that node.
