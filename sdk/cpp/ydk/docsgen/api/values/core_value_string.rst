SchemaValueStringType
=====================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueStringType : public SchemaValueType

String types.

    .. cpp:member:: SchemaConstraint length

        Length restriction.

    .. cpp:member:: std::vector<SchemaConstraint> patterns

        Pattern restrictions.

    .. cpp:function:: ~SchemaValueStringType()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const
