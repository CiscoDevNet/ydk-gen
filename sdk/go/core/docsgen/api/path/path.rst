Path
====

.. go:package:: ydk/path
    :synopsis: Path API

.. function:: ExecuteRPC(provider ServiceProvider, entity Entity, filter string, dataTag string, setConfigFlag bool)

    Executes payload converted from entity.

    :param provider: (:ref:`ydk-providers`).
    :param entity: (:ref:`Entity <types-entity>`).
    :param filter: (``string``) A Go string.
    :param dataTag: (``string``) A Go string.
    :param setConfigFlag: (``bool``) A Go bool.
    :return: A data node representing the result of the executed rpc.
    :rtype: :go:struct:`DataNode<ydk/types/DataNode>`

.. function:: ExecuteRPCEntity(provider ServiceProvider, rpcEntity, topEntity types.Entity)

    Executes payload converted from entity.

    :param provider: (:ref:`ydk-providers`).
    :param rpcEntity: (:ref:`Entity <types-entity>`).
    :param topEntity: (:ref:`Entity <types-entity>`). Optional arg. Use when expecting return data.
    :return: An entity representing the result of the executed rpc.
    :rtype: :ref:`Entity <types-entity>`

.. function:: ReadDatanode(filter Entity, readDataNode DataNode)

    Populates entity by reading the top level entity from a given data node

    :param filter: (:ref:`Entity <types-entity>`)
    :param readDataNode: (:go:struct:`DataNode<ydk/types/DataNode>`)
    :return: The top entity from readDataNode.
    :rtype: :ref:`Entity <types-entity>`

.. function:: ConnectToNetconfProvider(state *State, repo Repository, address, username, password string, port int)
    
    Connects to NETCONF service provider by creating a connection to the given provider using given address, username, password, and port.

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution
    :param repo: (:go:struct:`Repository<ydk/types/Repository>`).
    :param address: (``string``) A Go string.
    :param username: (``string``) A Go string.
    :param password: (``string``) A Go string.
    :param port: (``int``) An integer.
    :return: The connected service provider.
    :rtype: :go:struct:`CServiceProvider<ydk/types/CServiceProvider>`

.. function:: DisconnectFromNetconfProvider(provider CServiceProvider)

    Disconnects from NETCONF device and frees the given service provider

    :param: provider: (:go:struct:`CServiceProvider<ydk/types/CServiceProvider>`) A service provider instance.

.. function:: GetCapabilitesFromNetconfProvider(provider CServiceProvider)

    Gets the of capabilities supported by the given provider.

    :param: provider: (:go:struct:`CServiceProvider<ydk/types/CServiceProvider>`) A service provider instance.
    :return: The list of capabilities.
    :rtype: ``[]string``

.. function:: CleanUpErrorState(state *State)
    
    CleanUpErrorState cleans up memory for CState

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution

.. function:: ConnectToRestconfProvider(state *State, path, address, username, password string, port int)
    
    ConnectToRestconfProvider connects to RESTCONF device by creating a connection to the provider using given path, address, username, password, and port.

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution
    :param path: (``string``) A Go string.
    :param address: (``string``) A Go string.
    :param username: (``string``) A Go string.
    :param password: (``string``) A Go string.
    :param port: (``int``) An integer.
    :return: The connected service provider.
    :rtype: :go:struct:`CServiceProvider<ydk/types/CServiceProvider>`

.. function:: DisconnectFromRestconfProvider(provider CServiceProvider)

    DisconnectFromRestconfProvider disconnects from RESTCONF device and frees the given service provider

    :param: provider: (:go:struct:`CServiceProvider<ydk/types/CServiceProvider>`) A service provider instance.

.. function:: InitCodecServiceProvider(state *State, entity Entity, repo Repository)

    InitCodecServiceProvider initializes CodecServiceProvider
    
    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution
    :param entity: :ref:`Entity <types-entity>`
    :param repo: (:go:struct:`Repository<ydk/types/Repository>`).
    :return: The root schema node parsed from repository
    :rtype: :go:struct:`RootSchemaNode<ydk/types/RootSchemaNode>`

.. function:: CodecServiceEncode(state *State, entity Entity, rootSchema RootSchemaNode, encoding EncodingFormat)

    CodecServiceEncode encodes entity to XML/JSON payloads based on encoding format passed in

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution
    :param entity: (:ref:`Entity <types-entity>`).
    :param rootSchema: (:go:struct:`RootSchemaNode<ydk/types/RootSchemaNode>`).
    :param encoding: (:ref:`encoding-format-ydk`).
    :return: The resulting payload.
    :rtype: (``string``) A Go string.

.. function:: CodecServiceDecode(state *State, rootSchema RootSchemaNode, payload string, encoding EncodingFormat, topEntity Entity)

    CodecServiceDecode decodes XML/JSON payloads passed in to entity.

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution
    :param rootSchema: (:go:struct:`RootSchemaNode<ydk/types/RootSchemaNode>`).
    :param payload: (``string``) A Go string.
    :param encoding: (:ref:`encoding-format-ydk`).
    :param topEntity: (:ref:`Entity <types-entity>`)
    :return: The top level entity from resulting data node.
    :rtype: :ref:`Entity <types-entity>`

.. function:: ConnectToOpenDaylightProvider(state *State, path, address, username, password string, port int, encoding EncodingFormat, protocol Protocol)

    ConnectToOpenDaylightProvider connects to OpenDaylight device.

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution
    :param path: (``string``) A Go string.
    :param address: (``string``) A Go string.
    :param username: (``string``) A Go string.
    :param password: (``string``) A Go string.
    :param port: (``int``) An integer.
    :param encoding: (:ref:`encoding-format-ydk`).
    :param protocol: (:ref:`protocol-ydk`).
    :return: The connected service provider.
    :rtype: :go:struct:`COpenDaylightServiceProvider<ydk/types/COpenDaylightServiceProvider>`

.. function:: DisconnectFromOpenDaylightProvider(provider COpenDaylightServiceProvider)

    DisconnectFromOpenDaylightProvider disconnects from OpenDaylight device and frees allocated memory.

    :param provider: (:go:struct:`COpenDaylightServiceProvider<ydk/types/COpenDaylightServiceProvider>`).

.. function:: OpenDaylightServiceProviderGetNodeIDs(state *State, provider COpenDaylightServiceProvider)

    A getter function for the node ids given the opendaylight service provider.

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution
    :param provider: (:go:struct:`COpenDaylightServiceProvider<ydk/types/COpenDaylightServiceProvider>`).
    :returns: A slice of Go strings representing node ids.
    :rtype: ``[]string``

.. function:: OpenDaylightServiceProviderGetNodeProvider(provider COpenDaylightServiceProvider, nodeID string)

    A getter function for the node provider given the opendaylight service provider and node id.

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution
    :param provider: (:go:struct:`COpenDaylightServiceProvider<ydk/types/COpenDaylightServiceProvider>`).
    :param nodeID: (``string``) A Go string.
    :return: The service provider.
    :rtype: :go:struct:`CServiceProvider<ydk/types/CServiceProvider>`

.. function:: AddCState(state *State)

    AddCState creates and adds cstate to given state.

    :param state: (pointer to :go:struct:`State<ydk/errors/State>`) Current state of execution