SchemaConstraint
================


.. cpp:namespace:: ydk::path

.. cpp:class:: SchemaConstraint

YANG validity restriction (must, length, etc) structure providing information from the schema.

    .. cpp:member:: std::string expr

        The restriction expression/value (mandatory).


    .. cpp:member:: std::string error_app_tag

        The error-app-tag.

    .. cpp:member:: std::string error_message

        Error message.
