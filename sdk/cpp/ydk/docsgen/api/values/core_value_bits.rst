SchemaValueBitsType
===================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueBitsType  : public SchemaValueType

Single bit value specification.

    .. cpp:member:: std::vector<Bit> bits

        Bit definitions.

    .. cpp:function:: ~SchemaValueBitsType()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const

    .. cpp:class:: Bit

        .. cpp:function:: Bit(std::string m_name, uint32_t m_pos)

        .. cpp:member:: std::string name

            Bit name.

        .. cpp:member:: uint32_t pos

            Position.
