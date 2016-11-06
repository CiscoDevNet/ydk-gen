YDKPathException
================


.. cpp:namespace:: ydk::path

.. cpp:class:: YDKPathException : public YDKCoreException

Exception that encapsualtes the validation errors for YDK Path.

    .. cpp:member:: Error err

    .. cpp:function:: YDKPathException(YDKPathException::Error error_code)

    .. cpp:enum: Error

        .. cpp:enumerator:: SUCCESS

            No error.

        .. cpp:enumerator:: XPATH_INTOK

            Unexpected XPath token.

        .. cpp:enumerator:: XPATH_EOF

            Unexpected end of an XPath expression.

        .. cpp:enumerator:: XPATH_INOP

            Invalid XPath operation operands.

        .. cpp:enumerator:: XPATH_INCTX

            Invalid XPath context type.

        .. cpp:enumerator:: PATH_INCHAR

            Invalid characters (path).

        .. cpp:enumerator:: PATH_INMOD

            Invalid module name (path).

        .. cpp:enumerator:: PATH_MISSMOD

            Missing module name (path).

        .. cpp:enumerator:: PATH_INNODE

            Invalid node name (path).

        .. cpp:enumerator:: PATH_INKEY

            Invalid key name (path).

        .. cpp:enumerator:: PATH_MISSKEY

            Missing some list keys (path).

        .. cpp:enumerator:: PATH_EXISTS

            Target node already exists (path).

        .. cpp:enumerator:: PATH_MISSPAR

            Some parent of the target node is missing (path).

        .. cpp:enumerator:: PATH_AMBIGUOUS

            Thrown in create where the path expression cannot uniquely identify a given node.
