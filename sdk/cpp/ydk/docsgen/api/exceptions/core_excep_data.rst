YDKDataValidationException
==========================


.. cpp:namespace:: ydk::path

.. cpp:class:: YDKDataValidationException : public YDKCoreException

Exception that encapsualtes the validation errors on a data tree.

    .. cpp:member:: std::vector<std::pair<DataNode*,Error>> errors

        List of validation errors specific to this node.

    .. cpp:function:: YDKDataValidationException()

    .. cpp:enum:: Error

        Data Validation Error Enum.

        .. cpp:enumerator:: SUCCESS

            No error.

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
