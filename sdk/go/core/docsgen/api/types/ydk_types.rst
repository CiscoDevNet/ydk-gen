.. _types-ydk:

YDK Types
=========

.. go:package:: ydk/types
    :synopsis: YDK Types

.. code-block:: sh

   import "github.com/CiscoDevNet/ydk-go/ydk/types"

.. _types-service-provider:

.. object:: ServiceProvider

    ServiceProvider is an interface type defined for supported :ref:`ydk-providers`.

    .. function:: GetPrivate() 

        :rtype: ``interface{}``

    .. function:: Connect()
    
    .. function:: Disconnect()

    .. function:: GetState() 

        :rtype: :go:struct:`*State<ydk/errors/State>`

.. object:: CodecServiceProvider

    CodecServiceProvider is an interface type for :go:struct:`CodecServiceProvider<ydk/providers/CodecServiceProvider>`

    .. function:: Initialize(Entity)

    .. function:: GetEncoding()

        :rtype: :ref:`encoding-format-ydk`

    .. function:: GetRootSchemaNode(Entity)

        :rtype: :go:struct:`RootSchemaNode<ydk/types/RootSchemaNode>`

    .. function:: GetState()

        :rtype: :go:struct:`*State<ydk/errors/State>`

.. go:struct:: DataNode

    .. attribute:: Private

        Type is ``interface{}``

.. go:struct:: RootSchemaNode
    
    .. attribute:: Private

        Type is ``interface{}``

.. go:struct:: CServiceProvider

    .. attribute:: Private

        Type is ``interface{}``

.. go:struct:: COpenDaylightServiceProvider

    .. attribute:: Private

        Type is ``interface{}``

.. go:struct:: Repository

    .. attribute:: Path

        Type is ``string``

    .. attribute:: Private

        Type is ``interface{}``

YANG Types
----------

The Types package provides built-in types specified in
`YANG RFC 6020 <https://tools.ietf.org/html/rfc6020>`_ and types used in YDK Go APIs.

These are how YANG types are represented in Go. 

.. _type-bits:

.. attribute:: Bits

    Represents a YANG built-in bits type with base type of ``map[string]bool``.

.. go:struct:: Decimal64(value)

    Represents a YANG built-in decimal64 type.

    .. attribute:: Value

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

.. go:struct:: EntityPath
    
    .. attribute:: Path

        A Go ``string`` representing the path

    .. attribute:: ValuePaths

        A slice ([] :go:struct:`NameLeafData`) representing a list of YANG leafs

.. go:struct:: YChild

    YChild encapsulates the GoName of an entity as well as the entity itself

    .. attribute:: GoName

        A ``string`` representing the GoName of an entity

    .. attribute:: Value

        The :ref:`Entity <types-entity>` itself

.. go:struct:: YLeaf

    YLeaf encapsulates the GoName of a leaf as well as the leaf itself

    .. attribute:: GoName

        A ``string`` representing the GoName of an entity

    .. attribute:: Value

        The leaf (type ``interface{}``) itself

.. go:struct:: CommonEntityData

    CommonEntityData encapsulate common data within an :ref:`Entity <types-entity>`

    .. attribute:: YangName

        A ``string`` representing the yang name

    .. attribute:: BundleName

        A ``string`` representing the bundle name

    .. attribute:: ParentYangName

        A ``string`` representing the parent yang name

    .. attribute:: YFilter

        A :ref:`YFilter <y-filter>` representing the yfilter

    .. attribute:: Children

        A ``map`` of ``string`` representing yang name to :go:struct:`YChild`, representing the children

    .. attribute:: Leafs

        A ``map`` of ``string`` representing yang name to :go:struct:`YLeaf`, representing the leafs

    .. attribute:: SegmentPath

        A ``string`` representing the segment path

    .. attribute:: CapabilitiesTable

        A ``map[string]string`` representing the capabilities table

    .. attribute:: NamespaceTable

        A ``map[string]string`` representing the namespace table

    .. attribute:: BundleYangModelsLocation

        A ``string`` representing the models path

    .. attribute:: Parent

        An :ref:`Entity <types-entity>` representing the parent

.. _types-entity:

.. object:: Entity

    An interface type that represents a basic container in YANG

    .. function:: GetEntityData()

        :return: a reference to :go:struct:`CommonEntityData` representing the data of the entity

.. function:: GetSegmentPath(entity Entity)

    GetSegmentPath returns the segment path

    :return: (``string``) A Go string.

.. function:: GetParent(entity Entity)

    GetParent returns the given entity's parent

    :return: :ref:`Entity <types-entity>`

.. function:: SetParent(entity, parent Entity)

    SetParent sets the given :ref:`Entity <types-entity>` parent field to the given parent :ref:`Entity <types-entity>`

.. function:: HasDataOrFilter(entity Entity)

    HasDataOrFilter returns a bool representing whether the :ref:`Entity <types-entity>` or any of its children have their data/filter set

    :return: (``bool``) A Go boolean.

.. function:: GetEntityPath(entity Entity)

    GetEntityPath returns an EntityPath struct for the given entity

    :return: :go:struct:`EntityPath`

.. function:: GetChildByName(entity Entity, childYangName, segmentPath string)

    GetChildByName takes an :ref:`Entity <types-entity>` and returns the child :ref:`Entity <types-entity>` described by the given childYangName and segmentPath or nil if there is no match

    :return: :ref:`Entity <types-entity>`

.. function:: SetValue(entity Entity, valuePath string, value interface{})

    Given an entity, SetValue sets the leaf specified by valuePath to the given value
