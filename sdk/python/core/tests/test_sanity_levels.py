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

"""test_sanity_nested_containers.py
sanity test for ydktest-sanity.yang
"""
from __future__ import absolute_import

import unittest
from compare import is_equal

from ydk.types import Empty
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.providers import NetconfServiceProvider, NativeNetconfServiceProvider
from ydk.services import CRUDService


class SanityYang(unittest.TestCase):
    PROVIDER_TYPE = "non-native"

    @classmethod
    def setUpClass(self):
        if SanityYang.PROVIDER_TYPE == "native":
            self.ncc = NativeNetconfServiceProvider(address='127.0.0.1',
                                                    username='admin',
                                                    password='admin',
                                                    protocol='ssh',
                                                    port=12022)
        else:
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
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def test_one_level_pos(self):
        # READ
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.one.number, r_1.one.name = 1, 'runner:one:name'
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.one.number, r_1.one.name = 10, 'runner/one/name'
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)

    def test_two_level_pos(self):
        # READ
        r_1 = ysanity.Runner()
        r_1.two.number, r_1.two.name, r_1.two.sub1.number = 2, 'runner:two:name', 21
        self.crud.create(self.ncc, r_1)
        r_2 = ysanity.Runner()
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.two.number, r_1.two.name, r_1.two.sub1.number = 20, 'runner/two/name', 210
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)

    def test_three_level_pos(self):
        # READ
        r_1 = ysanity.Runner()
        r_1.three.number, r_1.three.name, \
            r_1.three.sub1.number, r_1.three.sub1.sub2.number = 3, 'runner:three:name', 31, 311
        self.crud.create(self.ncc, r_1)
        r_2 = ysanity.Runner()
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.three.number, r_1.three.name, \
            r_1.three.sub1.number, r_1.three.sub1.sub2.number = 30, 'runner/three/name', 310, 3110
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)


    def test_onelist_neg_dupkey(self):
        # netsim/enxr not complaining
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1, e_2 = ysanity.Runner.OneList.Ldata(), ysanity.Runner.OneList.Ldata()
        e_1.number = 1
        e_2.name = 'foo'
        e_2.number = 1
        e_2.name = 'bar'
        r_1.one_list.ldata.extend([e_1, e_2])
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)

    def test_onelist_neg_update_key_nonexist(self):
        # will create a nonexist elem
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1, e_2 = ysanity.Runner.OneList.Ldata(), ysanity.Runner.OneList.Ldata()
        e_1.number = 1
        e_1.name = 'runner:onelist:ldata['+str(e_1.number)+']:name'
        r_1.one_list.ldata.extend([e_1])
        self.crud.create(self.ncc, r_1)
        r_1.one_list.ldata[0].number = 2
        r_1.one_list.ldata[0].name = '2'
        # assuming update on nonexist key will raise Exception
        with self.assertRaises(Exception):
            self.crud.update(self.ncc, r_1.one_list.ldata[0])


    def test_onelsit_pos(self):
        # READ
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1, e_2 = ysanity.Runner.OneList.Ldata(), ysanity.Runner.OneList.Ldata()
        e_1.number = 1
        e_1.name = 'runner:onelist:ldata['+str(e_1.number)+']:name'
        e_2.number = 2
        e_2.name = 'runner:onelist:ldata['+str(e_2.number)+']:name'
        r_1.one_list.ldata.extend([e_1, e_2])
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1, e_2 = ysanity.Runner.OneList.Ldata(), ysanity.Runner.OneList.Ldata()
        e_1.number = 1
        e_1.name = 'runner:onelist:ldata['+str(e_1.number)+']:name'
        e_2.number = 2
        e_2.name = 'runner:onelist:ldata['+str(e_2.number)+']:name'
        r_1.one_list.ldata.extend([e_1, e_2])
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)


    def test_twolist_pos(self):
        # READ
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1, e_2 = ysanity.Runner.TwoList.Ldata(), ysanity.Runner.TwoList.Ldata()
        e_11, e_12 = ysanity.Runner.TwoList.Ldata.Subl1(), ysanity.Runner.TwoList.Ldata.Subl1()
        e_1.number = 21
        e_1.name = 'runner:twolist:ldata['+str(e_1.number)+']:name'
        e_11.number = 211
        e_11.name = 'runner:twolist:ldata['+str(e_1.number)+']:subl1['+str(e_11.number)+']:name'
        e_12.number = 212
        e_12.name = 'runner:twolist:ldata['+str(e_1.number)+']:subl1['+str(e_12.number)+']:name'
        e_1.subl1.extend([e_11, e_12])
        e_21, e_22 = ysanity.Runner.TwoList.Ldata.Subl1(), ysanity.Runner.TwoList.Ldata.Subl1()
        e_2.number = 22
        e_2.name = 'runner:twolist:ldata['+str(e_2.number)+']:name'
        e_21.number = 221
        e_21.name = 'runner:twolist:ldata['+str(e_2.number)+']:subl1['+str(e_21.number)+']:name'
        e_22.number = 222
        e_22.name = 'runner:twolist:ldata['+str(e_2.number)+']:subl1['+str(e_22.number)+']:name'
        e_2.subl1.extend([e_21, e_22])
        r_1.two_list.ldata.extend([e_1, e_2])
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1, e_2 = ysanity.Runner.TwoList.Ldata(), ysanity.Runner.TwoList.Ldata()
        e_11, e_12 = ysanity.Runner.TwoList.Ldata.Subl1(), ysanity.Runner.TwoList.Ldata.Subl1()
        e_1.number = 21
        e_1.name = 'runner/twolist/ldata['+str(e_1.number)+']/name'
        e_11.number = 211
        e_11.name = 'runner/twolist/ldata['+str(e_1.number)+']/subl1['+str(e_11.number)+']/name'
        e_12.number = 212
        e_12.name = 'runner/twolist/ldata['+str(e_1.number)+']/subl1['+str(e_12.number)+']/name'
        e_1.subl1.extend([e_11, e_12])
        e_21, e_22 = ysanity.Runner.TwoList.Ldata.Subl1(), ysanity.Runner.TwoList.Ldata.Subl1()
        e_2.number = 22
        e_2.name = 'runner/twolist/ldata['+str(e_2.number)+']/name'
        e_21.number = 221
        e_21.name = 'runner/twolist/ldata['+str(e_2.number)+']/subl1['+str(e_21.number)+']/name'
        e_22.number = 222
        e_22.name = 'runner/twolist/ldata['+str(e_2.number)+']/subl1['+str(e_22.number)+']/name'
        e_2.subl1.extend([e_21, e_22])
        r_1.two_list.ldata.extend([e_1, e_2])
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)


    def test_threelist_pos(self):
        # READ
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1 = ysanity.Runner.ThreeList.Ldata()
        e_1.number = 31
        e_1.name = 'runner:threelist:ldata['+str(e_1.number)+']:name'
        e_2 = ysanity.Runner.ThreeList.Ldata()
        e_2.number = 32
        e_2.name = 'runner:threelist:ldata['+str(e_2.number)+']:name'
        e_11 = ysanity.Runner.ThreeList.Ldata.Subl1()
        e_12 = ysanity.Runner.ThreeList.Ldata.Subl1()
        e_11.number = 311
        e_11.name = 'runner:threelist:ldata['+str(e_1.number)+']:subl1['+str(e_11.number)+']:name'
        e_12.number = 312
        e_12.name = 'runner:threelist:ldata['+str(e_1.number)+']:subl1['+str(e_12.number)+']:name'
        e_111 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_112 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_111.number = 3111
        e_111.name = 'runner:threelist:ldata['+str(e_1.number)+']:subl1['+str(e_11.number)+']:subsubl1['+str(e_111.number)+']:name'
        e_112.number = 3112
        e_112.name = 'runner:threelist:ldata['+str(e_1.number)+']:subl1['+str(e_11.number)+']:subsubl1['+str(e_112.number)+']:name'
        e_11.sub_subl1.extend([e_111, e_112])
        e_121 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_122 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_121.number = 3121
        e_121.name = 'runner:threelist:ldata['+str(e_1.number)+']:subl1['+str(e_12.number)+']:subsubl1['+str(e_121.number)+']:name'
        e_122.number = 3122
        e_121.name = 'runner:threelist:ldata['+str(e_1.number)+']:subl1['+str(e_12.number)+']:subsubl1['+str(e_122.number)+']:name'
        e_12.sub_subl1.extend([e_121, e_122])
        e_1.subl1.extend([e_11, e_12])
        e_21 = ysanity.Runner.ThreeList.Ldata.Subl1()
        e_22 = ysanity.Runner.ThreeList.Ldata.Subl1()
        e_21.number = 321
        e_21.name = 'runner:threelist:ldata['+str(e_2.number)+']:subl1['+str(e_21.number)+']:name'
        e_22.number = 322
        e_22.name = 'runner:threelist:ldata['+str(e_2.number)+']:subl1['+str(e_22.number)+']:name'
        e_211 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_212 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_211.number = 3211
        e_211.name = 'runner:threelist:ldata['+str(e_2.number)+']:subl1['+str(e_21.number)+']:subsubl1['+str(e_211.number)+']:name'
        e_212.number = 3212
        e_212.name = 'runner:threelist:ldata['+str(e_2.number)+']:subl1['+str(e_21.number)+']:subsubl1['+str(e_212.number)+']:name'
        e_21.sub_subl1.extend([e_211, e_212])
        e_221 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_222 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_221.number = 3221
        e_221.name = 'runner:threelist:ldata['+str(e_2.number)+']:subl1['+str(e_22.number)+']:subsubl1['+str(e_221.number)+']:name'
        e_222.number = 3222
        e_222.name = 'runner:threelist:ldata['+str(e_2.number)+']:subl1['+str(e_22.number)+']:subsubl1['+str(e_222.number)+']:name'
        e_22.sub_subl1.extend([e_221, e_222])
        e_2.subl1.extend([e_21, e_22])
        r_1.three_list.ldata.extend([e_1, e_2])
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1 = ysanity.Runner.ThreeList.Ldata()
        e_1.number = 31
        e_1.name = 'runner/threelist/ldata['+str(e_1.number)+']/name'
        e_2 = ysanity.Runner.ThreeList.Ldata()
        e_2.number = 32
        e_2.name = 'runner/threelist/ldata['+str(e_2.number)+']/name'
        e_11 = ysanity.Runner.ThreeList.Ldata.Subl1()
        e_12 = ysanity.Runner.ThreeList.Ldata.Subl1()
        e_11.number = 311
        e_11.name = 'runner/threelistldata['+str(e_1.number)+']/subl1['+str(e_11.number)+']/name'
        e_12.number = 312
        e_12.name = 'runner/threelist/ldata['+str(e_1.number)+']/subl1['+str(e_12.number)+']/name'
        e_111 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_112 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_111.number = 3111
        e_111.name = 'runner/threelist/ldata['+str(e_1.number)+']/subl1['+str(e_11.number)+']/subsubl1['+str(e_111.number)+']/name'
        e_112.number = 3112
        e_112.name = 'runner/threelist/ldata['+str(e_1.number)+']/subl1['+str(e_11.number)+']/subsubl1['+str(e_112.number)+']/name'
        e_11.sub_subl1.extend([e_111, e_112])
        e_121 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_122 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_121.number = 3121
        e_121.name = 'runner/threelist/ldata['+str(e_1.number)+']/subl1['+str(e_12.number)+']/subsubl1['+str(e_121.number)+']/name'
        e_122.number = 3122
        e_121.name = 'runner/threelist/ldata['+str(e_1.number)+']/subl1['+str(e_12.number)+']/subsubl1['+str(e_122.number)+']/name'
        e_12.sub_subl1.extend([e_121, e_122])
        e_1.subl1.extend([e_11, e_12])
        e_21 = ysanity.Runner.ThreeList.Ldata.Subl1()
        e_22 = ysanity.Runner.ThreeList.Ldata.Subl1()
        e_21.number = 321
        e_21.name = 'runner/threelist/ldata['+str(e_2.number)+']/subl1['+str(e_21.number)+']/name'
        e_22.number = 322
        e_22.name = 'runner/threelist/ldata['+str(e_2.number)+']/subl1['+str(e_22.number)+']/name'
        e_211 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_212 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_211.number = 3211
        e_211.name = 'runner/threelist/ldata['+str(e_2.number)+']/subl1['+str(e_21.number)+']/subsubl1['+str(e_211.number)+']/name'
        e_212.number = 3212
        e_212.name = 'runner/threelist/ldata['+str(e_2.number)+']/subl1['+str(e_21.number)+']/subsubl1['+str(e_212.number)+']/name'
        e_21.sub_subl1.extend([e_211, e_212])
        e_221 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_222 = ysanity.Runner.ThreeList.Ldata.Subl1.SubSubl1()
        e_221.number = 3221
        e_221.name = 'runner/threelist/ldata['+str(e_2.number)+']/subl1['+str(e_22.number)+']/subsubl1['+str(e_221.number)+']/name'
        e_222.number = 3222
        e_222.name = 'runner/threelist/ldata['+str(e_2.number)+']/subl1['+str(e_22.number)+']/subsubl1['+str(e_222.number)+']/name'
        e_22.sub_subl1.extend([e_221, e_222])
        e_2.subl1.extend([e_21, e_22])
        r_1.three_list.ldata.extend([e_1, e_2])
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)


    def test_InbtwList_pos(self):
        # READ
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1 = ysanity.Runner.InbtwList.Ldata()
        e_2 = ysanity.Runner.InbtwList.Ldata()
        e_1.number = 11
        e_1.name = 'runner:inbtwlist:['+str(e_1.number)+']:name'
        e_1.subc.number = 111
        e_1.subc.name = 'runner:inbtwlist:['+str(e_1.number)+']:subc:name'
        e_2.number = 12
        e_2.name = 'runner:inbtwlist:['+str(e_2.number)+']:name'
        e_2.subc.number = 121
        e_2.subc.name = 'runner:inbtwlist:['+str(e_2.number)+']:name'
        e_11 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_11.number = 111
        e_11.name = 'runner:inbtwlist:['+str(e_1.number)+']:subc:subcsubl1['+str(e_11.number)+']:name'
        e_12 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_12.number = 112
        e_12.name = 'runner:inbtwlist:['+str(e_1.number)+']:subc:subcsubl1['+str(e_12.number)+']:name'
        e_1.subc.subc_subl1.extend([e_11, e_12])
        e_21 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_21.number = 121
        e_21.name = 'runner:inbtwlist:['+str(e_2.number)+']:subc:subcsubl1['+str(e_21.number)+']:name'
        e_22 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_22.number = 122
        e_22.name = 'runner:inbtwlist:['+str(e_2.number)+']:subc:subcsubl1['+str(e_22.number)+']:name'
        e_2.subc.subc_subl1.extend([e_21, e_22])
        r_1.inbtw_list.ldata.extend([e_1, e_2])
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1 = ysanity.Runner.InbtwList.Ldata()
        e_2 = ysanity.Runner.InbtwList.Ldata()
        e_1.number = 11
        e_1.name = 'runner/inbtwlist/['+str(e_1.number)+']/name'
        e_1.subc.number = 111
        e_1.subc.name = 'runnerinbtwlist/['+str(e_1.number)+']/subc/name'
        e_2.number = 12
        e_2.name = 'runner/inbtwlist/['+str(e_2.number)+']/name'
        e_2.subc.number = 121
        e_2.subc.name = 'runner/inbtwlist/['+str(e_2.number)+']/name'
        e_11 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_11.number = 111
        e_11.name = 'runner/inbtwlist/['+str(e_1.number)+']/subc/subcsubl1['+str(e_11.number)+']/name'
        e_12 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_12.number = 112
        e_12.name = 'runner/inbtwlist/['+str(e_1.number)+']/subc/subcsubl1['+str(e_12.number)+']/name'
        e_1.subc.subc_subl1.extend([e_11, e_12])
        e_21 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_21.number = 121
        e_21.name = 'runner/inbtwlist/['+str(e_2.number)+']/subc/subcsubl1['+str(e_21.number)+']/name'
        e_22 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_22.number = 122
        e_22.name = 'runner/inbtwlist/['+str(e_2.number)+']/subc/subcsubl1['+str(e_22.number)+']/name'
        e_2.subc.subc_subl1.extend([e_21, e_22])
        r_1.inbtw_list.ldata.extend([e_1, e_2])
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)


    def test_leafref_simple_pos(self):
        # change ref and original data together
        # READ
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.ytypes.built_in_t.number8 = 100
        r_1.ytypes.built_in_t.leaf_ref = r_1.ytypes.built_in_t.number8
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.ytypes.built_in_t.number8 = 110
        r_1.ytypes.built_in_t.leaf_ref = r_1.ytypes.built_in_t.number8
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)

    def test_leafref_pos(self):
        # rfc: refer to leaf
        # 1.already exists
        # 2.has default value
        # create leafs will be referred to
        # CREATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.one.name = 'runner:one:name'
        r_1.two.sub1.number = 21
        r_1.three.sub1.sub2.number = 311
        e_1 = ysanity.Runner.InbtwList.Ldata()
        e_2 = ysanity.Runner.InbtwList.Ldata()
        e_1.number = 11
        e_2.number = 21
        e_1.name = 'runner:inbtwlist['+str(e_1.number)+']:name'
        e_2.name = 'runner:inbtwlist['+str(e_2.number)+']:name'
        e_1.subc.number = 111
        e_2.subc.number = 121
        e_1.subc.name = 'runner:inbtwlist['+str(e_1.number)+']:subc:name'
        e_2.subc.name = 'runner:inbtwlist['+str(e_2.number)+']:subc:name'
        e_11 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_12 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_11.number = 111
        e_12.number = 112
        e_11.name = 'runner:inbtwlist['+str(e_1.number)+']:subc:subcsubl1['+str(e_11.number)+']:name'
        e_12.name = 'runner:inbtwlist['+str(e_1.number)+']:subc:subcsubl1['+str(e_12.number)+']:name'
        e_1.subc.subc_subl1.extend([e_11, e_12])
        e_21 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_22 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_21.number = 311
        e_22.number = 122
        e_21.name = 'runner:inbtwlist['+str(e_2.number)+']:subc:subcsubl1['+str(e_21.number)+']:name'
        e_22.name = 'runner:inbtwlist['+str(e_2.number)+']:subc:subcsubl1['+str(e_22.number)+']:name'
        e_2.subc.subc_subl1.extend([e_21, e_22])
        r_1.inbtw_list.ldata.extend([e_1, e_2])

        r_1.leaf_ref.ref_one_name = r_1.one.name
        r_1.leaf_ref.ref_two_sub1_number = r_1.two.sub1.number
        r_1.leaf_ref.ref_three_sub1_sub2_number = r_1.three.sub1.sub2.number
        r_1.leaf_ref.ref_inbtw = e_21.name
        r_1.leaf_ref.one.name = 'runner:leaf-ref:one:name'
        r_1.leaf_ref.one.two.self_ref_one_name = r_1.leaf_ref.ref_one_name
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_1.one.name, r_1.leaf_ref.ref_one_name)
        self.assertEqual(r_1.two.sub1.number, r_1.leaf_ref.ref_two_sub1_number)
        self.assertEqual(r_1.three.sub1.sub2.number, r_1.leaf_ref.ref_three_sub1_sub2_number)
        self.assertEqual(r_1.inbtw_list.ldata[1].subc.subc_subl1[0].name, r_1.leaf_ref.ref_inbtw)
        self.assertEqual(r_1.leaf_ref.ref_one_name, r_1.leaf_ref.one.two.self_ref_one_name)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.one.name = 'runner/one/name'
        r_1.two.sub1.number = 21
        r_1.three.sub1.sub2.number = 311
        e_1 = ysanity.Runner.InbtwList.Ldata()
        e_2 = ysanity.Runner.InbtwList.Ldata()
        e_1.number = 11
        e_2.number = 21
        e_1.name = 'runner/inbtwlist['+str(e_1.number)+']/name'
        e_2.name = 'runner/inbtwlist['+str(e_2.number)+']/name'
        e_1.subc.number = 111
        e_2.subc.number = 121
        e_1.subc.name = 'runner/inbtwlist['+str(e_1.number)+']/subc/name'
        e_2.subc.name = 'runner/inbtwlist['+str(e_2.number)+']/subc/name'
        e_11 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_12 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_11.number = 111
        e_12.number = 112
        e_11.name = 'runner/inbtwlist['+str(e_1.number)+']/subc/subcsubl1['+str(e_11.number)+']/name'
        e_12.name = 'runner/inbtwlist['+str(e_1.number)+']/subc/subcsubl1['+str(e_12.number)+']/name'
        e_1.subc.subc_subl1.extend([e_11, e_12])
        e_21 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_22 = ysanity.Runner.InbtwList.Ldata.Subc.SubcSubl1()
        e_21.number = 311
        e_22.number = 122
        e_21.name = 'runner/inbtwlist['+str(e_2.number)+']/subc/subcsubl1['+str(e_21.number)+']/name'
        e_22.name = 'runner/inbtwlist['+str(e_2.number)+']/subc/subcsubl1['+str(e_22.number)+']/name'
        e_2.subc.subc_subl1.extend([e_21, e_22])
        r_1.inbtw_list.ldata.extend([e_1, e_2])

        r_1.leaf_ref.ref_one_name = r_1.one.name
        r_1.leaf_ref.ref_two_sub1_number = r_1.two.sub1.number
        r_1.leaf_ref.ref_three_sub1_sub2_number = r_1.three.sub1.sub2.number
        r_1.leaf_ref.ref_inbtw = e_21.name
        r_1.leaf_ref.one.name = 'runner/leaf-ref/one/name'
        r_1.leaf_ref.one.two.self_ref_one_name = r_1.leaf_ref.ref_one_name
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_1.one.name, r_1.leaf_ref.ref_one_name)
        self.assertEqual(r_1.two.sub1.number, r_1.leaf_ref.ref_two_sub1_number)
        self.assertEqual(r_1.three.sub1.sub2.number, r_1.leaf_ref.ref_three_sub1_sub2_number)
        self.assertEqual(r_1.inbtw_list.ldata[1].subc.subc_subl1[0].name, r_1.leaf_ref.ref_inbtw)
        self.assertEqual(r_1.leaf_ref.ref_one_name, r_1.leaf_ref.one.two.self_ref_one_name)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)

    def test_aug_one_pos(self):
        # CREATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.one.one_aug.number = 1
        r_1.one.one_aug.name =r_1.one.one_aug._common_path
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        r_1.one.one_aug.number = 10
        r_1.one.one_aug.name =r_1.one.one_aug._common_path.replace(':', '/')
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_1)
        self.assertEqual(r_2._has_data(), False)

    def test_aug_onelist_pos(self):
        # CREATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1 = ysanity.Runner.OneList.OneAugList.Ldata()
        e_2 = ysanity.Runner.OneList.OneAugList.Ldata()
        e_1.number = 1
        e_1.name = e_1._common_path
        e_2.number = 2
        e_2.name = e_2._common_path
        r_1.one_list.one_aug_list.ldata.extend([e_1, e_2])
        r_1.one_list.one_aug_list.enabled = True
        self.crud.create(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # UPDATE
        r_1, r_2 = ysanity.Runner(), ysanity.Runner()
        e_1 = ysanity.Runner.OneList.OneAugList.Ldata()
        e_2 = ysanity.Runner.OneList.OneAugList.Ldata()
        e_1.number = 1
        e_1.name = e_1._common_path.replace(':', '/')
        e_2.number = 2
        e_2.name = e_2._common_path.replace(':', '/')
        r_1.one_list.one_aug_list.ldata.extend([e_1, e_2])
        r_1.one_list.one_aug_list.enabled = True
        self.crud.update(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(is_equal(r_1, r_2), True)
        # DELETE
        r_1 = ysanity.Runner()
        self.crud.delete(self.ncc, r_1)
        r_2 = self.crud.read(self.ncc, r_2)
        self.assertEqual(r_2._has_data(), False)

    def test_parent_empty(self):
        runner = ysanity.Runner()
        runner.ytypes.enabled = Empty()
        runner.ytypes.built_in_t.emptee = Empty()

        self.crud.create(self.ncc, runner)

        runner_read = self.crud.read(self.ncc, ysanity.Runner())

        self.assertEqual(is_equal(runner_read, runner), True)


if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        SanityYang.PROVIDER_TYPE = sys.argv.pop()

    suite = unittest.TestLoader().loadTestsFromTestCase(SanityYang)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
