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

"""test_sanity_fitler_read.py
read API according to discussion
"""

import unittest
from tests.compare import is_equal

from ydk.models.ydktest import ydktest_filterread as ysanity 
from ydk.providers import NetconfServiceProvider
from ydk.services import CRUDService
from ydk.types import READ, YList

class SanityYang(unittest.TestCase):


    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider(
            address='127.0.0.1',
            username='admin',
            password='admin',
            protocol='ssh',
            port=12022)
        self.crud = CRUDService()
        a = self.getInitEntity()
        self.crud.delete(self.ncc, a)
        self.crud.create(self.ncc, a)

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        print '\nIn method', self._testMethodName + ':'

    def tearDown(self):
        pass

    @classmethod
    def getInitEntity(self):
        """ set and return entity with XML:
        <a>
          <a1>some value</a1>
          <a2>value of a2</a2>
          <a3>value of a3</a3>
          <b>
            <b1>some value</b1>
            <b2>value of b2</b2>
            <b3>value of b3</b3>
            <c/>
            <d>
              <d1>some value d1</d1>
              <d2>value of d2</d2>
              <d3>value of d3</d3>
              <e>
                <e1>some value e1</e1>
                <e2>value of e2</e2>
              </e>
            </d>
          </b>
          <lst>
            <number>1</number>
            <value>one</value>
          </lst>
          <lst>
            <number>2</number>
            <value>two</value>
          </lst>
          <lst>
            <number>3</number>
            <value>three</value>
          </lst>
        </a>        
        """          
        a = ysanity.A()
        a.a1, a.a2, a.a3 = "some value", "value of a2", "value of a3"
        a.b.b1, a.b.b2, a.b.b3 = "some value", "value of b2", "value of b3"
        a.b.c = a.b.C()
        a.b.d.d1, a.b.d.d2, a.b.d.d3 = "some value d1", "value of d2", "value of d3"
        a.b.d.e.e1, a.b.d.e.e2 = "some value e1", "value of e2"
        l1, l2, l3 = a.Lst(), a.Lst(), a.Lst()
        l1.number, l1.value = 1, "one"
        l2.number, l2.value = 2, "two"
        l3.number, l3.value = 3, "three"
        a.lst.extend([l1,l2,l3])
        return a

    def test_CASE1(self):
        a = ysanity.A()
        a = self.crud.read(self.ncc, a)
        # the whole thing
        _a = self.getInitEntity()
        self.assertEqual(is_equal(a, _a), True)

    def test_CASE2(self):
        a = ysanity.A()
        a.a1 = "some value"
        a = self.crud.read(self.ncc, a)
        # the whole thing        
        _a = self.getInitEntity()
        self.assertEqual(is_equal(a, _a), True)

    def test_CASE3(self):
        a = ysanity.A()
        a.a1 = READ()
        a = self.crud.read(self.ncc, a)
        _a = ysanity.A()
        _a.a1 = "some value"
        self.assertEqual(is_equal(a, _a), True)

    def test_CASE4(self):
        a = ysanity.A()
        a.b.b1 = "some value"
        a = self.crud.read(self.ncc, a)
        _a = self.getInitEntity()
        _a.lst, _a.a1, _a.a2, _a.a3 = YList(), None, None, None
        self.assertEqual(is_equal(a, _a), True)

    def test_CASE5(self):
        a = ysanity.A()
        e = a.b.d.e
        e.e1 = "some value e1"
        a = self.crud.read(self.ncc, a)
        _a = ysanity.A()
        _a.b.d.e.e1, _a.b.d.e.e2 = "some value e1", "value of e2"
        self.assertEqual(is_equal(a, _a), True)        

    def test_CASE6(self):
        a = ysanity.A()
        a.b.c = a.b.C()
        a = self.crud.read(self.ncc, a)
        _a = ysanity.A()
        _a.b.c = _a.b.C()
        self.assertEqual(is_equal(a, _a), True)

    def test_CASE7(self):
        a = ysanity.A()
        l1, l2 = a.Lst(), a.Lst()
        l1.number, l2.number = 1, 2
        a.lst.extend([l1, l2])
        a = self.crud.read(self.ncc, a)
        _a = ysanity.A()
        l1.value, l2.value = "one", "two"
        _a.lst.extend([l1, l2])
        self.assertEqual(is_equal(a, _a), True)


if __name__ == '__main__':
    unittest.main()
