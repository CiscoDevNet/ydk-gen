.. _ref-validationservice:

ValidationService
=================

.. cpp:namespace:: ydk


.. cpp:class:: ValidationService

Validation Service class for validating C++ model API objects of type :cpp:class:`Entity<Entity>`

    .. cpp:function:: void validate(const path::ServiceProvider & provider, Entity & entity, ValidationService::Option option)

        Validates an entity based on the option.

        :param provider: An instance of :cpp:class:`ServiceProvider<path::ServiceProvider>`
        :param entity: An instance of :cpp:class:`Entity<Entity>` class defined under a bundle
        :return: An instance of :cpp:class:`EntityDiagnostic<EntityDiagnostic>`
        :param option: An instance of type `Option<Option>`
        :raises YCPPError: If validation errors were detected

    .. cpp:enum:: Option

        All validation is performed in the context of some operation. These options capture the context of use.

        .. cpp:enumerator:: DATASTORE

            Datastore validation.

        .. cpp:enumerator:: GET_CONFIG

            Get config validation. Checks to see if only config nodes are references.

        .. cpp:enumerator:: GET

            Get validation.

        .. cpp:enumerator:: EDIT_CONFIG

            Edit validation. Checks on the values of leafs etc.

    .. cpp:function:: virtual ~ValidationService()
