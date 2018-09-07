.. _netconf-operations:

How do I create, update, read and delete?
=========================================

.. contents:: Table of Contents

This document contains some examples of creating, reading and deleting yang data using YDK. To perform these operations, the :go:struct:`CrudService<ydk/services/CrudService>` is used. Also, in these examples, :ref:`YFilter <y-filter>` is used to mark parts of the data for particular operations.

Creating a configuration with a list
------------------------------------

To configure a rule in the SNMP trap correlator, the below approach can be used.

Note that the ``Rule`` field in :go:struct:`Snmp_Correlator_Rules <ydk/models/cisco_ios_xr/snmp_agent_cfg/Snmp_Correlator_Rules>` is a Go ``slice`` of :go:struct:`Snmp_Correlator_Rules_Rule<ydk/models/cisco_ios_xr/snmp_agent_cfg/Snmp_Correlator_Rules_Rule>`, which needs to be instantiated in its parent's constructor.

Also, the field ``NonStateful`` is set to ``nil`` by default. Therefore it needs to be instantiated.

.. code-block:: c
    :linenos:

    package main

    from "github.com/CiscoDevNet/ydk-go/ydk/models/cisco_ios_xr/snmp_agent_cfg"

    func main() {
        // Create the top-level container and instantiate slice
        snmp := snmp_agent_cfg.Snmp_Correlator_Rules{}

        // Create the Rule
        rule := snmp_agent_cfg.Snmp_Correlator_Rules_Rule{}
        rule.Name = "PASS-ALL"

        // Instantiate and assign NonStateful
        rule.NonStateful = snmp_agent_cfg.Snmp_Correlator_Rules_Rule_NonStateful{}
        rule.NonStateful.Timeout = 3s

        // Append
        snmp.Rule = append(snmp.Rule, &rule)

        // Call the CRUD create on the top-level object
        // (assuming you have already instantiated the service and provider)
        result := crud.Create(&provider, &snmp)
    }

Creating and replacing a configuration
--------------------------------------

First, let us create a configuration for the :go:struct:`Bgp_Global_Config<ydk/models/openconfig/openconfig_bgp/Bgp_Global_Config>` class. Here, we set the leaf ``As``, which represents the autonomous system number, to ``65001`` and the leaf ``RouterId`` to ``"10.0.0.1"``.

.. code-block:: c
    :linenos:

    package main

    import (
        "github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
        "github.com/CiscoDevNet/ydk-go/ydk/models/openconfig/openconfig_bgp"
    )

    func main() {
        // First, create the top-level Bgp() objects
        bgp := openconfig_bgp.Bgp{}
        bgp.Global = openconfig_bgp.Bgp_Global{}
        bgp.Global.Config = openconfig_bgp.Bgp_Global_Config{}

        // Populate the values for the global config object
        bgp.Global.Config.As = 65001
        bgp.Global.Config.RouterId = "10.0.0.1"

        // Call the CRUD create on the top-level bgp object
        // (assuming you have already instantiated the service and provider)
        result := crud.Create(&provider, &bgp)
    }


Now, let us replace the above configuration with a new configuration for the :go:struct:`openconfig_bgp.Bgp_Global_Config<ydk/models/openconfig/openconfig_bgp/Bgp_Global_Config>` class using the below code.

.. code-block:: c
    :linenos:

    package main

    import (
        "github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
        "github.com/CiscoDevNet/ydk-go/ydk/models/openconfig/openconfig_bgp"
    )

    func main() {
        // First, create the top-level Bgp() objects
        bgp = openconfig_bgp.Bgp{}
        bgp.Global = openconfig_bgp.Bgp_Global{}
        bgp.Global.Config = openconfig_bgp.Bgp_Global_Config{}

        // Set the yfilter attribute of the config object to YFilter.Replace
        bgp.Global.Config.YFilter = yfilter.Replace

        // Populate the new values for the global config object
        bgp.Global.Config.As = 65023
        bgp.Global.Config.RouterId = "25.3.55.12"

        // Call the CRUD update on the top-level bgp object
        // (assuming you have already instantiated the service and provider)
        result = crud.Update(&provider, &bgp)
    }


Reading a list
--------------

For example, to read the instances of a deeply nested ``slice`` called :go:struct:`Rib_Vrfs_Vrf_Afs_Af_Safs_Saf_IpRibRouteTableNames_IpRibRouteTableName_Routes_Route<ydk/models/cisco_ios_xr/ip_rib_ipv4_oper/Rib_Vrfs_Vrf_Afs_Af_Safs_Saf_IpRibRouteTableNames_IpRibRouteTableName_Routes_Route>`  in the ``ip_rib_ipv4_oper`` package using YDK's :go:struct:`CrudService<ydk/services/CrudService>`, the below approach can be used.

.. code-block:: c
    :linenos:

    package main

    import (
        "github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
        "github.com/CiscoDevNet/ydk-go/ydk/models/cisco_ios_xr/ip_rib_ipv4_oper"
    )

    func main() {
        // First create the top-level Rib objects
        rib := ip_rib_ipv4_oper.Rib{}

        // Then create the list instance Vrf
        vrf := ip_rib_ipv4_oper.Rib_Vrfs_Vrf{}
        vrf.VrfName = "default"

        // Then create the child list element Af and the rest of the nested list instances
        af := ip_rib_ipv4_oper.Rib_Vrfs_Vrf_Afs_Af{}
        af.AfName = "IPv4"

        saf := ip_rib_ipv4_oper.Rib_Vrfs_Vrf_Afs_Af_Safs_Saf{}
        saf.SafName = "Unicast"

        tableName := ip_rib_ipv4_oper.Rib_Vrfs_Vrf_Afs_Af_Safs_Saf_IpRibRouteTableNames.IpRibRouteTableName{}
        tableName.RouteTableName = "default"

        // Create the final list instance Route
        route := ip_rib_ipv4_oper.Rib_Vrfs_Vrf_Afs_Af_Safs_Saf_IpRibRouteTableNames_IpRibRouteTableName_Routes_Route{}
        route.YFilter = yfilter.Read // set the yfilter attribute for route to yfilter.Read

        // Append each of the list instances to their respective parents
        tableName.Routes = ip_rib_ipv4_oper.Rib_Vrfs_Vrf_Afs_Af_Safs_Saf_IpRibRouteTableNames_IpRibRouteTableName_Routes{}
        tableName.Routes.Route = append(table_name.Routes.Route, &route)
        
        saf.IpRibRouteTableNames = ip_rib_ipv4_oper.Rib_Vrfs_Vrf_Afs_Af_Safs_Saf_IpRibRouteTableNames{}
        saf.IpRibRouteTableNames.IpRibRouteTableName = append(saf.IpRibRouteTableNames.IpRibRouteTableName, &tableName)
        
        af.Safs = ip_rib_ipv4_oper.Rib_Vrfs_Vrf_Afs_Af_Safs{}
        af.Safs.Saf = append(af.Safs.Saf, &saf)
        
        vrf.Afs = ip_rib_ipv4_oper.Rib_Vrfs_Vrf_Afs{}
        vrf.Afs.Af = append(vrf.Afs.Af, &af)
        
        rib.Vrfs = ip_rib_ipv4_oper.Rib_Vrf{}
        rib.Vrfs.Vrf = append(rib.Vrfs.Vrf, &vrf)

        // Call the CRUD read on the top-level rib object
        // (assuming you have already instantiated the service and provider)
        ribOper := crud.Read(&provider, &rib)
    }


Accessing list elements
-----------------------

Lets continue previous example to demonstrate, how user can access `rib.Vrfs.Vrf` directly and by key identifier, which is `vrf.VrfName`.

.. code-block:: c
    :linenos:

        // Directly iterate over the slice
        for _, eVrf := range rib.Vrfs.Vrf {
              eVrf := iVrf.(*ip_rib_ipv4_oper.Rib_Vrfs_Vrf)
              fmt.Printf("Key: %v, VRF name: %v\n", key, eVrf.VrfName)
        }
        
        // Access specific VRF configuration directly when VRF name is known
        iVrf := ylist.Get(rib.Vrfs.Vrf, "default")
        if iVrf != nil {
                eVrf := iVrf.(*ip_rib_ipv4_oper.Rib_Vrfs_Vrf)
                fmt.Printf("VRF name: %v\n", eVrf.VrfName)
        }

        // Get all VRF names present in BGP configuration
        allVrfNames := ylist.Keys(rib.Vrfs.Vrf)
        
        // Iterate over the VRF names
        for _, name := range allVrfNames {
                _, iVrf := ylist.Get(rib.Vrfs.Vrf, name)
                if iVrf != nil {
                        eVrf := iVrf.(*ip_rib_ipv4_oper.Rib_Vrfs_Vrf)
                        fmt.Printf("VRF name: %v\n", eVrf.VrfName)
                }
        }
        
        // Remove specific VRF from the configuration
        i, rVrf = ylist.Get(rib.Vrfs.Vrf, "vrf-to-remove")
        if rVrf != nil {
                rib.Vrfs.Vrf = append(rib.Vrfs.Vrf[:i], rib.Vrfs.Vrf[i+1:] ...)
                crud.Update(&provider, &rib)
        }

Reading a leaf
--------------

For example, to read a ``YLeaf`` called ``Running`` in the :go:struct:`Instance <ydk/models/cisco_ios_xr/clns_isis_cfg/Isis_Instances_Instance>` class in the ``clns_isis_cfg`` module using YDK's :go:struct:`CrudService <ydk/services/CrudService>`, the below approach can be used.

.. code-block:: c
    :linenos:

    package main

    import (
        "github.com/CiscoDevNet/ydk-go/ydk/types"
        "github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
        "github.com/CiscoDevNet/ydk-go/ydk/models/cisco_ios_xr/clns_isis_cfg"
    )

    func main() {
        // First create the top-level Isis object
        isis = clns_isis_cfg.Isis{}

        // Create ISIS instance
        ins := clns_isis_cfg.Isis.Instances.Instance{}
        ins.InstanceName = "default"

        // Set the leaf called 'running'
        ins.Running = types.Empty{}

        // Create the list and append the instance
        isis.Instances = clns_isis_cfg.Isis.Instances{}
        isis.Instances.Instance = append(isis.Instances.Instance, &ins)

        // Call the CRUD read on the top-level isis object
        // (assuming you have already instantiated the service and provider)
        result := crud.Read(&provider, &isis)
    }

Deleting a list
---------------

For example, to delete a Go ``slice`` called :go:struct:`Instance <ydk/models/cisco_ios_xr/clns_isis_cfg/Isis_Instances_Instance>` in the ``clns_isis_cfg`` module using YDK's :go:struct:`CrudService<ydk/services/CrudService>`, the below approach can be used.

.. code-block:: c
    :linenos:

    package main

    import (
        "github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
        "github.com/CiscoDevNet/ydk-go/ydk/models/cisco_ios_xr/clns_isis_cfg"
    )

    func main() {
        // First create the top-level Isis object
        isis = clns_isis_cfg.Isis{}

        // Create the ISIS instance
        ins := clns_isis_cfg.Isis.Instances.Instance{}
        ins.InstanceName = "xyz"

        // Set the YFilter attribute of the leaf called 'ins' to yfilter.Delete
        ins.YFilter = yfilter.Delete

        // Create the list and append the instance
        isis.Instances = clns_isis_cfg.Isis.Instances{}
        isis.Instances.Instance = append(isis.Instances.Instance, &ins)

        // Call the CRUD read on the top-level isis object
        // (assuming you have already instantiated the service and provider)
        result := crud.Read(&provider, &isis)
    }


Deleting a leaf
---------------

For example, to delete a ``YLeaf`` called ``Running`` in the :go:struct:`Instance <ydk/models/cisco_ios_xr/clns_isis_cfg/Isis_Instances>` class in the ``clns_isis_cfg`` module using YDK's :go:struct:`CrudService<ydk/services/CrudService>`, the below approach can be used.

.. code-block:: c
    :linenos:

    package main

    import (
        "github.com/CiscoDevNet/ydk-go/ydk/types"
        "github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
        "github.com/CiscoDevNet/ydk-go/ydk/models/cisco_ios_xr/clns_isis_cfg"
    )

    func main() {
        // First create the top-level Isis object
        isis = clns_isis_cfg.Isis{}

        // Create the ISIS instance
        ins := clns_isis_cfg.Isis.Instances.Instance{}
        ins.InstanceName = "default"

        // Not setting the Running leaf
        // Create list and Append the instance
        isis.Instances = clns_isis_cfg.Isis.Instances{}
        isis.Instances.Instance = append(isis.Instances.Instance, &ins)

        // Call the CRUD read on the top-level isis object
        // (assuming you have already instantiated the service and provider)
        result := crud.Read(&provider, &isis)
    }

Reading multiple configurations
--------------------------------------

In this example we read interfaces and BGP configuration as defined by openconfig Yang model.

.. code-block:: c
    :linenos:

    package main

    import (
        "github.com/CiscoDevNet/ydk-go/ydk"
        "github.com/CiscoDevNet/ydk-go/ydk/types"
        ocBgp "github.com/CiscoDevNet/ydk-go/ydk/models/openconfig/openconfig_bgp"
        ocInterfaces "github.com/CiscoDevNet/ydk-go/ydk/models/openconfig/openconfig_interfaces"
    )

    func main() {
        // Build filter
        interfacesFilter := ocInterfaces.Interfaces{};
        bgpFilter := ocBgp.Bgp{};
        filterList := types.NewFilter(&interfacesFilter, &bgpFilter)
        
        // Read running config
        getConfigEntity := crud.Read(&provider, filterList)
        
        // Get results
        getConfigEC := types.EntityToCollection(getConfigEntity)
        for _, entity := range getConfigEC.Entities() {
            ydk.YLogDebug(fmt.Sprintf("Printing %s", GetEntityXmlString(entity)))
        }
    }

Reading entire device configuration
--------------------------------------

When filters are not specified, the YDK attempts to get configuration data based on IETF Yang model. It is user responsibility to import corresponding entities to the application. If retrieved entity was not included in the import statement, an error message is developed and logged (the logger must be enabled); example:
        
    `[ydk] [error] [Go] Entity 'ietf-netconf-acm:nacm' is not registered. Please import corresponding package to your application.`


.. code-block:: c
    :linenos:

    package main

    import (
        "github.com/CiscoDevNet/ydk-go/ydk"
        "github.com/CiscoDevNet/ydk-go/ydk/types"
        // Import here all IETF model entities that you would like to see in Read response.
    )

    func main() {
        // Build filter
        filterList := types.NewFilter()
        
        // Read running config
        getConfigEntity := crud.ReadConfig(&provider, filterList)
        
        // Get results
        getConfigEC := types.EntityToCollection(getConfigEntity)
        for _, entity := range getConfigEC.Entities() {
            ydk.YLogDebug(fmt.Sprintf("Printing %s", GetEntityXmlString(entity)))
        }
    }
