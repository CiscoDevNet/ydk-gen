..
  #  YDK-YANG Development Kit
  #  Copyright 2016 Cisco Systems. All rights reserved
  # *************************************************************
  # Licensed to the Apache Software Foundation (ASF) under one
  # or more contributor license agreements.  See the NOTICE file
  # distributed with this work for additional information
  # regarding copyright ownership.  The ASF licenses this file
  # to you under the Apache License, Version 2.0 (the
  # "License"); you may not use this file except in compliance
  # with the License.  You may obtain a copy of the License at
  #
  #   http:#www.apache.org/licenses/LICENSE-2.0
  #
  #  Unless required by applicable law or agreed to in writing,
  # software distributed under the License is distributed on an
  # "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  # KIND, either express or implied.  See the License for the
  # specific language governing permissions and limitations
  # under the License.
  # *************************************************************
  # This file has been modified by Yan Gorelik, YDK Solutions.
  # All modifications in original under CiscoDevNet domain
  # introduced since October 2019 are copyrighted.
  # All rights reserved under Apache License, Version 2.0.
  # *************************************************************

Codec Services
--------------

Codec Service
=============

YDK CodecService class provides API for encoding and decoding of payload strings in XML or JSON format to/from instances of :py:class:`Entity<ydk.types.Entity>`,
which represent containers in the device supported YANG models.

.. py:class:: ydk.services.CodecService()

    .. py:method:: encode(provider, entity, pretty=True, subtree=False)

        Encodes :py:class:`Entity<ydk.types.Entity>` into payload string in XML or JSON format.

        :param provider: :py:class:`CodecServiceProvider<ydk.providers.CodecServiceProvider>` - Codec Provider instance.
        :param entity: :py:class:`Entity<ydk.types.Entity>` instance or collection of :py:class:`Entity<ydk.types.Entity>` instances of type ``list`` or ``dict``
                       or :py:class:`EntityCollection<ydk.types.EntityCollection>`.
        :param pretty: ``bool`` flag, which specifies if resulting string must be in human readable way with indentation.
        :param subtree: ``bool`` flag, which directs to encode entity to XML subtree.
        :return: Type of returned object corresponds to the type of **entity**: single payload ``str``, or ``list`` of ``str``, or a ``dictionary`` of ``str``.
        :raises: :py:exc:`YServiceError<ydk.errors.YServiceError>`, if error has occurred.

    .. py:method:: decode(provider, payload, subtree=False)

        Decodes **payload** string in XML or JSON format to instances of :py:class:`Entity<ydk.types.Entity>` class.

        :param provider: :py:class:`CodecServiceProvider<ydk.providers.CodecServiceProvider>` - Codec Provider instance.
        :param payload: ``str`` or collection of ``str`` Either a single encoded payload or a collection of payloads encapsulated to ``list`` or ``dict``.
        :param subtree: ``bool`` flag, which directs to encode entity to XML subtree.
        :return: Type of returned object corresponds to the type of **payload**. It is either an instance of :py:class:`Entity<ydk.types.Entity>`,
                 or a collection of :py:class:`Entity<ydk.types.Entity>` instances of type ``list`` or ``dict``.
        :raises: :py:exc:`YServiceError<ydk.errors.YServiceError>`, if error has occurred.

XmlSubtreeCodec
===============

.. py:class:: ydk.entity_utils.XmlSubtreeCodec

    XmlSubtreeCodec class designed to provide encoding and decoding Python model API objects of type :py:class:`Entity<ydk.types.Entity>` to/from XML encoded string.
    Compared to :py:class:`CodecService<ydk.services.CodecService>` the class does not validate encoded data for their types and values.

    .. py:method:: XmlSubtreeCodec()

        Constructs an instance of `XmlSubtreeCodec` class.

    .. py:method:: encode(entity, root_schema)

        Performs encoding of Python model API objects of type :py:class:`Entity<ydk.types.Entity>` to well formatted XML encoded string.

        :param entity: An instance of :py:class:`Entity<ydk.types.Entity>` class defined under a bundle.
        :param root_schema: An instance of :py:class:`RootSchemaNode<ydk.path.RootSchemaNode>`, which includes the model bundle.
        :return: ``str``, encoded well formatted multi-line XML payload string.
        :raises: :py:exc:`YServiceError<ydk.errors.YServiceError>`, if an error has occurred; usually appears when model is not present in the bundle.

    .. py:method:: decode(payload, entity)

        Decodes the XML encoded string to produce corresponding instance of :py:class:`Entity<ydk.types.Entity>`.

        :param payload: ``str``, XML encoded string to be decoded.
        :param entity: :py:class:`Entity<ydk.types.Entity>`, instance of shared pointer to expected top level `Entity` class.
        :return: :py:class:`Entity<ydk.types.Entity>`, shared pointer to the decoded `Entity`.
        :raises: :py:exc:`YInvalidArgumentError<ydk.errors.YInvalidArgumentError>`, if an error has occurred; usually appears when payload does not correspond to `Entity` model.

JsonSubtreeCodec
================

.. py:class:: ydk.entity_utils.JsonSubtreeCodec

    JsonSubtreeCodec class designed to provide encoding and decoding Python model API objects of type :py:class:`Entity<ydk.types.Entity>` to/from JSON encoded string.
    Compared to :py:class:`CodecService<ydk.services.CodecService>` the class does not validate encoded data for their types and values.

    .. py:method:: JsonSubtreeCodec()

        Constructs an instance of `JsonSubtreeCodec` class.

    .. py:method:: encode(entity, root_schema, pretty)

        Performs encoding of Python model API objects of type :py:class:`Entity<ydk.types.Entity>` to JSON encoded string.

        :param entity: An instance of :py:class:`Entity<ydk.types.Entity>` class defined under a bundle.
        :param root_schema: An instance of :py:class:`RootSchemaNode<path::RootSchemaNode>`, which includes the model bundle.
        :param pretty: ``bool``. If set to `True`, the function produces well formatted multi-line JSON string. If set to `False` - one line string.
        :return: ``str``, encoded JSON payload string.
        :raises: :py:exc:`YServiceError<ydk.errors.YServiceError>`, if an error has occurred; usually appears when model is not present in the bundle.

    .. py:method:: decode(payload, entity)

        Decodes the JSON encoded string to produce corresponding instance of :py:class:`Entity<ydk.types.Entity>`.

        :param payload: ``str``, JSON encoded string to be decoded.
        :param entity: :py:class:`Entity<ydk.types.Entity>`, instance of shared pointer to expected top level `Entity` class.
        :return: :py:class:`Entity<ydk.types.Entity>`, shared pointer to the decoded `Entity`.
        :raises: :py:exc:`YInvalidArgumentError<ydk.errors.YInvalidArgumentError>`, if an error has occurred; usually appears when payload does not correspond to `Entity` model.

Example of JsonSubtreeCodec usage
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this example we use :py:class:`gNMIServiceProvider<ydk.gnmi.providers.gNMIServiceProvider>` and
:py:class:`CRUDService<ydk.services.CRUDService>` to get interface configuration from IOS XR device and
then print it using :py:class:`JsonSubtreeCodec<ydk.entity_utils.JsonSubtreeCodec>`::

    from ydk.services import CRUDService
    from ydk.path import Repository
    from ydk.gnmi.providers import gNMIServiceProvider

    from ydk.models.cisco_ios_xr import Cisco_IOS_XR_ifmgr_cfg as ifmgr

    # Create gNMI service provider
    repo = Repository("/home/yan/ydk-gen/scripts/repository/10.30.110.84")
    provider = gNMIServiceProvider(repo=repo,
                               address=10.20.30.40,
                               port=57400,
                               username='admin',
                               password='admin')
    # Create CRUD service
    crud = CRUDService()

    # Build filter for interface configuration
    ifc_filter = ifmgr.InterfaceConfigurations()
    ifc = ifmgr.InterfaceConfigurations.InterfaceConfiguration()
    ifc.active = '"act"'
    ifc.interface_name = '"Loopback0"'
    ifc_filter.interface_configuration.append(ifc)

    # Read interface configuration
    ifc_read = crud.read(provider, ifc_filter)

    # Print interface configuration
    if ifc_read:
        from ydk.entity_utils import JsonSubtreeCodec
        jcodec = JsonSubtreeCodec()
        payload = jcodec.encode(ifc_read, provider.get_session().get_root_schema(), True)
        print('CREATED INTERFACE CONFIGURATION:')
        print(payload)
