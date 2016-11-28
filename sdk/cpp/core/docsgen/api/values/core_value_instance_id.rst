SchemaValueInstanceIdType
=========================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueInstanceIdType : public SchemaValueType

Instance identifier value type.

    .. cpp:member:: bool require_identifier = false

        Required.

    .. cpp:function:: ~SchemaValueInstanceIdType()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const
