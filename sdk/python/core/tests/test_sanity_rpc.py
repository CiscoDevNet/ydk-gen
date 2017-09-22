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
sanity test for RPCs
"""
from __future__ import print_function

import unittest

from ydk.errors import YPYError, YPYErrorCode
try:
    from ydk.models.ydktest.ydktest_sanity import Runner
except:
    from ydk.models.ydktest.ydktest_sanity.runner.runner import Runner

try:
    from ydk.models.ydktest.ietf_netconf import EditConfigRpc, GetRpc, GetConfigRpc, ValidateRpc,  CommitRpc, \
        UnlockRpc, LockRpc
    #from ydk.models.ydktest.ietf_netconf_monitoring import GetSchemaRpc
except:
    try:
        from ydk.models.ydktest.ietf_netconf.edit_config.edit_config import EditConfigRpc
        from ydk.models.ydktest.ietf_netconf.get.get import GetRpc
        from ydk.models.ydktest.ietf_netconf.get_config.get_config import GetConfigRpc
        from ydk.models.ydktest.ietf_netconf.validate.validate import ValidateRpc
        from ydk.models.ydktest.ietf_netconf.commit.commit import CommitRpc
        from ydk.models.ydktest.ietf_netconf.unlock.unlock import UnlockRpc
        from ydk.models.ydktest.ietf_netconf.lock.lock import LockRpc
        # from ydk.models.ydktest.ietf_netconf_monitoring.ietf_netconf_monitoring import GetSchemaRpc
    except:
        pass

from ydk.providers import NetconfServiceProvider, NativeNetconfServiceProvider
from ydk.services import ExecutorService
from ydk.types import Empty


class SanityRpc(unittest.TestCase):
    PROVIDER_TYPE = "non-native"

    @classmethod
    def setUpClass(self):
        if SanityRpc.PROVIDER_TYPE == "native":
            self.ncc = NativeNetconfServiceProvider(address='127.0.0.1',
                                                    username='admin',
                                                    password='admin',
                                                    protocol='ssh',
                                                    port=12022)
        else:
            self.ncc = NetconfServiceProvider(address='127.0.0.1',
                                              username='admin',
                                              password='admin',
                                              protocol='ssh',
                                              port=12022)
        self.executor = ExecutorService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        from ydk.services import CRUDService
        crud = CRUDService()
        runner = Runner()
        crud.delete(self.ncc, runner)


    def tearDown(self):
        pass

    def test_execute_edit_commit_get_rpc(self):
        runner = Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'

        edit_rpc = EditConfigRpc()
        edit_rpc.input.target.candidate = Empty()
        edit_rpc.input.config = runner
        op = self.executor.execute_rpc(self.ncc, edit_rpc)
        self.assertEqual(None, op)

        commit_rpc = CommitRpc()
        op = self.executor.execute_rpc(self.ncc, commit_rpc)
        self.assertEqual(None, op)

        get_rpc = GetRpc()
        get_rpc.input.filter = Runner()
        op = self.executor.execute_rpc(self.ncc, get_rpc)
        self.assertNotEqual(None, op)

    def test_execute_get_config_rpc(self):
        get_config_rpc = GetConfigRpc()
        get_config_rpc.input.source.candidate = Empty()
        get_config_rpc.input.filter = Runner()
        initial_candidate_data = self.executor.execute_rpc(self.ncc, get_config_rpc)

        runner = Runner()
        runner.two.number = 2
        runner.two.name = 'runner:two:name'

        edit_rpc = EditConfigRpc()
        edit_rpc.input.target.candidate = Empty()
        edit_rpc.input.config = runner
        op = self.executor.execute_rpc(self.ncc, edit_rpc)
        self.assertEqual(None, op)

        final_candidate_data = self.executor.execute_rpc(self.ncc, get_config_rpc)

        # self.assertNotEqual(initial_candidate_data, final_candidate_data) #TODO
        self.assertNotEqual(None, initial_candidate_data)
        self.assertNotEqual(None, final_candidate_data)

    def test_execute_validate_rpc(self):
        import logging
        log = logging.getLogger('ydk')
        log.setLevel(logging.INFO)
        handler = logging.StreamHandler()
        formatter = logging.Formatter(("%(asctime)s - %(name)s - %(levelname)s - %(message)s"))
        handler.setFormatter(formatter)
        log.addHandler(handler)
        validate_rpc = ValidateRpc()
        validate_rpc.input.source.candidate = Empty()
        op = self.executor.execute_rpc(self.ncc, validate_rpc)
        self.assertEqual(None, op)

    def test_execute_lock_unlock_rpc(self):
        lock_rpc = LockRpc()
        lock_rpc.input.target.candidate = Empty()
        op = self.executor.execute_rpc(self.ncc, lock_rpc)
        self.assertEqual(None, op)

        unlock_rpc = UnlockRpc()
        unlock_rpc.input.target.candidate = Empty()
        op = self.executor.execute_rpc(self.ncc, unlock_rpc)
        self.assertEqual(None, op)

    def test_execute_lock_unlock_rpc_fail(self):
        lock_rpc = LockRpc()
        lock_rpc.input.target.candidate = Empty()
        op = self.executor.execute_rpc(self.ncc, lock_rpc)
        self.assertEqual(None, op)

        unlock_rpc = UnlockRpc()
        unlock_rpc.input.target.running = Empty()
        try:
            op = self.executor.execute_rpc(self.ncc, unlock_rpc)
        except Exception as e:
            self.assertIsInstance(e, YPYError)

    def test_execute_non_rpc_fail(self):
        runner = Runner()
        try:
            self.executor.execute_rpc(self.ncc, runner)
        except Exception as e:
            self.assertIsInstance(e, YPYError)
            self.assertEqual(e.code, YPYErrorCode.INVALID_RPC)

    # @unittest.skip('TODO: get-schema rpc is not yet supported on netsim')
    # def test_execute_get_schema(self):
    #     get_schema_rpc = ietf_netconf_monitoring.GetSchemaRpc()
    #     get_schema_rpc.input.identifier = 'ietf-netconf-monitoring'
    #     get_schema_rpc.input.format = ietf_netconf_monitoring.Yang_Identity()
    #     op = self.executor.execute_rpc(self.ncc, get_schema_rpc)
    #     print(op)

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        SanityRpc.PROVIDER_TYPE = sys.argv.pop()

    suite = unittest.TestLoader().loadTestsFromTestCase(SanityRpc)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)

