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
import ydk.types as ytypes
import unittest
import copy

from ydk.services import CRUDService
try:
    from ydk.models.ydktest.ydktest_sanity import Runner, SubTest, ChildIdentityIdentity, ChildChildIdentityIdentity
    from ydk.models.ydktest.ydktest_sanity_types import YdktestTypeIdentity
except:
    from ydk.models.ydktest.ydktest_sanity.runner.runner import Runner
    from ydk.models.ydktest.ydktest_sanity.sub_test.sub_test import SubTest
    from ydk.models.ydktest.ydktest_sanity.ydktest_sanity import ChildIdentityIdentity, ChildChildIdentityIdentity
    from ydk.models.ydktest.ydktest_sanity_types.ydktest_sanity_types import YdktestTypeIdentity

from ydk.providers import NetconfServiceProvider, NativeNetconfServiceProvider
from ydk.types import Empty, DELETE, Decimal64
from compare import is_equal
from ydk.errors import YPYError, YPYModelError

try:
    from ydk.models.ydktest.ydktest_sanity import YdkEnumTestEnum, YdkEnumIntTestEnum
except:
    from ydk.models.ydktest.ydktest_sanity.ydktest_sanity import YdkEnumTestEnum, YdkEnumIntTestEnum


class SanityTest(unittest.TestCase):
    PROVIDER_TYPE = "non-native"

    @classmethod
    def setUpClass(self):
        if SanityTest.PROVIDER_TYPE == "native":
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
        runner = Runner()
        self.crud.delete(self.ncc, runner)
        self.ncc.close()

    def setUp(self):
        runner = Runner()
        self.crud.delete(self.ncc, runner)

    def _create_runner(self):
        runner = Runner()
        runner.ytypes = runner.Ytypes()
        runner.ytypes.built_in_t = runner.ytypes.BuiltInT()
        return runner

    def test_int8(self):
        # Create Runner
        runner = self._create_runner()
        runner.ytypes.built_in_t.number8 = 0
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_int16(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number16 = 126
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_int32(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number32 = 200000
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_bits(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.bits_value['disable-nagle'] = True
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_int64(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number64 = -9223372036854775808
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_uint8(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number8 = 0
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_uint16(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number16 = 65535
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_uint32(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number32 = 5927
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_uint64(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number64 = 18446744073709551615
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_decimal64(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.deci64 = Decimal64('3.14')
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_string_1(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.name = 'name_str'
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_string_2(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.name = b'name_str'
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        runner.ytypes.built_in_t.name = 'name_str'
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_empty(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.emptee = Empty()
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

        # explicit DELETE not support at the moment
        # runner1.ytypes.built_in_t.emptee = DELETE()
        # self.crud.update(self.ncc, runner1)

        # runner2 = self.crud.read(self.ncc, self._create_runner())

        # self.assertEqual(runner2.ytypes.built_in_t.emptee, None)

    def test_boolean(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.bool_value = True
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

        runner = self._create_runner()
        runner.ytypes.built_in_t.bool_value = False
        self.crud.update(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_embedded_enum(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.embeded_enum = Runner.Ytypes.BuiltInT.EmbededEnumEnum.zero
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_enum(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.enum_value = YdkEnumTestEnum.none
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.younion = YdkEnumTestEnum.none
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union_enum(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.enum_int_value = YdkEnumIntTestEnum.any
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union_int(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.enum_int_value = 2
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union_recursive(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.younion_recursive = 18
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)

        self.assertEqual(result, True)

    def test_union_list(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.llunion.append(1)
        runner.ytypes.built_in_t.llunion.append(3)
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('ConfD internal error.')
    def test_bits_leaflist(self):
        # User needs to append Bits instance manually to bits leaflist.
        runner = self._create_runner()
        bits_0 = runner.ytypes.built_in_t.BitsLlist_Bits()
        bits_1 = runner.ytypes.built_in_t.BitsLlist_Bits()
        bits_0['disable-nagle'] = True
        bits_1['auto-sense-speed'] = True
        runner.ytypes.built_in_t.bits_llist.extend([bits_0, bits_1])
        self.crud.create(self.ncc, runner)

        # Read into Runner1
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_enum_leaflist(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.enum_llist.append(YdkEnumTestEnum.local)
        runner.ytypes.built_in_t.enum_llist.append(YdkEnumTestEnum.remote)
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_identity_leaflist(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.identity_llist.append(ChildIdentityIdentity())
        runner.ytypes.built_in_t.identity_llist.append(ChildChildIdentityIdentity())
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

        #DEEPCOPY
        runner_copy = copy.deepcopy(runner)
        # result = is_equal(runner, runner_copy)
        # self.assertEqual(result, True)

        self.crud.delete(self.ncc, Runner())
        self.crud.create(self.ncc, runner_copy)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union_complex_list(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.younion_list.append('123:45')
        runner.ytypes.built_in_t.younion_list.append(23)
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        union_list = runner.ytypes.built_in_t.younion_list
        self.assertTrue(23 in union_list and '123:45' in union_list)

    def test_identityref(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.identity_ref_value = \
            ChildIdentityIdentity()
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_status_enum(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.status = runner.ytypes.built_in_t.StatusEnum.not_connected
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)


    def test_leaflist_unique(self):
        runner = self._create_runner()
        with self.assertRaises(YPYModelError):
            runner.ytypes.built_in_t.llstring.extend([None for i in range(3)])

    def test_list_max_elements(self):
        runner = self._create_runner()
        elems = []
        n = 10
        for i in range(n):
            l = Runner.OneList.Ldata()
            l.number = i
            l.name = str(i)
            elems.append(l)
        runner.one_list.ldata.extend(elems)
        self.assertRaises(YPYModelError,
                          self.crud.create, self.ncc, runner)

    def test_submodule(self):
        subtest = SubTest()
        subtest.one_aug.name = 'test'
        subtest.one_aug.number = 3

        res = self.crud.create(self.ncc, subtest)

        subtest1 = self.crud.read(self.ncc, SubTest())

        # Compare runners
        result = is_equal(subtest, subtest1)
        self.assertEqual(result, True)

    def test_identity_from_other_module(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.identity_ref_value = \
            YdktestTypeIdentity()
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # def test_binary(self):
    #     pass

    # def test_binary_invalid(self):
    #     pass

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        SanityTest.PROVIDER_TYPE = sys.argv.pop()

    suite = unittest.TestLoader().loadTestsFromTestCase(SanityTest)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
