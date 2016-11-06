DiagnosticNode
==============


.. cpp:namespace:: ydk::path

.. cpp:class:: template <typename E, typename T>\
				DiagnosticNode

A :cpp:class:`DiagnosticNode` is a tree of Diagnostics information is associated with a source ``E`` a vector of errors of type ``T`` are available.

	.. cpp:member:: DiagnosticNode<E,T>* parent

	.. cpp:member:: E source

	.. cpp:member:: std::vector<T> errors

	.. cpp:member:: std::vector<DiagnosticNode<E,T>> children

	.. cpp:function:: bool has_errors()
