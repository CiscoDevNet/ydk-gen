Statement
=========


.. cpp:namespace:: ydk::path

.. cpp:class:: Statement

Represents the YANG :cpp:class:`Statement<Statement>`.

    .. cpp:member:: std::string keyword

        YANG keyword corresponding to the :cpp:class:`Statement<Statement>`.

    .. cpp:member:: std::string  arg

        The arg if any.

    .. cpp:function:: Statement(): keyword{}, arg{}

    .. cpp:function:: Statement(const std::string& mkeyword,\
                                const std::string& marg)

    .. cpp:function:: Statement(const Statement& stmt)

    .. cpp:function:: Statement(Statement&& stmt)

    .. cpp:function:: ~Statement()

    .. cpp:function:: Statement& operator=(const Statement& stmt)

    .. cpp:function:: Statement& operator=(Statement&& stmt)
