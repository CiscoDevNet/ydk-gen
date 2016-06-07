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

"""test_sanity_codec.py
sanity test for CodecService
"""

import unittest
from tests.compare import is_equal

from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.providers import CodecServiceProvider
from ydk.services import CodecService


class SanityYang(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.codec = CodecService()
        self.provider = CodecServiceProvider(type='xml')

        self._enum_payload_1 = """<built-in-t xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <enum-value>local</enum-value>
</built-in-t>
"""

        self._enum_payload_2 = """<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <ytypes>
    <built-in-t>
      <enum-value>local</enum-value>
    </built-in-t>
  </ytypes>
</runner>
"""
        self._runner_payload = """<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <two-list>
    <ldata>
      <number>21</number>
      <name>runner:twolist:ldata[21]:name</name>
      <subl1>
        <number>211</number>
        <name>runner:twolist:ldata[21]:subl1[211]:name</name>
      </subl1>
      <subl1>
        <number>212</number>
        <name>runner:twolist:ldata[21]:subl1[212]:name</name>
      </subl1>
    </ldata>
    <ldata>
      <number>22</number>
      <name>runner:twolist:ldata[22]:name</name>
      <subl1>
        <number>221</number>
        <name>runner:twolist:ldata[22]:subl1[221]:name</name>
      </subl1>
      <subl1>
        <number>222</number>
        <name>runner:twolist:ldata[22]:subl1[222]:name</name>
      </subl1>
    </ldata>
  </two-list>
</runner>
"""

    @classmethod
    def tearDownClass(self):
        pass

    def setUp(self):
        print '\nIn method', self._testMethodName + ':'

    def tearDown(self):
        pass

    def _get_runner_entity(self):
        r_1 = ysanity.Runner()
        e_1 = ysanity.Runner.TwoList.Ldata()
        e_2 = ysanity.Runner.TwoList.Ldata()
        e_11 = ysanity.Runner.TwoList.Ldata.Subl1()
        e_12 = ysanity.Runner.TwoList.Ldata.Subl1()
        e_1.number = 21
        e_1.name = 'runner:twolist:ldata[%d]:name' % e_1.number
        e_11.number = 211
        e_11.name = ('runner:twolist:ldata[%d]:subl1[%d]:name'
                     % (e_1.number, e_11.number))
        e_12.number = 212
        e_12.name = ('runner:twolist:ldata[%d]:subl1[%d]:name'
                     % (e_1.number, e_12.number))
        e_1.subl1.extend([e_11, e_12])
        e_21 = ysanity.Runner.TwoList.Ldata.Subl1()
        e_22 = ysanity.Runner.TwoList.Ldata.Subl1()
        e_2.number = 22
        e_2.name = 'runner:twolist:ldata[%d]:name' % e_2.number
        e_21.number = 221
        e_21.name = ('runner:twolist:ldata[%d]:subl1[%d]:name'
                     % (e_2.number, e_21.number))
        e_22.number = 222
        e_22.name = ('runner:twolist:ldata[%d]:subl1[%d]:name'
                     % (e_2.number, e_22.number))
        e_2.subl1.extend([e_21, e_22])
        r_1.two_list.ldata.extend([e_1, e_2])
        return r_1

    def test_encode_1(self):
        r_1 = self._get_runner_entity()
        payload = self.codec.encode(self.provider, r_1)
        self.assertEqual(self._runner_payload, payload)

    def test_encode_2(self):
        from ydk.models.ydktest.ydktest_sanity import YdkEnumTestEnum
        r_1 = ysanity.Runner.Ytypes.BuiltInT()
        r_1.enum_value = YdkEnumTestEnum.LOCAL

        payload = self.codec.encode(self.provider, r_1)
        self.assertEqual(self._enum_payload_1, payload)

    def test_decode_1(self):
        entity = self.codec.decode(self.provider, self._enum_payload_2)
        self.assertEqual(self._enum_payload_2,
                         self.codec.encode(self.provider, entity))

    def test_encode_decode(self):
        r_1 = self._get_runner_entity()
        payload = self.codec.encode(self.provider, r_1)
        entity = self.codec.decode(self.provider, payload)
        self.assertEqual(is_equal(r_1, entity), True)
        self.assertEqual(payload, self.codec.encode(self.provider, entity))

if __name__ == '__main__':
    unittest.main()
