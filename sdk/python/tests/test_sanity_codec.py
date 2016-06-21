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
from ydk.errors import YPYServiceError


class SanityYang(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.codec = CodecService()
        self.provider = CodecServiceProvider(type='xml')

        self._enum_payload_1 = \
'<built-in-t xmlns="http://cisco.com/ns/yang/ydktest-sanity">\n  <enum-value>local</enum-value>\n</built-in-t>\n'

        self._enum_payload_2 = \
'<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">\n  <ytypes>\n    <built-in-t>\n      <enum-value>local</enum-value>\n    </built-in-t>\n  </ytypes>\n</runner>\n'

        self._runner_payload = \
'<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">\n\
  <two-list>\n\
    <ldata>\n\
      <number>21</number>\n\
      <name>runner:twolist:ldata[21]:name</name>\n\
      <subl1>\n\
        <number>211</number>\n\
        <name>runner:twolist:ldata[21]:subl1[211]:name</name>\n\
      </subl1>\n\
      <subl1>\n\
        <number>212</number>\n\
        <name>runner:twolist:ldata[21]:subl1[212]:name</name>\n\
      </subl1>\n\
    </ldata>\n\
    <ldata>\n\
      <number>22</number>\n\
      <name>runner:twolist:ldata[22]:name</name>\n\
      <subl1>\n\
        <number>221</number>\n\
        <name>runner:twolist:ldata[22]:subl1[221]:name</name>\n\
      </subl1>\n\
      <subl1>\n\
        <number>222</number>\n\
        <name>runner:twolist:ldata[22]:subl1[222]:name</name>\n\
      </subl1>\n\
    </ldata>\n\
  </two-list>\n\
</runner>\n'

    @classmethod
    def tearDownClass(self):
        pass

    def setUp(self):
        print '\nIn method', self._testMethodName + ':'

    def tearDown(self):
        pass

    def _get_runner_entity(self):
        r_1 = ysanity.Runner()
        e_1, e_2 = ysanity.Runner.TwoList.Ldata(), ysanity.Runner.TwoList.Ldata()
        e_11, e_12 = ysanity.Runner.TwoList.Ldata.Subl1(), ysanity.Runner.TwoList.Ldata.Subl1()
        e_1.number = 21
        e_1.name = 'runner:twolist:ldata[' + str(e_1.number) + ']:name'
        e_11.number = 211
        e_11.name = 'runner:twolist:ldata[' + str(e_1.number) + ']:subl1[' + str(e_11.number) + ']:name'
        e_12.number = 212
        e_12.name = 'runner:twolist:ldata[' + str(e_1.number) + ']:subl1[' + str(e_12.number) + ']:name'
        e_1.subl1.extend([e_11, e_12])
        e_21, e_22 = ysanity.Runner.TwoList.Ldata.Subl1(), ysanity.Runner.TwoList.Ldata.Subl1()
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

    def test_encode_2(self):
        from ydk.models.ydktest.ydktest_sanity import YdkEnumTestEnum
        r_1 = ysanity.Runner.Ytypes.BuiltInT()
        r_1.enum_value = YdkEnumTestEnum.LOCAL

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

if __name__ == '__main__':
    unittest.main()
