#  ----------------------------------------------------------------
# Copyright 2016 Cisco Systems
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

"""test_opendaylight.py
test ODL
"""
from __future__ import absolute_import

import os
import unittest

from ydk.models.ydktest import openconfig_bgp as oc_bgp
from ydk.providers import OpenDaylightServiceProvider
from ydk.services import CRUDService
from ydk.types import EncodingFormat
from ydk.path import Repository


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        # Need to keep a local reference for repo to keep it alive,
        # as the first argument for OpenDaylightServiceProvider in libydk
        # is a reference.
        repo_path = os.path.dirname(__file__)
        repo_path = os.path.join(repo_path, '..', '..', '..', 'cpp', 'core', 'tests', 'models')
        self.repo = Repository(repo_path)
        self.odl = OpenDaylightServiceProvider(self.repo, 'localhost', 'admin', 'admin', 12306, EncodingFormat.JSON)
        self.crud = CRUDService()

    def test_read_ODL(self):
        bgp_filter = oc_bgp.Bgp()
        node_provider = self.odl.get_node_provider('xr')
        bgp_read = self.crud.read_config(node_provider, bgp_filter)

        self.assertEqual(bgp_read.global_.config.as_, 65172)
        self.assertEqual(bgp_read.global_.config.router_id, '1.2.3.4')

    def test_create_ODL(self):
        bgp = oc_bgp.Bgp()
        bgp.global_.config.as_ = 65172
        bgp.global_.config.router_id = '1.2.3.4'

        neighbor = oc_bgp.Bgp.Neighbors.Neighbor()
        neighbor.neighbor_address = '6.7.8.9'
        neighbor.config.neighbor_address = '6.7.8.9'
        neighbor.config.peer_as = 65001
        neighbor.config.local_as = 65001
        neighbor.config.peer_group = 'IBGP'

        bgp.neighbors.neighbor.append(neighbor)

        peer_group = oc_bgp.Bgp.PeerGroups.PeerGroup()
        peer_group.peer_group_name = 'IBGP'
        peer_group.config.peer_group_name = 'IBGP'
        peer_group.config.description = 'test description'
        peer_group.config.peer_as = 65001
        peer_group.config.local_as = 65001

        bgp.peer_groups.peer_group.append(peer_group)

        node_provider = self.odl.get_node_provider('xr')
        self.crud.create(node_provider, bgp)

        bgp_read = self.crud.read_config(node_provider, oc_bgp.Bgp())
        self.assertEqual(bgp_read, bgp)


if __name__ == '__main__':
    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(SanityTest)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
