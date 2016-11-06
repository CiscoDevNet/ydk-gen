SchemaValueIdentityType
=======================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaValueIdentityType : public SchemaValueType

Identity Schema value type.

        .. cpp:member:: std::string name

            Identity name.

        .. cpp:member:: std::string module_name

            Name of the module.

        .. cpp:member:: SchemaValueIdentityType* base

            Pointer to the base identity.

        .. cpp:member:: std::vector<SchemaValueIdentityType*> derived

            Derived identities.

        ..  cpp:function:: ~SchemaValueIdentityType()

        ..  cpp:function:: DiagnosticNode<std::string, ValidationError>\
                             validate(const std::string& value) const
