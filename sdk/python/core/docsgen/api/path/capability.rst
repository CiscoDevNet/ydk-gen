Capability
==========

.. module:: ydk.path
    :synopsis: Path API' Capability

.. py:class:: Capability(model, revision)

    An instance of Capability is defined by the module name and revision.

    :param model: (``str``) Model name.
    :param revision: (``str``) Model revision.

    .. note::

        If no revision is provided, use empty string:

        .. code-block:: python

            >>> from ydk.path import Capability
            >>> cap1 = Capability('openconfig-bgp', '')
            >>> cap2 = Capability('openconfig-bgp', '2015-10-09')
