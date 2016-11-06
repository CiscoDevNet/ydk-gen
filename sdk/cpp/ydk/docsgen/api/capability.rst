Capability
==========


.. cpp:namespace:: ydk::path

.. cpp:class:: Capability

Class represents the Capability. An instance of Capability is defined by the module name and revision along with the set of enabled features defined in this modules as well as the list of deviations which target nodes defined by this module.

    .. cpp:member:: std::string module

        The module.

    .. cpp:member:: std::string revision

        The revision.

    .. cpp:member:: std::vector<std::string> features

        List of features defined in this module that are enabled.

    .. cpp:member:: std::vector<std::string> deviations

        List of deviations that target nodes defined by this module.

    .. cpp:function:: Capability(const std::string& mod, const std::string& rev)

    .. cpp:function:: Capability(const std::string& mod,\
                        const std::string& rev,\
                            const std::vector<std::string>& f,\
                                 const std::vector<std::string>& d)

    .. cpp:function:: Capability(const Capability& cap)

    .. cpp:function:: Capability(Capability&& cap)

    .. cpp:function:: Capability& operator=(const Capability& cap)

    .. cpp:function:: Capability& operator=(Capability&& cap)

    .. cpp:function:: bool operator==(const Capability& cap)
