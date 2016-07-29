ydk.errors module
=================

.. py:currentmodule:: ydk.errors

errors

Contains types representing the Exception hierarchy in YDK.

.. py:exception:: ydk.errors.YPYDataValidationError

	Bases: :exc:`ydk.errors.YPYError`
	
	Exception for Client Side Data Validation.
	
	Type Validation.
	
	Any data validation error encountered that is related to type validation encountered does not
	raise an Exception right away.
	
	To uncover as many client side issues as possible, an i_errors list is injected in the parent entity of
	any entity with issues. The items added to this i_errors list captures the object types that caused
	the error as well as an error message.
	
.. py:exception:: ydk.errors.YPYError

	Bases: :exc:`exceptions.Exception`
	
	Base Exception for YDK Errors.