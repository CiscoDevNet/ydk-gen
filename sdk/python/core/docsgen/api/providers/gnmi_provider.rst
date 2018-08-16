gNMI Service Provider
========================


.. py:class:: ydk.providers.gNMIServiceProvider(repo, address, username, password, port=57400)

    Constructs an instance of the ``gNMIServiceProvider`` to connect to a `gNMI <https://github.com/openconfig/gnmi>`_ server. By default, the provider works in secure TLS mode. The certificate file (with the name ``ems.pem``) and the key (with the name ``ems.key``) needs to be copied to the location of your python script

    :param repo: Instance of :py:class:`Repository<ydk.path.Repository>` with path to local directory containing the the `ydk yang model <https://raw.githubusercontent.com/CiscoDevNet/ydk-gen/1344b3f22d746764f17536ac4e666836de4ba84d/sdk/cpp/core/tests/models/ydk%402016-02-26.yang>`_ along with all the yang models supported on the gNMI server
    :param address: Host address of the device supporting a gNMI interface
    :param username: Username
    :param password: Password
    :param port: Port on which the gNMI interface can be accessed on the device. If not specified, the default value of ``57400`` is used

    .. py:method:: get_capabilities()

        Returns list of capabilities supported by the device


Examples
--------

YDK Logging can be enabled per below:

.. code-block:: python
    :linenos:

    import logging
    logger = logging.getLogger("ydk")
    logger.setLevel(logging.INFO)
    handler = logging.StreamHandler()
    formatter = logging.Formatter(("%(asctime)s - %(name)s - %(levelname)s - %(message)s"))
    handler.setFormatter(formatter)
    logger.addHandler(handler)

GRPC trace can be enabled per below:

.. code-block:: shell
    :linenos:

    export GRPC_VERBOSITY=debug
    export GRPC_TRACE=transport_security

Example of instantiating and using objects of ``gNMIServiceProvider`` is shown below (assuming you have ``openconfig`` bundle installed). This assumes you have the certificate file from the server (with a name like ``ems.pem``) copied to the location of the below python script:

.. code-block:: python
    :linenos:

    from ydk.models.openconfig import openconfig_bgp
    from ydk.path import Repository
    from ydk.providers import gNMIServiceProvider
    from ydk.services import CRUDService

    crud = CRUDService()
    # Create repository with location of directory where yang models from the server are downloaded
    repository = Repository('/Users/test/yang_models_location')
    # Instantiate provider
    provider = gNMIServiceProvider(repo=repository, address='10.0.0.1', username='admin', password='admin')

    capabilities = provider.get_capabilities() # Get list of capabilities

    bgp = openconfig_bgp.Bgp()

    bgp_read = crud.read(provider, bgp) # Perform read operation