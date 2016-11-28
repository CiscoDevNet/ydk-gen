SchemaValueBinaryType
=====================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueBinaryType : public SchemaValueType

Binary type.

    .. cpp:member:: SchemaConstraint length

    .. cpp:function:: ~SchemaValueBinaryType()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const
