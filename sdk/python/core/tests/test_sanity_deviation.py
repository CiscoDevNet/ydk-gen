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
from ydk.services import CRUDService
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.providers import NetconfServiceProvider
from ydk.types import Empty, Decimal64
from ydk.errors import YError, YModelError
from ydk.models.ydktest.ydktest_sanity import YdkEnumTest

from test_utils import ParametrizedTestCase
from test_utils import get_device_info


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ncc = NetconfServiceProvider(
            cls.hostname,
            cls.username,
            cls.password,
            12023,
            cls.protocol,
            cls.on_demand,
            cls.common_cache,
            cls.timeout)
        cls.crud = CRUDService()

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def test_int8(self):
        # type changed to int16
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.number8 = 126
        self.crud.create(self.ncc, runner)


        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    def test_int16(self):
        # type changed to int32
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.number16 = 20000
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    def test_int32(self):
        # type changed to int64
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.number32 = -9223372036854775808
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    def test_int64(self):
        # type changed to type uint8
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.number64 = -9223372036854775808
        self.assertRaises(YModelError, self.crud.create, self.ncc, runner)

    def test_uint8(self):
        # changed to type uint16
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.u_number8 = 256
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    def test_uint16(self):
        # changed to type uint32
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.u_number16 = 65536
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    def test_uint32(self):
        # changed to type uint64
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.u_number32 = 18446744073709551615
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    def test_decimal64(self):
        # changed to type string
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.deci64 = 'string'
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    def test_leafref(self):
        # changed to tye decimal64
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.leaf_ref = Decimal64('3.14')
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    def test_string(self):
        # changed to type empty
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.name = Empty()
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

    @unittest.skip('no exception raised during validation')
    def test_boolean(self):
        # changed to type YdkEnumTest
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.bool_value = YdkEnumTest.none
        self.crud.create(self.ncc, runner)

        runner_read = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read)

        self.assertEqual(runner, runner_read)

        runner = ysanity.Runner()
        runner.ytypes.built_in_t.bool_value = False
        self.assertRaises(YModelError, self.crud.update, self.ncc, runner)

    def test_leaflist_max_elements(self):
        # max val changed to 7
        runner = ysanity.Runner()
        runner.ytypes.built_in_t._python_type_validation_enabled = False
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(8)])
        self.assertRaises(YModelError, self.crud.create, self.ncc, runner)

    def test_not_supported_leaf(self):
        # not supported leaf
        runner = ysanity.Runner()
        runner.not_supported_1.not_supported_leaf = 'leaf value'
        self.assertRaises(YModelError, self.crud.create, self.ncc, runner)

    def test_not_supported_container(self):
        # not supported container
        runner = ysanity.Runner()
        runner.not_supported_1.not_supported_1_2.some_leaf = 'some leaf'
        self.assertRaises(YModelError, self.crud.create, self.ncc, runner)

    def test_not_supported_list(self):
        # not supported list
        runner = ysanity.Runner()
        elems = []
        for i in range(5):
            elem = runner.NotSupported2()
            elem.number = i
            elems.append(elem)
        runner.not_supported_2.extend(elems)
        self.assertRaises(YModelError, self.crud.create, self.ncc, runner)

    @unittest.skip('no exception raised during validation')
    def test_leaflist_max_elements(self):
        """This sanity test only tests deviation for max-elements. If min-elements is set
        to value larger than 0, this constraint will be required in all test cases, and will
        fail all other test cases.
        """
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(20)])
        self.assertRaises(YModelError, self.crud.create, self.ncc, runner)


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
