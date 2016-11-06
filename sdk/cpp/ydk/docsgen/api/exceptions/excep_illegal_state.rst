YDKIllegalStateException
========================

.. cpp:namespace:: ydk

.. cpp:class:: YDKIllegalStateException : public YDKException

Thrown when an operation/service is invoked on an object that is not in the right state. Use the ``msg`` for the error.

    .. cpp:function:: YDKIllegalStateException(const std::string& msg)
