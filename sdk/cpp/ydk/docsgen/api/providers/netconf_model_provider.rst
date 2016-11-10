NetconfModelProvider
======================

.. cpp:namespace:: ydk

.. cpp:class:: NetconfModelProvider

Implementation of :cpp:class:`ModelProvider<path::ModelProvider>` for netconf

    .. cpp:function:: NetconfModelProvider(NetconfClient & client) 

        Constructs an instance of the ``NetconfModelProvider``

        :param client: Instance of an existing netconf session

    .. cpp:function:: std::string get_model(const std::string& name, const std::string& version, Format format)

        Returns the model identified by the name and version

        :param name: name of the model
        :param version: version of the model
        :param format: :cpp:enum:`format<ydk::path::ModelProvider::Format>` of the model to download
        :return: ``std::string`` containing the model downloaded. If empty then the model probably cannot be provided

    .. cpp:function:: std::string get_hostname_port()

