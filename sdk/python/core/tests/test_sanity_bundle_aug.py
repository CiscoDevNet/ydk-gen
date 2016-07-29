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
import unittest

from ydk.services import CRUDService
from ydk.providers import NetconfServiceProvider
from compare import is_equal


class SanityYang(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider(address='127.0.0.1',
                                          username='admin',
                                          password='admin',
                                          protocol='ssh',
                                          port=12022)
        self.crud = CRUDService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        self.crud.delete(self.ncc, ietf_aug_base_1.Cpython())
        self.crud.delete(self.ncc, ietf_aug_base_2.Cpython())

    def tearDown(self):
        self.crud.delete(self.ncc, ietf_aug_base_1.Cpython())
        self.crud.delete(self.ncc, ietf_aug_base_2.Cpython())

    def test_aug_base_1(self):
        cpython = ietf_aug_base_1.Cpython()
        cpython.doc.ydktest_aug_1.aug_one = 'aug one'
        cpython.doc.ydktest_aug_2.aug_two = 'aug two'
        cpython.doc.ydktest_aug_4.aug_four = 'aug four'
        cpython.lib.ydktest_aug_1.ydktest_aug_nested_1.aug_one = 'aug one'
        cpython.lib.ydktest_aug_2.ydktest_aug_nested_2.aug_two = 'aug two'
        cpython.lib.ydktest_aug_4.ydktest_aug_nested_4.aug_four = 'aug four'
        cpython.doc.disutils.four_aug_list.enabled = True

        item1 = cpython.doc.disutils.four_aug_list.Ldata()
        item2 = cpython.doc.disutils.four_aug_list.Ldata()

        item1.name, item1.number = 'one', 1
        item2.name, item1.number = 'two', 2

        self.crud.create(self.ncc, cpython)
        cpython_read = self.crud.read(self.ncc, ietf_aug_base_1.Cpython())

        self.assertEqual(is_equal(cpython, cpython_read), True)

    def test_aug_base_2(self):
        cpython = ietf_aug_base_2.Cpython()
        cpython.tools.aug_four = 'aug four'

        self.crud.create(self.ncc, cpython)
        cpython_read = self.crud.read(self.ncc, ietf_aug_base_2.Cpython())

        self.assertEqual(is_equal(cpython, cpython_read), True)


if __name__ == '__main__':
    # execute if and only if we have augmentation bundle is installed
    import pip
    import sys
    installed_packages = pip.get_installed_distributions()
    installed_packages = {i.key for i in installed_packages}
    augmentation_bundles = {'ydk-models-ietf',
                            'ydk-models-ydktest-aug-ietf-1',
                            'ydk-models-ydktest-aug-ietf-2',
                            'ydk-models-ydktest-aug-ietf-4'}
    if augmentation_bundles.issubset(installed_packages):
        from ydk.models.ietf import ietf_aug_base_1
        from ydk.models.ietf import ietf_aug_base_2
        suite = unittest.TestLoader().loadTestsFromTestCase(SanityYang)
        ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
        sys.exit(ret)
