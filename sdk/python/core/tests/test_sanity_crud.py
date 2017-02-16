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
"""
    test_sanity_bundle_augmentation.py
        Unittest for bundle augmentation.
"""
from __future__ import absolute_import
import unittest
from ydk_.services import CrudService
from ydk_.providers import NetconfServiceProvider
from ydk_.types import EncodingFormat
import ydktest_sanity


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider('127.0.0.1',
                                          'admin',
                                          'admin',
                                          12022)
        self.crud = CrudService()

    @classmethod
    def tearDownClass(self):
        pass

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_subtest(self):
        subtest = ydktest_sanity.SubTest()
        subtest.one_aug.name = 'name'
        subtest.one_aug.number = 123

        self.crud.create(self.ncc, subtest)


if __name__ == '__main__':
    import sys

    suite = unittest.TestLoader().loadTestsFromTestCase(SanityTest)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
