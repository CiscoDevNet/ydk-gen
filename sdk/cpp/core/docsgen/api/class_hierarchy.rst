YDK API
============


.. toctree::
   :maxdepth: 3

   services/service.rst
   providers/provider.rst
   filters.rst
   types.rst
   exceptions/exception.rst

Path API
=========
YDK C++ provides a new interface in the form of Path API, which can be used to write apps using a generic API, using xpath-like path expression to create and access YANG data nodes. Internally, the nodes created using the YDK model API are converted to Path API data nodes for validation and encoding to respective protocol payloads.


.. toctree::
   :maxdepth: 3

   annotation.rst
   capability.rst
   repository.rst
   providers/model_provider.rst
   rpc.rst
   statement.rst

   nodes/core_node_data.rst
   nodes/core_node_schema.rst
