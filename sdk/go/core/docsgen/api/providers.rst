.. _ydk-providers:

Service Providers
=================

.. go:package:: ydk/providers
    :synopsis: Service Providers API

.. code-block:: sh

   import "github.com/CiscoDevNet/ydk-go/ydk/providers"

.. contents:: Table of Contents

Open Daylight
-------------

.. go:struct:: OpendaylightServiceProvider

	OpenDaylightServiceProvider is a service provider to be used to communicate with an OpenDaylight instance: https://www.opendaylight.org

	.. attribute:: Path

		A Go ``string`` that represents the path

	.. attribute:: Address

		A Go ``string`` that represents the IP address of the device supporting a open daylight interface

	.. attribute:: Username

		A Go ``string`` that represents the username to log in to the device

	.. attribute:: Password

		A Go ``string`` that represents the password to log in to the device
	
	.. attribute:: Port

		An ``int`` that represents the device port used to access the open daylight interface.

	.. :noindex:attribute:: EncodingFormat

		An instance of :ref:`encoding-format-ydk`

	.. :noindex:attribute:: Protocol

		A Go ``string`` that represents protocol used to connect to the device

.. method:: (provider *OpenDaylightServiceProvider) Connect()

	Connect to OpenDaylightServiceProvider using Path/Address/Username/Password/Port

.. method:: (provider *OpenDaylightServiceProvider) GetNodeIDs()

	:return: OpenDaylightServiceProvider Node IDs
	:rtype: ``[]string``

.. method:: (provider *OpenDaylightServiceProvider) GetNodeProvider(nodeID string)

	:return: Node provider by ID
	:rtype: :ref:`ServiceProvider <types-service-provider>`

.. method:: (provider *OpenDaylightServiceProvider) Disconnect()

	Disconnect from OpenDaylightServiceProvider


Netconf
-------

.. go:struct:: NetconfServiceProvider

	NetconfServiceProvider Implementation of ServiceProvider for the NETCONF protocol: https://tools.ietf.org/html/rfc6241

	.. attribute:: Repo

		An instance of :go:struct:`Repository<ydk/types/Repository>` This attribute represents the repository of YANG models.

	.. attribute:: Address

		A Go ``string`` that represents the IP address of the device supporting a netconf interface

	.. attribute:: Username

		A Go ``string`` that represents the username to log in to the device

	.. attribute:: Password

		A Go ``string`` that represents the password to log in to the device

	.. attribute:: Port

		An ``int`` that represents the device port used to access the netconf interface.

	.. :noindex:attribute:: Protocol

		A Go ``string`` that represents protocol used to connect to the device

.. method:: (provider *NetconfServiceProvider) Connect()
	
	Implementation of ServiceProvider interface. Connects to Netconf Server using Repo/Address/Username/Password/Port.

.. method:: (provider *NetconfServiceProvider) Disconnect()

	Implementation of ServiceProvider interface. Disconnects from Netconf Server.

.. method:: (provider *NetconfServiceProvider) GetState() *errors.State
	
	Implementation of ServiceProvider interface. Returns error state for the NetconfServiceProvider.

.. method:: (provider *NetconfServiceProvider) ExecuteRpc(oper string, ent Entity, options map[string]string) DataNode
	
	Implementation of ServiceProvider interface, which is designed specifically for CRUD operations.
	Sends RPC to Netconf server and gets response.

	:param oper: Go ``string`` containing requested operation; one of values ``create``, ``read``, ``update``, ``delete``.
	:param ent: YDK Entity (single entity) or EntityCollection (multiple entities) representing data model.
	:param options: Go ``map[string]string`` to hold options for operations. 
	                Currently is used only for **read** operation: key - ``mode``, values - ``config``, ``state``.
	:return: YDK data node, containing requested data (gnmi-get RPC) or ``nil``.
	:rtype: ``DataNode``

.. method:: (provider *NetconfServiceProvider) GetCapabilities()

	Gets the capabilities supported by NetconfServiceProvider

	:return: The list of capabilities.
	:rtype: ``[]string``


Restconf
--------

.. go:struct:: RestconfServiceProvider

	RestconfServiceProvider Implementation of ServiceProvider for the RESTCONF protocol: https://tools.ietf.org/html/draft-ietf-netconf-restconf-18

	.. attribute:: Path

		A Go ``string`` that represents the path

	.. attribute:: Address

		A Go ``string`` that represents the IP address of the device supporting a restconf interface

	.. attribute:: Username

		A Go ``string`` that represents the username to log in to the device

	.. attribute:: Password

		A Go ``string`` that represents the password to log in to the device

	.. attribute:: Port

		An ``int`` that represents the device port used to access the restconfs interface.

	.. attribute:: Encoding

		An instance of :ref:`encoding-format-ydk`

	.. attribute:: StateURLRoot

		A Go ``string``. This attribute provides backwards compatibility with older drafts of restconf RFC, this can be "/operational" or "/data"

	.. attribute:: ConfigURLRoot

		A Go ``string``. This attribute provides backwards compatibility with older drafts of restconf RFC, this can be "/config" or "/data" (which is the default)

.. method:: (provider *RestconfServiceProvider) Connect()

	Implementation of ServiceProvider interface. Connects to Restconf Server using Path/Address/Username/Password/Port.

.. method:: (provider *RestconfServiceProvider) Disconnect
	
	Implementation of ServiceProvider interface. Disconnects from Restconf Server.

.. method:: (provider *RestconfServiceProvider) GetState() *errors.State
	
	Implementation of ServiceProvider interface. Returns error state for the RestconfServiceProvider.

.. method:: (provider *RestconfServiceProvider) ExecuteRpc(oper string, ent Entity, options map[string]string) DataNode
	
	Implementation of ServiceProvider interface, which is designed specifically for CRUD operations.
	Sends RPC to Restconf server and gets response.

	:param oper: Go ``string`` containing requested operation; one of values ``create``, ``read``, ``update``, ``delete``.
	:param ent: YDK Entity (single entity) or EntityCollection (multiple entities) representing data model.
	:param options: Go ``map[string]string`` to hold options for operations. 
	                Currently is used only for **read** operation: key - ``mode``, values - ``config``, ``state``.
	:return: YDK data node, containing requested data (gnmi-get RPC) or ``nil``.
	:rtype: ``DataNode``


gNMI
-------

.. go:struct:: GnmiServiceProvider

	Implementation of ServiceProvider for the gNMI protocol.

	.. attribute:: Repo

		An instance of :go:struct:`Repository<ydk/types/Repository>` This attribute represents the repository of YANG models.

	.. attribute:: Address

		A Go ``string`` that represents the IP address of the device supporting a netconf interface.

	.. attribute:: Username

		A Go ``string`` that represents the username to log in to the device.

	.. attribute:: Password

		A Go ``string`` that represents the password to log in to the device.

	.. attribute:: Port

		An ``int`` that represents the device port used to access the gRPC interface.

	.. attribute:: ServerCert
	
	    A Go ``string`` that represents full path to a file containing gNMI server certificate of authorization (public key).
	    If not specified the service provider creates non-secure connection to the gNMI server.
	    
	.. attribute:: PrivateKey

		A Go ``string`` that represents full path to a file containing private key of YDK application host.
		If not specified and **ServerCert** attribute is defined (secure connection) the gRPC protocol uses its own private key.

.. method:: (provider *GnmiServiceProvider) GetPrivate() interface{}
	
	Implementation of ServiceProvider interface. Returns private interface value.

.. method:: (provider *GnmiServiceProvider) Connect()
	
	Implementation of ServiceProvider interface. Connects to gNMI server using Repo/Address/Username/Password/Port.

.. method:: (provider *GnmiServiceProvider) Disconnect()

	Implementation of ServiceProvider interface. Disconnects from gNMI server.

.. method:: (provider *GnmiServiceProvider) GetState() *errors.State
	
	Implementation of ServiceProvider interface. Returns error state for the GnmiServiceProvider.

.. method:: (provider *GnmiServiceProvider) ExecuteRpc(oper string, ent Entity, options map[string]string) DataNode
	
	Implementation of ServiceProvider interface, which is designed specifically for CRUD operations.
	Sends RPC to gNMI server and gets response.

	:param oper: Go ``string`` containing requested operation; one of values ``create``, ``read``, ``update``, ``delete``.
	:param ent: YDK Entity (single entity) or EntityCollection (multiple entities) representing data model.
	:param options: Go ``map[string]string`` to hold options for operations. 
	                Currently is used only for **read** operation: key - ``mode``, values - ``CONFIG``, ``STATE``, ``OPEARATIONAL``, or ``ALL``.
	:return: YDK data node, containing requested data (gnmi-get RPC) or ``nil``.
	:rtype: ``DataNode``

.. method:: (provider *GnmiServiceProvider) GetSession() *path.GnmiSession

	Gets pointer to GnmiSession structure

	:return: Pointer to GnmiSession structure.
	:rtype: ``*path.GnmiSession``


Codec
-----

.. go:struct:: CodecServiceProvider

	CodecServiceProvider Encode and decode to XML/JSON format

	.. attribute:: Repo

		An instance of :go:struct:`Repository<ydk/types/Repository>` This attribute represents the repository of YANG models.

	.. attribute:: Encoding

		An instance of :ref:`encoding-format-ydk`

	.. attribute:: RootSchemaTable

		An instance of ``map[string]RootSchemaNode`` (see docs for :go:struct:`RootSchemaNode<ydk/types/RootSchemaNode>`)

.. function:: (provider *CodecServiceProvider) Initialize()

	Initialize the CodecServiceProvider

.. function:: (provider *CodecServiceProvider) GetEncoding()

	:return: the encoding format for CodecServiceProvider
	:rtype: :ref:`encoding-format-ydk`

.. function:: (provider *CodecServiceProvider) GetRootSchemaNode(entity types.Entity)

	:return: root schema node for entity
	:rtype: :go:struct:`RootSchemaNode<ydk/types/RootSchemaNode>`

