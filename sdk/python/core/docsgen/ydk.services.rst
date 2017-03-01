ydk.services module
===================

.. toctree::
    :maxdepth: 1

services.py

The Services module. Supported services include

CRUDService: Provides Create/Read/Update/Delete API's
-------------------------------------------------------

.. py:class:: ydk.services.CRUDService

        Bases: :class:`ydk.services.Service`

        CRUD Service class for supporting CRUD operations on entities.

        .. py:method:: create(provider, entity)

                Create the entity

                :param provider: An instance of ydk.providers.ServiceProvider
                :param entity: An instance of an entity class defined under the ydk.models package or subpackages.

                :return: None

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred
                :raises ydk.errors.YPYError: if other error has occurred

                Possible Errors:

                * a server side error
                * there isn't enough information in the entity to prepare the message (eg. missing keys)


        .. py:method:: read(provider, read_filter, only_config=False)

                Read the entity or entities.

                :param provider: An instance of ydk.providers.ServiceProvider

                :param read_filter: A read_filter is an instance of an entity class. An entity class is a class defined under the ydk.models package that is not an Enum, Identity or a subclass of FixedBitsDict). Attributes of this entity class may contain values that act as match expressions or can be explicitly marked as to be read by assigning an instance of type `ydk.types.READ` to them.

                :param only_config: Flag that indicates that only the data that represents configuration data is to be fetched. Default is set to False i.e both oper and config data will be fetched.


                :return: The entity or list of entities as identified by the `read_filter`

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred
                :raises ydk.errors.YPYError: if other error has occurred

                Possible errors could be

                * a server side error
                * if there isn't enough information in the entity to prepare the message (missing keys for example)


        .. py:method:: update(provider, entity)

                Update the entity.

                Note:

                * An attribute of an entity class can be deleted by setting to an instance of `ydk.types.DELETE`.
                * An entity can only be updated if it exists on the server. Otherwise a `ydk.errors.YPYError` will be raised.

                :param provider: An instance of ydk.providers.ServiceProvider
                :param entity: An instance of an entity class defined under the ydk.models package or subpackages.

                :return: None

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred
                :raises ydk.errors.YPYError: if other error has occurred

                Possible errors could be

                * a server side error
                * if there isn't enough information in the entity to the message (missing keys for example)


        .. py:method:: delete(provider, entity)

                Delete the entity

                :param provider: An instance of ydk.providers.ServiceProvider
                :param entity: An instance of an entity class defined under the ydk.models package or subpackages.

                :return: None

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred
                :raises ydk.errors.YPYError: if other error has occurred

                Possible errors could be

                * a server side error
                * if there isn't enough information in the entity to the message (missing keys for example)


NetconfService: Provides API's to execute netconf operations
--------------------------------------------------------------
.. py:class:: ydk.services.Datastore

    Bases: :class:`enum.Enum`

    Netconf datastore type

    .. data:: candidate = 1

        Candidate

    .. data:: running = 2

        Running

    .. data:: startup = 3

        Startup


.. py:class:: ydk.services.NetconfService

Bases: :py:class:`ydk.services.Service`.

Netconf Service class for executing netconf operations.

        .. py:method:: cancel_commit(provider, persist_id=None)

                Execute an cancel-commit operation to cancel an ongoing confirmed commit.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param str persist_id: This parameter is given in order to cancel a persistent confirmed commit. The value must be equal to the value given in the 'persist' parameter to the commit operation. If it does not match, the operation fails with an 'invalid-value' error.

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: If validation error has occurred.
                :raises ydk.errors.YPYError: If other error has occurred. Possible errors could be:

                    * A server side error
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: close_session(provider)

                Execute a close-session operation to cancel an ongoing confirmed commit.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: commit(provider, confirmed=False, confirm_timeout=None, persist=False, persist_id=None)

                Execute a commit operation to commit the candidate configuration as the device's new current configuration.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param bool confirmed: Perform a confirmed commit operation.
                :param int confirm_timeout: The timeout interval for a confirmed commit.
                :param str persist: Make a confirmed commit persistent. A persistent confirmed commit is not aborted if the NETCONF session terminates. The only way to abort a persistent confirmed commit is to let the timer expire, or to use the <cancel-commit> operation. The value of this parameter is a token that must be given in the 'persist-id' parameter of <commit> or <cancel-commit> operations in order to confirm or cancel the persistent confirmed commit. The token should be a random string.
                :param str persist_id: This parameter is given in order to commit a persistent confirmed commit. The value must be equal to the value given in the 'persist' parameter to the <commit> operation. If it does not match, the operation fails with an 'invalid-value' error.

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: copy_config(provider, target, source, with_defaults_option=None)

                Execute a copy-config operation to create or replace an entire configuration datastore with the contents of another complete configuration datastore.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param target: Particular configuration to copy to. Valid options are :py:attr:`.Datastore.candidate`, :py:attr:`.Datastore.running`, :py:attr:`.Datastore.startup` and url(``str``) if the device has such feature advertised in device capability.
                :param source: Particular configuration to copy from. Valid options are :py:attr:`.Datastore.candidate`, :py:attr:`.Datastore.running`, :py:attr:`.Datastore.startup` and url(``str``) if the deivce has such feature advertised in capability. A YDK entity object can also be used for this parameter.
                :param with_defaults: The explicit defaults processing mode requested.
                :type with_defaults: :py:attr:`ietf_netconf.WithDefaultsModeEnum`

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: delete_config(provider, target)

                Execute an delete-config operation to delete a configuration datastore.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param target: Particular configuration to delete. Valid options are :py:attr:`.Datastore.startup` or url(``str``).

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: discard_changes(provider)

                Execute a discard-changes operation to revert the candidate configuration to the current running configuration.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: edit_config(provider, target, config, default_operation=None, error_option=None, test_option=None)

                Execute an edit-config operation to load all or part of a specified configuration to the specified target configuration.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param target: Particular configuration to copy from. Valid options are :py:attr:`.Datastore.candidate`, :py:attr:`.Datastore.running`.
                :param config: A YDK entity object used as a config block.
                :param default_operation: Selects the default operation for this edit-config request.
                :type default_operation: :py:class:`EditConfigRpc.Input.DefaultOperationEnum`
                :param error_option: Selects the error option for this edit-config request.
                :type error_option: :py:class:`EditConfigRpc.Input.ErrorOptionEnum`
                :param test_option: Selects the test option for this edit-config request.
                :type test_option: :py:class:`EditConfigRpc.Input.TestOptionEnum`

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: get_config(provider, source, get_filter, with_defaults_option=None)

                Execute a get-config operation to retrieve all or part of a specified configuration.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param get_filter:  A YDK entity object used as a subtree filter or XPath filter.
                :param source: Particular configuration to retrieve. Valid options are :py:attr:`.Datastore.candidate`, :py:attr:`.Datastore.running`, and :py:attr:`.Datastore.startup`.
                :param with_defaults: The explicit defaults processing mode requested.
                :type with_defaults: :py:attr:`ietf_netconf.WithDefaultsModeEnum`

                :return: A YDK entity object represents copy of the running datastore subset and/or state data that matched the filter criteria (if any). An empty data container indicates that the request did not produce any results.
                :rtype: object

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: get(provider, get_filter, with_defaults_option=None)

                Execute a get operation to retrieve running configuration and device state information.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param get_filter: This parameter specifies the portion of the system configuration and state data to retrieve.
                :param with_defaults: The explicit defaults processing mode requested.
                :type with_defaults: :py:attr:`ietf_netconf.WithDefaultsModeEnum`

                :return: A YDK entity object represents copy of the running datastore subset and/or state data that matched the filter criteria (if any). An empty data container indicates that the request did not produce any results.
                :rtype: object

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: kill_session(provider, session_id)

                Execute a kill-session operation to force the termination of a NETCONF session.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param int session_id: Particular session to kill.

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: lock(provider, target)

                Execute a lock operation to allow the client to lock the configuration system of a device.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param target: Particular configuration to lock. Valid options are :py:attr:`.Datastore.candidate`, :py:attr:`.Datastore.running`, and :py:attr:`.Datastore.startup` if the device has such feature advertised.

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: unlock(provider, target)

                Execute an unlock operation to  release a configuration lock, previously obtained with the 'lock' operation.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param target: Particular configuration to unlock. Valid options are :py:attr:`.Datastore.candidate`, :py:attr:`.Datastore.running`, and :py:attr:`.Datastore.startup` if the device has such feature advertised.

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred
                :raises ydk.errors.YPYError: if other error has occurred

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).

        .. py:method:: validate(provider, source=None, config=None)

                Execute a validate operation to validate the contents of the specified configuration.

                :param provider: A provider instance.
                :type provider: ydk.providers.ServiceProvider
                :param source: Particular configuration to validate. Valid options are :py:attr:`.Datastore.candidate`, :py:attr:`.Datastore.running`, :py:attr:`.Datastore.startup` and url(``str``) if the deivce has such feature advertised in device capability. A YDK entity object can also be used for this parameter.
                :param with_defaults: The explicit defaults processing mode requested.
                :type with_defaults: :py:attr:`ietf_netconf.WithDefaultsModeEnum`

                :return: An ok reply string if operation succeeds.
                :rtype: str

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred.
                :raises ydk.errors.YPYError: if other error has occurred.

                    Possible errors could be:

                    * A server side error.
                    * If there isn't enough information in the entity to the message (missing keys for example).


CodecService: Provides encode/decode API's
--------------------------------------------

.. py:class:: ydk.services.CodecService

        Bases: :class:`ydk.services.Service`

        Codec Service class for supporting encoding entities and decoding payloads.

        .. py:method:: encode(provider, entity)

                Encodes the python entity and returns the payload. Entity is either:
                  - an instance of an entity class defined under the ydk.models package or subpackages, or
                  - a dictionary containing:
                     - module names as keys and
                     - entity instances as values

                :return: encoded value can be:
                  - an instance of an XML payload defined for a yang module, or
                  - a dictionary containing:
                     - module names as keys and
                     - instances of XML payload defined for a yang module as values

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred

        .. py:method:: decode(provider, payload)

                Decodes the the payload and returns the python entity. Payload is either:
                  - an instance of an XML payload defined for a yang module, or
                  - a dictionary containing:
                     - module names as keys and
                     - instances of XML payload defined for a yang module as values

                :return: decoded entity. Entity is either:
                  - an instance of an entity class defined under the ydk.models package or subpackages, or
                  - a dictionary containing:
                     - module names as keys and
                     - entity instances as values

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred


ExecutorService: Provides API to execute RPCs
---------------------------------------------

.. py:class:: ydk.services.ExecutorService

        Bases: :class:`ydk.services.Service`

        Executor Service class for supporting execution of RPCs.

        .. py:method:: execute_rpc(self, provider, rpc):

                Create the entity

                :param provider: An instance of ydk.providers.ServiceProvider
                :param rpc: An instance of an RPC class defined under the ydk.models package or subpackages

                :return: None

                :raises ydk.errors.YPYDataValidationError: if validation error has occurred
                :raises ydk.errors.YPYError: if other error has occurred

                Possible Errors:

                * a server side error
                * there isn't enough information in the entity to prepare the message (eg. missing keys)

