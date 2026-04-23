#  ----------------------------------------------------------------
# Copyright 2017-2019 Cisco Systems
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------
# This file has been modified by Yan Gorelik, YDK Solutions.
# All modifications in original under CiscoDevNet domain
# introduced since October 2019 are copyrighted.
# All rights reserved under Apache License, Version 2.0.
# ------------------------------------------------------------------

"""
netconf_session.py
NetconfSession Python wrapper.
"""

from ydk_.path import NetconfSession as _NetconfSession
from ydk.providers._endpoint import validate_endpoint


class NetconfSession(_NetconfSession):
    """
     Python wrapper for NetconfSession
    """

    def __init__(self,
                 address, username, password=None, port=830, protocol="ssh",
                 on_demand=True, common_cache=False, timeout=None, repo=None,
                 private_key_path=None, public_key_path=None):

        if timeout is None:
            timeout = -1
        address, port = validate_endpoint(address, port, protocol)
        if private_key_path is None:
            private_key_path = ""
        if public_key_path is None:
            public_key_path = ""

        self.ns = super()
        if repo is None:
            if len(public_key_path) == 0:
                self.ns.__init__(address, username, password,
                                 port, protocol, on_demand,
                                 common_cache, timeout)
            else:
                self.ns.__init__(address, username,
                                 private_key_path, public_key_path,
                                 port, protocol, on_demand,
                                 common_cache, timeout)
        else:
            if len(public_key_path) == 0:
                self.ns.__init__(repo, address, username, password,
                                 port, protocol,
                                 on_demand, timeout)
            else:
                self.ns.__init__(repo, address, username,
                                 private_key_path, public_key_path,
                                 port, protocol, on_demand,
                                 common_cache, timeout)

    def get_root_schema(self):
        return self.ns.get_root_schema()

    def invoke(self, rpc):
        return self.ns.invoke(rpc)

    def execute_netconf_operation(self, rpc):
        return self.ns.execute_netconf_operation(rpc)

    def get_capabilities(self):
        return self.ns.get_capabilities()
