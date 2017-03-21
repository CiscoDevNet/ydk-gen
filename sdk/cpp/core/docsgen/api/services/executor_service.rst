ExecutorService
===========

.. cpp:namespace:: ydk

.. cpp:class:: ydk::ExecutorService : public Service

Executor Service class for supporting execution of RPCs.

    .. cpp:function:: ExecutorService()

    .. cpp:function:: bool create(path::ServiceProvider & provider, Entity & entity)

    .. cpp:function:: std::shared_ptr<Entity> execute_rpc(NetconfServiceProvider & provider, Entity & entity)

        Create the entity.
        
        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param entity: An instance of :cpp:class:`Entity<ydk::Entity>` class defined under a bundle
        :return: The requested data as :cpp:class:`Entity<ydk::Entity>`
        :raises YCPPError: If an error has occurred

        