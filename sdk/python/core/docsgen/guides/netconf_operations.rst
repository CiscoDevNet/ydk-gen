.. _netconf-operations:

NETCONF operations
==================

This document explains how to use :py:class:`YOperation<ydk.types.YOperation>` as defined under netconf
edit-config operation attribute in
`RFC 6241 <https://tools.ietf.org/html/rfc6241#section-7.2>`_. This guide
assumes you have ``core`` and ``openconfig`` bundle installed(see :ref:`howto-install`).

The first step in any application is to create a service provider instance. In this case, the NETCONF service provider (defined in :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>`) is responsible for mapping between the CRUD service API and the underlying manageability protocol (NETCONF RPCs).

Let's instantiate an instance of the service provider that creates a NETCONF session to the machine with address 127.0.0.1:

.. code-block:: python
    :linenos:

    from ydk.types import YOperation
    from ydk.services import CRUDService
    from ydk.providers import NetconfServiceProvider
    from ydk.models.openconfig import openconfig_bgp as oc_bgp
    from ydk.models.openconfig import openconfig_bgp_types as oc_bgp_types
    provider = NetconfServiceProvider(address='127.0.0.1',
                                      username='admin',
                                      password='admin',
                                      protocol='ssh',
                                      port=830)
    crud = CRUDService()

Then create a helper function to return YDK object with bgp configurations:

.. code-block:: python
    :linenos:
    :lineno-start: 12

    def config_bgp(bgp):
        """Add config data to bgp object."""
        # global configuration
        bgp.global_.config.as_ = 65001
        afi_safi = bgp.global_.afi_safis.AfiSafi()
        afi_safi.afi_safi_name = oc_bgp_types.Ipv4Unicast()
        afi_safi.config.afi_safi_name = oc_bgp_types.Ipv4Unicast()
        afi_safi.config.enabled = True
        bgp.global_.afi_safis.afi_safi.append(afi_safi)

        # configure IBGP peer group
        peer_group = bgp.peer_groups.PeerGroup()
        peer_group.peer_group_name = "IBGP"
        peer_group.config.peer_group_name = "IBGP"
        peer_group.config.peer_as = 65001
        peer_group.transport.config.local_address = "Loopback0"
        afi_safi = peer_group.afi_safis.AfiSafi()
        afi_safi.afi_safi_name = oc_bgp_types.Ipv4Unicast()
        afi_safi.config.afi_safi_name = oc_bgp_types.Ipv4Unicast()
        afi_safi.config.enabled = True
        peer_group.afi_safis.afi_safi.append(afi_safi)
        bgp.peer_groups.peer_group.append(peer_group)

        # configure IBGP neighbor
        neighbor = bgp.neighbors.Neighbor()
        neighbor.neighbor_address = "172.16.255.2"
        neighbor.config.neighbor_address = "172.16.255.2"
        neighbor.config.peer_group = "IBGP"
        bgp.neighbors.neighbor.append(neighbor)

.. note::

    The configuration above is truncated from one of `ydk-py sample apps <https://github.com/CiscoDevNet/ydk-py-samples/blob/5382b1dc4ae6998f34e702b37799d98cf4ede2c7/samples/basic/crud/models/openconfig/openconfig-bgp/nc-create-oc-bgp-40-ydk.py>`_,
    you can explore more than 500 apps at `ydk-py-samples <https://github.com/CiscoDevNet/ydk-py-samples>`_!

Let's use :py:meth:`CRUDService<ydk.services.CRUDService.create>` to create configuration:

.. code-block:: python
    :linenos:
    :lineno-start: 41

    bgp_cfg = oc_bgp.Bgp()
    config_bgp(bgp_cfg)
    crud.create(provider, bgp_cfg)

After configuration is created, let's use :py:attr:`YOperation.replace<ydk.types.YOperation.replace>` and :py:meth:`CRUDService<ydk.services.CRUDService.update>` to udpate configuration:

.. code-block:: python
    :linenos:
    :lineno-start: 44

    bgp_cfg.neighbors.neighbor[0].config.neighbor_address = "172.16.255.3"
    bgp_cfg.neighbors.neighbor[0].neighbor_address = "172.16.255.3"
    bgp_cfg.neighbors.neighbor[0].operation = YOperation.replace
    crud.update(provider, bgp_cfg)


With logging enabled(see :ref:`howto-logging`), we can see the CRUD update payload sent and to the device:

.. TODO, YPYInvalidArgumentError:  Path is invalid: openconfig-bgp:bgp

.. code-block:: xml

    Executing CRUD update operation
    =============Generating payload to send to device=============
    <rpc xmlns="urn:ietf:params:xml:ns:netconf:base:1.0"><edit-config xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
      <target>
        <candidate/>
      </target>
      <error-option>rollback-on-error</error-option>
      <config><bgp xmlns="http://openconfig.net/yang/bgp" xmlns:nc="urn:ietf:params:xml:ns:netconf:base:1.0" nc:operation="merge">
      <global>
        <afi-safis>
          <afi-safi>
            <afi-safi-name xmlns:bgp-types="http://openconfig.net/yang/bgp-types">bgp-types:ipv4-unicast</afi-safi-name>
            <config>
              <afi-safi-name xmlns:bgp-types="http://openconfig.net/yang/bgp-types">bgp-types:ipv4-unicast</afi-safi-name>
              <enabled>true</enabled>
            </config>
          </afi-safi>
        </afi-safis>
        <config>
          <as>65001</as>
        </config>
      </global>
      <neighbors>
        <neighbor nc:operation="replace">
          <neighbor-address>172.16.255.3</neighbor-address>
          <config>
            <neighbor-address>172.16.255.3</neighbor-address>
            <peer-group>IBGP</peer-group>
          </config>
        </neighbor>
      </neighbors>
      <peer-groups>
        <peer-group>
          <peer-group-name>IBGP</peer-group-name>
          <afi-safis>
            <afi-safi>
              <afi-safi-name xmlns:bgp-types="http://openconfig.net/yang/bgp-types">bgp-types:ipv4-unicast</afi-safi-name>
              <config>
                <afi-safi-name xmlns:bgp-types="http://openconfig.net/yang/bgp-types">bgp-types:ipv4-unicast</afi-safi-name>
                <enabled>true</enabled>
              </config>
            </afi-safi>
          </afi-safis>
          <config>
            <peer-as>65001</peer-as>
            <peer-group-name>IBGP</peer-group-name>
          </config>
          <transport>
            <config>
              <local-address>Loopback0</local-address>
            </config>
          </transport>
        </peer-group>
      </peer-groups>
    </bgp>
    </config>
    </edit-config>
    </rpc>
