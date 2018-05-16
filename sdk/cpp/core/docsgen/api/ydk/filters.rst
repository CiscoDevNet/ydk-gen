YDK Filters
===========


.. cpp:enum-class:: ydk::YFilter

    Filters as defined under netconf edit-config operation attribute in `RFC 6241 <https://tools.ietf.org/html/rfc6241#section-7.2>`_ and read filters by leaf to be used with various :ref:`services<ref-service>` and :cpp:class:`entities<Entity>`.

        .. cpp:enumerator:: merge

            The configuration data identified by the element
            containing this attribute is merged with the configuration
            at the corresponding level in the configuration datastore
            identified by the <target> parameter.  This is the default
            behavior.

        .. cpp:enumerator:: create

            The configuration data identified by the element
            containing this attribute is added to the configuration if
            and only if the configuration data does not already exist in
            the configuration datastore.  If the configuration data
            exists, an <rpc-error> element is returned with an
            <error-tag> value of "data-exists".

        .. cpp:enumerator:: remove

            The configuration data identified by the element
            containing this attribute is deleted from the configuration
            if the configuration data currently exists in the
            configuration datastore.  If the configuration data does not
            exist, the "remove" operation is silently ignored by the
            server.

        .. cpp:enumerator:: delete_

            The configuration data identified by the element
            containing this attribute is deleted from the configuration
            if and only if the configuration data currently exists in
            the configuration datastore.  If the configuration data does
            not exist, an <rpc-error> element is returned with an
            <error-tag> value of "data-missing".

        .. cpp:enumerator:: replace

            The configuration data identified by the element
            containing this attribute replaces any related configuration
            in the configuration datastore identified by the <target>
            parameter.  If no such configuration data exists in the
            configuration datastore, it is created.  Unlike a
            <copy-config> operation, which replaces the entire target
            configuration, only the configuration actually present in
            the <config> parameter is affected.

        .. cpp:enumerator:: read

            When reading configuration or operational data from a network device and a specific leaf is desired to be read, the operation can be set to `read` on that leaf. See example below

        .. cpp:enumerator:: not_set

            Default value to which all configuration data is initialized to, indicating no filter has been selected. If no operation is selected, ``merge`` is performed

Example usage
~~~~~~~~~~~~~~~

An example of setting the filter for an :cpp:class:`entity<Entity>` (address family) under :cpp:class:`openconfig BGP<ydk::openconfig_bgp::Bgp>` is shown below

.. code-block:: c++
  :linenos:

  // Instantiate a bgp object representing the bgp container from the openconfig-bgp YANG model
  ydk::openconfig_bgp::Bgp bgp{};

  // Instantiate an af-safi object representing the af-safi list from the openconfig-bgp YANG model
  auto afi_safi = make_shared<ydk::openconfig_bgp::Bgp::Global::AfiSafis::AfiSafi>();

  // Set the key
  afi_safi->afi_safi_name = L3VpnIpv4Unicast();

  // Set the filter to delete, which will delete this instance of the address family
  afi_safi->filter = YFilter::delete_;

  //Append the list instance to afi-safis's afi-safi field
  bgp.global->afi_safis->afi_safi.append(afi_safi);

  // Instantiate the CRUD service and Netconf provider to connect to a device with address 10.0.0.1
  CrudService crud_service{};
  NetconfServiceProvider provider{"10.0.0.1", "test", "test", 830};

  // Invoke the CRUD Update method
  crud_service.update(provider, bgp);


An example of setting the read filter for an :cpp:class:`leaf<YLeaf>` (specifically, the `as number` leaf) under :cpp:class:`openconfig BGP<ydk::openconfig_bgp::Bgp>` is shown below

.. code-block:: c++
  :linenos:

  // Instantiate a bgp object representing the bgp container from the openconfig-bgp YANG model
  ydk::openconfig_bgp::Bgp bgp{};

  // Indicate that the `as number` is desired to be read
  bgp.config->as->filter = YFilter::read;


  // Instantiate the CRUD service and Netconf provider to connect to a device with address 10.0.0.1
  CrudService crud_service{};
  NetconfServiceProvider provider{"10.0.0.1", "test", "test", 830};

  // Invoke the CRUD Read method
  crud_service.read(provider, bgp);

