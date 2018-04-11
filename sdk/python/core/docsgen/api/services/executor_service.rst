Executor Service
================


.. py:class:: ydk.services.ExecutorService

    Provides the functionality to execute RPCs

    .. py:method:: execute_rpc(self, provider, rpc_entity, top_entity=None)

        Create the entity

        :param provider: (:py:class:`ServiceProvider<ydk.path.ServiceProvider>`.) Provider instance.
        :param rpc_entity: (:py:class:`Entity<ydk.types.Entity>`) An instance of an RPC class defined under the ydk.models package or any of its subpackages.
        :param top_entity: (:py:class:`Entity<ydk.types.Entity>` optional)  Provide an instance of :py:class:`Entity<ydk.types.Entity>` only when expecting data to be returned.

        :return: An instance of :py:class:`Entity<ydk.types.Entity>` when provided top_entity or None otherwise
        :raises: :py:exc:`YError<ydk.errors.YError>` if an error has occurred.

        Possible Errors:

            * a server side error
            * there isn't enough information in the entity to prepare the message (eg. missing keys)
