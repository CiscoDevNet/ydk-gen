YDKSchemaValidationException
============================


.. cpp:namespace:: ydk::path

.. cpp:class:: YDKSchemaValidationException : public YDKCoreException

Exception that encapsualtes the validation errors for schema validation.

    .. cpp:member: std::vector<std::pair<SchemaNode*, Error>> errors

    .. cpp:enum:: Error

        .. cpp:enumerator:: SUCCESS

            No error.

        .. cpp:enumerator:: INSTMT

            Invalid statement (schema).

        .. cpp:enumerator:: INID

            Nvalid identifier (schema).

        .. cpp:enumerator:: INDATE

            Invalid date format.

        .. cpp:enumerator:: INARG

            Invalid value of a statement argument (schema).

        .. cpp:enumerator:: MISSSTMT

            Missing required statement (schema).

        .. cpp:enumerator:: MISSARG

            Missing required statement argument (schema).

        .. cpp:enumerator:: TOOMANY

            Too many instances of some object.

        .. cpp:enumerator:: DUPID

            Duplicated identifier (schema).

        .. cpp:enumerator:: DUPLEAFLIST

            Multiple instances of leaf-list.

        .. cpp:enumerator:: DUPLIST

            Multiple instances of list.

        .. cpp:enumerator:: NOUNIQ

            Unique leaves match on 2 list instances (data).

        .. cpp:enumerator:: ENUM_DUPVAL

            Duplicated enum value (schema).

        .. cpp:enumerator:: ENUM_DUPNAME

            Duplicated enum name (schema).

        .. cpp:enumerator:: ENUM_WS

            Enum name with leading/trailing whitespaces (schema).

        .. cpp:enumerator:: BITS_DUPVAL

            Duplicated bits value (schema).

        .. cpp:enumerator:: BITS_DUPNAME

            Duplicated bits name (schema).

        .. cpp:enumerator:: INMOD

            Nvalid module name.

        .. cpp:enumerator:: KEY_NLEAF

            List key is not a leaf (schema).

        .. cpp:enumerator:: KEY_TYPE

            Invalid list key type (schema).

        .. cpp:enumerator:: KEY_CONFIG

            Key config value differs from the list config value.

        .. cpp:enumerator:: KEY_MISS

            List key not found (schema).

        .. cpp:enumerator:: KEY_DUP

            Duplicated key identifier (schema).

        .. cpp:enumerator:: INREGEX

            Nvalid regular expression (schema).

        .. cpp:enumerator:: INRESOLV

            No resolvents found (schema).

        .. cpp:enumerator:: INSTATUS

            Nvalid derivation because of status (schema).

        .. cpp:enumerator:: CIRC_LEAFREFS

            Circular chain of leafrefs detected (schema).

        .. cpp:enumerator:: CIRC_IMPORTS

            Circular chain of imports detected (schema).

        .. cpp:enumerator:: CIRC_INCLUDES

            Circular chain of includes detected (schema).
