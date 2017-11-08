.. _netconf-operations:

How do I create, update, read and delete?
=========================================

.. contents:: Table of Contents

This document contains some examples of creating, reading and deleting yang data using YDK. To perform these operations, the :py:class:`CRUDService<ydk.services.CRUDService>` can be used. Also, in these examples, :py:class:`YFilter<ydk.filters.YFilter>` is used to mark parts of the data for particular operations.

Creating a configuration
-------------------------

First, let us create a configuration for the :py:class:`openconfig_bgp.Bgp.Global_.Config<ydk.models.openconfig.openconfig_bgp.Bgp.Global_.Config>` class. Here, we set the leaf ``as_``, which represents the autonomous system number, to ``65001`` and the leaf ``router_id`` to ``'10.0.0.1'``.

.. code-block:: python
    :linenos:

    from ydk.types import YFilter
    from ydk.models.openconfig import openconfig_bgp

    # First, create the top-level Bgp() object
    bgp = openconfig_bgp.Bgp()

    # Populate the values for the global config object
    bgp.global_.config.as_ = 65001
    bgp.global_.config.router_id = '10.0.0.1'

    # Call the CRUD create on the top-level bgp object
    # (assuming you have already instantiated the service and provider)
    rib_oper = crud.create(provider, bgp)

Replacing a configuration
-------------------------

Now, let us replace the above configuration with a new configuration for the :py:class:`openconfig_bgp.Bgp.Global_.Config<ydk.models.openconfig.openconfig_bgp.Bgp.Global_.Config>` class using the below code.

.. code-block:: python
    :linenos:

    from ydk.types import YFilter
    from ydk.models.openconfig import openconfig_bgp

    # First, create the top-level Bgp() object
    bgp = openconfig_bgp.Bgp()

    # Set the yfilter attribute of the config object to YFilter.replace
    bgp.global_.config.yfilter = YFilter.replace

    # Populate the new values for the global config object
    bgp.global_.config.as_ = 65023
    bgp.global_.config.router_id = '25.3.55.12'

    # Call the CRUD update on the top-level bgp object
    # (assuming you have already instantiated the service and provider)
    rib_oper = crud.update(provider, bgp)


Reading a list
--------------

For example, to read the instances of a deeply nested :py:class:`YList<ydk.types.YList>` called :py:class:`Cisco_IOS_XR_ip_rib_ipv4_oper.Rib.Vrfs.Vrf.Afs.Af.Safs.Saf.IpRibRouteTableNames.IpRibRouteTableName.Routes.Route<ydk.models.cisco_ios_xr.Cisco_IOS_XR_ip_rib_ipv4_oper.Rib.Vrfs.Vrf.Afs.Af.Safs.Saf.IpRibRouteTableNames.IpRibRouteTableName.Routes.Route>`  in the ``Cisco_IOS_XR_ip_rib_ipv4_oper`` module using YDK's :py:class:`CRUDService<ydk.services.CRUDService>`, the below approach can be used.

.. code-block:: python
    :linenos:

    from ydk.models.cisco_ios_xr import Cisco_IOS_XR_ip_rib_ipv4_oper
    from ydk.filters import YFilter

    # First create the top-level Rib() object
    rib = Cisco_IOS_XR_ip_rib_ipv4_oper.Rib()

    # Then create the list instance Vrf()
    vrf = Cisco_IOS_XR_ip_rib_ipv4_oper.Rib.Vrfs.Vrf()
    vrf.vrf_name='default'

    # Then create the child list element Af() and the rest of the nested list instances
    af = Cisco_IOS_XR_ip_rib_ipv4_oper.Rib.Vrfs.Vrf.Afs.Af()
    af.af_name = 'IPv4'

    saf = Cisco_IOS_XR_ip_rib_ipv4_oper.Rib.Vrfs.Vrf.Afs.Af.Safs.Saf()
    saf.saf_name='Unicast'

    table_name = Cisco_IOS_XR_ip_rib_ipv4_oper.Rib.Vrfs.Vrf.Afs.Af.Safs.Saf.IpRibRouteTableNames.IpRibRouteTableName()
    table_name.route_table_name = 'default'

    # Create the final list instance Route()
    route = Cisco_IOS_XR_ip_rib_ipv4_oper.Rib.Vrfs.Vrf.Afs.Af.Safs.Saf.IpRibRouteTableNames.IpRibRouteTableName.Routes.Route()
    route.yfilter = YFilter.read # set the yfilter attribute for route to YFilter.read

    # Append each of the list instances to their respective parents
    table_name.routes.route.append(route)
    saf.ip_rib_route_table_names.ip_rib_route_table_name.append(table_name)
    af.safs.saf.append(saf)
    vrf.afs.af.append(af)
    rib.vrfs.vrf.append(vrf)

    # Call the CRUD read on the top-level rib object
    # (assuming you have already instantiated the service and provider)
    rib_oper = crud.read(provider, rib)


Reading a leaf
--------------

For example, to read a :py:class:`YLeaf<ydk.types.YLeaf>` called ``running`` in the :py:class:`Cisco_IOS_XR_clns_isis_cfg.Isis.Instances.Instance<ydk.models.cisco_ios_xr.Cisco_IOS_XR_clns_isis_cfg.Isis.Instances.Instance>` class in the ``Cisco_IOS_XR_clns_isis_cfg`` module using YDK's :py:class:`CRUDService<ydk.services.CRUDService>`, the below approach can be used.

.. code-block:: python
    :linenos:


    from ydk.models.cisco_ios_xr import Cisco_IOS_XR_clns_isis_cfg
    from ydk.types import Empty
    from ydk.filters import YFilter

    # First create the top-level Isis() object
    isis = Cisco_IOS_XR_clns_isis_cfg.Isis()

    # Create the list instance
    ins = Cisco_IOS_XR_clns_isis_cfg.Isis.Instances.Instance()
    ins.instance_name = 'default'

    # Set the yfilter attribute of the leaf called 'running' to YFilter.read
    ins.running.yfilter = YFilter.read

    # Append the instance to the parent
    isis.instances.instance.append(ins)

    # Call the CRUD read on the top-level isis object
    # (assuming you have already instantiated the service and provider)
    result = crud.read(provider, isis)


Deleting a list
---------------

For example, to delete a :py:class:`YList<ydk.types.YList>` called :py:class:`Instance<ydk.models.cisco_ios_xr.Cisco_IOS_XR_clns_isis_cfg.Isis>` in the ``Cisco_IOS_XR_clns_isis_cfg`` module using YDK's :py:class:`CRUDService<ydk.services.CRUDService>`, the below approach can be used.

.. code-block:: python
    :linenos:


    from ydk.models.cisco_ios_xr import Cisco_IOS_XR_clns_isis_cfg
    from ydk.types import Empty
    from ydk.filters import YFilter

    # First create the top-level Isis() object
    isis = Cisco_IOS_XR_clns_isis_cfg.Isis()

    # Create the list instance
    ins = Cisco_IOS_XR_clns_isis_cfg.Isis.Instances.Instance()
    ins.instance_name = 'xyz'

    # Set the yfilter attribute of the leaf called 'running' to YFilter.delete
    ins.yfilter = YFilter.delete

    # Append the instance to the parent
    isis.instances.instance.append(ins)

    # Call the CRUD update on the top-level isis object
    # (assuming you have already instantiated the service and provider)
    result = crud.update(provider, isis)



Deleting a leaf
---------------

For example, to delete a :py:class:`YLeaf<ydk.types.YLeaf>` called ``running`` in the :py:class:`Instance<ydk.models.cisco_ios_xr.Cisco_IOS_XR_clns_isis_cfg.Isis>` class in the ``Cisco_IOS_XR_clns_isis_cfg`` module using YDK's :py:class:`CRUDService<ydk.services.CRUDService>`, the below approach can be used.

.. code-block:: python
    :linenos:


    from ydk.models.cisco_ios_xr import Cisco_IOS_XR_clns_isis_cfg
    from ydk.types import Empty
    from ydk.filters import YFilter

    # First create the top-level Isis() object
    isis = Cisco_IOS_XR_clns_isis_cfg.Isis()

    # Create the list instance
    ins = Cisco_IOS_XR_clns_isis_cfg.Isis.Instances.Instance()
    ins.instance_name = 'default'

    # Set the yfilter attribute of the leaf called 'running' to YFilter.delete
    ins.running = Empty()
    ins.running.yfilter = YFilter.delete

    # Append the instance to the parent
    isis.instances.instance.append(ins)

    # Call the CRUD update on the top-level isis object
    # (assuming you have already instantiated the service and provider)
    result = crud.update(provider, isis)
