.. _ref-types:

Types
=====

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


YANG container and list
~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:class:: ydk::Entity

    Super class of all classes that represents containers in YANG. YANG lists are represented as ``std::vector`` of Entity objects, with support for hanging a parent

    .. cpp:member:: YFilter filter

        Optional attribute of the Entity class which can be set to perform various :cpp:class:`filtering<YFilter>`

YANG leaf, list and leaf-list
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:class:: ydk::YLeaf

    Concrete class that represents a YANG leaf, to which data can be assigned.

    .. cpp:member:: YFilter filter

        Optional attribute of the YLeaf class which can be set to perform various :cpp:class:`filtering<YFilter>`

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
        
.. cpp:class:: YLeafList

    Concrete class that represents a YANG leaf-list, to which multiple instances of data can be appended to.

    .. cpp:member:: YFilter filter

        Optional attribute of the YLeafList class which can be set to perform various :cpp:class:`filtering<YFilter>`

YANG type
~~~~~~~~~~

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

Example usage
~~~~~~~~~~~~~~~

Examples of how to instantiate and use objects of :cpp:class:`Entity<Entity>` type:

.. code-block:: c++
  :linenos:

  // Instantiate a shared pointer object representing the BGP container from the openconfig-bgp YANG model
  auto bgp = std::make_shared<ydk::openconfig_bgp::Bgp>();

  // Instantiate a shared pointer object representing the afi-safi list member from the openconfig-bgp YANG model
  auto afi_safi = make_shared<ydk::openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
  afi_safi->parent = bgp->global->afi_safis.get();
  
  // Append the afi-safi to the YList instance
  bgp->global->afi_safis->afi_safi.append(afi_safi);

Examples of how to assign values to leafs:

.. code-block:: c++
  :linenos:

  // Assign values to leafs of various types
  bgp->global->config->as = 65172; // uint32
  bgp->global->config->router_id = "1.2.3.4"; //ip-address
  
  afi_safi->afi_safi_name = L3VpnIpv4Unicast(); //identityref
  afi_safi->config->enabled = false; //boolean
  
  neighbor->config->peer_type = PeerType::INTERNAL // enum
  
  Decimal64 deci{"1.2"};
  node->decimal_value = deci; //decimal64
  
  node->empty_value = Empty(); // empty
  
  node->bits_value["first-position"] = true // bits
  node->bits_value["second-position"] = false // bits

Examples of how to append values to leaf-lists:

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
  
Examples of how to access YList entities

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
  