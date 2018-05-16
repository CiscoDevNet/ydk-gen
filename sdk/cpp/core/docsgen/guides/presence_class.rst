.. _presence-class:

Presence Classes
==================
According to `RFC 6020 <https://tools.ietf.org/html/rfc6020#section-7.5.1>`_, YANG supports two styles of containers, one for organizing hierarchy, another for representing configuration data. The latter type of containers are called presence containers. For instance, the existence of a presence container ``ssh`` may be used to indicate whether ssh login is enabled or not.

Let us consider a class named ``Conditions``, with two members ``match_prefix_set`` (which is a presence node) and ``match_neighbor_set`` (which is a non-presence node).

.. code-block:: cpp

    Conditions::Conditions()
      :     match_prefix_set(nullptr),
            match_neighbor_set(std::make_shared<openconfig_bgp::Bgp::Conditions::MatchNeighborSet>())

When instantiating the ``Conditions`` class, the child ``match_prefix_set`` will be initially assigned to a null pointer. So, in order to configure the match prefix set, the user has to initialize the  ``match_prefix_set`` as shown below:


.. code-block:: cpp

    auto conditions = std::make_shared<openconfig_bgp::Bgp::Conditions>();
    conditions->match_prefix_set = std::make_shared<openconfig_bgp::Bgp::Conditions::MatchPrefixSet>(); // instantiate the presence node
    conditions->match_prefix_set = conditions.get(); //set the parent

