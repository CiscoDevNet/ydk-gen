Serivces
========

.. go:package:: ydk/services
    :synopsis: Services API

.. contents:: Table of Contents

CRUD
----

.. go:struct:: CrudService

	Supports CRUD operations on entities.

.. function:: (c *CrudService) Create(provider ServiceProvider, entity Entity)

	Create the entity.

	:param provider: An instance :go:struct:`ServiceProvider`
	:param entity: An instance of :go:struct:`Entity<ydk/types/Entity>`
	:return: whether the operation was successful or not
	:rtype: ``bool``

.. function:: (c *CrudService) Update(provider ServiceProvider, entity Entity)

	Update the entity.

	:param provider: An instance :go:struct:`ServiceProvider`
	:param entity: An instance of :go:struct:`Entity<ydk/types/Entity>`
	:return: whether the operation was successful or not
	:rtype: ``bool``

.. function:: (c *CrudService) Delete(provider ServiceProvider, entity Entity)

	Delete the entity.

	:param provider: An instance :go:struct:`ServiceProvider`
	:param entity: An instance of :go:struct:`Entity<ydk/types/Entity>`
	:return: whether the operation was successful or not
	:rtype: ``bool``

.. function:: (c *CrudService) Read(provider ServiceProvider, filter Entity)

	Read the entity.

	:param provider: An instance :go:struct:`ServiceProvider`
	:param entity: An instance of :go:struct:`Entity<ydk/types/Entity>`
	:return: the entity as identified by the given filter
	:rtype: :go:struct:`Entity<ydk/types/Entity>`

.. function:: (c *CrudService) ReadConfig(provider ServiceProvider, filter Entity)

	Read only config.

	:param provider: An instance :go:struct:`ServiceProvider`
	:param entity: An instance of :go:struct:`Entity<ydk/types/Entity>`
	:return: the entity as identified by the given filter
	:rtype: :go:struct:`Entity<ydk/types/Entity>`


Codec
-----

.. go:struct:: CodecService

	Supports encoding and decoding Go model API objects of type :go:struct:`Entity`

.. function:: (c *CodecService) Encode(provider CodecServiceProvider, entity Entity)

	Encode converts entity object to XML/JSON payload

	:param provider: An instance :go:struct:`CodecServiceProvider<ydk/types/CodecServiceProvider>`
	:param entity: An instance of :go:struct:`Entity<ydk/types/Entity>`
	:return: encoded payload
	:rtype: A Go ``string``

.. function:: (c *CodecService) Decode(provider CodecServiceProvider, payload string)

	Decode converts XML/JSON object to entity object

	:param provider: An instance :go:struct:`CodecServiceProvider<ydk/types/CodecServiceProvider>`
	:param payload: A Go ``string`` representing an encoded payload to decode
	:return: the decoded entity object
	:rtype: :go:struct:`Entity<ydk/types/Entity>`

Executor
--------

.. go:struct:: ExecutorService

	Provides the functionality to execute RPCs

.. function:: (es *ExecutorService) ExecuteRpc (provider types.ServiceProvider, rpcEntity, topEntity types.Entity)

	Create the entity

	:param provider: An instance of :go:struct:`ServiceProvider`
	:param rpcEntity: An instance of :go:struct:`Entity<ydk/types/Entity>` representing an RPC entity
	:param topEntity: Provide an instance of :go:struct:`Entity<ydk/types/Entity>` only when expecting data to be returned
	:return: Any data the resulting from the operation when provided topEntity parameter
	:rtype: :go:struct:`Entity<ydk/types/Entity>` or ``nil``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

	Possible Errors:
	* a server side error
	* there isn't enough information in the entity to prepare the message (eg. missing keys)

Netconf
-------

.. go:struct:: NetconfService

	Implements the NETCONF Protocol Operations: https://tools.ietf.org/html/rfc6241.

.. function:: (ns *NetconfService) CancelCommit(provider ServiceProvider, persistID int)

	Cancels an ongoing confirmed commit. If the persistID < 1, the operation **MUST** be issued on the same session that issued the confirmed commit.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param persistID: An ``int``
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) CloseSession(provider ServiceProvider)

	Request graceful termination of a NETCONF session

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) Commit(
	provider ServiceProvider, confirmed bool, confirmTimeOut, persist, persistID int)

	Instructs the device to implement the configuration data contained in the candidate configuration.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param confirmed: A ``bool`` that signals a confirmed commit operation
	:param comfirmTimeOut: An ``int`` representing the timeout interval for a confirmed commit
	:param persist: An ``int`` that makes the confirmed commit persistent
	:param persistID: An ``int`` that is given in order to commit a persistent confirmed commit
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) CopyConfig(
	provider ServiceProvider, target, sourceDS DataStore, sourceEntity Entity, url string)

	Create or replace an entire configuration DataStore with the contents of another complete configuration DataStore. If the target DataStore exists, it is overwritten. Otherwise, a new one is created, if allowed.
	sourceEntity should be nil OR sourceDS should be nil, but not neither or both. url is ignored unless target/sourceDS is set to Url.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param target: An instance of :go:struct:`DataStore<ydk/types/DataStore>` representing the configuration being used as the destination
	:param sourceDS: An instance of :go:struct:`DataStore<ydk/types/DataStore>` representing the configuration being used as the source
	:param sourceEntity: An instance of :go:struct:`Entity<ydk/types/Entity>` representing the configuration being used as the source
	:param url: A ``string`` representing the configuration url
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) DeleteConfig(provider ServiceProvider, target DataStore, url string)

	Delete a configuration DataStore. The RUNNING configuration DataStore cannot be deleted.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param target: An instance of :go:struct:`DataStore<ydk/types/DataStore>` representing the configuration being used as the destination
	:param url: A ``string`` representing the configuration url
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) DiscardChanges(provider ServiceProvider)

	Used to revert the candidate configuration to the current running configuration.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) EditConfig(
    provider ServiceProvider, target DataStore, config Entity, defaultOper, testOp, errorOp string)

    Loads all or part of a specified configuration to the specified target configuration datastore. Allows the new configuration to be expressed using a local file, a remote file, or inline. If the target configuration datastore does not exist, it will be created.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param target: An instance of :go:struct:`DataStore<ydk/types/DataStore>` representing the configuration being used as the destination
	:param config: An instance of :go:struct:`Entity<ydk/types/Entity>` that is a hierarchy configuration of data as defined by one of the device’s data models
	:param defaultOper: A ``string`` that changes the default from ``merge`` to either ``merge``, ``replace``, or ``none``
	:param testOp: A ``string`` that can be set to ``test-then-set``, ``set``, or ``test-only`` if the device advertises the :validate:1.1 capability
	:param errOp: A ``string`` that can be set to ``stop-on-error``, ``continue-on-error``, or ``rollback-on-error``
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) GetConfig(provider ServiceProvider, source DataStore, filter Entity)

	Retrieve all or part of a specified configuration datastore

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param source: An instance of :go:struct:`DataStore<ydk/types/DataStore>` representing the configuration being used as the source
	:param filter: An instance of :go:struct:`Entity<ydk/types/Entity>` which specifies the portion of the system configuration and state data to retrieve
	:return: The requested data
	:rtype: :go:struct:`Entity<ydk/types/Entity>`
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) Get(provider ServiceProvider, filter Entity)

	Retrieve running configuration and device state information.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param filter: An instance of :go:struct:`Entity<ydk/types/Entity>` which specifies the portion of the system configuration and state data to retrieve
	:return: The requested data
	:rtype: :go:struct:`Entity<ydk/types/Entity>`
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) KillSession(provider ServiceProvider, sessionID int)

	Force the termination of a NETCONF session.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param sessionID: An ``int`` that is the session identifier of the NETCONF session to be terminated
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) Lock(provider ServiceProvider, target DataStore)

	Allows the client to lock the entire configuration datastore system of a device.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param target: An instance of :go:struct:`DataStore<ydk/types/DataStore>` representing the configuration to lock
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) Unlock(provider ServiceProvider, target DataStore)

	Used to release a configuration lock, previously obtained with the LOCK operation.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param target: An instance of :go:struct:`DataStore<ydk/types/DataStore>` representing the configuration to unlock
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred

.. function:: (ns *NetconfService) Validate(
	provider ServiceProvider, sourceDS DataStore, sourceEntity Entity, url string)

	Validates the contents of the specified configuration. sourceEntity should be nil OR sourceDS should be nil, but not neither or both. url is ignored unless target/sourceDS is set to Url.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/types/NetconfServiceProvider>`
	:param sourceEntity: An instance of :go:struct:`Entity<ydk/types/Entity>` representing the configuration being used as the source
	:param sourceDS: An instance of :go:struct:`DataStore<ydk/types/DataStore>` representing the configuration being used as the source
	:param url: A ``string`` representing the configuration url
	:return: whether or not the operation succeeded
	:rtype: ``bool``
	:raises: `YGOError<ydk/errors/YGOError>` If error has occurred
