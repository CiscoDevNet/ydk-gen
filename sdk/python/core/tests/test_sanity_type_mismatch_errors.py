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

"""test_sanity_type_mismatch_errors.py

Test type mismatch errors not covered by test_sanity_types.py
"""
from __future__ import absolute_import

import unittest
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.models.ydktest import ydktest_sanity_types as ytypes
from ydk.providers import NetconfServiceProvider, NativeNetconfServiceProvider
from ydk.services import CRUDService


class SanityYang(unittest.TestCase):
    PROVIDER_TYPE = "non-native"

    @classmethod
    def setUpClass(self):
        if SanityYang.PROVIDER_TYPE == "native":
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
        self.crud = CRUDService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def assertRaisesWithMessage(self, msg, func, *args, **kwargs):
        try:
            func(*args, **kwargs)
            self.assertFail()
        except Exception as inst:
            self.assertEqual(inst.message, msg)

    def test_invalid_class_assignment_int(self):
        runner = ysanity.Runner()
        runner.one = 1
        self.assertRaisesWithMessage(
            "Attempt to assign non YDK entity object of type "
            "int to Runner.One",
            self.crud.create, self.ncc, runner)

    def test_invalid_class_assignment_str(self):
        runner = ysanity.Runner()
        runner.one = "haha"
        self.assertRaisesWithMessage(
            "Attempt to assign non YDK entity object of type "
            "str to Runner.One",
            self.crud.create, self.ncc, runner)

    def test_invalid_class_assignment_identity(self):
        runner = ysanity.Runner()
        runner.one = ytypes.AnotherOneIdentity()
        self.assertRaisesWithMessage(
            "Attempt to assign non YDK entity object of type "
            "AnotherOneIdentity to Runner.One",
            self.crud.create, self.ncc, runner)

    def test_invalid_class_assignment_enum(self):
        runner = ysanity.Runner()
        runner.one = ysanity.YdkEnumTestEnum.none
        self.assertRaisesWithMessage(
            "Attempt to assign non YDK entity object of type "
            "YdkEnumTestEnum to Runner.One",
            self.crud.create, self.ncc, runner)

    def test_invalid_class_assignment_ylist(self):
        runner = ysanity.Runner()
        elem = ysanity.Runner.OneList.Ldata()
        elem.number, elem.name = 1, '1'
        runner.one_list.ldata.append(elem)
        runner.one = runner.one_list.ldata
        self.assertRaisesWithMessage(
            "Attempt to assign non YDK entity object of type "
            "YList to Runner.One",
            self.crud.create, self.ncc, runner)

    def test_invalid_class_assignment_yleaflist(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(5)])
        runner.one = runner.ytypes.built_in_t.llstring
        self.assertRaisesWithMessage(
            "Attempt to assign non YDK entity object of type "
            "YLeafList to Runner.One",
            self.crud.create, self.ncc, runner)

    def test_invalid_list_assignment_int(self):
        runner = ysanity.Runner()
        runner.one_list.ldata = 1
        self.assertRaisesWithMessage(
            "Attempt to assign object of type int to YList ldata. "
            "Please use list append or extend method.",
            self.crud.create, self.ncc, runner)

    def test_invalid_list_assignment_entity(self):
        runner = ysanity.Runner()
        runner.one_list.ldata = runner.one
        self.assertRaisesWithMessage(
            "Attempt to assign object of type One to YList ldata. "
            "Please use list append or extend method.",
            self.crud.create, self.ncc, runner)

    def test_invalid_list_assignment_llist(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(5)])
        runner.one_list.ldata = runner.ytypes.built_in_t.llstring
        self.assertRaisesWithMessage(
            "Attempt to assign object of type YLeafList to YList ldata. "
            "Please use list append or extend method.",
            self.crud.create, self.ncc, runner)

    def test_invalid_llist_assignment_int(self):
        # Wrongly assign empty YList or YLeaflist will not change payload.
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring = 1
        self.assertRaisesWithMessage(
            "Attempt to assign object of type int to YLeafList llstring. "
            "Please use list append or extend method.",
            self.crud.create, self.ncc, runner)

    def test_invalid_llist_assignment_list(self):
        runner = ysanity.Runner()
        elem = ysanity.Runner.OneList.Ldata()
        elem.number, elem.name = 1, '1'
        runner.one_list.ldata.append(elem)
        runner.ytypes.built_in_t.llstring = runner.one_list.ldata
        self.assertRaisesWithMessage(
            "Attempt to assign object of type YList to YLeafList llstring. "
            "Please use list append or extend method.",
            self.crud.create, self.ncc, runner)


if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        SanityYang.PROVIDER_TYPE = sys.argv.pop()

    suite = unittest.TestLoader().loadTestsFromTestCase(SanityYang)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
