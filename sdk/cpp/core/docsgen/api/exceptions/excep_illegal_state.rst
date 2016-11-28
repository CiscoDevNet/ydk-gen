YCPPIllegalStateError
========================

.. cpp:namespace:: ydk

.. cpp:class:: YCPPIllegalStateError : public YCPPError

Thrown when an operation/service is invoked on an object that is not in the right state. Use the ``msg`` for the error.

    .. cpp:function:: YCPPIllegalStateError(const std::string& msg)
