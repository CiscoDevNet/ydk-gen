CRUD Service
============


YDK CrudService class provides API for Create/Read/Update/Delete operations on device configuration. 

All CRUD operations performed on entities, where :py:class:`Entity<ydk.types.Entity>` instance represents single container in one of the device supported models.

.. py:class:: ydk.services.CRUDService()

    .. py:method:: create(provider, entities)

        Create one or more entities in device configuration.

        :param provider: :py:class:`ServiceProvider<ydk.path.ServiceProvider>` - Provider instance.
        :param entities: :py:class:`Entity<ydk.types.Entity>` instance for single container in device model.

                         For multiple entities encapsulate :py:class:`Entity<ydk.types.Entity>` instances in Python ``list`` or :py:class:`Config<ydk.types.Config>`.
        :return: ``True``, if configuration created successfully; ``False`` otherwise.
        :raises: Exception :py:exc:`YError<ydk.errors.YError>`, if an error has occurred.

    .. py:method:: read(provider, filter)

        Read one or more entities from device (configuration and state).

        :param provider: :py:class:`ServiceProvider<ydk.path.ServiceProvider>` - Provider instance.
        :param filter: :py:class:`Entity<ydk.types.Entity>` instance for single container in device model.

                       For multiple containers the :py:class:`Entity<ydk.types.Entity>` instances must be encapsulate in Python ``list`` or :py:class:`Filter<ydk.types.Filter>`.
        :return: For single entity filter - an instance of :py:class:`Entity<ydk.types.Entity>` as identified by the **filter** or ``None``, if operation fails.

                 For multiple filters - collection of :py:class:`Entity<ydk.types.Entity>` instances encapsulated into Python ``list`` or :py:class:`Config<ydk.types.Config>` accordingly to the type of **filter**.
        :raises: Exception :py:exc:`YError<ydk.errors.YError>`, if an error has occurred.

    .. py:method:: read_config(provider, filter)

        Read one or more entities from device running configuration.

        :param provider: :py:class:`ServiceProvider<ydk.path.ServiceProvider>` - Provider instance.
        :param filter: :py:class:`Entity<ydk.types.Entity>` instance for single container in device model.

                       For multiple containers the :py:class:`Entity<ydk.types.Entity>` instances must be encapsulate in Python ``list`` or :py:class:`Filter<ydk.types.Filter>`.
        :return: For single entity filter - an instance of :py:class:`Entity<ydk.types.Entity>` as identified by the **filter** or ``None``, if operation fails.

                 For multiple filters - collection of :py:class:`Entity<ydk.types.Entity>` instances encapsulated into Python ``list`` or :py:class:`Config<ydk.types.Config>` accordingly to the type of **filter**.
        :raises: Exception :py:exc:`YError<ydk.errors.YError>`, if an error has occurred.

    .. py:method:: update(provider, entities)

        Update one or more entities in device configuration.

        :param provider: :py:class:`ServiceProvider<ydk.path.ServiceProvider>` - Provider instance.
        :param entities: :py:class:`Entity<ydk.types.Entity>` instance for single container in device model.

                         For multiple containers encapsulate :py:class:`Entity<ydk.types.Entity>` instances in Python ``list`` or :py:class:`Config<ydk.types.Config>`.
        :return: ``True`` if successful, ``False`` - otherwise.
        :raises: Exception :py:exc:`YError<ydk.errors.YError>`, if an error has occurred.

    .. py:method:: delete(provider, entities)

        Delete one or more entities in device configuration.

        :param provider: :py:class:`ServiceProvider<ydk.path.ServiceProvider>` - Provider instance.
        :param entities: :py:class:`Entity<ydk.types.Entity>` instance for single container in device model.

                         For multiple containers encapsulate :py:class:`Entity<ydk.types.Entity>` instances in Python ``list`` or :py:class:`Config<ydk.types.Config>`.
        :return: ``True`` if successful, ``False`` - otherwise.
        :raises: Exception :py:exc:`YError<ydk.errors.YError>`, if an error has occurred.
