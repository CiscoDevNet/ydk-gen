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
    test_sanity_delete.py
        Unittest for DELETE object.
"""
import ydk.types as ytypes
import unittest

from ydk.services import CRUDService
from ydk.providers import NetconfServiceProvider
from ydk.types import Empty, DELETE, Decimal64
from tests.compare import is_equal
from ydk.errors import YPYError, YPYDataValidationError
from ydk.models.ydktest import ydktest_sanity as ysanity

class SanityYang(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider(
            address='127.0.0.1', username='admin', 
            password='admin', protocol='ssh', port=12022)
        self.crud = CRUDService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)
        print '\nIn method', self._testMethodName + ':'

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def test_delete_object_on_leaf(self):
        # create runner with two leaves, one and two
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        runner_create.two.name = 'two'
        self.crud.create(self.ncc, runner_create)

        runner_read_filter = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read_filter)

        # use DELETE object to remove leaf one
        runner_delete = runner_read
        runner_delete.one.name = DELETE()
        self.crud.update(self.ncc, runner_delete)

        # manually create remaining runner with leaf two
        runner_read = self.crud.read(self.ncc, runner_read_filter)
        runner_left = runner_create
        runner_left.one.name = None

        self.assertEqual(is_equal(runner_read, runner_left), True)

    @unittest.skip('DELETE object for leaf-list')
    def test_delete_object_on_leaflist(self):
        # create runner with a leaf and a leaflist
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        runner_create.ytypes.built_in_t.llstring = [str(i) for i in range(5)]
        self.crud.create(self.ncc, runner_create)

        runner_read_filter = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read_filter)

        # user DELETE object to remove leaflist
        runner_delete = runner_read
        runner_delete.ytypes.built_in_t.llstring = DELETE()
        self.crud.update(self.ncc, runner_delete)

        # manually create remaining runner with leaf one
        runner_read = self.crud.read(self.ncc, runner_read_filter)
        runner_left = runner_create
        del runner_left.ytypes.built_in_t.llstring[:]

        self.assertEqual(is_equal(runner_read, runner_left), True)

    def test_delete_operation_on_container(self):
        # create runner with a container
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        runner_create.two.name = 'two'
        self.crud.create(self.ncc, runner_create)

        runner_read_filter = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read_filter)

        # delete contianer two
        self.crud.delete(self.ncc, runner_read.two)

        runner_read = self.crud.read(self.ncc, runner_read_filter)
        runner_left = runner_create
        runner_left.two.name = None

        self.assertEqual(is_equal(runner_read, runner_left), True)

    def test_delete_operation_on_list(self):
        # create runner with a container and a list
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'

        elem1 = ysanity.Runner.OneList.Ldata()
        elem2 = ysanity.Runner.OneList.Ldata()
        elem1.number = 1
        elem2.name = 'foo'
        elem2.number = 1
        elem2.name = 'bar'
        runner_create.one_list.ldata.extend([elem1, elem2])

        self.crud.create(self.ncc, runner_create)

        runner_read_filter = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read_filter)

        self.crud.delete(self.ncc, runner_read.one_list.ldata)

        runner_read = self.crud.read(self.ncc, runner_read_filter)
        runner_left = runner_create
        del runner_left.one_list.ldata[:]

        self.assertEqual(is_equal(runner_read, runner_left), True)


if __name__ == '__main__':
    unittest.main()
