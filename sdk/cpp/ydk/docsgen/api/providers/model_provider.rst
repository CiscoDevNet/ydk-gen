ModelProvider
======================

.. toctree::
   :maxdepth: 2

   netconf_model_provider.rst

.. cpp:namespace:: ydk::path

.. cpp:class:: ModelProvider

Interface for a YANG model provider which can download YANG models from a server

    .. cpp:enum:: Format

       Format of model to be downloaded

       .. cpp:enumerator:: YANG
       .. cpp:enumerator:: YIN

    .. cpp:function:: ModelProvider()

        Constructs an instance of the ``ModelProvider``

    .. cpp:function:: virtual std::string get_model(const std::string& name, const std::string& version, Format format)=0

        Returns the model identified by the name and version

        :param name: name of the model
        :param version: version of the model
        :param format: :cpp:enum:`format<Format>` of the model to download
        :return: ``std::string`` containing the model downloaded. If empty then the model probably cannot be provided

    .. cpp:function:: virtual std::string get_hostname_port()=0

       Return the hostname and port of the connected server

