ServiceProvider
===============


.. cpp:namespace:: ydk::path

A ServiceProvider extends the class :cpp:class:`ServiceProvider<ServiceProvider>` and provides an interface to obtain the root :cpp:class:`SchemaNode<SchemaNode>` tree based on the set of :cpp:class:`Capability<Capability>`\(s\) supported by it.

ServiceProvider Errors
----------------------
TODO

Capability
----------
A capability is a tuple that contains:

* module-name
* revision
* set of enabled features
* set of deviations active on this module

.. Note::

Use the :cpp:class:`Repository<Repository>` class to instantiate a :cpp:class:`SchemaNode<SchemaNode>` tree based on the :cpp:class:`Capability<Capability>`.
