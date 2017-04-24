CRUD Service
============

.. module:: ydk.services
    :synopsis: YDK CRUD Service


YDK CRUDService provides Create/Read/Update/Delete funcionalities.

.. py:class:: CRUDService()

    Supports CRUD operations on entities.
\
    .. py:method:: create(provider, entity)

        Create the entity.

        :param provider: An instance of :py:class:`ServiceProvider<ydk.path.ServiceProvider>`.
        :param entity: An instance of an :py:class:`Entity<ydk.types.Entity>` class defined under a bundle.
        :return: ``True`` if successful, ``False`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.

    .. py:method:: read(provider, entity)

        Read the entity.

        :param provider: An instance of :py:class:`ServiceProvider<ydk.path.ServiceProvider>`.
        :param filter: An instance of :py:class:`Entity<ydk.types.Entity>` class defined under a bundle.
        :return: An instance of :py:class:`Entity<ydk.types.Entity>` as identified by the ``filter`` if successful, ``None`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.

    .. py:method:: read_config(provider, entity)

        Read only config.

        :param provider: An instance of :py:class:`ServiceProvider<ydk.path.ServiceProvider>`.
        :param filter: An instance of :py:class:`Entity<ydk.types.Entity>` class defined under a bundle.
        :return: An instance of :py:class:`Entity<ydk.types.Entity>` as identified by the ``filter`` if successful, ``None`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.

    .. py:method:: update(provider, entity)

        Update the entity.

        :param provider: An instance of :py:class:`ServiceProvider<ydk.path.ServiceProvider>`.
        :param entity: An instance of an :py:class:`Entity<ydk.types.Entity>` class defined under a bundle.
        :return: ``True`` if successful, ``False`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.

    .. py:method:: delete(provider, entity)

        Delete the entity

        :param provider: An instance of :py:class:`ServiceProvider<ydk.path.ServiceProvider>`.
        :param entity: An instance of an :py:class:`Entity<ydk.types.Entity>` class defined under a bundle.
        :return: ``True`` if successful, ``False`` if not.
        :raises: :py:exc:`YPYError<ydk.errors.YPYError>` if an error has occurred.
