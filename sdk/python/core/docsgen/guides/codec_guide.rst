How do I work with instances of YANG data?
==========================================

.. contents:: Table of Contents

This document contains some examples of encoding and decoding yang data. To perform these operations, the :py:class:`CodecService<ydk.services.CodecService>` is used.

The below approaches can be used to perform encoding and decoding of an interface Ipv4 loopback configuration. For these examples, the :py:class:`Cisco_IOS_XR_ifmgr_cfg.InterfaceConfigurations<ydk.models.cisco_ios_xr.Cisco_IOS_XR_ifmgr_cfg.InterfaceConfigurations>` class is used. Note that the ``ydk`` and ``ydk-models-cisco-ios-xr`` python packages need to be installed for this example.

Converting between JSON and XML
-------------------------------

To parse a JSON string representing yang data into a YDK python object and then to an XML string, the below approach can be used.

.. code-block:: python
    :linenos:

    from ydk.providers import CodecServiceProvider
    from ydk.services import CodecService

    # Instantiate the codec service
    codec = CodecService()

    # Instantiate codec providers with json and xml options
    json_provider = CodecServiceProvider(type='json')
    xml_provider = CodecServiceProvider(type='xml')

    # Declare the JSON configuration
    if_json = ''' {
      "Cisco-IOS-XR-ifmgr-cfg:interface-configurations": {
        "interface-configuration": [
          {
            "active": "act",
            "interface-name": "Loopback0",
            "description": "PRIMARY ROUTER LOOPBACK",
            "Cisco-IOS-XR-ipv4-io-cfg:ipv4-network": {
              "addresses": {
                "primary": {
                  "address": "172.16.255.1",
                  "netmask": "255.255.255.255"
                }
              }
            }
          }
        ]
      }
    }
    '''

    # Invoke the decode method  to decode the JSON payload to a YDK python object
    interface_configurations = codec.decode(json_provider, if_json)

    # Invoke the encode method to encode the YDK python object to an XML string
    if_xml = codec.encode(xml_provider, interface_configurations)
    print(if_xml)


Converting to JSON
-------------------

To convert a YDK python object into a JSON string, the below approach can be used. Note that the attribute ``primary`` is an instance of a :ref:`presence class<presence-class>`, which is set to ``None`` by default. So it needs to be assigned to a new instance of its class.

.. code-block:: python
    :linenos:

    from ydk.providers import CodecServiceProvider
    from ydk.services import CodecService
    from ydk.models.cisco_ios_xr import Cisco_IOS_XR_ifmgr_cfg

    # Instantiate the codec service
    codec = CodecService()

    # Instantiate the provider with json option
    json_provider = CodecServiceProvider(type='json')

    # Instantiate the interface configuration class to configure the IPv4 loopback
    interface_configurations =  Cisco_IOS_XR_ifmgr_cfg.InterfaceConfigurations()

    # Instantiate the InterfaceConfiguration list instance
    interface_configuration = interface_configurations.InterfaceConfiguration()
    interface_configuration.active = "act"
    interface_configuration.interface_name = "Loopback0"
    interface_configuration.description = "PRIMARY ROUTER LOOPBACK"

    # Instantiate the Primary presence node
    interface_configuration.ipv4_network.addresses.primary = interface_configuration.ipv4_network.addresses.Primary()
    interface_configuration.ipv4_network.addresses.primary.address = "172.16.255.1"
    interface_configuration.ipv4_network.addresses.primary.netmask = "255.255.255.255"

    # Append the list instance to the parent list
    interface_configurations.interface_configuration.append(interface_configuration)

    # Invoke the encode method to encode the YDK python object to a JSON payload
    json = codec.encode(json_provider, interface_configurations)
    print(json)
