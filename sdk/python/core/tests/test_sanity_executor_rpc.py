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

"""test_sanity_rpc.py
sanity test for netconf
"""
from __future__ import absolute_import

import sys
import unittest

from ydk.errors import YError, YServiceError
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.models.ydktest import ietf_netconf, openconfig_bgp
from ydk.providers import NetconfServiceProvider, CodecServiceProvider
from ydk.services import ExecutorService, CodecService
from ydk.types import Empty, EncodingFormat

from test_utils import ParametrizedTestCase
from test_utils import get_device_info


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.csp = CodecServiceProvider(type=EncodingFormat.XML)
        self.es = ExecutorService()
        self.cs = CodecService()

    def setUp(self):
        # start a brand new session for every single test case
        # so test_close_session_rpc will not interfere with other test cases
        # self.ncc = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 12022)
        self.ncc = NetconfServiceProvider(
            self.hostname,
            self.username,
            self.password,
            self.port,
            self.protocol,
            self.on_demand,
            self.common_cache,
            self.timeout)
        from ydk.services import CRUDService
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def tearDown(self):
        # close session by close session rpc
        try:
            rpc = ietf_netconf.CloseSession()
            self.es.execute_rpc(self.ncc, rpc)
        except YError:
            pass
        del self.ncc

    @unittest.skip('Issues in confd')
    def test_close_session_rpc(self):
        rpc = ietf_netconf.CloseSession()

        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    def test_commit_rpc(self):
        rpc = ietf_netconf.Commit()
        rpc.input.confirmed = Empty()
        rpc.input.confirm_timeout = 5

        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    def test_copy_config_rpc(self):
        rpc = ietf_netconf.CopyConfig()
        rpc.input.target.candidate = Empty()
        rpc.input.source.running = Empty()

        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    @unittest.skip('Issues in netsim')
    def test_delete_config_rpc(self):
        rpc = ietf_netconf.DeleteConfig()
        rpc.input.target.url = "http://test"

        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    def test_discard_changes_rpc(self):
        rpc = ietf_netconf.DiscardChanges()
        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    # test edit config, get config, and get rpcs
    def test_edit_config_rpc(self):
        runner = ysanity.Runner()
        runner.ydktest_sanity_one.number = 1
        runner.ydktest_sanity_one.name = 'runner:one:name'

        runner_xml = self.cs.encode(self.csp, runner)
        filter_xml = self.cs.encode(self.csp, ysanity.Runner())

        # Edit Config
        edit_rpc = ietf_netconf.EditConfig()
        edit_rpc.input.target.candidate = Empty()
        edit_rpc.input.config = runner_xml
        reply = self.es.execute_rpc(self.ncc, edit_rpc)
        self.assertIsNone(reply)

        # Get Config
        get_config_rpc = ietf_netconf.GetConfig()
        get_config_rpc.input.source.candidate = Empty()
        get_config_rpc.input.filter = filter_xml
        reply = self.es.execute_rpc(self.ncc, get_config_rpc, runner)
        self.assertIsNotNone(reply)
        self.assertEqual(reply, runner)

        # Commit
        commit_rpc = ietf_netconf.Commit()
        reply = self.es.execute_rpc(self.ncc, commit_rpc)
        self.assertIsNone(reply)

        # Get
        get_rpc = ietf_netconf.Get()
        get_rpc.input.filter = filter_xml
        reply = self.es.execute_rpc(self.ncc, get_rpc, runner)
        self.assertIsNotNone(reply)
        self.assertEqual(reply, runner)

    @unittest.skip('YServiceProviderError')
    def test_kill_session(self):
        rpc = ietf_netconf.KillSession()
        rpc.input.session_id = 3
        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    # test lock, unlock rpc
    def test_lock_rpc(self):
        lock_rpc = ietf_netconf.Lock()
        lock_rpc.input.target.candidate = Empty()
        reply = self.es.execute_rpc(self.ncc, lock_rpc)
        self.assertIsNone(reply)

        unlock_rpc = ietf_netconf.Unlock()
        unlock_rpc.input.target.candidate = Empty()
        reply = self.es.execute_rpc(self.ncc, unlock_rpc)
        self.assertIsNone(reply)

    def test_unlock_rpc_fail(self):
        lock_rpc = ietf_netconf.Lock()
        lock_rpc.input.target.candidate = Empty()
        reply = self.es.execute_rpc(self.ncc, lock_rpc)
        self.assertIsNone(reply)

        unlock_rpc = ietf_netconf.Unlock()
        unlock_rpc.input.target.running = Empty()
        try:
            reply = self.es.execute_rpc(self.ncc, unlock_rpc)
        except Exception as e:
            self.assertIsInstance(e, YError)

    def test_validate_rpc_1(self):
        rpc = ietf_netconf.Validate()
        rpc.input.source.candidate = Empty()
        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    def test_validate_rpc_2(self):
        runner = ysanity.Runner()
        runner.ydktest_sanity_one.number = 1
        runner.ydktest_sanity_one.name = 'runner:one:name'

        runner_xml = self.cs.encode(self.csp, runner)

        rpc = ietf_netconf.Validate()
        rpc.input.source.config = runner_xml
        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    def test_non_rpc_fail(self):
        runner = ysanity.Runner()
        try:
            self.es.execute_rpc(self.ncc, runner)
        except Exception as e:
            self.assertIsInstance(e, YError)
            # self.assertEqual(e.code, YErrorCode.INVALID_RPC)

    def test_execute_get_schema(self):
        get_rpc = ietf_netconf.Get()
        get_rpc.input.filter = '<bgp xmlns="http://openconfig.net/yang/bgp"/>'
        self.es.execute_rpc(self.ncc, get_rpc, openconfig_bgp.Bgp())

if __name__ == '__main__':
    device, non_demand, common_cache, timeout = get_device_info()

    suite = unittest.TestSuite()
    suite.addTest(ParametrizedTestCase.parametrize(
        SanityTest,
        device=device,
        non_demand=non_demand,
        common_cache=common_cache,
        timeout=timeout))
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)

