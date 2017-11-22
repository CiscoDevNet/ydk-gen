.. _types-yang:
YANG Types
==========

.. go:package:: ydk/types
    :synopsis: YDK Go YANG Built-In Types

.. contents:: Table of Contents

The Types package provides built-in types specified in
`YANG RFC 6020 <https://tools.ietf.org/html/rfc6020>`_ and types used in YDK Go APIs.

These are how YANG types are represented in Go. 


YANG Built-in Types
-------------------

.. attribute:: Bits

    Represents a YANG built-in bits type with (map[string]bool).

.. go:struct:: Decimal64(value)

    Represents a YANG built-in decimal64 type.

    .. attribute:: value

        A string representation for decimal value.

.. go:struct:: Empty

    Represents a YANG built-in empty type.

.. function:: (e *Empty) String()

    Returns the string representation of empty type

    :param e: :go:struct:`Empty`
    :return: The string representation of the given type
    :rtype: A Go ``string``

.. go:struct:: EnumYLeaf

    Represents variable data

    .. attribute:: value

        The value of the variable

    .. attribute:: name

        The name of the variable

.. go:struct:: Enum

    Represents a YANG built-in enum type, a base type for all YDK enums.

    .. atribute:: EnumYLeaf

        (:go:struct:`EnumYLeaf`) A struct representation for enum value


YANG Leaf and Leaf-list
-----------------------

.. go:struct:: LeafData

    Represents the data contained in a YANG leaf

    .. attribute:: Value

        A Go ``string`` representing the data of the leaf

    .. attribute:: Filter

        Optional attribute which can be set to perform various filtering (:ref:`YFilter <y-filter>`)

    .. attribute:: IsSet

        ``bool`` representing whether the filter is set or not

.. go:struct:: NameLeafData

    Represents a YANG leaf to which a name and data can be assigned

    .. attribute:: Name

        A Go ``string`` representing the name of the leaf

    .. attribute:: Data

        The :go:struct:`LeafData <ydk/types/LeafData>` represents the data contained in the leaf

.. attribute:: NameLeafDataList

    A slice ([] :go:struct:`NameLeafData`) that represents a YANG leaf-list

.. function:: (p NameLeafDataList) Len()

    :return: The length of a given leaf-list
    :rtype: ``int``

.. function:: (p NameLeafDataList) Swap(i, j int)

    Swaps the :go:struct:`NameLeafData` at indices i and j

.. function:: (p NameLeafDataList) Less(i, j int)

    :return: If the name of the :go:struct:`NameLeafData` at index i is less than the one at index j
    :rtype: ``bool``

.. go:struct:: YLeaf

    Represents a YANG ``leaf`` to which data can be assigned.

    .. attribute:: name

        A Go ``string`` representing the name of the leaf

    .. attribute:: leafType

        :ref:`YType <y-type>` represents the YANG data type assigned to the leaf

    .. attribute:: bitsValue

        The :ref:`Bits <bits>` value assigned to the leaf

    .. attribute:: Value

        A Go ``string`` representing the value of the leaf

    .. attribute:: IsSet

        ``bool`` representing whether the leaf is set or not

    .. attribute:: Filter

        :ref:`YFilter <y-filter>`

.. function:: (y *YLeaf) GetNameLeafdata()

    Instantiates and returns name leaf data type for this leaf

    :return: name leaf data
    :rtype: :go:struct:`NameLeafData`

Example usage for creating a ``YLeaf`` of YANG type ``int8``:

.. code-block:: go

    import "github.com/CiscoDevNet/ydk-go/ydk/types"

    var yleaf YLeaf
    yleaf.leafType = types.Int8
    yleaf.name = "afi-safi-name"

.. go:struct:: YLeafList

    Represents a YANG ``leaf-list`` to which multiple instances of data can be appended to

    .. attribute:: name

        A Go ``string`` representing the name of the leaf-list

    .. attribute:: values

        A slice of :go:struct:`YLeaf` representing the multiple instances of data

    .. attribute:: Filter

        :ref:`YFilter <y-filter>`

    .. attribute:: leafType

        :ref:`YType <y-type>` represents the YANG data type assigned to the leaf

.. function:: (y *YLeafList) GetYLeafs()

    Getter function for :go:struct:`YLeafList` values

    :return: A slice of the multiple instances of data
    :rtype: [] :go:struct:`YLeaf`

.. function:: (y *YLeafList) GetNameLeafdata()

    Instantiates and returns name leaf data type for this leaf-list

    :return: slice of name leaf data
    :rtype: [] :go:struct:`NameLeafData`


YANG Container and List
-----------------------

.. go:struct:: EntityPath
    
    .. attribute:: Path

        A Go ``string`` representing the path

    .. attribute:: ValuePaths

        A slice ([] :go:struct:`NameLeafData`) representing a list of YANG leafs

.. _augment-capabilities-function:
.. attribute:: AugmentCapabilitiesFunction

    Represents an augment capabilities function with (func() map[string]string)

.. interface
.. go:struct:: Entity

    Basic type that represents containers in YANG

    .. function:: GetEntityPath(Entity)

        :return: :go:struct:`EntityPath`

    .. function:: GetSegmentPath()

        :return: (``string``) A Go string.

    .. function:: HasDataOrFilter()

        :return: (``bool``) A Go boolean.

    .. function:: SetValue(string, string)

    .. function:: GetChildByName(string, string)

        :return: :go:struct:`Entity`

    .. function:: GetChildren()

        :return: map[string] :go:struct:`Entity`

    .. function:: SetParent(Entity)

    .. function:: GetParent()

        :return: :go:struct:`Entity`

    .. function:: GetAugmentCapabilitiesFunction()

        :return: :ref:`AugmentCapabilitiesFunction <augment-capabilities-function>`

    .. function:: GetBundleYangModelsLocation()

        :return: (``string``) The bundle yang model's location

    .. function:: GetBundleName()

        :return: (``string``) The name of the bundle

    .. function:: GetYangName()

        :return: (``string``) The yang name

    .. function:: GetParentYangName()

        :return: (``string``) The parent's yang name

    .. function:: GetFilter()

        :return: :ref:`YFilter <y-filter>`
