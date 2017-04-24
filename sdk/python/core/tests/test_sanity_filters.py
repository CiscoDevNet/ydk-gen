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

"""test_sanity_levels.py
sanity test for ydktest-sanity.yang
"""
from __future__ import absolute_import

import unittest

from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.providers import NetconfServiceProvider
from ydk.services import CRUDService
from ydk.filters import YFilter


class SanityYang(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider(address='127.0.0.1',
                                          username='admin',
                                          password='admin',
                                          protocol='ssh',
                                          port=12022)
        self.crud = CRUDService()

    @classmethod
    def tearDownClass(self):
        pass

    def setUp(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def test_read_on_ref_class(self):
        r_1 = ysanity.Runner()
        r_1.one.number, r_1.one.name = 1, 'runner:one:name'
        self.crud.create(self.ncc, r_1)
        r_2 = ysanity.Runner()

        r_2.one.operation = YFilter.read
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_1.one.number, r_2.one.number)
        self.assertEqual(r_1.one.name, r_2.one.name)

    def test_read_on_leaf(self):
        r_1 = ysanity.Runner()
        r_1.one.number, r_1.one.name = 1, 'runner:one:name'
        self.crud.create(self.ncc, r_1)
        r_2 = ysanity.Runner()
        r_2.one.number.operation = YFilter.read
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_2.one.number, r_1.one.number)

        # this will also read r_2.one.name, not able to read only one of them
        r_2 = ysanity.Runner()
        r_2.one.number = 1
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_2.one.number, r_1.one.number)

        # no such value, will return empty data
        r_2 = ysanity.Runner()
        r_2.one.number = 2
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_2, None)

    def test_read_on_ref_enum_class(self):
        from ydk.models.ydktest.ydktest_sanity import YdkEnumTest
        r_1 = ysanity.Runner.Ytypes.BuiltInT()
        r_1.enum_value = YdkEnumTest.local
        self.crud.create(self.ncc, r_1)

        r_2 = ysanity.Runner.Ytypes.BuiltInT()
        r_2.enum_value.operation = YFilter.read
        # r_2 = self.crud.read(self.ncc, r_2)
        # self.assertEqual(r_1.enum_value, r_2.enum_value)
        # TODO: CRUD read using non top entity for YANG enum
        runner_read = self.crud.read(self.ncc, ysanity.Runner())
        self.assertEqual(r_1.enum_value, runner_read.ytypes.built_in_t.enum_value)

        r_2 = ysanity.Runner.Ytypes.BuiltInT()
        r_2.enum_value = YdkEnumTest.local
        # r_2 = self.crud.read(self.ncc, r_2)
        # self.assertEqual(r_1.enum_value, r_2.enum_value)
        # TODO: CRUD read using non top entity for YANG enum
        runner_read = self.crud.read(self.ncc, ysanity.Runner())
        self.assertEqual(r_1.enum_value, runner_read.ytypes.built_in_t.enum_value)

        # no such value, nothing returned
        r_2 = ysanity.Runner.Ytypes.BuiltInT()
        r_2.enum_value = YdkEnumTest.remote
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_2, None)

    def test_read_on_ref_list(self):
        r_1 = ysanity.Runner.OneList()
        l_1, l_2 = ysanity.Runner.OneList.Ldata(), ysanity.Runner.OneList.Ldata()
        l_1.number, l_2.number = 1, 2
        r_1.ldata.extend([l_1, l_2])
        self.crud.create(self.ncc, r_1)

        r_2 = ysanity.Runner.OneList()
        # r_2.ldata.operation = YFilter.read
        # r_2 = self.crud.read(self.ncc, r_2)
        # TODO: CRUD read using non top entity for YANG list
        runner_read = self.crud.read(self.ncc, ysanity.Runner())

        self.assertEqual(runner_read.one_list.ldata[0].number, r_1.ldata[0].number)
        self.assertEqual(runner_read.one_list.ldata[1].number, r_1.ldata[1].number)

    def test_read_on_list_with_key(self):
        r_1 = ysanity.Runner.OneList()
        l_1, l_2 = ysanity.Runner.OneList.Ldata(), ysanity.Runner.OneList.Ldata()
        l_1.number, l_2.number = 1, 2
        r_1.ldata.extend([l_1, l_2])
        self.crud.create(self.ncc, r_1)

        r_2 = ysanity.Runner.OneList()
        r_2.ldata.extend([l_1])
        # r_2 = self.crud.read(self.ncc, r_2)
        # TODO: CRUD read using non top entity for YANG list
        runner_read = ysanity.Runner()
        runner_read.one_list.ldata.extend([l_1])
        runner_read = self.crud.read(self.ncc, runner_read)

        # r_3 = ysanity.Runner.OneList()
        # r_3.ldata.extend([l_1])
        # TODO: not able to reuse l_1 using non top entity
        r_3 = ysanity.Runner()
        r_3.one_list.ldata.extend([l_1])
        self.assertEqual(runner_read.one_list.ldata[0].number, r_3.one_list.ldata[0].number)

    def test_read_on_leaflist(self):
        r_1 = ysanity.Runner.Ytypes.BuiltInT()
        r_1.llstring.extend(['1', '2', '3'])
        self.crud.create(self.ncc, r_1)
        # r_2 = ysanity.Runner.Ytypes.BuiltInT()
        # r_2.llstring.extend(['1', '2', '3'])
        # TODO: not able to use non top entity for YANG list
        r_2 = ysanity.Runner()
        r_2.ytypes.built_in_t.llstring.extend(['1','2','3'])
        runner_read = self.crud.read(self.ncc, r_2)
        self.assertEqual(runner_read.ytypes.built_in_t.llstring, r_1.llstring)

        # r_2 = ysanity.Runner.Ytypes.BuiltInT()
        # invalid input, user should use READ()
        # or the same data on device
        # r_2.llstring.extend(['something else'])
        # TODO: not able to use non top entity for YANG list
        r_2 = ysanity.Runner()
        r_2.ytypes.built_in_t.llstring.extend(['something else'])
        runner_read = self.crud.read(self.ncc, r_2)
        self.assertEqual(runner_read, None)

    def test_read_on_identity_ref(self):
        r_1 = ysanity.Runner.Ytypes.BuiltInT()
        r_1.identity_ref_value = ysanity.ChildIdentity()
        self.crud.create(self.ncc, r_1)
        # r_2 = ysanity.Runner.Ytypes.BuiltInT()
        # r_2.identity_ref_value.operation = YFilter.read
        # r_2 = self.crud.read(self.ncc, r_2)
        # TODO: CRUD read using non top entity for identity ref class
        runner_read = ysanity.Runner()
        runner_read.ytypes.built_in_t.identity_ref_value.operation = YFilter.read
        runner_read = self.crud.read(self.ncc, runner_read)
        self.assertEqual(r_1.identity_ref_value, runner_read.ytypes.built_in_t.identity_ref_value)

        # r_2 = ysanity.Runner.Ytypes.BuiltInT()
        # r_2.identity_ref_value = ysanity.ChildIdentity()
        # r_2 = self.crud.read(self.ncc, r_2)
        # TODO: CRUD read using non top entity for identity ref class
        r_2 = ysanity.Runner()
        r_2.ytypes.built_in_t.identity_ref_value = ysanity.ChildIdentity()
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_1.identity_ref_value, r_2.ytypes.built_in_t.identity_ref_value)

    def test_read_only_config(self):
        r_1 = ysanity.Runner()
        r_1.one.number, r_1.one.name = 1, 'runner:one:name'
        self.crud.create(self.ncc, r_1)
        r_2, r_3 = ysanity.Runner(), ysanity.Runner()
        r_2.one.number.operation = YFilter.read
        r_3.one.number.operation = YFilter.read

        r_2 = self.crud.read_config(self.ncc, r_2)
        r_3 = self.crud.read(self.ncc, r_3)
        # ysanity only have config data, ok to compare
        self.assertEqual(r_2.one.number, r_3.one.number)
        self.assertEqual(r_2.one.name, r_3.one.name)

    def test_decoder(self):
        # send payload to device
        runner = ysanity.Runner()
        element = ysanity.Runner.OneList.Ldata()
        element.number = 5
        element.name = 'five'
        runner.one_list.ldata.append(element)

        self.crud.create(self.ncc, runner)

        # self.crud.read(self.ncc, ysanity.Runner.OneList.Ldata())
        # TODO: CRUD read using non top entity for YANG list


if __name__ == '__main__':
    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(SanityYang)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
