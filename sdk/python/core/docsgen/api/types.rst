Types
=====

.. module:: ydk.types
    :synopsis: YDK Datatypes

.. contents:: Table of Contents

This module contains YDK Python types. It provides built-in types specified in
`YANG RFC 6020 <https://tools.ietf.org/html/rfc6020>`_ and types used in YDK Python APIs.


.. _types-yang:

YANG built-in types
-------------------

For `YANG Built-In types <https://tools.ietf.org/html/rfc6020#section-4.2.4>`_,
ydkgen generates Python classes for ``bits``, ``decimal64``, ``empty``,
``enumeration``, ``identityref`` and ``instance-identifier``. Other built-in
types, such as ``boolean`` and ``int8`` could be specified by :class:`~YLeaf`
and :class:`~YType`.

.. class:: Bits

    Represents a YANG built-in bits type.

    Instantiate a bit object::

        >>> from ydk.types import Bits
        >>> bits = Bits()

    .. method:: __setitem__(self, name, value):

        Called to implement assignment to ``self[name]``. Assign boolean value for ``name``::

            >>> bits['disable-nagle'] = False

    .. method:: __getitem__(self, name):

        Called to implement evaluation of ``self[name]``. Return boolean value for ``name``::

            >>> bits['disable-nagle']
            False

    .. method:: get_bitmap(self):

        Return a dictionary wrapper for an internal C++ ``std::map<std::string, bool>`` bitmap::

            >>> bits.get_bitmap()
            {'disable-nagle': False}

.. class:: Decimal64(value)

    Represents a YANG built-in decimal64 type.

    :param value: (``str``) String representation of value:

    Instantiate a decimal64 object::

        >>> from ydk.types import Decimal64
        >>> decimal = Decimal64('922337203685.4775807')

    .. attribute:: value

        A string representation for decimal value.

            >>> decimal.value
            '922337203685.4775807'

.. class:: Empty

    Represents a YANG built-in empty type.

    Instantiate an empty object::

        >>> from ydk.types import Empty
        >>> empty = Empty()

.. class:: Enum

    Represents a YANG built-in enum type, a base type for all YDK enums.
    The usage is the same as a Python enum::

        >>> from ydk.models.openconfig import openconfig_bgp_types
        >>> e = openconfig_bgp_types.BgpSessionDirection.INBOUND

.. class:: Identity

    Represents a YANG built-in identity type, a base type for all YDK identities::

        >>> from ydk.models.openconfig import openconfig_bgp_types
        >>> identity = openconfig_bgp_types.L3VpnIpv6Multicast()


.. _types-ydk:

YDK types
---------

.. class:: EncodingFormat

    Enum class for encoding format.

    .. py:data:: XML

        XML format.

    .. py:data:: JSON

        JSON format.

.. class:: Entity

    Super class of all classes that represent containers in YANG. YANG lists are represented as :py:class:`YList` of **Entity** objects, with support for hanging a parent.

    .. py:attribute:: operation

        Optional attribute of the **Entity** class, which can be set to perform various :py:class:`operations<ydk.filters.YFilter>`, see :ref:`netconf-operations`.

.. class:: EntityCollection

    Base class to represent collection of unique :py:class:`Entity<ydk.types.Entity>` instances. EntityCollection designed as a wrapper class around ordered dictionary collection of type ``OrderedDict``.
    It is created specifically to collect Entity class instances. Each Entity instance has unique segment path value, which is used as a key in the dictionary.
    If 'entity' is and instance of :py:class:`Entity<ydk.types.Entity>` class, the key could be retrieved ad 'entity.get_segment_path()' or simply 'entity.path()'.

    .. py:method:: __init__(*entities):

        Create **EntityCollection** instance:

        :param entities: If not present or ``None``, creates empty collection. Otherwise the instances of :py:class:`Entity<ydk.types.Entity>` class should be listed as parameters.
        :raises: Exception :py:exc:`YInvalidArgumentError<ydk.error.YInvalidArgumentError>`, if type of **entities** is different.

    .. py:method:: append(entities):

        Add entity or multiple entities to collection.

        :param entities: Instance of an :py:class:`Entity<ydk.types.Entity>` or Python ``list`` of :py:class:`Entity<ydk.types.Entity>` instances.
        :raises: Exception :py:exc:`YInvalidArgumentError<ydk.error.YInvalidArgumentError>`, if type of **entities** is different.

        Example usage for creating **EntityCollection**:

        .. code-block:: python

            >>> from ydk.models.ydktest import ydktest_sanity as ysanity
            >>> from ydk.types import EntityCollection
            >>> 
            >>> runner = ysanity.Runner()
            >>> native = ysanity.Native()
            >>> 
            >>> config = EntityCollection()
            >>> config.append(runner)
            >>> config.append(native)
            >>> # or simply
            >>> config = EntityCollection(runner, native)

    .. py:method:: __getitem__(item)

        Get single entity instance from collection.
        
        :param item: If **item** type is ``int``, the operator returns :py:class:`Entity<ydk.types.Entity>` instance by its sequence number in the collection.
        
                     If **item** type is ``str``, the operator returns :py:class:`Entity<ydk.types.Entity>` instance, which has matching key (entity.path()==item).
                     
                     If **item** type is ``Entity``, the operator returns :py:class:`Entity<ydk.types.Entity>` instance, which has matching key (entity.path()==item.path()).
        :return: Instance of :py:class:`Entity<ydk.types.Entity>` or ``None``, if matching instance is not in the collection.
        :raises: Exception :py:exc:`YInvalidArgumentError<ydk.error.YInvalidArgumentError>`, if type of **item** is other than ``int`` or ``str`` or ``Entity``.

        Examples for accessing **EntityCollection** members:

        .. code-block:: python

            >>> from ydk.models.ydktest import ydktest_sanity as ysanity
            >>> from ydk.types import EntityCollection
            >>> 
            >>> config = EntityCollection(ysanity.Runner(), ysanity.Native())
            >>> 
            >>> runner = config[0]
            >>> native = config['ydktest-sanity:native']
            >>> native = config[ysanity.Native()]

    .. py:method:: entities()

        Get collection of all entities as Python ``list`` container. If collection is empty the method returns empty list.

    .. py:method:: keys()

        Get list of keys for the collection entities. If collection is empty, the method returns empty list.

        Examples of accessing the entire **EntityCollection** content:

        .. code-block:: python

            >>> from ydk.models.ydktest import ydktest_sanity as ysanity
            >>> from ydk.types import EntityCollection
            >>> 
            >>> config = EntityCollection(ysanity.Runner(), ysanity.Native())
            >>> 
            >>> print(config.entities())
            ['ydk.models.ydktest.ydktest_sanity.Runner', 'ydk.models.ydktest.ydktest_sanity.Native']
            >>> print(config.keys())
            ['ydktest-sanity:runner', 'ydktest-sanity:native']

    .. py:method:: clear()

        Delete all collection members.

    .. py:method:: pop(item)

        Delete single entity instance from collection.

        :param item: If **item** type is ``int``, finds and deletes collection member by its sequence number.
        
                     If **item** type is ``str``, finds and deletes collection member, which has **Entity.get_segment_path()==item**.
                     
                     If **item** type is ``Entity``, finds and deletes collection member, which has matching key.
        :return: :py:class:`Entity<ydk.types.Entity>` instance of deleted member of collection, or ``None``, if matching instance is not found in collection.
        :raises: Exception :py:exc:`YInvalidArgumentError<ydk.error.YInvalidArgumentError>`, if type of **item** is other than ``int`` or ``str`` or ``Entity``.
        
        Examples of deleting items in the collection:

        .. code-block:: python

            >>> from ydk.models.ydktest import ydktest_sanity as ysanity
            >>> from ydk.types import EntityCollection
            >>> 
            >>> config = EntityCollection(ysanity.Runner(), ysanity.Native())
            >>> 
            >>> native = config.pop('ydktest-sanity:native')
            >>> # or
            >>> del config[ysanity.Runner()]
            
    The class also overwrites the following methods of ``OrderedDict`` class:
    
    .. py:method:: has_key(key)

    .. py:method:: get(item)

    .. py:method:: __eq__(other)
    
    .. py:method:: __ne__(other)
    
    .. py:method:: __len__()
    
    .. py:method:: __delitem__(item)

    .. py:method:: __iter__():

    .. py:method:: __str__():

.. class:: Config

    Alias of class :py:class:`EntityCollection<ydk.types.EntityCollection>`

.. class:: Filter

    Alias of class :py:class:`EntityCollection<ydk.types.EntityCollection>`

.. class:: YLeaf(leaf_type, name)

    Concrete class that represents a YANG ``leaf`` to which data can be assigned.

    Create a ``YLeaf`` instance.

    :param leaf_type: (:py:class:`YType`) YANG type for this ``leaf``.
    :param name: (``str``) YANG argument for this leaf.

    .. py:attribute:: operation

        Optional attribute of the ``Entity`` class which can be set to perform various :py:class:`operations<ydk.filters.YFilter>`, see :ref:`netconf-operations`.

    .. py:method:: set(self, value):

        Set value for current leaf.

        :param value: Value to be set.

    .. py:method:: get(self):

        Get leaf value.

    Example usage for creating a ``YLeaf`` of YANG type ``int8``:

    .. code-block:: python

        >>> from ydk.types import YLeaf, YType
        >>> yleaf = YLeaf(YType.int8, 'afi-safi-name')

.. class:: YLeafList(self, leaflist_type, name)

    Concrete class that represents a YANG ``leaf-list`` to which multiple instances of data can be appended to.

    :param leaflist_type: (:py:class:`YType`) YANG type for this ``leaf-list``.
    :param name: (``str``) YANG argument for this ``leaf-list``.

    .. py:method:: append(self, value):

        Append value to current ``leaf-list``.

.. class:: YList(parent)

    Concrete class that represents a YANG ``list``, with pointer to its `parent`. The class extends :py:class:`EntityCollection<ydk.types.EntityCollection>` and implements OrderedDict functionality.

    :param parent: :py:class:`Entity<ydk.types.Entity>` object, which hosts the YList.

    .. py:method:: append(self, item):

        Append single :py:class:`Entity<ydk.types.Entity>` object to current list.

        :param item: :py:class:`Entity<ydk.types.Entity>` object to be appended.

    .. py:method:: extend(self, items):

        Append multiple instances of :py:class:`Entity<ydk.types.Entity>` class to current list.

        :param items: List of :py:class:`Entity<ydk.types.Entity>` objects to be appended.

    .. py:method:: get(self, key) and __getitem__(self, key), which implements operator `[]`

        Access list elements by their key(s), if it is defined in the Yang model.

        :param key: Key value for single key or key list for multiple keys as defined in the Yang model of the list.
        :return: List element having matching key value - :py:class:`Entity<ydk.types.Entity>` object, or None if element is not found. If multiple elements have matching key, the function returns list of Entity objects.  

    .. py:method:: keys(self):
    
        :return: List of keys for all elements in the list.

.. class:: YType

    Enum class representing YANG types.

    .. py:data:: YType.bits

        bits type.

    .. py:data:: YType.boolean

        boolean type.

    .. py:data:: YType.decimal64

        decimal64 type.

    .. py:data:: YType.empty

        empty type.

    .. py:data:: YType.enumeration

        enumeration type.

    .. py:data:: YType.identityref

        identityref type.

    .. py:data:: YType.int16

        int16 type.

    .. py:data:: YType.int32

        int32 type.

    .. py:data:: YType.int64

        int64 type.

    .. py:data:: YType.int8

        int8 type.

    .. py:data:: YType.str

        string type.

    .. py:data:: YType.uint16

        uint16 type.

    .. py:data:: YType.uint32

        uint32 type.

    .. py:data:: YType.uint64

        uint64 type.

    .. py:data:: YType.uint8

        uint8 type.


Examples
--------

Examples of instantiating and using objects of Entity type are shown below(assuming you have ``openconfig`` bundle installed, see :ref:`howto-install`):

.. code-block:: python
    :linenos:

    from ydk.models.openconfig import openconfig_bgp as oc_bgp
    from ydk.models.openconfig import openconfig_bgp_types as oc_bgp_types
    from ydk.models.openconfig import openconfig_routing_policy as oc_routing_policy
    bgp = oc_bgp.Bgp()
    afi_safi = bgp.Global_.AfiSafis.AfiSafi()
    bgp.global_.afi_safis.afi_safi.append(afi_safi)

Examples of assigning values to leafs:

.. code-block:: python
    :linenos:
    :lineno-start: 7

    bgp.global_.config.as_ = 65172                                          # uint32
    bgp.global_.config.router_id = '1.2.3.4'                                # string
    afi_safi.afi_safi_name = oc_bgp_types.L3VpnIpv4Unicast()                # identityref
    afi_safi.config.enabled = True                                          # bool
    neighbor.config.peer_type = oc_bgp_types.PeerType.INTERNAL              # enum
    neighbor.timers.config.hold_time = Decimal64('90.00')                   # decimal64

    routing_policy = oc_routing_policy.RoutingPolicy()
    policy_definition = routing_policy.policy_definitions.PolicyDefinition()
    statement = policy_definition.statements.Statement()
    statement.actions.accept_route = Empty()                                # empty

    node.bits_type['first-option'] = True                                   # bits, node is a dummy container
    node.bits_type['second-option'] = False

Examples of appending values to leaf-lists:

.. code-block:: python
    :linenos:
    :lineno-start: 21

    config.as_list.append(65172)                                            # uint32, config is a dummy container
    config.router_id.append("1.2.3.4")                                      # ip-address, config is a dummy container
    id = oc_bgp_types.L3VpnIpv4Unicast                                      # identityref
    config.types_list.append(id)                                            # identityref, config is a dummy container
    config.enabled_list.append(false)                                       # bool, config is a dummy container
    config.peer_types.append(PeerTypeEnum::INTERNAL)                        # enum, config is a dummy container
    deci = Decimal64("1.2")
    node.decimal_values.append(deci)                                        # decimal64, node is a dummy container

    bits_value = Bits()                                                     # bits
    bits_value["first-position"] = True                                     # bits
    bits_value["first-position"] = False                                    # bits
    node.bits_values.append(bits_value)                                     # bits, node is a dummy container

.. _read-filter:

An example of setting the read filter for an :cpp:class:`leaf<YLeaf>` (specifically, the `as number` leaf) under :py:class:`openconfig BGP<ydk.openconfig_bgp.Bgp>` is shown below

.. code-block:: python
  :linenos:
  :lineno-start: 1

  from ydk.filters import YFilter

  # Instantiate a bgp object representing the bgp container from the openconfig-bgp YANG model
  bgp = ydk.models.openconfig_bgp.Bgp()

  # Indicate that the `as number` is desried to be read
  bgp.config.as_.operation = YFilter.read

  # Instantiate the CRUD service and Netconf provider to connect to a device with address 10.0.0.1
  CrudService crud_service{};
  NetconfServiceProvider provider{"10.0.0.1", "test", "test", 830};

  # Invoke the CRUD Read method
  crud_service.read(provider, bgp);


Validation
----------

YDK performs local validation of leafs based on the model type definition. A few examples of validation are given below (assuming you have ``openconfig`` bundle installed, see :ref:`howto-install`).  Assigning an invalid type results in a local validation error.

The :py:class:`openconfig BGP <ydk.models.openconfig.openconfig_bgp.Bgp.Global.Config>` model defines the field `as_` as a `int` (specifically, unsigned 32 bit integer).

.. code-block:: python
    :linenos:

    # Instantiate a bgp object representing the bgp container from the openconfig-bgp YANG model
    bgp = ydk.models.openconfig_bgp.Bgp()
    bgp.global_.config.as_ = "Hello" #invalid type

Assigning invalid type results in a :py:class:`YModelError <ydk.errors.YModelError>` being thrown.

.. code-block:: ssh

    YModelError: Invalid value Hello for 'as_'. Got type: 'str'. Expected types: 'int'


The :py:class:`openconfig BGP <ydk.models.openconfig.openconfig_bgp.Bgp.Global.Config>` model defines the field `router_id` as a `str` (specifically, with IP address pattern).

.. code-block:: python
    :linenos:

    # Instantiate a bgp object representing the bgp container from the openconfig-bgp YANG model
    bgp = ydk.models.openconfig_bgp.Bgp()
    bgp.global_.config.router_id = "Hello" #invalid value

Assigning invalid value results in a :py:class:`YModelError <ydk.errors.YModelError>` being thrown.

.. code-block:: ssh

    YModelError:  Value "Hello" does not satisfy the constraint "(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])" (range, length, or pattern). Path: /openconfig-bgp:bgp/global/config/router-id

