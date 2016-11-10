.. _ref-types:

Types
============

.. contents:: Table of Contents

The C++ types present in ydk namespace corresponding to YANG types. See below for example usage.

.. cpp:namespace:: ydk


YANG container and list
~~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:class:: Entity

Super class of all classes that represents containers in YANG. YANG lists are represented as ``std::vector`` of Entity objects, with support for hanging a parent

YANG leaf and leaf-list
~~~~~~~~~~~~~~~~~~~~~~~~

.. cpp:class:: Value

Concrete class that represents a YANG leaf to which data can be assigned.

.. cpp:class:: ValueList

Concrete class that represents a YANG leaf-list to which multiple instances of data can be appended to.

YANG type
~~~~~~~~~~

.. cpp:class:: Bits

Concrete class representing a bits data type.

.. cpp:class:: Decimal64 

Concrete class representing a decimal64 data type.

.. cpp:class:: Empty

Represents the empty type in YANG. The empty built-in type represents a leaf that does not have any value, it conveys information by its presence or absence.

.. cpp:class:: Enum

Super class of all classes that represents the enumeration type in YANG.

.. cpp:class:: Identity

Super class of all classes that represents the identity type in YANG. Instances of this type of classes are assigned as data to leafs of ``identityref`` type. 

Example usage
~~~~~~~~~~~~~~~

Examples of instantiating and using objects of :cpp:class:`Entity<Entity>` type are shown below

.. code-block:: c++
  :linenos:

  // Instantiate a bgp smart pointer object representing the bgp container from the openconfig-bgp YANG model
  auto bgp = std::make_unique<ydk::openconfig_bgp::Bgp>();
  
  // Instantiate an af-safi object representing the af-safi list from the openconfig-bgp YANG model
  auto afi_safi = make_unique<ydk::openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();
  // Set afi-safis as the parent of the list instance
  afi_safi->parent = bgp->global->afi_safis.get();
  //Append the list instance to afi-safis's afi-safi field
  bgp->global->afi_safis->afi_safi.push_back(std::move(afi_safi));

Examples of assigning values to leafs are shown below

.. code-block:: c++
  :linenos:

  // Assign values to leafs of various types
  
  bgp->global->config->as = 65172; // uint32
  bgp->global->config->router_id = "1.2.3.4"; //ip-address   
  afi_safi->afi_safi_name = L3VpnIpv4UnicastIdentity(); //identityref
  afi_safi->config->enabled = false; //boolean
  neighbor->config->peer_type = PeerTypeEnum::INTERNAL // enum
  Decimal64 deci{"1.2"};
  node->decimal_value = deci; //decimal64
  node->empty_value = Empty(); // empty
  node->bits_value["first-position"] = true // bits
  node->bits_value["second-position"] = false // bits
  
Examples of appending values to leaf-lists are shown below

.. code-block:: c++
  :linenos:

  // Append values to leaf-lists of various types

  config->as_list.append(65172); // uint32
  config->router_id.append("1.2.3.4"); //ip-address   
  L3VpnIpv4UnicastIdentity id{}; //identityref
  config->types_list.append(id); //identityref
  config->enabled_list.append(false); //boolean
  config->peer_types.append(PeerTypeEnum::INTERNAL) // enum
  Decimal64 deci{"1.2"};
  node->decimal_values.append(deci); //decimal64

  Bits bits_value; // bits
  bits_value["first-position"] = true; // bits
  bits_value["first-position"] = false; // bits
  node->bits_values.append(bits_value); // bits
