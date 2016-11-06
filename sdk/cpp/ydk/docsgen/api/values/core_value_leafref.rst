SchemaValueLeafrefType
======================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueLeafrefType : public SchemaValueType

Leafref type.

    .. cpp:member:: std::string path

        Path.

    .. cpp:member:: SchemaNode* target

        Target node.

    .. cpp:function:: ~SchemaValueLeafrefType()

    .. cpp:function:: DiagnosticNode<std::string, ValidationError>\
                         validate(const std::string& value) const
