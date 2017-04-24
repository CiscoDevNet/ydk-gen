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
from ydk.providers import NetconfServiceProvider
from ydk.services import CRUDService
from ydk.errors import YPYModelError
from test_utils import assert_with_error


test_invalid_class_assignment_int_pattern = "Invalid value '1' in '<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*One object at [0-9a-z]+>'"
test_invalid_class_assignment_str_pattern = "Invalid value 'haha' in '<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*One object at [0-9a-z]+>'"
test_invalid_class_assignment_identity_pattern = "Invalid value '<ydk.models.ydktest.ydktest_sanity_types.AnotherOne object at [0-9a-z]+>' in '<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*One object at [0-9a-z]+>'"
test_invalid_class_assignment_enum_pattern = "Invalid value 'ydk.types.Enum.YLeaf\(none\)' in '<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*One object at [0-9a-z]+>'"
test_invalid_class_assignment_ylist_pattern = "Invalid value '\[<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>\]' in '<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*One object at [0-9a-z]+>"
test_invalid_class_assignment_yleaflist_pattern = "Invalid value 'YLeafList\('llstring', \[0, 1, 2, 3, 4\]\)' in '<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*One object at [0-9a-z]+>'"
test_invalid_list_assignment_int_pattern = "Attempt to assign value of '1' to YList ldata. Please use list append or extend method."
test_invalid_list_assignment_entity_pattern = "Attempt to assign value of '<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*One object at [0-9a-z]+>' to YList ldata. Please use list append or extend method."
test_invalid_list_assignment_llist_pattern = "Attempt to assign value of 'YLeafList\('llstring', \[0, 1, 2, 3, 4\]\)' to YList ldata. Please use list append or extend method."
test_invalid_llist_assignment_int_pattern = "Invalid value '1' in 'llstring'"
test_invalid_llist_assignment_list_pattern = "Invalid value '\[<ydk.models.ydktest.ydktest_sanity.[a-zA-Z\.]*Ldata object at [0-9a-z]+>\]' in 'llstring'"


class SanityYang(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 12022)
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

    @assert_with_error(test_invalid_class_assignment_int_pattern, YPYModelError)
    def test_invalid_class_assignment_int(self):
        runner = ysanity.Runner()
        runner.one = 1
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_str_pattern, YPYModelError)
    def test_invalid_class_assignment_str(self):
        runner = ysanity.Runner()
        runner.one = "haha"
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_identity_pattern, YPYModelError)
    def test_invalid_class_assignment_identity(self):
        runner = ysanity.Runner()
        runner.one = ytypes.AnotherOne()
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_enum_pattern, YPYModelError)
    def test_invalid_class_assignment_enum(self):
        runner = ysanity.Runner()
        runner.one = ysanity.YdkEnumTest.none
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_ylist_pattern, YPYModelError)
    def test_invalid_class_assignment_ylist(self):
        runner = ysanity.Runner()
        elem = ysanity.Runner.OneList.Ldata()
        elem.number, elem.name = 1, '1'
        runner.one_list.ldata.append(elem)
        runner.one = runner.one_list.ldata
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_class_assignment_yleaflist_pattern, YPYModelError)
    def test_invalid_class_assignment_yleaflist(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(5)])
        runner.one = runner.ytypes.built_in_t.llstring

    @assert_with_error(test_invalid_list_assignment_int_pattern, YPYModelError)
    def test_invalid_list_assignment_int(self):
        runner = ysanity.Runner()
        runner.one_list.ldata = 1
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_list_assignment_entity_pattern, YPYModelError)
    def test_invalid_list_assignment_entity(self):
        runner = ysanity.Runner()
        runner.one_list.ldata = runner.one
        self.crud.crud(self.ncc, runner)

    @assert_with_error(test_invalid_list_assignment_llist_pattern, YPYModelError)
    def test_invalid_list_assignment_llist(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring.extend([str(i) for i in range(5)])
        runner.one_list.ldata = runner.ytypes.built_in_t.llstring
        self.crud.crud(self.ncc, runner)

    @assert_with_error(test_invalid_llist_assignment_int_pattern, YPYModelError)
    def test_invalid_llist_assignment_int(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.llstring = 1
        self.crud.create(self.ncc, runner)

    @assert_with_error(test_invalid_llist_assignment_list_pattern, YPYModelError)
    def test_invalid_llist_assignment_list(self):
        runner = ysanity.Runner()
        elem = ysanity.Runner.OneList.Ldata()
        elem.number, elem.name = 1, '1'
        runner.one_list.ldata.append(elem)
        runner.ytypes.built_in_t.llstring = runner.one_list.ldata
        self.crud.create(self.ncc, runner)


if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        SanityYang.PROVIDER_TYPE = sys.argv.pop()

    suite = unittest.TestLoader().loadTestsFromTestCase(SanityYang)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
