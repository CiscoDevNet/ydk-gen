.. _presence-class:

Presence Classes
==================
According to `RFC 6020 <https://tools.ietf.org/html/rfc6020#section-7.5.1>`_, YANG supports two styles of containers, one for organizing hierarchy, another for representing configuration data. For instance the existence of presence container ssh indicates the capability to log in to the device using ssh. Let's consider a container named conditions, with two sub container match-prefix-set(presence) and match-neighbor-set(non-presence), be generated as YDK class Conditions:

.. code-block:: python

    class Conditions(object):

        def __init__(self):
            self.match_prefix_set = None
            self.match_neighbor_set = Conditions.MatchNeighborSet()

Since the existence of sub container match-prefix-set its self representing configuration data, we should not assign an instance of class MatchPrefixSet to it when initializing, and the user need to manually assign it afterwards.
