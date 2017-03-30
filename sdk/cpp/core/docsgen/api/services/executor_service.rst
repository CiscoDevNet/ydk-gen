ExecutorService
===============

.. cpp:namespace:: ydk

.. cpp:class:: ydk::ExecutorService : public Service

Executor Service class for supporting execution of RPCs.

    .. cpp:function:: ExecutorService()

    .. cpp:function:: std::shared_ptr<Entity> execute_rpc(NetconfServiceProvider & provider, Entity & rpc_entity, std::shared_ptr<Entity> top_entity = nullptr)

        Create the rpc entity.
        
        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param rpc_entity: An rpc instance of :cpp:class:`Entity<ydk::Entity>` class defined under a bundle
        :param top_entity: Optional arg that should be provided when expecting data to be returned
        :return: The requested data as :cpp:class:`Entity<ydk::Entity>` or nullptr if N/A.
        :raises YCPPError: If an error has occurred

        