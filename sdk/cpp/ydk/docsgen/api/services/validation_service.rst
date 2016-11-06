.. _ref-validationservice:

ValidationService
=================

.. cpp:namespace:: ydk

.. cpp:class:: EntityDiagnostic : public path::DiagnosticNode<Entity*, path::ValidationError>

Contains validation error details, if any

    .. cpp:function:: bool has_errors()

        Indicates if any errors have occurred

        :return: ``true`` if errors have occurred, else, returns ``false``

    .. cpp:member:: std::vector<DiagnosticNode<std::string, path::ValidationError>> attrs

        ``std::vector`` of errors, indicating error paths and error types 

.. cpp:class:: ValidationService

Validation Service class for validating C++ model API objects of type :cpp:class:`Entity<Entity>`

    .. cpp:function:: EntityDiagnostic validate(const path::ServiceProvider & provider, Entity & entity, ValidationService::Option option)

        Validates an entity based on the option.

        :param provider: An instance of :cpp:class:`ServiceProvider<path::ServiceProvider>`
        :param entity: An instance of :cpp:class:`Entity<Entity>` class defined under a bundle
        :return: An instance of :cpp:class:`EntityDiagnostic<EntityDiagnostic>`
        :param option: An instance of type `Option<Option>`
        :raises YDKException: If validation errors were detected

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

.. _ref-validationerror:

.. cpp:namespace:: ydk::path
.. cpp:enum-class:: ValidationError

Data validation error enum.

        .. cpp:enumerator:: SUCCESS

            No error.

        .. cpp:enumerator:: SCHEMA_NOT_FOUND

            Entity's schema node is not found.

        .. cpp:enumerator:: INVALID_USE_OF_SCHEMA

            If element cannot have children as per schema (``leaf``, ``leaf-list``, ``anyxml``).

        .. cpp:enumerator:: TOOMANY

            Too many instances of some object.

        .. cpp:enumerator:: DUPLEAFLIST

            Multiple instances of leaf-list.

        .. cpp:enumerator:: DUPLIST

            Multiple instances of list.

        .. cpp:enumerator:: NOUNIQ

            Unique leaves match on 2 list instances (data).

        .. cpp:enumerator:: OBSDATA

            Obsolete data instantiation (data).

        .. cpp:enumerator:: NORESOLV

            No resolvents found for an expression (data).

        .. cpp:enumerator:: INELEM

            Nvalid element (data).

        .. cpp:enumerator:: MISSELEM

            Missing required element (data).

        .. cpp:enumerator:: INVAL

            Invalid value of an element (data).

        .. cpp:enumerator:: INVALATTR

            Invalid attribute value (data).

        .. cpp:enumerator:: INATTR

            Invalid attribute in an element (data).

        .. cpp:enumerator:: MISSATTR

            Missing attribute in an element (data).

        .. cpp:enumerator:: NOCONSTR

            Value out of range/length/pattern (data).

        .. cpp:enumerator:: INCHAR

            Unexpected characters (data).

        .. cpp:enumerator:: INPRED

            Predicate resolution fail (data).

        .. cpp:enumerator:: MCASEDATA

            Data for more cases of a choice (data).

        .. cpp:enumerator:: NOMUST

            Unsatisfied must condition (data).

        .. cpp:enumerator:: NOWHEN

            Unsatisfied when condition (data).

        .. cpp:enumerator:: INORDER

            Invalid order of elements (data).

        .. cpp:enumerator:: INWHEN

            Irresolvable when condition (data).

        .. cpp:enumerator:: NOMIN

            Min-elements constraint not honored (data).

        .. cpp:enumerator:: NOMAX

            Max-elements constraint not honored (data).

        .. cpp:enumerator:: NOREQINS

            Required instance does not exits (data).

        .. cpp:enumerator:: NOLEAFREF

            Leaf pointed to by leafref does not exist (data).

        .. cpp:enumerator:: NOMANDCHOICE

            No mandatory choice case branch exists (data).

        .. cpp:enumerator:: INVALID_BOOL_VAL

            Invalid boolean value.

        .. cpp:enumerator:: INVALID_EMPTY_VAL

            Invalid empty value.

        .. cpp:enumerator:: INVALID_PATTERN

            Pattern did not match.

        .. cpp:enumerator:: INVALID_LENGTH

            Length is invalid.

        .. cpp:enumerator:: INVALID_IDENTITY

            Invalid identity.

        .. cpp:enumerator:: INVALID_ENUM

            Invalid enumeration.
