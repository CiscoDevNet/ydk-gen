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

import unittest

from ydk.errors import YPYModelError, YPYError, YPYServiceError
from ydk.models.ydktest import ydktest_sanity as ysanity
try:
    from ydk.models.ydktest import ietf_netconf
except:
    pass
from ydk.providers import NetconfServiceProvider, CodecServiceProvider
from ydk.services import ExecutorService, CodecService
from ydk.types import Empty, EncodingFormat

class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 12022)
        self.csp = CodecServiceProvider(type=EncodingFormat.XML)

        self.es = ExecutorService()
        self.cs = CodecService()

    @classmethod
    def tearDownClass(self):
        pass

    def setUp(self):
        from ydk.services import CRUDService
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def tearDown(self):
        pass

    def test_zclose_session_rpc(self):
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
        runner.one.number = 1
        runner.one.name = 'runner:one:name'

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
        self.assertEqual(reply.one.number, runner.one.number)
        self.assertEqual(reply.one.name, runner.one.name)

        # Commit
        commit_rpc = ietf_netconf.Commit()
        reply = self.es.execute_rpc(self.ncc, commit_rpc)
        self.assertIsNone(reply)

        # Get
        get_rpc = ietf_netconf.Get()
        get_rpc.input.filter = filter_xml
        reply = self.es.execute_rpc(self.ncc, get_rpc, runner)
        self.assertIsNotNone(reply)
        self.assertEqual(reply.one.number, runner.one.number)
        self.assertEqual(reply.one.name, runner.one.name)

    @unittest.skip('YCPPServiceProviderError')
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
            self.assertIsInstance(e, YPYError)

    def test_validate_rpc_1(self):
        rpc = ietf_netconf.Validate()
        rpc.input.source.candidate = Empty()
        reply = self.es.execute_rpc(self.ncc, rpc)
        self.assertIsNone(reply)

    def test_validate_rpc_2(self):
        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'

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
            self.assertIsInstance(e, YPYError)
            # self.assertEqual(e.code, YPYErrorCode.INVALID_RPC)

    @unittest.skip('TODO: get-schema rpc is not yet supported on netsim')
    def test_execute_get_schema(self):
        get_schema_rpc = ietf_netconf_monitoring.GetSchema()
        get_schema_rpc.input.identifier = 'ietf-netconf-monitoring'
        get_schema_rpc.input.format = ietf_netconf_monitoring.Yang_Identity()
        reply = self.executor.execute_rpc(self.ncc, get_schema_rpc)

if __name__ == '__main__':
    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(SanityTest)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
