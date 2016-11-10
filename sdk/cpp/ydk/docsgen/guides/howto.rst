Using the Path API
===================
YDK C++ provides a new interface in the form of Path API, which can be used to write apps using a generic API, using xpath-like path expression to create and access YANG data nodes. Internally, the nodes created using the YDK model API are converted to Path API data nodes for validation and encoding to respective protocol payloads.

.. toctree::
    :maxdepth: 2

    howto_codec.rst
    howto_data.rst
    howto_exceptions.rst
    howto_logger.rst
    howto_memory.rst
    howto_path.rst
    howto_rpc.rst
    howto_serviceprovider.rst
    howto_schemas.rst
    howto_validation.rst
