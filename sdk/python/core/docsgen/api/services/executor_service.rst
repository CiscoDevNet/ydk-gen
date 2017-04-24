Executor Service
================

.. module:: ydk.services
    :synopsis: YDK Executor service

.. py:class:: ExecutorService

    Provides the functionality to execute RPCs
\
    .. py:method:: execute_rpc(self, provider, rpc_entity, top_entity=None):

        Create the entity

        :param provider: An instance of :py:class:`ServiceProvider<ydk.path.ServiceProvider>`.
        :param rpc_entity: An instance of an RPC class defined under the ydk.models package or any of its subpackages
        :param top_entity: Optional arg: Provide an instance of :py:class:`Entity<ydk.types.Entity>` only when expecting data to be returned

        :return: An instance of :py:class:`Entity<ydk.types.Entity>` when provided top_entity or None otherwise

        :raises ydk.errors.YPYDataValidationError: if validation error has occurred
        :raises ydk.errors.YPYError: if other error has occurred

        Possible Errors:
        * a server side error
        * there isn't enough information in the entity to prepare the message (eg. missing keys)