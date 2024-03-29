#  ----------------------------------------------------------------
# Copyright 2018-2019 Cisco Systems
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
gnmi_session.py
gNMISession Python wrapper.
"""

from ydk_gnmi_.path import gNMISession as _gNMISession


class gNMISession(object):
    """
     Python wrapper for gNMISession
    """

    def __init__(self,
                 repo, address, port, username, password,
                 server_certificate=None, private_key=None):

        if port is None:
            port = 57400
        if server_certificate is None:
            server_certificate = ""
        if private_key is None:
            private_key = ""

        self.gs = _gNMISession(repo, address, port, username, password,
                               server_certificate, private_key)

    def get_root_schema(self):
        return self.gs.get_root_schema()

    def invoke(self, rpc):
        return self.gs.invoke(rpc)

    def subscribe(self, rpc, out_func=None, poll_func=None):
        return self.gs.subscribe(rpc, out_func, poll_func)

    def get_capabilities(self):
        return self.gs.get_capabilities()
