Services
========

.. go:package:: ydk/services
    :synopsis: Services API

.. code-block:: sh

   import "github.com/CiscoDevNet/ydk-go/ydk/services"

.. contents:: Table of Contents


CRUD
----

.. go:struct:: CrudService

    Supports CRUD operations on entities.

    .. function:: (c *CrudService) Create(provider ServiceProvider, entity Entity)

	Create the configuration

	:param provider: An instance of :ref:`ServiceProvider <types-service-provider>`
	:param entity: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>`
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``

    .. function:: (c *CrudService) Update(provider ServiceProvider, entity Entity)

	Update the configuration

	:param provider: An instance of :ref:`ServiceProvider <types-service-provider>`
	:param entity: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>`
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``

    .. function:: (c *CrudService) Delete(provider ServiceProvider, entity Entity)

	Delete the configuration

	:param provider: An instance of :ref:`ServiceProvider <types-service-provider>`
	:param entity: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>`
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``

    .. function:: (c *CrudService) Read(provider ServiceProvider, filter Entity)

	Read the configuration and state data

	:param provider: An instance of :ref:`ServiceProvider <types-service-provider>`
	:param entity: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>`
	:return: the entity or entity collection as identified by the `filter`. If empty collection is specified in `filter`, the data returned is an `EntityCollection` containing entire device running configuration and state
	:rtype: :ref:`Entity <types-entity>`
	
    .. function:: (c *CrudService) ReadConfig(provider ServiceProvider, filter Entity)

	Read only configuration data

	:param provider: An instance of :ref:`ServiceProvider <types-service-provider>`
	:param entity: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>`
	:return: the entity or entity collection as identified by the `filter`. If empty collection is specified in `filter`, the data returned is an `EntityCollection` containing entire device running configuration
	:rtype: :ref:`Entity <types-entity>`

        :note: When entire device configuration is requested (filter is an empty entity collection), the resulting data will include only those entities that were imported to the application. If an entity was not included, an error message is developed and logged (the logger must be enabled); example:
        
                `[ydk] [error] [Go] Entity 'ietf-netconf-acm:nacm' is not registered. Please import corresponding package to your application.`

Codec
-----

.. go:struct:: CodecService

    Supports encoding and decoding Go model API objects of type :ref:`Entity <types-entity>`

    .. function:: (c *CodecService) Encode(provider CodecServiceProvider, entity Entity)

	Encode converts entity object to XML/JSON payload

	:param provider: An instance of :go:struct:`CodecServiceProvider<ydk/providers/CodecServiceProvider>`
	:param entity: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>`
	:return: encoded payload; only one entity from entity collection can be encoded to JSON payload
	:rtype: A Go ``string``

    .. function:: (c *CodecService) Decode(provider CodecServiceProvider, payload string)

	Decode converts XML/JSON object to entity object

	:param provider: An instance :go:struct:`CodecServiceProvider<ydk/providers/CodecServiceProvider>`
	:param payload: A Go ``string`` representing an encoded payload to decode
	:return: the decoded entity object; if XML payload represents multiple entities, the method returns entity collection
	:rtype: :ref:`Entity <types-entity>`

Executor
--------

.. go:struct:: ExecutorService

	Provides the functionality to execute RPCs

    .. function:: (es *ExecutorService) ExecuteRpc (provider ServiceProvider, rpcEntity, topEntity Entity)

	Create the entity

	:param provider: An instance of :ref:`ServiceProvider <types-service-provider>`
	:param rpcEntity: An instance of :ref:`Entity <types-entity>` representing an RPC entity
	:param topEntity: Provide an instance of :ref:`Entity <types-entity>` only when expecting data to be returned
	:return: Any data the resulting from the operation when provided topEntity parameter
	:rtype: :ref:`Entity <types-entity>` or ``nil``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

	Possible Errors:
	* a server side error
	* there isn't enough information in the entity to prepare the message (eg. missing keys)

Netconf
-------

.. go:struct:: NetconfService

	Implements the NETCONF Protocol Operations: https://tools.ietf.org/html/rfc6241.

    .. function:: (ns *NetconfService) CancelCommit(provider ServiceProvider, persistID int)

	Cancels an ongoing confirmed commit. If the persistID < 1, the operation **MUST** be issued on the same session that issued the confirmed commit.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param persistID: An ``int``
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) CloseSession(provider ServiceProvider)

	Request graceful termination of a NETCONF session

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) Commit(
	provider ServiceProvider, confirmed bool, confirmTimeOut, persist, persistID int)

	Instructs the device to implement the configuration data contained in the candidate configuration.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param confirmed: A ``bool`` that signals a confirmed commit operation
	:param comfirmTimeOut: An ``int`` representing the timeout interval for a confirmed commit
	:param persist: An ``int`` that makes the confirmed commit persistent
	:param persistID: An ``int`` that is given in order to commit a persistent confirmed commit
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) CopyConfig(
	provider ServiceProvider, target, sourceDS DataStore, sourceEntity Entity, url string)

	Create or replace an entire configuration DataStore with the contents of another complete configuration DataStore. If the target DataStore exists, it is overwritten. Otherwise, a new one is created, if allowed.
	sourceEntity should be nil OR sourceDS should be nil, but not neither or both. The `url` is ignored unless target/sourceDS is set to Url.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param target: An instance of :ref:`DataStore <datastore-ydk>` representing the configuration being used as the destination
	:param sourceDS: An instance of :ref:`DataStore <datastore-ydk>` representing the configuration being used as the source
	:param sourceEntity: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>` representing the configuration being used as the source
	:param url: A ``string`` representing the configuration url
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) DeleteConfig(provider ServiceProvider, target DataStore, url string)

	Delete a configuration DataStore. The RUNNING configuration DataStore cannot be deleted.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param target: An instance of :ref:`DataStore <datastore-ydk>` representing the configuration being used as the destination
	:param url: A ``string`` representing the configuration url
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) DiscardChanges(provider ServiceProvider)

	Used to revert the candidate configuration to the current running configuration.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) EditConfig(
        provider ServiceProvider, target DataStore, config Entity, defaultOper, testOp, errorOp string)

        Loads all or part of a specified configuration to the specified target configuration datastore. Allows the new configuration to be expressed using a local file, a remote file, or inline. If the target configuration datastore does not exist, it will be created.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param target: An instance of :ref:`DataStore <datastore-ydk>` representing the configuration being used as the destination
	:param config: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>` that is a hierarchy configuration of data as defined by one of the device’s data models
	:param defaultOper: A ``string`` that changes the default from ``merge`` to either ``merge``, ``replace``, or ``none``
	:param testOp: A ``string`` that can be set to ``test-then-set``, ``set``, or ``test-only`` if the device advertises the :validate:1.1 capability
	:param errOp: A ``string`` that can be set to ``stop-on-error``, ``continue-on-error``, or ``rollback-on-error``
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) GetConfig(provider ServiceProvider, source DataStore, filter Entity)

	Retrieve all or part of a specified configuration datastore

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param source: An instance of :ref:`DataStore <datastore-ydk>` representing the configuration being used as the source
	:param filter: An instance of :ref:`Entity <types-entity>` which specifies the portion of the system configuration and state data to retrieve
	:return: The requested data as defined by filter. If empty collection is specified in `filter`, the data returned is an `EntityCollection` containing entire device running configuration
	:rtype: :ref:`Entity <types-entity>`
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred


    .. function:: (ns *NetconfService) Get(provider ServiceProvider, filter Entity)

	Retrieve running configuration and device state information.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param filter: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>` which specifies the portion of the system configuration and state data to retrieve
	:return: The requested data as defined by filter. If empty collection is specified in `filter`, the data returned is an `EntityCollection` containing entire device running configuration and state
	:rtype: :ref:`Entity <types-entity>`
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

        :note: When entire device configuration is requested (filter is an empty entity collection), the resulting data will include only those entities that were imported to the application. If an entity was not included, an error message is developed and logged (the logger must be enabled); example:
        
                `[ydk] [error] [Go] Entity 'ietf-netconf-acm:nacm' is not registered. Please import corresponding package to your application.`

    .. function:: (ns *NetconfService) KillSession(provider ServiceProvider, sessionID int)

	Force the termination of a NETCONF session.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param sessionID: An ``int`` that is the session identifier of the NETCONF session to be terminated
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) Lock(provider ServiceProvider, target DataStore)

	Allows the client to lock the entire configuration datastore system of a device.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param target: An instance of :ref:`DataStore <datastore-ydk>` representing the configuration to lock
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) Unlock(provider ServiceProvider, target DataStore)

	Used to release a configuration lock, previously obtained with the LOCK operation.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param target: An instance of :ref:`DataStore <datastore-ydk>` representing the configuration to unlock
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred

    .. function:: (ns *NetconfService) Validate(
	provider ServiceProvider, sourceDS DataStore, sourceEntity Entity, url string)

	Validates the contents of the specified configuration. sourceEntity should be nil OR sourceDS should be nil, but not neither or both. url is ignored unless target/sourceDS is set to Url.

	:param provider: An instance of :go:struct:`NetconfServiceProvider<ydk/providers/NetconfServiceProvider>`
	:param sourceEntity: An instance of :ref:`Entity <types-entity>` or :ref:`EntityCollection <entity-collection>` representing the configuration being used as the source
	:param sourceDS: An instance of :ref:`DataStore <datastore-ydk>` representing the configuration being used as the source
	:param url: A ``string`` representing the configuration url
	:return: ``true`` if the operation was successful, ``false`` - otherwise
	:rtype: ``bool``
	:raises: :go:struct:`YError<ydk/errors/YError>`, if error has occurred
