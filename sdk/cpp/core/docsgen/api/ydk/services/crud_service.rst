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

CRUD Service
============

.. cpp:class:: ydk::CrudService

    CRUD Service class for supporting CRUD operations on entities.

    .. cpp:function:: CrudService()

    .. cpp:function:: bool create(ydk::ServiceProvider & provider, Entity & entity)

        Create the entity.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param entity: An instance of :cpp:class:`Entity<ydk::Entity>` class defined under a bundle.
        :return: **true**, if successful, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool create(ydk::ServiceProvider & provider, std::vector<Entity\*> & entities)

        Create multiple entities.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param entities: An instance of **std::vector<Entity\*>** class, which contains one or more entities.
        :return: **true**, if successful, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: std::shared_ptr<ydk::Entity> read(ydk::ServiceProvider & provider, Entity & filter)

        Read configuration and state data for specified **filter**.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param filter: An instance of :cpp:class:`entity<ydk::Entity>` class defined under a bundle.
        :return: A pointer to an instance of :cpp:class:`Entity<ydk::Entity>` as identified by the **filter** if successful, ``nullptr`` - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: std::vector<std::shared_ptr<Entity>> read(ydk::ServiceProvider & provider, std::vector<Entity\*> & filters)

        Read configuration and state data for specified **filter**.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param filter: An instance of **std::vector<Entity\*>** class, which contains one or more entities defined under a bundle.
        :return: An instance of **std::vector<std::shared_ptr<Entity>>** as identified by the **filters** if successful, instance of empty std::vector - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: std::shared_ptr<ydk::Entity> read_config(ydk::ServiceProvider & provider, Entity & filter)

        Read only configuration data for specified **filter**.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param filter: An instance of :cpp:class:`entity<ydk::Entity>` class defined under a bundle.
        :return: A pointer to an instance of :cpp:class:`Entity<ydk::Entity>` as identified by the **filter** if successful, ``nullptr`` - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: std::vector<std::shared_ptr<Entity>> read_config(ydk::ServiceProvider & provider, std::vector<Entity\*> & filters)

        Read only configuration data for specified **filter**.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param filters: An instance of **std::vector<Entity\*>** class, which contains one or more entities defined under a bundle.
        :return: An instance of **std::vector<std::shared_ptr<Entity>>** as identified by the **filters** if successful, instance of empty std::vector - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool update(ydk::ServiceProvider & provider, Entity & entity)

        Update the entity.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param entity: An instance of :cpp:class:`Entity<ydk::Entity>` class defined under a bundle.
        :return: **true**, if successful, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool update(ydk::ServiceProvider & provider, std::vector<Entity\*> & entities)

        Update multiple entities.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param entities: An instance of **std::vector<Entity\*>** class, which contains one or more entities defined under a bundle.
        :return: **true**, if successful, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool delete\_(ydk::ServiceProvider & provider, Entity & entity)

        Delete the entity.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param entity: An instance of :cpp:class:`Entity<ydk::Entity>` class defined under a bundle.
        :return: **true**, if successful, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.

    .. cpp:function:: bool delete\_(ydk::ServiceProvider & provider, std::vector<Entity\*> & entities)

        Delete multiple entities.

        :param provider: An instance of :cpp:class:`ServiceProvider<ydk::ServiceProvider>`.
        :param entity: An instance of **std::vector<Entity\*>** class, which contains one or more entities defined under a bundle.
        :return: **true**, if successful, **false** - otherwise.
        :raises: :cpp:class:`YServiceProviderError<YServiceProviderError>`, if an error has occurred.
