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

import unittest
from tests.compare import is_equal

from ydk.errors import YPYModelError, YPYError
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.providers import NetconfServiceProvider, NativeNetconfServiceProvider
from ydk.services import NetconfService
from ydk.services import Datastore


class SanityNetconf(unittest.TestCase):
    PROVIDER_TYPE = "non-native"

    @classmethod
    def setUpClass(self):
        if SanityNetconf.PROVIDER_TYPE == "native":
            self.ncc = NativeNetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        else:
            self.ncc = NetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        self.netconf_service = NetconfService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        from ydk.services import CRUDService
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

        print '\nIn method', self._testMethodName + ':'

    def tearDown(self):
        pass

    def test_edit_commit_get(self):
        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'

        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
        self.assertIn('ok', op)

        result = self.netconf_service.get_config(self.ncc, Datastore.candidate, get_filter)
        self.assertEqual(is_equal(runner, result), True)

        op = self.netconf_service.commit(self.ncc)
        self.assertIn('ok', op)

        result = self.netconf_service.get(self.ncc, get_filter)
        self.assertEqual(is_equal(runner, result), True)

    def test_copy(self):
        op = self.netconf_service.copy_config(self.ncc, target=Datastore.candidate, source=Datastore.running)
        self.assertIn('ok', op)

    def test_lock_unlock(self):
        op = self.netconf_service.lock(self.ncc, Datastore.running)
        self.assertIn('ok', op)

        op = self.netconf_service.unlock(self.ncc, Datastore.running)
        self.assertIn('ok', op)

    def test_lock_unlock_fail(self):
        op = self.netconf_service.lock(self.ncc, Datastore.candidate)
        self.assertIn('ok', op)

        try:
            op = self.netconf_service.unlock(self.ncc, Datastore.running)
        except Exception as e:
            self.assertIsInstance(e, YPYError)

    def test_validate(self):
        op = self.netconf_service.validate(self.ncc, source=Datastore.candidate)
        self.assertIn('ok', op)

        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'
        op = self.netconf_service.validate(self.ncc, config=runner)
        self.assertIn('ok', op)

    def test_validate_fail(self):
        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 2
        try:
            self.netconf_service.validate(self.ncc, config=runner)
        except Exception as e:
            self.assertIsInstance(e, YPYModelError)

    def test_commit_discard(self):
        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner:two:name'
        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
        self.assertIn('ok', op)

        op = self.netconf_service.discard_changes(self.ncc)
        self.assertIn('ok', op)

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
        self.assertIn('ok', op)

        op = self.netconf_service.commit(self.ncc)
        self.assertIn('ok', op)

        result = self.netconf_service.get(self.ncc, get_filter)
        self.assertEqual(is_equal(runner, result), True)

    def test_copy_config(self):
        op = self.netconf_service.copy_config(self.ncc, Datastore.candidate, Datastore.running)
        self.assertIn('ok', op)


if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        SanityNetconf.PROVIDER_TYPE = sys.argv.pop()

    unittest.main()
