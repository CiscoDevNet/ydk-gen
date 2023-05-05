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

Netconf Service
===============

.. cpp:enum-class:: ydk::DataStore

    Type of datastore to perform operation on.

    .. cpp:enumerator:: candidate

    .. cpp:enumerator:: running

    .. cpp:enumerator:: startup

    .. cpp:enumerator:: url

    .. cpp:enumerator:: na

.. cpp:class:: NetconfService

    Netconf Service class for supporting encoding and decoding C++ model API objects of type :cpp:class:`Entity<ydk::Entity>`.

    .. cpp:function:: NetconfService()

        Constructs an instance of NetconfService.

    .. cpp:function:: bool cancel_commit(NetconfServiceProvider & provider, std::string persist_id = "")

        Cancels an ongoing confirmed commit.  If the **persist_id** parameter is not given, the operation MUST be issued on the same provider that issued the confirmed commit.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param persist_id: Cancels a persistent confirmed commit with **persist_id**.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool close_provider(NetconfServiceProvider & provider)

        Request graceful termination of a NETCONF provider.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool commit(NetconfServiceProvider & provider, std::string confirmed = "", std::string confirm_timeout = "", std::string persist = "", std::string persist_id = "")

        Instructs the device to implement the configuration data contained in the candidate configuration.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param confirmed: An optional argument.
        :param confirm_timeout: An optional argument.
        :param persist: An optional argument.
        :param persist_id: An optional argument.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool copy_config(NetconfServiceProvider & provider, DataStore target, DataStore source)

        Create or replace an entire configuration datastore with the contents of another complete configuration datastore.  If target datastore exists, it gets overwritten.  Otherwise, new datastore is created, if allowed.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`
        :param target: The configuration being used as the destination of type :cpp:class:`DataStore`
        :param source: The configuration being used as the source of type :cpp:class:`DataStore`
        :return: **true**, if the operation succeeds, **false** - otherwise
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool copy_config(NetconfServiceProvider & provider, DataStore target, Entity& source)

        Create or replace part of **target** configuration datastore with content of **source** entity.  If target datastore exists, it gets overwritten.  Otherwise, new datastore is created, if allowed.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param target: The configuration being used as the destination of type :cpp:class:`DataStore`.
        :param source: The configuration being used as the source of type :cpp:class:`Entity<ydk::Entity>`.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool copy_config(NetconfServiceProvider & provider, DataStore target, std::vector<Entity*>& source)

        Create or replace part of **target** configuration datastore with content of **source** multiple entities.  If target datastore exists, it gets overwritten.  Otherwise, new datastore is created, if allowed.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param target: The configuration being used as the destination of type :cpp:class:`DataStore`.
        :param source: The configuration being used as the source of type **std::vector<Entity*>** for multiple entities.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool delete_config(NetconfServiceProvider & provider, DataStore target, std::string url = "")

        Delete a configuration datastore.  The RUNNING configuration datastore cannot be deleted.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param target: The configuration of type :cpp:class:`DataStore` to be deleted.
        :param url: Required only when target is set to :cpp:enumerator:`url<DataStore::url>`.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool discard_changes(NetconfServiceProvider & provider)

        Revert candidate configuration to the current running configuration.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool edit_config(NetconfServiceProvider & provider, DataStore target, Entity& config, std::string default_operation = "", std::string test_option = "", std::string error_option = "")

        Loads entity configuration to specified target configuration datastore. If target configuration datastore does not exist, it will be created.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param target: The configuration being edited of type :cpp:class:`DataStore`.
        :param config: An instance of :cpp:class:`Entity<ydk::Entity>` that is a hierarchy configuration of data as defined by one of the device's data models.
        :param default_operation: Selects the default operation (merge, replace, or none). The default value for this parameter is "merge".
        :param test_option: Optionally set to "test-then-set", "set", or "test-only" if the device advertises the :validate:1.1 capability.
        :param error_option: Optionally set to "stop-on-error", "continue-on-error", or "rollback-on-error".
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool edit_config(NetconfServiceProvider & provider, DataStore target, std::vector<Entity\*>& config, std::string default_operation = "", std::string test_option = "", std::string error_option = "")

        Loads configuration of multiple entities to specified target configuration datastore. If target configuration datastore does not exist, it will be created.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param target: The configuration being edited of type :cpp:class:`DataStore`.
        :param config: An instance of **std::vector<Entity\*>** for multiple entities. That is a hierarchy configuration of data as defined by one of the device's data models.
        :param default_operation: Selects the default operation (merge, replace, or none). The default value for this parameter is "merge".
        :param test_option: Optionally set to "test-then-set", "set", or "test-only" if the device advertises the :validate:1.1 capability.
        :param error_option: Optionally set to "stop-on-error", "continue-on-error", or "rollback-on-error".
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: std::shared_ptr<Entity> get_config(NetconfServiceProvider & provider, DataStore source, Entity& filter)

        Retrieve all or part of a specified configuration datastore for specified **filter**.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param source: The configuration being queried of type :cpp:class:`DataStore`.
        :return: The requested data as :cpp:class:`Entity<ydk::Entity>`.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: std::vector<std::shared_ptr<Entity>> get_config(NetconfServiceProvider & provider, DataStore source, std::vector<Entity\*>& filter)

        Retrieve all or part of a specified configuration datastore for specified in **filter** multiple entities.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param source: The configuration being queried of type **std::vector<Entity\*>**.
        :return: The requested data as **std::vector<std::shared_ptr<Entity>>** instance; if request fails - empty **std::vector**.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: std::shared_ptr<Entity> get(NetconfServiceProvider & provider, Entity& filter)

        Retrieve running configuration and device state information.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param filter: An instance of :cpp:class:`Entity<ydk::Entity>` that contains requested part of the device configuration and state data to be retrieved as defined by one of the device’s data models.
        :return: The requested data as :cpp:class:`Entity<ydk::Entity>`.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: std::vector<std::shared_ptr<Entity>> get(NetconfServiceProvider & provider, std::vector<Entity\*>& filter)

        Retrieve running configuration and device state information for multiple entities.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param filter: An instance of **std::vector<Entity\*>** that contains requested part of device configuration and state data; if request fails - empty **std::vector**.
        :return: The requested data as :cpp:class:`Entity<ydk::Entity>`.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool kill_provider(NetconfServiceProvider & provider, int provider_id)

        Force the termination of a NETCONF provider

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param provider_id: An instance of int that is the provider identifier of the NETCONF provider to be terminated.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool lock(NetconfServiceProvider & provider, DataStore target)

        Allows the client to lock the entire configuration datastore system of a device.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param target: The configuration of type :cpp:class:`DataStore` to lock.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool unlock(NetconfServiceProvider & provider, DataStore target)

        Used to release a configuration lock, previously obtained with the LOCK operation.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param target: The configuration of type :cpp:class:`DataStore` to unlock.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool validate(NetconfServiceProvider & provider, DataStore source)

        Checks a complete configuration for syntactical and semantic errors before applying the configuration to the device.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param source: An instance of :cpp:class:`DataStore`.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool validate(NetconfServiceProvider & provider, Entity& source_config)

        Checks a complete configuration for syntactical and semantic errors before applying the configuration to the device.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<NetconfServiceProvider>`.
        :param source: An instance of :cpp:class:`Entity<ydk::Entity>`.
        :return: **true**, if the operation succeeds, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.
