NETCONF service
===============

YDK NetconfService class provides API for various operations on device configuration. 

All NetconfService operations performed on :py:class:`Datastore<ydk.ext.services.Datastore>`, which instance represents data storage of configuration on device, and/or :py:class:`Entity<ydk.types.Entity>`, which instance represents single container in one of the device supported models.

.. py:class:: ydk.ext.services.Datastore

    Type of data storage on device.

    .. py:data:: candidate
    .. py:data:: running
    .. py:data:: startup
    .. py:data:: url
    .. py:data:: na

.. py:class:: ydk.services.NetconfService

    .. py:method:: cancel_commit(provider, persist_id=None)

        Cancels an ongoing confirmed commit. If the **persist_id** parameter is None, the operation **must** be issued on the same session that issued the confirmed commit.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param persist_id: An ``int`` that cancels a persistent confirmed commit.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: close_session(provider)

        Request graceful termination of a NETCONF session.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: commit(provider, confirmed=False, confirm_timeout=None, persist=None, persist_id=None)

        Instructs the device to implement the configuration data contained in the candidate configuration.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param confirmed: A ``bool`` that signals a confirmed commit operation.
        :param confirm_timeout: An ``int`` representing timeout interval for a confirmed commit.
        :param persist: An ``int`` that makes the confirmed commit persistent.
        :param persist_id: An ``int`` that is given in order to commit a persistent confirmed commit.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: copy_config(provider, target, source=None, url='', source-config=None)

        Create or replace an entire **target** configuration from one of another source of configuration: **source**, **url**, or **source-config**. Only one source of configuration must be specified. If target datastore exists, it is overwritten; otherwise - new datastore is created.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param target: An instance of :py:class:`Datastore<ydk.ext.services.Datastore>` representing configuration being used as destination.
        :param source: An instance of :py:class:`Datastore<ydk.ext.services.Datastore>` representing configuration being used as source.
        :param url: A ``str`` representing the configuration URL.
        :param source-config: :py:class:`Entity<ydk.types.Entity>` instance, which represents single container in device supported model.

                              For multiple containers the :py:class:`Entity<ydk.types.Entity>` instances must be encapsulate in Python ``list`` or :py:class:`Config<ydk.types.Config>`.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: delete_config(provider, target, url="")

        Delete a configuration Datastore. The RUNNING configuration Datastore cannot be deleted.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param target: An instance of :py:class:`Datastore<ydk.ext.services.Datastore>` representing configuration to be deleted.
        :param url: A ``str`` representing the configuration URL. Optional parameter required only when target is set to ``url``.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: discard_changes(provider)

        Used to revert the candidate configuration to the current running configuration.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: edit_config(provider, target, config, default_operation='', error_option='', test_option='')

        Loads all or part of a specified configuration to the specified target configuration datastore. Allows new configuration to be read from local file, remote file, or inline. If the target configuration datastore does not exist, it will be created.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param target: An instance of :py:class:`Datastore<ydk.ext.services.Datastore>` representing the configuration being edited.
        :param config: An instance of :py:class:`Entity<ydk.types.Entity>`, which represents single container in device supported model.

                       For multiple containers the :py:class:`Entity<ydk.types.Entity>` instances must be encapsulate in Python ``list`` or :py:class:`Config<ydk.types.Config>`.
        :param default_operation: A ``str`` that changes default from ``merge`` to either ``merge``, ``replace``, or ``none``; this parameter is optional.
        :param error_option: A ``str`` that can be set to ``test-then-set``, ``set``, or ``test-only`` if the device advertises the :validate:1.1 capability; this parameter is optional.
        :param test_option: A ``str`` that can be set to ``stop-on-error``, ``continue-on-error``, or ``rollback-on-error``; this parameter is optional.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: get_config(provider, source, filter)

        Retrieve all or part of a specified configuration datastore.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param source: An instance of :py:class:`Datastore<ydk.ext.services.Datastore>` representing source configuration.
        :param filter: An instance of :py:class:`Entity<ydk.types.Entity>`, which represents single container in device supported model.
 
                       For multiple containers the :py:class:`Entity<ydk.types.Entity>` instances must be encapsulate in Python ``list`` or :py:class:`Filter<ydk.types.Filter>`.
        :return: For single entity filter - an instance of :py:class:`Entity<ydk.types.Entity>` as identified by the **filter** or ``None``, if operation fails.

                 For multiple filters - collection of :py:class:`Entity<ydk.types.Entity>` instances encapsulated into Python ``list`` or :py:class:`Config<ydk.types.Config>` accordingly to the type of **filter**.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: get(provider, filter)

        Retrieve running configuration and device state information.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param filter: An instance of :py:class:`Entity<ydk.types.Entity>`, which represents single container in device supported model.

                       For multiple containers the :py:class:`Entity<ydk.types.Entity>` instances must be encapsulate in Python ``list`` or :py:class:`Filter<ydk.types.Filter>`.
        :return: For single entity filter - an instance of :py:class:`Entity<ydk.types.Entity>` as identified by the **filter** or ``None``, if operation fails.

                 For multiple filters - collection of :py:class:`Entity<ydk.types.Entity>` instances encapsulated into Python ``list`` or :py:class:`Config<ydk.types.Config>` accordingly to the type of **filter**.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: kill_session(provider, session_id)

        Force the termination of a NETCONF session.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param session_id: An ``int`` - session identifier of the NETCONF session to be terminated.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: lock(provider, target)

        Allows the client to lock the entire configuration datastore system of a device.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param target: An instance of :py:class:`Datastore<ydk.ext.services.Datastore>` representing the configuration to lock.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: unlock(provider, target)

        Used to release a configuration lock, previously obtained with the LOCK operation.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param target: An instance of :py:class:`Datastore<ydk.ext.services.Datastore>` representing the configuration to unlock.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.

    .. py:method:: validate(provider, source=None, url='', source_config=None)

        Execute a validate operation to validate the contents of the specified configuration.

        :param provider: :py:class:`NetconfServiceProvider<ydk.providers.NetconfServiceProvider>` instance.
        :param source: An instance of :py:class:`Datastore<ydk.ext.services.Datastore>` representing the configuration datastore to validate.
        :param url: A ``str`` representing the configuration **url**.
        :param source_config: :py:class:`Entity<ydk.types.Entity>` An instance of :py:class:`Entity<ydk.types.Entity>` representing the configuration to validate.
        :return: ``True`` if the operation succeeds, ``False`` - otherwise.
        :raises: :py:exc:`YError<ydk.errors.YError>`, if error has occurred.
