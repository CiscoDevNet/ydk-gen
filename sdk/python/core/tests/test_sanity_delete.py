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
"""
from __future__ import absolute_import
import unittest

from ydk.services import CRUDService
from ydk.providers import NetconfServiceProvider
from ydk.errors import YPYError
from ydk.filters import YFilter
from ydk.models.ydktest import ydktest_sanity as ysanity


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

    def read_from_empty_filter(self):
        empty_runner = ysanity.Runner()
        return self.crud.read(self.ncc, empty_runner)

    def get_nested_object(self):
        # return nested object with selected list elements
        runner_create = ysanity.Runner()
        e_1 = ysanity.Runner.InbtwList.Ldata()
        e_2 = ysanity.Runner.InbtwList.Ldata()
        e_1.number = 11
        e_1.name = 'runner:inbtwlist:[11]:name'
        e_1.subc.number = 111
        e_1.subc.name = 'runner:inbtwlist:[11]:subc:name'
        e_2.number = 12
        e_2.name = 'runner:inbtwlist:[12]:name'
        e_2.subc.number = 121
        e_2.subc.name = 'runner:inbtwlist:[12]:name'
        e_11 = e_1.subc.SubcSubl1()
        e_11.number = 111
        e_11.name = 'runner:inbtwlist:[11]:subc:subcsubl1[111]:name'
        e_12 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_12.number = 112
        e_12.name = 'runner:inbtwlist:[11]:subc:subcsubl1[112]:name'
        e_1.subc.subc_subl1.extend([e_11, e_12])
        e_21 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_21.number = 121
        e_21.name = 'runner:inbtwlist:[12]:subc:subcsubl1[121]:name'
        e_22 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_22.number = 122
        e_22.name = 'runner:inbtwlist:[12]:subc:subcsubl1[122]:name'
        e_2.subc.subc_subl1.extend([e_21, e_22])
        runner_create.inbtw_list.ldata.extend([e_1, e_2])

        return runner_create, e_2, e_22

    def test_delete_object_on_leaf(self):
        # create runner with two leaves, one and two
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        runner_create.two.name = 'two'
        self.crud.create(self.ncc, runner_create)

        runner_read_filter = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read_filter)

        # use DELETE object to remove leaf one
        runner_update = runner_read
        runner_update.one.name.operation = YFilter.delete
        self.crud.update(self.ncc, runner_update)

        # manually create remaining runner with leaf two
        runner_read = self.crud.read(self.ncc, runner_read_filter)
        runner_compare = ysanity.Runner()
        runner_compare.two.name = 'two'

        self.assertEqual(runner_compare.one.name, runner_read.one.name)
        self.assertEqual(runner_compare.two.name, runner_read.two.name)

    def test_delete_on_leaflist_slice(self):
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        runner_create.ytypes.built_in_t.llstring.extend([str(i) for i in range(5)])

        self.crud.create(self.ncc, runner_create)

        runner_update = ysanity.Runner()
        # specify the leaflist value to be deleted
        runner_update.ytypes.built_in_t.llstring.append('0')
        runner_update.ytypes.built_in_t.llstring.append('3')
        # set operation
        runner_update.ytypes.built_in_t.llstring.operation = YFilter.delete

        self.crud.update(self.ncc, runner_update)
        runner_read = self.read_from_empty_filter()

        self.assertEqual(runner_create.one.name, runner_read.one.name)
        self.assertEqual(len(runner_read.ytypes.built_in_t.llstring), 3)

    def test_delete_on_leaflist(self):
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        runner_create.ytypes.built_in_t.llstring.extend(['0', '1', '2', '3', '4'])

        self.crud.create(self.ncc, runner_create)

        runner_update = ysanity.Runner()
        runner_update.ytypes.built_in_t.llstring.append('3')
        runner_update.ytypes.built_in_t.llstring.operation = YFilter.delete

        self.crud.update(self.ncc, runner_update)
        runner_read = self.read_from_empty_filter()

        runner_compare = ysanity.Runner()
        runner_compare.ytypes.built_in_t.llstring.extend(['0', '1', '2', '4'])

        self.assertEqual(runner_read.ytypes.built_in_t.llstring, runner_compare.ytypes.built_in_t.llstring)

    def test_delete_on_list_with_identitykey(self):
        runner = ysanity.Runner()
        a1 = ysanity.Runner.OneList.IdentityList()
        a1.config.id = ysanity.ChildIdentity()
        a1.id_ref =  a1.config.id.get()
        runner.one_list.identity_list.append(a1)
        self.crud.create(self.ncc, runner)

        empty_runner = ysanity.Runner()

        runner_update = ysanity.Runner()
        k = ysanity.Runner.OneList.IdentityList()
        k.config.id = ysanity.ChildIdentity()
        k.id_ref = k.config.id.get()
        k.operation = YFilter.delete
        runner_update.one_list.identity_list.append(k)

        self.crud.update(self.ncc, runner_update)

        runner_read = self.crud.read(self.ncc, empty_runner)
        self.assertEqual(runner_read, None)

    def test_delete_on_container(self):
        # create runner with a container
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        runner_create.two.name = 'two'
        self.crud.create(self.ncc, runner_create)

        runner_read_filter = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_read_filter)

        # delete contianer two
        runner_update = ysanity.Runner()
        runner_update.two.operation = YFilter.delete
        self.crud.update(self.ncc, runner_update)

        runner_read = self.crud.read(self.ncc, runner_read_filter)
        runner_compare = ysanity.Runner()
        runner_compare.one.name = 'one'

        self.assertEqual(runner_compare.one.name, runner_read.one.name)

    def test_delete_on_nested_list(self):
        runner_create, _, e_22 = self.get_nested_object()
        self.crud.create(self.ncc, runner_create)

        runner_read = self.read_from_empty_filter()

        runner_read.inbtw_list.ldata[1].subc.subc_subl1.operation = YFilter.delete
        self.crud.update(self.ncc, runner_read)

        runner_read = self.read_from_empty_filter()

        self.assertEqual(len(runner_read.inbtw_list.ldata[1].subc.subc_subl1), 0)

    def test_delete_on_nested_list(self):
        runner_create, _, e_22 = self.get_nested_object()
        self.crud.create(self.ncc, runner_create)

        runner_update = self.read_from_empty_filter()
        runner_update.inbtw_list.ldata[1].subc.subc_subl1[1].operation = YFilter.delete
        self.crud.update(self.ncc, runner_update)

        # get object after a crud delete operation
        runner_read = self.read_from_empty_filter()
        self.assertEqual(len(runner_read.inbtw_list.ldata[1].subc.subc_subl1), 1)
        self.assertEqual(runner_read.inbtw_list.ldata[1].subc.subc_subl1[0].name.get(), 'runner:inbtwlist:[12]:subc:subcsubl1[121]:name')

    def test_delete_on_list_element(self):
        runner_create, e_2, _ = self.get_nested_object()
        self.crud.create(self.ncc, runner_create)

        runner_read = self.read_from_empty_filter()

        runner_update = runner_create
        runner_update.inbtw_list.ldata[1].operation = YFilter.delete

        self.crud.update(self.ncc, runner_update)
        runner_read = self.read_from_empty_filter()

        self.assertEqual(len(runner_read.inbtw_list.ldata), 1)
        self.assertEqual(runner_read.inbtw_list.ldata[0].name.get(), 'runner:inbtwlist:[11]:name')

    def test_delete_on_list_elements(self):
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        foo = ysanity.Runner.OneList.Ldata()
        bar = ysanity.Runner.OneList.Ldata()
        baz = ysanity.Runner.OneList.Ldata()
        foo.number = 1
        foo.name = 'foo'
        bar.number = 2
        bar.name = 'bar'
        baz.number = 3
        baz.name = 'baz'
        runner_create.one_list.ldata.extend([foo, bar, baz])
        self.crud.create(self.ncc, runner_create)

        runner_update = self.read_from_empty_filter()
        runner_update.one_list.ldata[1].operation = YFilter.delete
        runner_update.one_list.ldata[2].operation = YFilter.delete
        self.crud.update(self.ncc, runner_update)

        # read after a crud delete operation
        runner_read = self.read_from_empty_filter()
        self.assertEqual(len(runner_read.one_list.ldata), 1)
        self.assertEqual(runner_read.one_list.ldata[0].name.get(), 'foo')

    def test_delete_on_list(self):
        runner_create = ysanity.Runner()
        runner_create.one.name = 'one'
        foo = ysanity.Runner.OneList.Ldata()
        bar = ysanity.Runner.OneList.Ldata()
        foo.number = 1
        foo.name = 'foo'
        bar.number = 2
        bar.name = 'bar'
        runner_create.one_list.ldata.extend([foo, bar])
        self.crud.create(self.ncc, runner_create)

        runner_update = self.read_from_empty_filter()
        runner_update.one_list.ldata.operation = YFilter.delete
        self.crud.update(self.ncc, runner_update)

        runner_read = self.read_from_empty_filter()
        self.assertEqual(len(runner_read.one_list.ldata), 0)


if __name__ == '__main__':
    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(SanityYang)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
