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
from __future__ import absolute_import
import unittest
from compare import is_equal
import copy

try:
    from ydk.models.ydktest.ydktest_sanity import Runner
    from ydk.models.ydktest.oc_pattern import OcA
except:
    from ydk.models.ydktest.ydktest_sanity.runner.runner import Runner
    from ydk.models.ydktest.oc_pattern.oc_a.oc_a import OcA

from ydk.providers import CodecServiceProvider
from ydk.services import CodecService
from ydk.errors import YPYServiceError


class SanityYang(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.codec = CodecService()
        self.provider = CodecServiceProvider(type='xml')

        self._enum_payload_1 = '''<built-in-t xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <enum-value>local</enum-value>
</built-in-t>
'''

        self._enum_payload_2 = '''<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <ytypes>
    <built-in-t>
      <enum-value>local</enum-value>
    </built-in-t>
  </ytypes>
</runner>
'''

        self._runner_payload = '''<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
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
'''
        self._oc_pattern_payload = '''<oc-A xmlns="http://cisco.com/ns/yang/oc-pattern">
  <a>Hello</a>
</oc-A>
'''

    @classmethod
    def tearDownClass(self):
        pass

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def _get_runner_entity(self):
        r_1 = Runner()
        e_1, e_2 = Runner.TwoList.Ldata(), Runner.TwoList.Ldata()
        e_11, e_12 = Runner.TwoList.Ldata.Subl1(), Runner.TwoList.Ldata.Subl1()
        e_1.number = 21
        e_1.name = 'runner:twolist:ldata[' + str(e_1.number) + ']:name'
        e_11.number = 211
        e_11.name = 'runner:twolist:ldata[' + str(e_1.number) + ']:subl1[' + str(e_11.number) + ']:name'
        e_12.number = 212
        e_12.name = 'runner:twolist:ldata[' + str(e_1.number) + ']:subl1[' + str(e_12.number) + ']:name'
        e_1.subl1.extend([e_11, e_12])
        e_21, e_22 = Runner.TwoList.Ldata.Subl1(), Runner.TwoList.Ldata.Subl1()
        e_2.number = 22
        e_2.name = 'runner:twolist:ldata[' + str(e_2.number) + ']:name'
        e_21.number = 221
        e_21.name = 'runner:twolist:ldata[' + str(e_2.number) + ']:subl1[' + str(e_21.number) + ']:name'
        e_22.number = 222
        e_22.name = 'runner:twolist:ldata[' + str(e_2.number) + ']:subl1[' + str(e_22.number) + ']:name'
        e_2.subl1.extend([e_21, e_22])
        r_1.two_list.ldata.extend([e_1, e_2])
        return r_1

    def test_encode_1(self):
        r_1 = self._get_runner_entity()
        payload = self.codec.encode(self.provider, r_1)
        self.assertEqual(self._runner_payload, payload)

        # DEEPCOPY
        r_1_copy = copy.deepcopy(r_1)
        self.assertEqual(is_equal(r_1_copy, r_1), True)
        payload_copy = self.codec.encode(self.provider, r_1)
        self.assertEqual(self._runner_payload, payload_copy)


    def test_encode_2(self):
        try:
            from ydk.models.ydktest.ydktest_sanity import YdkEnumTestEnum
        except:
            from ydk.models.ydktest.ydktest_sanity.ydktest_sanity import YdkEnumTestEnum
        r_1 = Runner.Ytypes.BuiltInT()
        r_1.enum_value = YdkEnumTestEnum.local

        payload = self.codec.encode(self.provider, r_1)
        self.assertEqual(self._enum_payload_1, payload)

    def test_encode_invalid_1(self):
        try:
            self.codec.encode(self.provider, None)
        except YPYServiceError as err:
            self.assertEqual(
                err.message, "'encoder' and 'entity' cannot be None")
        else:
            raise Exception('YPYServiceError not raised')

    def test_encode_invalid_2(self):
        try:
            self.codec.encode(None, self._get_runner_entity())
        except YPYServiceError as e:
            err = e
            self.assertEqual(
                err.message, "'encoder' and 'entity' cannot be None")
        else:
            raise Exception('YPYServiceError not raised')

    def test_encode_invalid_3(self):
        try:
            self.codec.encode(None, None)
        except YPYServiceError as e:
            err = e
            self.assertEqual(
                err.message, "'encoder' and 'entity' cannot be None")
        else:
            raise Exception('YPYServiceError not raised')

    def test_decode_1(self):
        entity = self.codec.decode(self.provider, self._enum_payload_2)
        self.assertEqual(
            self._enum_payload_2, self.codec.encode(self.provider, entity))

    def test_decode_invalid_1(self):
        try:
            self.codec.decode(None, self._enum_payload_2)
        except YPYServiceError as e:
            err = e
            self.assertEqual(
                err.message, "'decoder' and 'payload' cannot be None")
        else:
            raise Exception('YPYServiceError not raised')

    def test_decode_invalid_2(self):
        try:
            self.codec.decode(self.provider, None)
        except YPYServiceError as e:
            err = e
            self.assertEqual(
                err.message, "'decoder' and 'payload' cannot be None")
        else:
            raise Exception('YPYServiceError not raised')

    def test_decode_invalid_3(self):
        try:
            self.codec.decode(None, None)
        except YPYServiceError as e:
            err = e
            self.assertEqual(
                err.message, "'decoder' and 'payload' cannot be None")
        else:
            raise Exception('YPYServiceError not raised')

    def test_encode_decode(self):
        r_1 = self._get_runner_entity()
        payload = self.codec.encode(self.provider, r_1)
        entity = self.codec.decode(self.provider, payload)
        self.assertEqual(is_equal(r_1, entity), True)
        self.assertEqual(payload, self.codec.encode(self.provider, entity))

    def test_encode_decode_dict(self):
        r_1 = self._get_runner_entity()
        r_entity = {'ydktest-sanity':r_1}
        payload = self.codec.encode(self.provider, r_entity)
        entity = self.codec.decode(self.provider, payload)
        for module in entity:
            self.assertEqual(is_equal(r_entity[module], entity[module]), True)
        self.assertEqual(payload, self.codec.encode(self.provider, entity))

    def test_decode_oc_pattern(self):
        obj_A = OcA()
        obj_A.a = 'Hello'
        entity = self.codec.decode(self.provider, self._oc_pattern_payload)

        self.assertEqual(is_equal(obj_A, entity), True)

if __name__ == '__main__':
    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(SanityYang)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
