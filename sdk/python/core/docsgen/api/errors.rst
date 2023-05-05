Errors
======

.. module:: ydk.errors
    :synopsis: YDK Exceptions

This module contains YDK Python errors classes. These errors are thrown in case of data not conforming to the yang model or due to a server-side error.

.. py:exception:: YError

    Bases: :exc:`exceptions.Exception`

    Base class for Y Errors. The subclasses give a specialized view of the error that has occurred.

.. py:exception:: YModelError

    Bases: :exc:`ydk.errors.YError`

    Model Error. Thrown when a model constraint is violated.

.. py:exception:: YServiceProviderError

    Bases: :exc:`ydk.errors.YError`

    Exception for Service Provider. Thrown in case of a server-side error.

.. py:exception:: YClientError

    Bases: :exc:`ydk.errors.YError`

    Exception for client connection

.. py:exception:: YIllegalStateError

    Bases: :exc:`ydk.errors.YError`

    Illegal State Error. Thrown when an operation/service is invoked on an object that is not in the right state. Use the error_msg for the error.

.. py:exception:: YInvalidArgumentError

    Bases: :exc:`ydk.errors.YError`

    Invalid Argument. Use the error_msg for the error.

.. py:exception:: YOperationNotSupportedError

    Bases: :exc:`ydk.errors.YError`

    Operation Not Supported Error. Thrown when an operation is not supported.

.. py:exception:: YServiceError

    Bases: :exc:`ydk.errors.YError`

    Exception for Service Side Validation


