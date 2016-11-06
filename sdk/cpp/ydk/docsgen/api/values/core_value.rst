SchemaValueType
===============

.. toctree::
   :maxdepth: 2

   core_value_binary.rst
   core_value_bits.rst
   core_value_bool.rst
   core_value_dec64.rst
   core_value_empty.rst
   core_value_enumeration.rst
   core_value_identity.rst
   core_value_instance_id.rst
   core_value_leafref.rst
   core_value_num.rst
   core_value_string.rst
   core_value_union.rst


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueType

The type of the ``leaf`` or ``leaf-list``.

    .. cpp:member:: std::string module_name

        Module name of the type referenced.

    .. cpp:member:: DataType type

        Data type.

    .. cpp:function:: virtual ~SchemaValueType()


    .. cpp:function:: virtual DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const

        Validate the value and return a :cpp:class:`DiagnosticNode<DiagnosticNode>`.

    .. cpp:enum:: DataType

        Enumeration representing the yang data types.

        .. cpp:enumerator:: UNKNOWN

            Unknown.

        .. cpp:enumerator:: BINARY

            Binary.

        .. cpp:enumerator:: BITS

            Bits.

        .. cpp:enumerator:: BOOL

            Boolean.

        .. cpp:enumerator:: DEC64

            Decimal64.

        .. cpp:enumerator:: EMPTY

            Empty.

        .. cpp:enumerator:: ENUMERATION

            Enumeration.

        .. cpp:enumerator:: IDENTITY

            Identity.

        .. cpp:enumerator:: LEAFREF

            Leafref.

        .. cpp:enumerator:: STRING

            String.

        .. cpp:enumerator:: INT8

            Int8.

        .. cpp:enumerator:: UINT8

            Uint8.

        .. cpp:enumerator:: INT16

            Int16.

        .. cpp:enumerator:: UINT16

            Uint16.

        .. cpp:enumerator:: INT32

            Int32.

        .. cpp:enumerator:: UINT32

            Uint32.

        .. cpp:enumerator:: INT64

            Int64.

        .. cpp:enumerator:: UINT64

            Uint64.

        .. cpp:enumerator:: UNION

            Union.
