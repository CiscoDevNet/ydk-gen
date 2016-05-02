ydk.providers module
====================

.. py:module:: ydk.providers

providers.py

Service Providers module. Current implementation supports the NetconfServiceProvider which
uses ncclient (a Netconf client library) to provide CRUD services.


.. py:class:: ydk.providers.NetconfServiceProvider(**kwargs)

	Bases: :class:`ydk.providers.ServiceProvider`
	
	NCClient based Netconf Service Provider.
	
	Initialization parameter of NetconfServiceProvider
	
	:param address: The address of the netconf server
	:param port: The port to use default is 830
	:param username: The name of the user
	:param password: The password to use
	:param protocol: One of either ssh or tcp
	:timeout: Default to 30
	
	.. py:method:: close()

		Closes the netconf session.

	
.. py:class:: ydk.providers.ServiceProvider

	Bases: :class:`object` 
	
	Base class for Service Providers.
	
	.. py:method:: close()

		Base method to close service provider instance session.

	
	Exception for Client Side Data Validation.
	
	Type Validation.
	
	Any data validation error encountered that is related to type validation encountered does not
	raise an Exception right away.
	
	To uncover as many client side issues as possible, an i_errors list is injected in the parent entity of
	any entity with issues. The items added to this i_errors list captures the object types that caused
	the error as well as an error message.
	
