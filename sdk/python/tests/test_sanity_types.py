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

import ydk.types as ytypes
import unittest
# from errors import YPYDataValidationError 

from ydk.services import CRUDService
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.models.ydktest import ydktest_sanity_types as ysanity_types
from ydk.models.ydktest import ydktest_types as y_types
from ydk.providers import NetconfServiceProvider
from ydk.types import Empty, DELETE, Decimal64
from tests.compare import is_equal
from ydk.errors import YPYError, YPYDataValidationError

from ydk.models.ydktest.ydktest_sanity import YdkEnumTestEnum, YdkEnumIntTestEnum

class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider(
            address='127.0.0.1',
            username='admin',
            password='admin',
            protocol='ssh',
            port=12022)
        self.crud = CRUDService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        print "\nIn method", self._testMethodName
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def _create_runner(self):
        runner = ysanity.Runner()
        runner.ytypes = runner.Ytypes()
        runner.ytypes.built_in_t = runner.ytypes.BuiltInT()

        return runner

    # def test_invalids(self):
    #     runner = self._create_runner()
    #     runner.ytypes.built_in_t.name = []
    #     runner.ytypes.built_in_t.u_number8 = -1
    #     runner.ytypes.built_in_t.enum_value = ''

    #     self.crud.create(self.ncc, runner)
    #     with self.assertRaises(YPYDataValidationError):
    #     self.crud.create(self.ncc, runner)

    def test_int8(self):
        # Create Runner
        runner = self._create_runner()
        runner.ytypes.built_in_t.number8 = 0
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('skip till validation in place')
    def test_int8_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number8 = 8.5
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # invalid object is not equal
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_int16(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number16 = 126
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('skip till validation in place')
    def test_int16_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number16 = {}
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_int32(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number32 = 200000
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('skip till validation in place')
    def test_int32_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number32 = []
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_bits(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.bits_value['disable-nagle'] = True
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_int64(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number64 = -9223372036854775808
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('skip till validation in place')
    def test_int64_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number64 = 9223372036854775808
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_uint8(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number8 = 0
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('skip till validation in place')
    def test_uint8_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number8 = -1
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_uint16(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number16 = 65535
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('skip till validation in place')
    def test_unint16_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number16 = 'not an uint'
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_uint32(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number32 = 5927
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('skip till validation in place')
    def test_unint32_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number32 = 4294967296
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_decimal64(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.deci64 = Decimal64('3.14')
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('Revisit implementation')
    def test_uint64(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number64 = 18446744073709551615
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    @unittest.skip('skip till validation in place')
    def test_uint64_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number64 = 18446744073709551616
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_string(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.name = 'name_str'
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # but invalid input is sent to the device
    def test_string_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.name = ['name_str']
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)
        # self.crud.create(self.ncc, runner)

        # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_empty(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.emptee = Empty()
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

        # explicit DELETE not support at the moment
        # runner1.ytypes.built_in_t.emptee = DELETE()
        # self.crud.update(self.ncc, runner1)

        # runner2 = self.crud.read(self.ncc, self._create_runner())

        # self.assertEqual(runner2.ytypes.built_in_t.emptee, None)


    def test_empty_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.emptee = 0
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # Read into Runner2
        # runner1 = ysanity.Runner()
        # self.assertRaises(YPYDataValidationError,
            # self.crud.create, self.ncc, runner)
        # runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_boolean(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.bool_value = True
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

        runner = self._create_runner()
        runner.ytypes.built_in_t.bool_value = False
        self.crud.update(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)


    @unittest.skip('boolean type will convert empty string to false')
    def test_boolean_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.bool_value = ''
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, True)

    def test_enum(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.enum_value = YdkEnumTestEnum.NONE
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_enum_invalid(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.enum_value = 'not an enum'
        # self.crud.create(self.ncc, runner)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

        # OLD
        # with self.assertRaises(YPYError) as e:
        #     self.crud.create(self.ncc, runner)

        # # Read into Runner2
        # runner1 = ysanity.Runner()
        # runner1 = self.crud.read(self.ncc, runner1)

        # # # Compare runners
        # result = is_equal(runner, runner1)
        # self.assertEqual(result, False)

    def test_union(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.younion = YdkEnumTestEnum.NONE
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union_enum(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.enum_int_value = YdkEnumIntTestEnum.ANY
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union_int(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.enum_int_value = 2
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union_recursive(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.younion_recursive = 18
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
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
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_union_complex_list(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.younion_list.append("123:45")
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    def test_identityref(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.identity_ref_value = \
            ysanity.ChildIdentity_Identity()
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)


    def test_leaflist_unique(self):
        runner = self._create_runner()
        with self.assertRaises(YPYDataValidationError):
            runner.ytypes.built_in_t.llstring.extend([None for i in range(3)])

    def test_list_max_elements(self):
        runner = self._create_runner()
        elems = []
        n = 10
        for i in range(n):
            l = ysanity.Runner.OneList.Ldata()
            l.number = i
            l.name = str(i)
            elems.append(l)
        runner.one_list.ldata.extend(elems)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

    def test_submodule(self):
        subtest = ysanity.SubTest()
        subtest.one_aug.name = 'test'
        subtest.one_aug.number = 3

        res = self.crud.create(self.ncc, subtest)

        subtest1 = self.crud.read(self.ncc, ysanity.SubTest())

        # Compare runners
        result = is_equal(subtest, subtest1)
        self.assertEqual(result, True)

    def test_identity_from_other_module(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.identity_ref_value = \
            ysanity_types.YdktestType_Identity()
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # def test_binary(self):
    #     pass

    # def test_binary_invalid(self):
    #     pass

    # def test_bits(self):
    #     pass

    # def test_bits_invalid(self):
    #     pass

    # def test_union(self):
    #     pass

    # def test_union_invalid(self):
    #     pass



if __name__ == '__main__':
    unittest.main()
