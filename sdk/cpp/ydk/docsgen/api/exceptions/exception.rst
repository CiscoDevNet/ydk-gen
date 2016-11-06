YDKException
============

.. toctree::
   :maxdepth: 2

   core_exception.rst
   excep_illegal_state.rst
   excep_invalid_argument.rst
   excep_operation_not_supported.rst
   excep_service_provider.rst

.. cpp:namespace:: ydk

.. cpp:class:: YDKException

Base class for YDK Exceptions.

    .. cpp:member:: std::string err_msg

    .. cpp:function:: YDKException(const std::string& msg)
