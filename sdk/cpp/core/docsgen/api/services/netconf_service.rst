NetconfService
==============

.. toctree::
   :maxdepth: 2

.. cpp:namespace:: ydk

.. cpp:enum-class:: Datastore

Type of datastore to perform operation on

    .. cpp:enumerator:: candidate
    .. cpp:enumerator:: running
    .. cpp:enumerator:: startup
    .. cpp:enumerator:: url

.. cpp:class:: NetconfService : public Service

Netconf Service class for supporting encoding and decoding C++ model API objects of type :cpp:class:`Entity<Entity>`

    .. cpp:function:: NetconfService()

        Constructs an instance of NetconfService

    .. cpp:function:: bool cancel_commit(NetconfServiceProvider & provider, std::string persist_id = "")

        Cancels an ongoing confirmed commit.  If the `persist_id` parameter is not given, the operation MUST be issued on the same session that issued the confirmed commit.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param persist_id: Cancels a persistent confirmed commit.
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool close_session(NetconfServiceProvider & provider)

        Request graceful termination of a NETCONF session

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool commit(NetconfServiceProvider & provider, std::string confirmed = "", std::string confirm_timeout = "", std::string persist = "", std::string persist_id = "")

        Instructs the device to implement the configuration data contained in the candidate configuration

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param confirmed: An optional argument
        :param confirm_timeout: An optional argument
        :param persist: An optional argument
        :param persist_id: An optional argument
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool copy_config(NetconfServiceProvider & provider, DataStore target, DataStore source)

        Create or replace an entire configuration datastore with the contents of another complete configuration datastore.  If the target datastore exists, it is overwritten.  Otherwise, a new one is created, if allowed.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param target: The configuration being used as the destination of type :cpp:class:`Datastore`
        :param source: The configuration being used as the source of type :cpp:class:`Datastore`
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool copy_config(NetconfServiceProvider & provider, DataStore target, Entity& source)

        Create or replace an entire configuration datastore with the contents of another complete configuration datastore.  If the target datastore exists, it is overwritten.  Otherwise, a new one is created, if allowed.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param target: The configuration being used as the destination of type :cpp:class:`Datastore`
        :param source: The configuration being used as the source of type :cpp:class:`Entity<ydk::Entity>` 
        :return: true if the operation succeeds, else false 
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool delete_config(NetconfServiceProvider & provider, DataStore target, std::string url = "")

        Delete a configuration datastore.  The RUNNING configuration datastore cannot be deleted.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param target: The configuration of type :cpp:class:`Datastore` to be deleted
        :param url: Required only when target is set to :cpp:enumerator:`url<Datastore::url>`
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool discard_changes(NetconfServiceProvider & provider)

        Used to revert the candidate configuration to the current running configuration

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool edit_config(NetconfServiceProvider & provider, DataStore target, Entity& config, std::string default_operation = "", std::string test_option = "", std::string error_option = "")

        Loads all or part of a specified configuration to the specified target configuration datastore. Allows the new configuration to be expressed using a local file, a remote file, or inline.  If the target configuration datastore does not exist, it will be created.

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param target: The configuration being edited of type :cpp:class:`Datastore`
        :param config: An instance of :cpp:class:`Entity<ydk::Entity>` that is a hierarchy configuration of data as defined by one of the device's data models
        :param default_operation: Selects the default operation (merge, replace, or none). The default value for this parameter is "merge".
        :param test_option: Optionally set to "test-then-set", "set", or "test-only" if the device advertises the :validate:1.1 capability
        :param error_option: Optionally set to "stop-on-error", "continue-on-error", or "rollback-on-error"
        :return: true if the operation succeeds, else false.
        :raises YCPPError: If an error has occurred

    .. cpp:function:: std::shared_ptr<Entity> get_config(NetconfServiceProvider & provider, DataStore source, Entity& filter)

        Retrieve all or part of a specified configuration datastore

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param source: The configuration being queried of type :cpp:class:`Datastore`
        :return: The requested data as :cpp:class:`Entity<ydk::Entity>`
        :raises YCPPError: If an error has occurred
        
    .. cpp:function:: std::shared_ptr<Entity> get(NetconfServiceProvider & provider, Entity& filter)

        Retrieve running configuration and device state information

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param filter: An instance of :cpp:class:`Entity<ydk::Entity>` that specifies the portion of the system configuration and state data to retrieve
        :return: The requested data as :cpp:class:`Entity<ydk::Entity>`
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool kill_session(NetconfServiceProvider & provider, int session_id)

        Force the termination of a NETCONF session

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param session_id: An instance of int that is the session identifier of the NETCONF session to be terminated
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool lock(NetconfServiceProvider & provider, DataStore target)

        Allows the client to lock the entire configuration datastore system of a device

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param target: The configuration of type :cpp:class:`Datastore` to lock
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool unlock(NetconfServiceProvider & provider, DataStore target)

        Used to release a configuration lock, previously obtained with the LOCK operation

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param target: The configuration of type :cpp:class:`Datastore` to unlock
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred
        
    .. cpp:function:: bool validate(NetconfServiceProvider & provider, DataStore source)

        Checks a complete configuration for syntactical and semantic errors before applying the configuration to the device

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param source: An instance of :cpp:class:`Datastore`
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred

    .. cpp:function:: bool validate(NetconfServiceProvider & provider, Entity& source_config)

        Checks a complete configuration for syntactical and semantic errors before applying the configuration to the device

        :param provider: An instance of :cpp:class:`NetconfServiceProvider<ydk::NetconfServiceProvider>`
        :param source: An instance of :cpp:class:`Entity<ydk::Entity>`
        :return: true if the operation succeeds, else false
        :raises YCPPError: If an error has occurred
