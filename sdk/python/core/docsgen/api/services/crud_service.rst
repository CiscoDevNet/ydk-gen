CRUD Service
============


YDK CRUDService provides Create/Read/Update/Delete funcionalities.

.. py:class:: ydk.services.CRUDService()

    Supports CRUD operations on entities.

    .. py:method:: create(provider, entity)

        Create the entity.

        :param provider: (:py:class:`ServiceProvider<ydk.path.ServiceProvider>`) Provider instance.
        :param entity: (:py:class:`Entity<ydk.types.Entity>`) Entity instance.
        :return: ``True`` if successful, ``False`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.

    .. py:method:: read(provider, read_filter)

        Read the entity.

        :param provider: (:py:class:`ServiceProvider<ydk.path.ServiceProvider>`) Provider instance.
        :param read_filter: (:py:class:`Entity<ydk.types.Entity>`) Read filter entity instance.
        :return: An instance of :py:class:`Entity<ydk.types.Entity>` as identified by the ``filter`` if successful, ``None`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.

    .. py:method:: read_config(provider, read_filter)

        Read only config.

        :param provider: (:py:class:`ServiceProvider<ydk.path.ServiceProvider>`) Provider instance.
        :param read_filter: (:py:class:`Entity<ydk.types.Entity>`) Read filter entity instance.
        :return: An instance of :py:class:`Entity<ydk.types.Entity>` as identified by the ``filter`` if successful, ``None`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.

    .. py:method:: update(provider, entity)

        Update the entity.

        :param provider: (:py:class:`ServiceProvider<ydk.path.ServiceProvider>`) Provider instance.
        :param entity: (:py:class:`Entity<ydk.types.Entity>`) Entity instance.
        :return: ``True`` if successful, ``False`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.

    .. py:method:: delete(provider, entity)

        Delete the entity

        :param provider: (:py:class:`ServiceProvider<ydk.path.ServiceProvider>`) Provider instance.
        :param entity: (:py:class:`Entity<ydk.types.Entity>`) Entity instance.
        :return: ``True`` if successful, ``False`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.
