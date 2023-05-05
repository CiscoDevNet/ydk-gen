..
  # ***************************************************************
  # YDK - YANG Development Kit 
  # Copyright 2020 Yan Gorelik, YDK Solutions. All rights reserved
  # ***************************************************************
  # Licensed to the Apache Software Foundation (ASF) under one
  # or more contributor license agreements.  See the NOTICE file
  # distributed with this work for additional information
  # regarding copyright ownership.  The ASF licenses this file
  # to you under the Apache License, Version 2.0 (the
  # "License"); you may not use this file except in compliance
  # with the License.  You may obtain a copy of the License at
  #
  #   http:#www.apache.org/licenses/LICENSE-2.0
  #
  # Unless required by applicable law or agreed to in writing,
  # software distributed under the License is distributed on an
  # "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
  # KIND, either express or implied.  See the License for the
  # specific language governing permissions and limitations
  # under the License.
  # ***************************************************************

Using Restconf with HTTPS
=========================

By default the RestconfServiceProvider initializes to support HTTP non-secure protocol.
But YDK also provides partial support for HTTPS protocol. Here 'partial' means that YDK is capable communicate over secure protocol,
provides data encryption, checks Restconf server CA certificate, but the peer and host name verifications are permanently disabled.
This limitation should be addressed in future YDK releases.


CA Certificate Installation
---------------------------

In order to enable HTTPS protocol, the user must obtain and install the Restconf server CA certificate on application server.
On Ubuntu the installation procedure is as followed:

.. code-block:: sh

    cd /usr/local/share/ca-certificates/
    sudo mkdir ydk
    cp ~/myrestconf.crt ydk/
    # Make sure the permissions are OK (755 for the folder, 644 for the file)
    sudo update-ca-certificates
    # In the output of the last command check that the certificate was added

The installation procedure on CentOS-7:

.. code-block:: sh

    sudo cp ~/myrestconf.crt /etc/pki/ca-trust/source/anchors/
    sudo update-ca-trust

Getting Mac-OSX to trust self-signed SSL Certificates:

 1. Locate your CA certificate file.
 2. Open up Keychain Access.
 3. Drag your certificate into Keychain Access.
 4. Go into the Certificates section and locate the certificate you just added.
 5. Double click on it, enter the trust section and under “When using this certificate” select “Always Trust”.

Code Snippet
------------

In the application the user must explicitly specify HTTPS protocol in the host address.
The following example shows, how the RestconfServiceProvider is used to read names of all interfaces from secure Restconf server:

.. code-block:: python
 :linenos:

 #!/usr/bin/env python
 #
 from ydk.services import CRUDService
 from ydk.providers import RestconfServiceProvider
 from ydk.types import EncodingFormat
 from ydk.path import Repository

 from ydk.models.openconfig import openconfig_interfaces

 if __name__ == '__main__':

    repo = Repository('/Users/ygorelik/.ydk/sbx-iosxr-mgmt.cisco.com')
    provider = RestconfServiceProvider(
        repo,
        'https://ios-xe-mgmt.cisco.com',   # Add 'https://' prefix to the host name or address
        'developer',
        'C1sco12345',
        9443,    # HTTPS port
        EncodingFormat.JSON)

    interfaces = openconfig_interfaces.Interfaces()

    crud = CRUDService()
    all_interfaces = crud.read(provider, interfaces)

    for intf in all_interfaces.interface:
        print(intf.name)
