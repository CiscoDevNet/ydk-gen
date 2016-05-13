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
from ydk.providers import NetconfServiceProvider
from ydk.types import Empty, DELETE, Decimal64
from tests.compare import is_equal
from ydk.errors import YPYError, YPYDataValidationError

from pdb import set_trace as bp
from ydk.models.ydktest.ydktest_sanity import YdkEnumTestEnum

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

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def _create_runner(self):
        runner = ysanity.Runner()
        runner.ytypes = runner.Ytypes()
        runner.ytypes.built_in_t = runner.ytypes.BuiltInT()

        return runner

    # changed to type int16
    def test_int8(self):
        # Create Runner
        runner = self._create_runner()
        runner.ytypes.built_in_t.number8 = 126
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to type int32
    def test_int16(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number16 = 20000
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to type int64
    def test_int32(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number32 = -9223372036854775808
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to type uint8
    def test_int64(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.number64 = -9223372036854775808
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

    # changed to type uint16
    def test_uint8(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number8 = 256
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to type uint32
    def test_uint16(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number16 = 65536
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to type uint64
    def test_uint32(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.u_number32 = 4294967296
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to type string
    def test_decimal64(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.deci64 = 'string'
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to tye decimal64
    def test_leafref(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.leaf_ref = Decimal64('3.14')
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to type empty
    def test_string(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.name = Empty()
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

    # changed to type YdkEnumTestEnum
    def test_boolean(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.bool_value = YdkEnumTestEnum.NONE
        self.crud.create(self.ncc, runner)

        # Read into Runner2
        runner1 = ysanity.Runner()
        runner1 = self.crud.read(self.ncc, runner1)

        # Compare runners
        result = is_equal(runner, runner1)
        self.assertEqual(result, True)

        runner = self._create_runner()
        runner.ytypes.built_in_t.bool_value = False
        self.assertRaises(YPYDataValidationError,
            self.crud.update, self.ncc, runner)


    # max val changed to 7
    def test_leaflist_max_elements(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(8)])
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

    # not supported leaf
    def test_not_supported_leaf(self):
        runner = self._create_runner()
        runner.not_supported_1.not_supported_leaf = 'leaf value'
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

    # not supported container
    def test_not_supported_container(self):
        runner = self._create_runner()
        runner.not_supported_1.not_supported_1_2.some_leaf = 'some leaf'
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

    # not supported list
    def test_not_supported_list(self):
        runner = self._create_runner()
        elems = []
        for i in range(5):
            elems.append(runner.NotSupported2())
        runner.not_supported_2.extend(elems)
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)

    # Will only test max-elements. If min-elements is set, then this
    # constraint is required for every READ/UPDATE operation. So it will fail all other cases.
    def test_leaflist_max_elements(self):
        runner = self._create_runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(20)])
        self.assertRaises(YPYDataValidationError,
            self.crud.create, self.ncc, runner)


if __name__ == '__main__':
    unittest.main()
