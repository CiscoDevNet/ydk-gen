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

from __future__ import absolute_import


import sys
import unittest

import ydk.types as ytypes
from ydk.providers import NetconfServiceProvider
from ydk.services import CRUDService
try:
    from ydk.models.ydktest.ydktest_sanity import Runner, SubTest, ChildIdentity, ChildChildIdentity
    from ydk.models.ydktest.ydktest_sanity_types import YdktestType
except:
    from ydk.models.ydktest.ydktest_sanity.runner.runner import Runner
    from ydk.models.ydktest.ydktest_sanity.sub_test.sub_test import SubTest
    from ydk.models.ydktest.ydktest_sanity.ydktest_sanity import ChildIdentity, ChildChildIdentity
    from ydk.models.ydktest.ydktest_sanity_types.ydktest_sanity_types import YdktestType

from ydk.models.ydktest import ydktest_types as y_types
from ydk.types import Empty, Decimal64,  YLeaf, Bits
from ydk.errors import  YPYModelError, YPYServiceProviderError
try:
    from ydk.models.ydktest.ydktest_sanity import YdkEnumTest, YdkEnumIntTest
except:
    from ydk.models.ydktest.ydktest_sanity.ydktest_sanity import YdkEnumTest, YdkEnumIntTest

from test_utils import ParametrizedTestCase
from test_utils import get_device_info


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ncc = NetconfServiceProvider(
            cls.hostname,
            cls.username,
            cls.password,
            cls.port,
            cls.protocol,
            cls.on_demand,
            cls.common_cache,
            cls.timeout)
        cls.crud = CRUDService()

    def setUp(self):
        runner = Runner()
        self.crud.delete(self.ncc, runner)

    def tearDown(self):
        runner = Runner()
        self.crud.delete(self.ncc, runner)

    def _create_runner(self):
        # runner = Runner()
        # runner.ytypes = runner.Ytypes()
        # runner.ytypes.built_in_t = runner.ytypes.BuiltInT()

        # return runner
        pass

    def test_int8(self):
        # Create Runner
        runner = Runner()
        runner.ytypes.built_in_t.number8 = 0
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_int16(self):
        runner = Runner()
        runner.ytypes.built_in_t.number16 = 126
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_int32(self):
        runner = Runner()
        runner.ytypes.built_in_t.number32 = 200000
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_bits(self):
        runner = Runner()
        runner.ytypes.built_in_t.bits_value['disable-nagle'] = True
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_int64(self):
        runner = Runner()
        runner.ytypes.built_in_t.number64 = -9223372036854775808
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_uint8(self):
        runner = Runner()
        runner.ytypes.built_in_t.u_number8 = 0
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_uint16(self):
        runner = Runner()
        runner.ytypes.built_in_t.u_number16 = 65535
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_uint32(self):
        runner = Runner()
        runner.ytypes.built_in_t.u_number32 = 5927
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_uint64(self):
        runner = Runner()
        runner.ytypes.built_in_t.u_number64 = 18446744073709551615
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_decimal64(self):
        runner = Runner()
        runner.ytypes.built_in_t.deci64 = Decimal64('3.14')
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_string_1(self):
        runner = Runner()
        runner.ytypes.built_in_t.name = 'name_str'
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    @unittest.skip("bytes currently not supported by pybind11, see #49")
    def test_string_2(self):
        runner = Runner()
        runner.ytypes.built_in_t.name = b'name_str'
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        runner.ytypes.built_in_t.name = 'name_str'
        self.assertEqual(runner, runner1)

    def test_empty(self):
        runner = Runner()
        runner.ytypes.built_in_t.emptee = Empty()
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_boolean(self):
        runner = Runner()
        runner.ytypes.built_in_t.bool_value = True
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

        runner = Runner()
        runner.ytypes.built_in_t.bool_value = False
        self.crud.update(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_embedded_enum(self):
        runner = Runner()
        runner.ytypes.built_in_t.embeded_enum = Runner.Ytypes.BuiltInT.EmbededEnum.zero
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_enum(self):
        runner = Runner()
        runner.ytypes.built_in_t.enum_value = YdkEnumTest.none
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_union(self):
        runner = Runner()
        runner.ytypes.built_in_t.younion = YdkEnumTest.none
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_union_enum(self):
        runner = Runner()
        runner.ytypes.built_in_t.enum_int_value = YdkEnumIntTest.any
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_union_int(self):
        runner = Runner()
        runner.ytypes.built_in_t.enum_int_value = 2
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_union_recursive(self):
        runner = Runner()
        runner.ytypes.built_in_t.younion_recursive = 18
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_union_list(self):
        runner = Runner()
        runner.ytypes.built_in_t.llunion.append(1)
        runner.ytypes.built_in_t.llunion.append(3)
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    @unittest.skip('ConfD internal error.')
    def test_bits_leaflist(self):
        # User needs to append Bits instance manually to bits leaflist.
        runner = Runner()
        bits_0 = Bits()
        bits_1 = Bits()
        bits_0['disable-nagle'] = True
        bits_1['auto-sense-speed'] = True
        runner.ytypes.built_in_t.bits_llist.extend([bits_0, bits_1])
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_enum_leaflist(self):
        runner = Runner()
        runner.ytypes.built_in_t.enum_llist.append(YdkEnumTest.local)
        runner.ytypes.built_in_t.enum_llist.append(YdkEnumTest.remote)
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_identity_leaflist(self):
        runner = Runner()
        runner.ytypes.built_in_t.identity_llist.append(ChildIdentity())
        runner.ytypes.built_in_t.identity_llist.append(ChildChildIdentity())
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_union_complex_list(self):
        runner = Runner()
        runner.ytypes.built_in_t.younion_list.append("123:45")
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_identityref(self):
        runner = Runner()
        runner.ytypes.built_in_t.identity_ref_value = \
            ChildIdentity()
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_status_enum(self):
        runner = Runner()
        runner.ytypes.built_in_t.status = runner.ytypes.built_in_t.Status.not_connected
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    @unittest.skip('No unique check')
    def test_leaflist_unique(self):
        runner = Runner()
        with self.assertRaises(YPYModelError):
            for i in range(3):
                runner.ytypes.built_in_t.llstring.append(0)

    def test_list_max_elements(self):
        runner = Runner()
        elems = []
        n = 10
        for i in range(n):
            l = Runner.OneList.Ldata()
            l.number = i
            l.name = str(i)
            elems.append(l)
        runner.one_list.ldata.extend(elems)
        with self.assertRaises(YPYServiceProviderError):
            self.crud.create(self.ncc, runner)

    def test_submodule(self):
        subtest = SubTest()
        subtest.one_aug.name = 'test'
        subtest.one_aug.number = 3

        res = self.crud.create(self.ncc, subtest)
        subtest1 = self.crud.read(self.ncc, SubTest())

        # Compare runners
        self.assertEqual(subtest, subtest1)

    def test_identity_from_other_module(self):
        runner = Runner()
        runner.ytypes.built_in_t.identity_ref_value = \
            YdktestType()
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

    def test_boolean_update_read(self):
        runner = Runner()
        runner.ytypes.built_in_t.bool_value = True
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        self.assertEqual(runner, runner1)

        # Update the leaf and run update
        runner1.ytypes.built_in_t.bool_value = True
        self.crud.create(self.ncc, runner1)

        # Read into Runner2
        runner2 = self.crud.read(self.ncc, Runner())
        # Compare runners
        self.assertEqual(runner2, runner1)

    # def test_binary(self):
    #     pass

    # def test_binary_invalid(self):
    #     pass

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

