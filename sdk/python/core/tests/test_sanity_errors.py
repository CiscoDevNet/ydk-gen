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

from ydk.errors import YModelError
from ydk.services import CRUDService
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.providers import NetconfServiceProvider

from test_utils import assert_with_error
from test_utils import ParametrizedTestCase
from test_utils import get_device_info


test_int64_invalid_pattern = """"""
test_uint8_invalid_pattern = """Invalid value "-1" in "u_number8" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/u_number8"""
test_uint16_invalid_pattern = """Invalid value "not an uint" in "u_number16" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/u_number16"""
test_uint32_invalid_pattern = """Invalid value "4294967296" in "u_number32" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/u_number32"""
test_uint64_invalid_pattern = """"""
test_ylist_assignment_pattern = ''.join(["Attempt to assign value of '\[<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>, ",
                                         "<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>\]' to YList ldata. ",
                                         "Please use list append or extend method."])


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
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    @assert_with_error("Invalid value 8.5 for 'number8'..*Expected types: 'int'", YModelError)
    def test_int8_invalid(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number8 = 8.5
        self.crud.create(self.ncc, runner)

    @assert_with_error("Invalid value {} for 'number16'..*Expected types: 'int'", YModelError)
    def test_int16_invalid(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number16 = {}
        self.crud.create(self.ncc, runner)

    @assert_with_error("Invalid value \[\]", YModelError)
    def test_int32_invalid(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number32 = []
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_int64_invalid_pattern, YModelError)
    def test_int64_invalid(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number64 = 9223372036854775808
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_uint8_invalid_pattern, YModelError)
    def test_uint8_invalid(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.u_number8 = -1
        self.crud.create(self.ncc, runner)

    @assert_with_error("Invalid value not an uint for 'u_number16'..*Expected types: 'int'", YModelError)
    def test_uint16_invalid(self):
            runner = ysanity.Runner()
            runner.ytypes.built_in_t.u_number16 = 'not an uint'
            self.crud.create(self.ncc, runner)

    @assert_with_error(test_uint32_invalid_pattern, YModelError)
    def test_uint32_invalid(self):
            runner = ysanity.Runner()
            runner.ytypes.built_in_t.u_number32 = 4294967296
            self.crud.create(self.ncc, runner)

    @assert_with_error(test_uint64_invalid_pattern, YModelError)
    def test_uint64_invalid(self):
            runner = ysanity.Runner()
            runner.ytypes.built_in_t.u_number64 = 18446744073709551616
            self.crud.create(self.ncc, runner)

    @assert_with_error("Invalid value", YModelError)
    def test_string_invalid(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.name = ['name_str']
        self.crud.create(self.ncc, runner)

    @assert_with_error("Invalid value 0 for 'emptee'..*Expected types: 'Empty'", YModelError)
    def test_empty_invalid(self):
            runner = ysanity.Runner()
            runner.ytypes.built_in_t.emptee = '0'
            self.crud.create(self.ncc, runner)

    @assert_with_error("Invalid value  for 'bool_value'..*Expected types: 'bool'", YModelError)
    def test_boolean_invalid(self):
            runner = ysanity.Runner()
            runner.ytypes.built_in_t.bool_value = ''
            self.crud.create(self.ncc, runner)

    @assert_with_error(
        "Invalid value not an enum for 'enum_value'..*Expected types: 'ydk.models.ydktest.ydktest_sanity.YdkEnumTest'",
        YModelError)
    def test_enum_invalid(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.enum_value = 'not an enum'
        self.crud.create(self.ncc, runner)

    @assert_with_error("Invalid value", YModelError)
    def test_yleaflist_assignment(self):
            runner = ysanity.Runner()
            runner.ytypes.built_in_t.llstring = ['invalid', 'leaf-list', 'assignment']
            self.crud.create(self.ncc, runner)

    @assert_with_error(test_ylist_assignment_pattern, YModelError)
    def test_ylist_assignment(self):
        runner = ysanity.Runner()
        elems, n = [], 10
        for i in range(n):
            l = ysanity.Runner.OneList.Ldata()
            l.number = i
            l.name = str(i)
            elems.append(l)
        runner.one_list.ldata = elems
        self.crud.create(self.ncc, runner)


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
