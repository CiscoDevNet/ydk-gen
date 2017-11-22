.. _protocol-ydk:

YTypes
------

.. go:package:: ydk/types/ytype
    :synopsis: YType

.. y-type:
.. attribute:: YType

    Represents YANG data type with underlying type ``int``

    .. attribute:: Uint8

        Represents YANG's ``uint8`` (8-bit unsigned integer) by a value of 0

    .. attribute:: Uint16

        Represents YANG's ``uint16`` (16-bit unsigned integer) by a value of 1

    .. attribute:: Uint32

        Represents YANG's ``uint32`` (32-bit unsigned integer) by a value of 2

    .. attribute:: Uint64

        Represents YANG's ``uint64`` (64-bit unsigned integer) by a value of 3

    .. attribute:: Int8

        Represents YANG's ``int8`` (8-bit unsigned integer) by a value of 4

    .. attribute:: Int16

        Represents YANG's ``int16`` (16-bit unsigned integer) by a value of 5

    .. attribute:: Int32

        Represents YANG's ``int32`` (32-bit unsigned integer) by a value of 6

    .. attribute:: Int64

        Represents YANG's ``int64`` (64-bit unsigned integer) by a value of 7

    .. attribute:: Empty

        Represents YANG ``empty`` type (a leaf that does not have any value) by a value of 8

    .. attribute:: Identityref

        Represents YANG ``identityref`` type (a reference to an abstract identity) by a value of 9

    .. attribute:: Str

        Represents YANG ``string type by a value of 10
