.. _path-api-guide:

YDK Path API
============

.. module:: ydk.path

This module contains details about the YDK Python Path API.

YDK Python provides a new interface in the form of Path API, which can be used to write apps using a generic API, using xpath-like path expressions to create and access YANG data nodes. As a related note, the nodes created using the YDK model API are converted to Path API data nodes for validation and encoding to respective protocol payloads.


.. toctree::
    :maxdepth: 2

    annotation.rst
    capability.rst
    codec.rst
    data_node.rst
    repository.rst
    root_schema_node.rst
    rpc.rst
    schema_node.rst
    statement.rst
    netconf_session.rst
    restconf_session.rst
