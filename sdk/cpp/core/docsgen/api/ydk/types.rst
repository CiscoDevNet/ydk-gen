.. _ref-types:

Types
-----

.. contents::
.. toctree::
   :maxdepth: 2

The C++ types present in ydk namespace corresponding to YANG types. See below for example usage.


.. cpp:enum-class:: ydk::EncodingFormat

    Format of encoding to be used when creating the payload.

    .. cpp:enumerator:: XML

        XML

    .. cpp:enumerator:: JSON

        JSON


.. cpp:enum-class:: ydk::Protocol

    Type of protocol.

    .. cpp:enumerator:: netconf

        Netconf protocol

    .. cpp:enumerator:: restconf

        Restconf protocol


YANG types
==========

.. cpp:class:: ydk::Bits

    Concrete class representing a bits data type.

.. cpp:class:: ydk::Decimal64

    Concrete class representing a decimal64 data type.

.. cpp:class:: ydk::Empty

    Represents the empty type in YANG. The empty built-in type represents a leaf that does not have any value, it conveys information by its presence or absence.

.. cpp:class:: ydk::Enum

    Super class of all classes that represents the enumeration type in YANG.

.. cpp:class:: ydk::Identity

    Super class of all classes that represents the identity type in YANG. Instances of this type of classes are assigned as data to leafs of ``identityref`` type.


YANG leaf, leaf-list, and list
==============================

.. cpp:enum:: ydk::YType

    Enum class to represent various types of leaf value

.. code-block:: c++

    enum class YType {
        uint8,
        uint16,
        uint32,
        uint64,
        int8,
        int16,
        int32,
        int64,
        empty,
        identityref,
        str,
        boolean,
        enumeration,
        bits,
        decimal64
    };

.. cpp:class:: ydk::YLeaf

    Concrete class that represents a YANG leaf

    .. cpp:function:: YLeaf(YType type, std::string name)

        Constructs an instance of the `YLeaf`.

        :param type: :cpp:enum:`YType<ydk::YType>`, type of the leaf 
        :param name: YANG name of the leaf

    .. cpp:member:: std::string name

        Leaf YANG name

    .. cpp:member:: std::string value

        String representation of the leaf value

    .. cpp:member:: int enum_value

        Enum member value for `enumeration` type of a leaf

    .. cpp:member:: YType type

        Leaf type from :cpp:enum:`YType<ydk::YType>`

    .. cpp:member:: YFilter yfilter

        Optional attribute of the YLeaf class, which can be set to perform various :cpp:class:`filtering<ydk::YFilter>` operations

    .. cpp:function:: void operator = (YFilter filter)

        A shortcut operator to set `yfilter` value

.. cpp:class:: YLeafList

    Concrete class that represents a YANG leaf-list, to which multiple instances of data can be appended to.

    .. cpp:function:: YLeafList(YType type, std::string name)

        Constructs an instance of the `YLeafList`.

        :param type: :cpp:enum:`YType<ydk::YType>`, type of the leaves in the leaf-list
        :param name: YANG name of the leaf-list

    .. cpp:member:: YFilter yfilter

        Optional attribute of the YLeafList class, which can be set to perform various :cpp:class:`filtering<ydk::YFilter>` operations

    .. cpp:function:: void append(Type val)

        Appends a value to the leaf-list; the Type is one of YDK supported data types listed in :cpp:enum:`YType<ydk::YType>`

    .. cpp:function:: void operator = (YFilter filter)

        A shortcut operator to set `yfilter` value

    .. cpp:function:: YLeaf & operator [] (size_t index)

        Access function for the YLeafList elements by their index

    .. cpp:function:: std::vector<YLeaf> getYLeafs() const

        Access function to get all elements in the YLeafList

.. cpp:class:: ydk::YList

    Concrete class that represents a YANG list. The class implements ordered dictionary, which can be accessed by item number or key value. The key value is calculated internally based on YLeaf key values for the entity.

    .. cpp:function:: YList(std::initializer_list<std::string> key_names)

        Constructs an instance of the `YList`.

        :param key_names: list of strings, which represent names of keys for the list entities, including empty list.
        
    .. cpp:function:: std::shared_ptr<Entity> operator [] (const std::string& key) const

        Gets `YList` entity, which has matching `key` value.

        :param key: ``strings``, which represent list entity key(s).
        :return: ``std::shared_ptr<Entity>`` for the found entity, nullptr - otherwise.
        
    .. cpp:function:: std::shared_ptr<Entity> operator [] (const std::size_t item) const

        Gets `YList` element, which has sequence number `item`. The sequence is defined by the order in which the entity was added to the list.

        :param key: ``std::size_t``, which represents entity sequential number in the list (starts from 0).
        :return: ``std::shared_ptr<Entity>`` for the found entity.
        :raises: `YInvalidArgumentError` exception if `item` value is out of bounds.
        
    .. cpp:function:: void append(std::shared_ptr<Entity> ep)

        Adds entity to the list.

        :param ep: ``std::shared_ptr<Entity>``, which represents list entity.
        
    .. cpp:function:: void extend(std::initializer_list<std::shared_ptr<Entity>> ep_list)

        Adds multiple entities to the list.

        :param ep: ``std::initializer_list<std::shared_ptr<Entity>>``, which represents list of entities to be appended to the YList.
        
    .. cpp:function:: std::vector<std::string> keys() const

        Gets key values for all entities in the list.

        :return: ``std::vector<std::string>``.
        
    .. cpp:function:: std::vector<std::shared_ptr<Entity>> entities() const

        Gets vector of all entities in the list in order, in which they were added.

        :return: ``std::vector<std::shared_ptr<Entity>>``.
        
    .. cpp:function:: std::size_t len() const

        Returns size of the list.

    .. cpp:function:: std::shared_ptr<Entity> pop(const std::string& key)

        Removes from `YList` an entity, which has matching `key` value.

        :param key: ``strings``, which represent list entity key(s).
        :return: ``std::shared_ptr<Entity>`` for the removed entity, or nullptr matching `key` value is not found.
        
    .. cpp:function:: std::shared_ptr<Entity> pop(const std::size_t item)

        Removes from `YList` an entity, which has sequence number `item`. The sequence number is defined by the order in which the entity was added to the list.

        :param key: ``std::size_t``, which represents entity sequential number in the list (starts from 0).
        :return: ``std::shared_ptr<Entity>`` for the found entity.
        :raises: `YInvalidArgumentError` exception if `item` value is out of bounds.
        
YANG container and list
=======================

.. cpp:class:: ydk::Entity

    Super class of all classes that represents containers in YANG. YANG lists are represented as `YList` instance

    .. cpp:member:: Entity* parent
    
        Pointer to parent entity; the parent is set automatically during entity initialization except for presence container, which must be set manually after presence container is initialized

    .. cpp:member:: std::string yang_name
    
        YANG name of container or list that this entity represents

    .. cpp:member:: std::string yang_parent_name
    
        YANG name of container or list of parent entity

    .. cpp:member:: YFilter yfilter
    
        Optional attribute of the `Entity` class, which can be set to perform various :cpp:class:`filtering<ydk::YFilter>` operations

    .. cpp:member:: bool is_presence_container
    
        Boolean flag set to `true` if this entity represents presence container

    .. cpp:member:: bool is_top_level_class

        Boolean flag set to `true` if this entity represents top-level container (does not have parent entity)

    .. cpp:member::     bool has_list_ancestor

        Boolean flag set to `true` if this entity is member of a list

    .. cpp:member::     bool ignore_validation

        Boolean flag for user to control validation of entity data (leaf and leaf-list data values); default setting is `false`, meaning the validation is on

    .. cpp:member:: std::vector<std::string> ylist_key_names

        If this entity is member of a list, the vector specifies leaf names, which represents list keys 

    .. cpp:member::     std::string ylist_key

        If this entity is member of a list, the `ylist_key` is set to composite list key of this entity

    .. cpp:member::     YList* ylist

        If this entity is member of a list, the `ylist` is set to a pointer of corresponding `YList` class

    .. cpp:function:: std::shared_ptr<Entity> clone() const

        Returns copy of this entity

    .. cpp:function:: std::shared_ptr<Entity> clone_ptr() const

        Returns new instance of the entity class

    .. cpp:function:: std::string get_segment_path() const

        Returns relative path of this entity in terms of XPath

    .. cpp:function:: std::string get_absolute_path() const

        Returns absolute path of this entity in terms of XPath

    .. cpp:function:: bool has_data() const

        Returns `true` if any leaf in this entity or its child entity is assigned value; `false` otherwise

    .. cpp:function:: bool has_operation() const

        Returns `true` if any leaf or container in this entity or its child entity has setting of `yfilter`; `false` otherwise

    .. cpp:function:: void set_value(const std::string & path, const std::string & value, const std::string & name_space="", const std::string & name_space_prefix="")
    
        Sets leaf value

        :param path: YANG name of the leaf
        :param value: std::string representation of the leaf value
        :param name_space: optional parameter for enumeration and identity types of leaf, which is name space of a module where the enum or identity is defined
        :param name_space_prefix: optional parameter for enumeration and identity types of leaf, which is name space prefix of a module where the enum or identity is defined

    .. cpp:function:: void set_filter(const std::string & path, YFilter filter)

        Sets `yfilter` value in leaf

        :param path: YANG name of the leaf
        :param filter: :cpp:class:`filtering<ydk::YFilter>`, filter value

    .. cpp:function:: std::vector<std::pair<std::string, LeafData> > get_name_leaf_data() const

        Gets set of leaves, which have data or operation

    .. cpp:function:: std::map<std::string, std::shared_ptr<Entity>> get_children() const

        Gets map of child entities, where map key is a segment path of child entity

    .. cpp:function:: bool operator == (Entity & other) const
    .. cpp:function:: bool operator == (const Entity & other) const

        Operator to compare two entities

Utility functions
~~~~~~~~~~~~~~~~~

.. cpp:function:: std::string absolute_path(Entity & entity)

    Utility function to get absolute path of the entity.

    :param entity: An instance of :cpp:class:`Entity<ydk::Entity>`.
    :return: A `string` representing entity's absolute path.

.. cpp:function:: std::map<std::string,std::string> entity_to_dict(Entity & entity)

    Utility function to get map of all leaves and presence containers recursively in this entity and its children.

    :param entity: An instance of :cpp:class:`Entity<ydk::Entity>`.
    :return: A map, where key represents leaf absolute path and value represents string value of the leaf;
             In case of presence container the key represents the container's absolute path and value is empty string.

.. cpp:function:: std::map<std::string, std::pair<std::string,std::string>> entity_diff(Entity & ent1, Entity & ent2)

    Utility function to compare two entities of the same underlying type.
    Compared are presence containers and all leaves recursively.

    :param ent1: An instance of :cpp:class:`Entity<ydk::Entity>`.
    :param ent2: An instance of :cpp:class:`Entity<ydk::Entity>`.
    :return: A map of differences between two entities, where key of type `std::string` represents leaf or presence
             container absolute path and value of type `std::pair` represents difference in string values of the leaves.
    :raises: :cpp:class:`YInvalidArgumentError<ydk::YInvalidArgumentError>` exception, if supplied entities have different types.


Usage examples
==============

How to instantiate and use objects of Entity type
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++
  :linenos:

  // Instantiate a shared pointer object representing the BGP container from the openconfig-bgp YANG model
  auto bgp = std::make_shared<ydk::openconfig_bgp::Bgp>();

  // Instantiate a shared pointer object representing the afi-safi list member from the openconfig-bgp YANG model
  auto afi_safi = make_shared<ydk::openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
  
  // Append the afi-safi to the YList instance
  bgp->global->afi_safis->afi_safi.append(afi_safi);


How to assign values to leaves
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++
  :linenos:

  // Assign values to leafs of various types
  bgp->global->config->as = 65172; // uint32
  bgp->global->config->router_id = "1.2.3.4"; // ip-address
  
  afi_safi->afi_safi_name = L3VpnIpv4Unicast(); // identityref
  afi_safi->config->enabled = false; //boolean
  
  neighbor->config->peer_type = PeerType::INTERNAL // enum
  
  Decimal64 deci{"1.2"};
  node->decimal_value = deci; //decimal64
  
  node->empty_value = Empty(); // empty
  
  node->bits_value["first-position"] = true // bits
  node->bits_value["second-position"] = false // bits


How to append values to leaf-lists
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++
  :linenos:

  // Append values to leaf-lists of various types
  config->as_list.append(65172); // uint32
  config->router_id.append("1.2.3.4"); //ip-address
  L3VpnIpv4Unicast id{}; //identityref
  config->types_list.append(id); //identityref
  config->enabled_list.append(false); //boolean
  config->peer_types.append(PeerType::INTERNAL) // enum
  Decimal64 deci{"1.2"};
  node->decimal_values.append(deci); //decimal64

  Bits bits_value; // bits
  bits_value["first-position"] = true; // bits
  bits_value["first-position"] = false; // bits
  node->bits_values.append(bits_value); // bits
  

How to access YList entities
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c++
  :linenos:

  // Access by entity order
  for (auto ent : bgp->global->afi_safis->afi_safi.entities()) {
      auto afi_safi = dynamic_cast<ydk::openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi*> (ent.get());
      //Access afi_safi members
  }
  
  // Access by entity key value
  for (auto key : bgp->global->afi_safis->afi_safi.keys()) {
      auto ent = bgp->global->afi_safis->afi_safi[key];
      auto afi_safi = dynamic_cast<ydk::openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi*> (ent.get());
      //Access afi_safi members
  }
  