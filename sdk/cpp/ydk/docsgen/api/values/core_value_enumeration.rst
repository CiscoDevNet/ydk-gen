SchemaValueEnumerationType
==========================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueEnumerationType : public SchemaValueType

Enumeration value specification.

    .. cpp:member:: std::vector<Enum> enums

        Enum literals.

    .. cpp:function:: ~SchemaValueEnumerationType()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const

    .. cpp:class:: Enum

        .. cpp:function:: Enum(std::string m_name, uint8_t m_flags, int32_t m_value)

        .. cpp:member:: std::string name

            Enum's name (mandatory).

        .. cpp:member:: int8_t flags

            Enum's flags , whether the value was auto-assigned.

        .. cpp:member:: int32_t value

            Enum's value (mandatory).
