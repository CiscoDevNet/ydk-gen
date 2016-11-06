SchemaValueDec64Type
====================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueDec64Type : public SchemaValueType

Decimal64 type.

    .. cpp:member:: SchemaConstraint range

        Range restriction.

    .. cpp:member:: uint8_t fraction_digits

        Fraction digits.

    .. cpp:function:: ~SchemaValueDec64Type()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const
