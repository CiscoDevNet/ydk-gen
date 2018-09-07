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

    .. attribute:: EnumYLeaf

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

.. attribute:: List

    In YDK YANG list is represented by Go slice of references to a structure, which implements interface :ref:`Entity <types-entity>`. 
    
    Example: 
    
    If Go structure `YangList` implements interface :ref:`Entity <types-entity>`, meaning implements `func (e *YangList) GetEntityData() *CommonEntityData {}`, then the list of entities should be implemented as `[]*YangList`. \
    according to the YANG model the list may have one or more keys, which uniquely identify list element, or may have no keys. The slices can be created and list elements can be accessed using standard Go functions and methods. \
    The YDK provides package `ylist` containing functions, which allow user access list elements by key or keys.
    
    .. function:: Get(slice interface{}, keys ... interface{}) (int, Entity)
    
       Get list element (entity) by key or keys, if list element has more than one key
       
       :param slice: Slice variable, which is defined in the list holding entity
       :param keys:  Comma separated list of key values
       :return: Tuple of found element index and the element itself, which has matching key(s) value(s). If element is not found - (-1, nil).
       :rtype: Tuple( `int`, :ref:`Entity <types-entity>`)
    
    .. function:: Keys(slice interface{}) []interface{} Entity
    
       Get keys for all list elements
       
       :param slice: Slice variable, which is defined in the list holding entity
       :return: Values of all keys, that have matching elements in the list
       :rtype: Go ``[]interface{}``; if list has more than one key, the set of keys for one element is returned as ``[]interface{}``
       
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

        A ``string`` representing Yang name of the entity

    .. attribute:: BundleName

        A ``string`` representing the bundle name of the entity

    .. attribute:: ParentYangName

        A ``string`` representing the parent Yang name of the entity

    .. attribute:: YFilter

        A :ref:`YFilter <y-filter>` representing the a filter

    .. attribute:: Children

        A ``map`` of ``string`` representing Yang name to :go:struct:`YChild`, representing the children

    .. attribute:: Leafs

        A ``map`` of ``string`` representing Yang name to :go:struct:`YLeaf`, representing the leafs

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

    .. function:: GetEntityData() *CommonEntityData
    
        The :ref:`Entity <types-entity>` interface function

        :return: a pointer to :go:struct:`CommonEntityData` representing entity data

.. function:: GetSegmentPath(entity Entity) string

    :param entity: An instance of :ref:`Entity <types-entity>`
    :return: The entity's `SegmentPath` value

.. function:: GetParent(entity Entity) Entity

    :param entity: An instance of :ref:`Entity <types-entity>`
    :return: :ref:`Entity <types-entity>`, which represents given entity's parent; if parent entity is not set, returns ``nil``

.. function:: SetParent(entity, parent Entity)

    SetParent sets the given :ref:`Entity <types-entity>` parent field to the given parent :ref:`Entity <types-entity>`

    :param entity: An instance of :ref:`Entity <types-entity>`

.. function:: HasDataOrFilter(entity Entity) bool

    :param entity: An instance of :ref:`Entity <types-entity>`
    :return: A Go boolean representing whether the :ref:`Entity <types-entity>` or any of its children have their data/filter set

.. function:: GetEntityPath(entity Entity)

    :param entity: An instance of :ref:`Entity <types-entity>`
    :return: :go:struct:`EntityPath` for the given entity

.. function:: GetChildByName(entity Entity, childYangName string, segmentPath string) Entity

    Finds entity's child entity by name and segment path

    :param entity: An instance of :ref:`Entity <types-entity>`
    :param childYangName: The `YangName` of the child entity
    :param segmentPath: The `SegmentPath` value of the child entity
    :return: :ref:`Entity <types-entity>` described by the given `childYangName` and `segmentPath` or ``nil`` if there is no match

.. function:: SetValue(entity Entity, valuePath string, value interface{})

    Sets leaf value

    :param entity: An instance of :ref:`Entity <types-entity>`
    :param valuePath: The :go:struct:`YLeaf` `name` value
    :param value: Instance of value interface

.. function:: IsPresenceContainer(entity Entity) bool

    returns if the given :ref:`Entity <types-entity>` is a presence container

    :param entity: An instance of :ref:`Entity <types-entity>`
    :return: A Go boolean representing whether the :ref:`Entity <types-entity>` is a presence container or not

.. function:: GetPresenceFlag(entity Entity) bool

    returns whether the presence flag of the given :ref:`Entity <types-entity>`

    :param entity: An instance of :ref:`Entity <types-entity>`
    :return: A Go boolean representing whether the :ref:`Entity <types-entity>` is the presence flag has been set or not

.. function:: SetPresenceFlag(entity Entity)

    sets the presence flag of the given :ref:`Entity <types-entity>` if it is a presence container

    :param entity: An instance of :ref:`Entity <types-entity>`

.. function:: EntityToString(entity Entity) string

    Utility function to get string representation of the entity.

    :return:  Go ``string`` in format: "Type: `entity-instance-type`, Path: `entity-segment-path`".



.. _entity-collection:

.. go:struct:: EntityCollection

    Type `EntityCollection` along with its methods implements ordered map collection of entities. The string value of entity `SegmentPath` serves as a map key for the entity. Ordered means, the collection retains order of entities, in which they were added to collection. 
    
    The `EntityCollection` type has two aliases - `Config` and `Filter`.

    .. function:: GetEntityData() *CommonEntityData

        Implements :ref:`Entity <types-entity>` interface.
        
        :return: a reference to :go:struct:`CommonEntityData` representing data for the first entity in the collection; ``nil`` - if collection is empty

    .. function:: Add(entities ... Entity)
        
        Method of :ref:`EntityCollection <entity-collection>`. Adds new elements into collection.
        
        :param entities: Non-empty list of comma separated instances of :ref:`Entity <types-entity>`.
          
    .. function:: Append(entities []Entity)
        
        Method of :ref:`EntityCollection <entity-collection>`. Adds new elements into `EntityCollection`.
       
        :param entities: Non-empty slicen of 'Entity' type instances.
            
    .. function:: Len() int
        
        Method of :ref:`EntityCollection <entity-collection>`.
        
        :return: Number of elements in the collection.
        
    .. function:: Get(key string) Entity
        
        Method of :ref:`EntityCollection <entity-collection>`. Gets collection elements by key.
        
        :param key: Go ``string``, which represents `SegmentPath` of an entity.
        :return: Instance of Entity if matching key is present in collection, ``nil`` - otherwise.
    
    .. function:: GetItem(item int) Entity
    
        Method of :ref:`EntityCollection <entity-collection>`. Gets collection elements by item/order number.
    
        :param item: Sequential number of an entity in the collection.
        :return: Instance of `Entity`, which stands in the ordered map by the number `item`; ``nil`` - `item` value is not in the range of collection size.
    
    .. function:: HasKey(key string) bool
    
        Method of :ref:`EntityCollection <entity-collection>`. Checks if the collection contains an entity with given key value.
    
        :param key: Go ``string``, which represents `SegmentPath` of an entity.
        :return: Go ``bool``: ``true`` - if collection contains corresponding entity; ``false`` - otherwise.
        
    .. function:: Pop(key string) Entity
    
        Method of :ref:`EntityCollection <entity-collection>`. Removes entity from collection. When element is removed from inside of the collection, all the following elements are shifted, meaning their indexes changed by -1. This way the order of elements is retained.
    
        :param key: Go ``string``, which represents `SegmentPath` of an entity.
        :return: Instance of `Entity`, if corresponding `key` is found in the collection; ``nil`` - otherwise.
    
    .. function:: Clear()
    
        Method of :ref:`EntityCollection <entity-collection>`. Removes all elements from collection.
    
    .. function:: Keys() []string
    
        Method of :ref:`EntityCollection <entity-collection>`. 
    
        :return: Slice of ``string`` representing array of all the keys in the collection.
        
    .. function:: Entities() []Entity
    
        Method of :ref:`EntityCollection <entity-collection>`. 
        
        :return: Slice/array of all the `Entity` instances in the collection.
        
    .. function:: String() string
    
        Method of :ref:`EntityCollection <entity-collection>`. 
        
        :return: Go ``string``, which represents entity collection.

.. function:: NewEntityCollection(entities ... Entity) EntityCollection

    Function creates new `EntityCollection` instance and populates it with supplied entities.
    
    :param entities: list of comma separated instances of :ref:`Entity <types-entity>`, including empty list.
    :return: Instance of `EntityCollection`, which includes specified in the parameters entities. If no entities is listed as parameters, the function returns empty collection.
    
.. function:: NewConfig(entities ... Entity) Config
 
    Function creates new `Config` instance similar to function `NewEntityCollection`.

.. function:: NewFilter(entities ... Entity) Filter
 
    Function creates new `Filter` instance similar to function `NewEntityCollection`.

.. function:: EntityToCollection (entity Entity) *EntityCollection

    Function converts or casts :ref:`Entity <types-entity>` to :ref:`EntityCollection <entity-collection>`.
    
    :param entity: Instance of type `Entity` or `EntityCollection`.
    :return: Pointer to instance of `EntityCollection`. If parameter is instance of `EntityCollection` the function returns the same. If `entity` is instance of `Entity`, the function creates new entity collection, which includes `entity` as its element.
    
.. function:: IsEntityCollection (entity Entity) bool

    Function checks type of `entity`.
    
    :param entity: Instance of type `Entity` or `EntityCollection`.
    :return: Go ``bool``: ``true`` - if `entity` is instance of :ref:`EntityCollection <entity-collection>`; ``false`` - otherwise.

    