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

from ydk.services import CRUDService
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.models.ydktest import ydktest_sanity_types as ysanity_types
from ydk.providers import NetconfServiceProvider, NativeNetconfServiceProvider
from ydk.types import Empty, DELETE, Decimal64
from compare import is_equal
from ydk.errors import YPYError, YPYModelError, YPYServiceError

from ydk.models.ydktest.ydktest_sanity import YdkEnumTestEnum, YdkEnumIntTestEnum


class SanityTest(unittest.TestCase):
    PROVIDER_TYPE = "non-native"

    @classmethod
    def setUpClass(self):
        if SanityTest.PROVIDER_TYPE == "native":
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

    def test_int8_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.number8 = 8.5
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.number8: (INVALID_TYPE, Invalid type: 'float'. Expected type: 'int')"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_int16_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.number16 = {}
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.number16: (INVALID_TYPE, Invalid type: 'dict'. Expected type: 'int')"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_int32_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.number32 = []
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.number32: (INVALID_TYPE, Invalid type: 'list'. Expected type: 'int')"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_int64_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.number64 = 9223372036854775808
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.number64: (INVALID_VALUE, Value is invalid: 9223372036854775808 not in range (-9223372036854775808, 9223372036854775807))"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_uint8_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.u_number8 = -1
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.u_number8: (INVALID_VALUE, Value is invalid: -1 not in range (0, 255))"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_uint16_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.u_number16 = 'not an uint'
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.u_number16: (INVALID_TYPE, Invalid type: 'str'. Expected type: 'int')"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_uint32_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.u_number32 = 4294967296
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.u_number32: (INVALID_VALUE, Value is invalid: 4294967296 not in range (0, 4294967295))"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_uint64_invalid_1(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.u_number64 = -1
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.u_number64: (INVALID_VALUE, Value is invalid: -1 not in range (0, 18446744073709551615L))"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_uint64_invalid_2(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.u_number64 = 18446744073709551616
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.u_number64: (INVALID_VALUE, Value is invalid: 18446744073709551616 not in range (0, 18446744073709551615L))"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_string_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.name = ['name_str']
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.name: (INVALID_TYPE, Invalid type: 'list'. Expected type: 'str')"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_empty_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.emptee = '0'
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.emptee: (INVALID_TYPE, Invalid type: 'str'. Expected type: 'Empty')"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_boolean_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.bool_value = ''
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.bool_value: (INVALID_TYPE, Invalid type: 'str'. Expected type: 'bool')"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_enum_invalid(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.enum_value = 'not an enum'
            self.crud.create(self.ncc, runner)
        except YPYModelError as err:
            expected_msg = "Runner.Ytypes.BuiltInT.enum_value: (INVALID_TYPE, Invalid type: 'str'. Expected type: Enum)"
            self.assertEqual(err.message.strip(), expected_msg)
        else:
            raise Exception('YPYModelError not raised')

    def test_yleaflist_assignment(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.llstring = ['invalid', 'leaf-list', 'assignment']
            self.crud.create(self.ncc, runner)
        except YPYServiceError as err:
            expected_msg = "Assigned object to YLeafList Runner.Ytypes.BuiltInT. Use list append or extend method instead."
            self.assertEqual(err.message.strip(), expected_msg)


    def test_ylist_assignment(self):
        try:
            runner = self._create_runner()
            elems, n = [], 10
            for i in range(n):
                l = ysanity.Runner.OneList.Ldata()
                l.number = i
                l.name = str(i)
                elems.append(l)
            runner.one_list.ldata = elems
            self.crud.create(self.ncc, runner)
        except YPYServiceError as err:
            expected_msg = "Assigned object to YList Runner.OneList. Use list append or extend method instead."
            self.assertEqual(err.message.strip(), expected_msg)

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        SanityTest.PROVIDER_TYPE = sys.argv.pop()
    unittest.main()
