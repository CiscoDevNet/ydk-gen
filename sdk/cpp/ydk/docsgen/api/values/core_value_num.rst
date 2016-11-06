SchemaValueNumType
==================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueNumType : public SchemaValueType

Number types.

    .. cpp:member:: SchemaConstraint range

        Range constraint.

    .. cpp:function:: ~SchemaValueNumType()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const
