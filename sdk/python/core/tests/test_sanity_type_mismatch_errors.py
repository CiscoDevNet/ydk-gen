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

import sys
import unittest

from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.models.ydktest import ydktest_sanity_types as ytypes
from ydk.providers import NetconfServiceProvider
from ydk.services import CRUDService
from ydk.errors import YModelError

from test_utils import assert_with_error
from test_utils import ParametrizedTestCase
from test_utils import get_device_info


test_invalid_class_assignment_int_pattern = "Invalid value 1' in 'ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*'"
test_invalid_class_assignment_str_pattern = "Invalid value haha' in 'ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*'"
test_invalid_class_assignment_identity_pattern = "Invalid value ydktest-sanity-types:another-one' in 'ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*'"
test_invalid_class_assignment_enum_pattern = "Invalid value none' in 'ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*'"
test_invalid_class_assignment_ylist_pattern = "Invalid value \[<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>\]' in 'ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*'"
test_invalid_class_assignment_yleaflist_pattern = "Invalid value \['0', '1', '2', '3', '4'\]' in 'ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*'"
# test_invalid_class_assignment_yleaflist_pattern = "Invalid value YLeafList\('llstring', \[0, 1, 2, 3, 4\]\)' in '<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*One object at [0-9a-z]+>'"
test_invalid_list_assignment_int_pattern = "Attempt to assign value of '1' to YList ldata. Please use list append or extend method."
test_invalid_list_assignment_entity_pattern = "Attempt to assign value of 'ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*' to YList ldata. Please use list append or extend method."
test_invalid_list_assignment_llist_pattern = "Attempt to assign value of '\['0', '1', '2', '3', '4'\]' to YList ldata. Please use list append or extend method."
test_invalid_llist_assignment_int_pattern = "Invalid value 1' in '\[\]'"
test_invalid_llist_assignment_list_pattern = "Invalid value Entities in YList:"


class SanityYang(unittest.TestCase):

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

    @assert_with_error(test_invalid_class_assignment_int_pattern, YModelError)
    def test_invalid_class_assignment_int(self):
        runner = ysanity.Runner()
        runner.ydktest_sanity_one = 1
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_str_pattern, YModelError)
    def test_invalid_class_assignment_str(self):
        runner = ysanity.Runner()
        runner.ydktest_sanity_one = "haha"
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_identity_pattern, YModelError)
    def test_invalid_class_assignment_identity(self):
        runner = ysanity.Runner()
        runner.ydktest_sanity_one = ytypes.AnotherOne()
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_enum_pattern, YModelError)
    def test_invalid_class_assignment_enum(self):
        runner = ysanity.Runner()
        runner.ydktest_sanity_one = ysanity.YdkEnumTest.none
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_yleaflist_pattern, YModelError)
    def test_invalid_class_assignment_yleaflist(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(5)])
        runner.ydktest_sanity_one = runner.ytypes.built_in_t.llstring

    @assert_with_error(test_invalid_list_assignment_int_pattern, YModelError)
    def test_invalid_list_assignment_int(self):
        runner = ysanity.Runner()
        runner.one_list.ldata = 1
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_list_assignment_entity_pattern, YModelError)
    def test_invalid_list_assignment_entity(self):
        runner = ysanity.Runner()
        runner.one_list.ldata = runner.ydktest_sanity_one
        self.crud.crud(self.ncc, runner)

    @assert_with_error(test_invalid_list_assignment_llist_pattern, YModelError)
    def test_invalid_list_assignment_llist(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(5)])
        runner.one_list.ldata = runner.ytypes.built_in_t.llstring
        self.crud.crud(self.ncc, runner)

    @assert_with_error(test_invalid_llist_assignment_list_pattern, YModelError)
    def test_invalid_llist_assignment_list(self):
        runner = ysanity.Runner()
        elem = ysanity.Runner.OneList.Ldata()
        elem.number, elem.name = 1, '1'
        runner.one_list.ldata.append(elem)
        runner.ytypes.built_in_t.llstring = runner.one_list.ldata
        self.crud.create(self.ncc, runner)

    @assert_with_error("Attempt to assign unknown attribute 'invalid_attribute' to 'Runner'", YModelError)
    def test_invalid_attribute(self):
        runner = ysanity.Runner()
        runner.invalid_attribute = 'xyz'

    @assert_with_error(
        "Invalid value not connected for 'enum_int_value'..*Expected types: 'ydk.models.ydktest.ydktest_sanity.YdkEnumIntTest' or 'int'",
        YModelError)
    def test_union_enum(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.enum_int_value = runner.ytypes.built_in_t.Status.not_connected

    @assert_with_error("Invalid value 1 for 'number8'..*Expected types: 'int'", YModelError)
    def test_int(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number8 = "1"

    @assert_with_error("Invalid value -1 for 'name'..*Expected types: 'str'", YModelError)
    def test_str(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.name = -1

    @assert_with_error("Invalid value not connected for 'enum_value'..*Expected types: 'ydk.models.ydktest.ydktest_sanity.YdkEnumTest'", YModelError)
    def test_str_2(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.enum_value = runner.ytypes.built_in_t.Status.not_connected

    @assert_with_error("Invalid value -1 for 'deci64'..*Expected types: 'Decimal64'", YModelError)
    def test_deci64(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.deci64 = -1

    @assert_with_error("Invalid value 1 for 'bool_value'..*Expected types: 'bool'", YModelError)
    def test_bool(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.bool_value = 1


if __name__ == '__main__':
    device, non_demand, common_cache, timeout = get_device_info()

    suite = unittest.TestSuite()
    suite.addTest(ParametrizedTestCase.parametrize(
        SanityYang,
        device=device,
        non_demand=non_demand,
        common_cache=common_cache,
        timeout=timeout))
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
