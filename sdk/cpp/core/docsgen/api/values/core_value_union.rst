SchemaValueUnionType
====================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueUnionType : public SchemaValueTyp

Union type.

    .. cpp:member:: std::vector<SchemaValueType*> types

        Types defined.

    .. cpp:function:: ~SchemaValueUnionType()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const
