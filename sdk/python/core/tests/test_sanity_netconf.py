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
from ydk.providers import NetconfServiceProvider
from ydk.services import NetconfService, DataStore


class SanityNetconf(unittest.TestCase):
    PROVIDER_TYPE = "non-native"

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 12022)
        self.netconf_service = NetconfService()

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

    def test_edit_commit_get(self):
        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'

        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, DataStore.candidate, runner)
        self.assertTrue(op)

        result = self.netconf_service.get_config(self.ncc, DataStore.candidate, get_filter)
        self.assertEqual(runner.one.number, result.one.number)
        self.assertEqual(runner.one.name, result.one.name)

        op = self.netconf_service.commit(self.ncc)
        self.assertTrue(op)

        result = self.netconf_service.get(self.ncc, get_filter)
        self.assertEqual(runner.one.number, result.one.number)
        self.assertEqual(runner.one.name, result.one.name)

    def test_lock_unlock(self):
        op = self.netconf_service.lock(self.ncc, DataStore.running)
        self.assertTrue(op)

        op = self.netconf_service.unlock(self.ncc, DataStore.running)
        self.assertTrue(op)

    # Failing - NetconfService glue code needed
    def test_lock_unlock_fail(self):
        op = self.netconf_service.lock(self.ncc, DataStore.candidate)
        self.assertTrue(op)

        try:
            op = self.netconf_service.unlock(self.ncc, DataStore.running)
        except Exception as e:
            self.assertIsInstance(e, YPYError)

    def test_validate(self):
        op = self.netconf_service.validate(self.ncc, source=DataStore.candidate)
        self.assertTrue(op)

        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'
        op = self.netconf_service.validate(self.ncc, source_config=runner)
        self.assertTrue(op)

    def test_validate_fail(self):
        # should have been handled by YDK local validation
        pass

    def test_commit_discard(self):
        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner:two:name'
        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, DataStore.candidate, runner)
        self.assertTrue(op)

        op = self.netconf_service.discard_changes(self.ncc)
        self.assertTrue(op)

        op = self.netconf_service.edit_config(self.ncc, DataStore.candidate, runner)
        self.assertTrue(op)

        op = self.netconf_service.commit(self.ncc)
        self.assertTrue(op)

        result = self.netconf_service.get(self.ncc, get_filter)
        self.assertEqual(runner.two.number, result.two.number)
        self.assertEqual(runner.two.name, result.two.name)

    @unittest.skip('No message id in cancel commit payload')
    def test_confirmed_commit(self):
        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner:two:name'
        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, DataStore.candidate, runner)
        self.assertTrue(op)

        op = self.netconf_service.commit(self.ncc, confirmed=True, confirm_timeout=120)
        self.assertTrue(op)

        result = self.netconf_service.get(self.ncc, get_filter)
        self.assertEqual(runner.two.number, result.two.number)
        self.assertEqual(runner.two.name, result.two.name)

        op = self.netconf_service.cancel_commit(self.ncc)
        self.assertTrue(op)

    def test_copy_config(self):
        op = self.netconf_service.copy_config(self.ncc, DataStore.candidate, DataStore.running)
        self.assertTrue(op)

        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner:two:name'
        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, DataStore.candidate, runner)
        self.assertTrue(op)

        op = self.netconf_service.copy_config(self.ncc, DataStore.running, DataStore.candidate)
        self.assertTrue(op)

        result = self.netconf_service.get_config(self.ncc, DataStore.running, get_filter)
        self.assertEqual(runner.two.number, result.two.number)
        self.assertEqual(runner.two.name, result.two.name)

        runner.two.name = '%smodified' % runner.two.name

        op = self.netconf_service.copy_config(self.ncc, DataStore.running, runner)
        self.assertTrue(op)

        result = self.netconf_service.get_config(self.ncc, DataStore.running, get_filter)
        self.assertEqual(runner.two.number, result.two.number)
        self.assertEqual(runner.two.name, result.two.name)

    def test_delete_config(self):
        pass
        # startup and candidate cannot be both enabled in ConfD
        # op = self.netconf_service.delete_config(self.ncc, DataStore.startup)
        # self.assertIn('ok', op)

    # Failing - NetconfService glue code needed
    def test_delete_config_fail(self):
        self.assertRaises(YPYModelError,
                          self.netconf_service.delete_config,
                          self.ncc,
                          DataStore.running)
        self.assertRaises(YPYModelError,
                          self.netconf_service.delete_config,
                          self.ncc,
                          DataStore.candidate)

    # Failing - NetconfService glue code needed
    def test_copy_config_fail(self):
        self.assertRaises(YPYServiceError,
                          self.netconf_service.copy_config,
                          self.ncc,
                          target=123,
                          source=456)

    # Failing - NetconfService glue code needed
    def test_edit_config_fail(self):
        self.assertRaises(YPYServiceError,
                          self.netconf_service.edit_config,
                          self.ncc,
                          DataStore.startup,
                          DataStore.candidate)

    # Failing - NetconfService glue code needed
    def test_get_config_fail(self):
        runner = ysanity.Runner()
        self.assertRaises(YPYServiceError,
                          self.netconf_service.get_config,
                          self.ncc,
                          "invalid-input",
                          runner)

    # Failing - NetconfService glue code needed
    def test_lock_fail(self):
        self.assertRaises(YPYServiceError,
                          self.netconf_service.lock,
                          self.ncc,
                          "invalid-input")

    # Failing - NetconfService glue code needed
    def test_unlock_fail(self):
        self.assertRaises(YPYServiceError,
                          self.netconf_service.unlock,
                          self.ncc,
                          "invalid-input")


if __name__ == '__main__':
    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(SanityNetconf)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
