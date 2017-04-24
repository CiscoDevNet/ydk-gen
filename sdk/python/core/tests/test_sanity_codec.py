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

from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.models.ydktest import oc_pattern
from ydk.providers import CodecServiceProvider
from ydk.services import CodecService
from ydk.errors import YPYServiceError
from ydk.types import EncodingFormat
from test_utils import assert_with_error


class SanityYang(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.codec = CodecService()
        self.provider = CodecServiceProvider(type='xml')

        self._xml_enum_payload_1 = '''<built-in-t xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <enum-value>local</enum-value>
</built-in-t>
'''
        self._json_enum_payload_1 = """{
  "ydktest-sanity:built-in-t": {
    "enum-value": "local"
  }
}
"""

        self._xml_enum_payload_2 = '''<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
  <ytypes>
    <built-in-t>
      <enum-value>local</enum-value>
    </built-in-t>
  </ytypes>
</runner>
'''

        self._xml_runner_payload = '''<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
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

        self._json_runner_payload = """{
  "ydktest-sanity:runner": {
    "two-list": {
      "ldata": [
        {
          "number": 21,
          "name": "runner:twolist:ldata[21]:name",
          "subl1": [
            {
              "number": 211,
              "name": "runner:twolist:ldata[21]:subl1[211]:name"
            },
            {
              "number": 212,
              "name": "runner:twolist:ldata[21]:subl1[212]:name"
            }
          ]
        },
        {
          "number": 22,
          "name": "runner:twolist:ldata[22]:name",
          "subl1": [
            {
              "number": 221,
              "name": "runner:twolist:ldata[22]:subl1[221]:name"
            },
            {
              "number": 222,
              "name": "runner:twolist:ldata[22]:subl1[222]:name"
            }
          ]
        }
      ]
    }
  }
}
"""
        self._xml_oc_pattern_payload = '''<oc-A xmlns="http://cisco.com/ns/yang/oc-pattern">
  <a>Hello</a>
</oc-A>
'''
        self._json_oc_pattern_payload = """{
  "oc-pattern:oc-A": [
    {
      "a": "Hello",
      "B": {
        "b": "Hello"
      }
    }
  ]
}"""

    @classmethod
    def tearDownClass(self):
        pass

    def setUp(self):
        pass

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

    def _compare_entities(self, r_1, r_2):
        self.assertEqual(r_1.two_list.ldata[0].name, r_2.two_list.ldata[0].name)
        self.assertEqual(r_1.two_list.ldata[0].number, r_2.two_list.ldata[0].number)
        self.assertEqual(r_1.two_list.ldata[0].subl1[0].name, r_2.two_list.ldata[0].subl1[0].name)
        self.assertEqual(r_1.two_list.ldata[0].subl1[0].number, r_2.two_list.ldata[0].subl1[0].number)
        self.assertEqual(r_1.two_list.ldata[0].subl1[1].name, r_2.two_list.ldata[0].subl1[1].name)
        self.assertEqual(r_1.two_list.ldata[0].subl1[1].number, r_2.two_list.ldata[0].subl1[1].number)
        self.assertEqual(r_1.two_list.ldata[1].name, r_2.two_list.ldata[1].name)
        self.assertEqual(r_1.two_list.ldata[1].number, r_2.two_list.ldata[1].number)
        self.assertEqual(r_1.two_list.ldata[1].subl1[0].name, r_2.two_list.ldata[1].subl1[0].name)
        self.assertEqual(r_1.two_list.ldata[1].subl1[0].number, r_2.two_list.ldata[1].subl1[0].number)
        self.assertEqual(r_1.two_list.ldata[1].subl1[1].name, r_2.two_list.ldata[1].subl1[1].name)
        self.assertEqual(r_1.two_list.ldata[1].subl1[1].number, r_2.two_list.ldata[1].subl1[1].number)

    def test_xml_encode_1(self):
        self.provider.encoding = EncodingFormat.XML
        r_1 = self._get_runner_entity()
        payload = self.codec.encode(self.provider, r_1)
        self.assertEqual(self._xml_runner_payload, payload)

    def test_xml_encode_2(self):
        self.provider.encoding = EncodingFormat.XML
        from ydk.models.ydktest.ydktest_sanity import YdkEnumTest
        r_1 = ysanity.Runner.Ytypes.BuiltInT()
        r_1.enum_value = YdkEnumTest.local

        payload = self.codec.encode(self.provider, r_1)
        self.assertEqual(self._xml_enum_payload_1, payload)

    @assert_with_error("'provider' and 'entity_holder' cannot be None", YPYServiceError)
    def test_encode_invalid_1(self):
        self.codec.encode(self.provider, None)

    @assert_with_error("'provider' and 'entity_holder' cannot be None", YPYServiceError)
    def test_encode_invalid_2(self):
            self.codec.encode(None, self._get_runner_entity())

    @assert_with_error("'provider' and 'entity_holder' cannot be None", YPYServiceError)
    def test_encode_invalid_3(self):
            self.codec.encode(None, None)

    def test_xml_decode_1(self):
        self.provider.encoding = EncodingFormat.XML
        entity = self.codec.decode(self.provider, self._xml_enum_payload_2)
        self.assertEqual(
            self._xml_enum_payload_2, self.codec.encode(self.provider, entity))

    @assert_with_error("'provider' and 'payload_holder' cannot be None", YPYServiceError)
    def test_decode_invalid_1(self):
            self.codec.decode(None, self._xml_enum_payload_2)

    @assert_with_error("'provider' and 'payload_holder' cannot be None", YPYServiceError)
    def test_decode_invalid_2(self):
        self.codec.decode(self.provider, None)

    @assert_with_error("'provider' and 'payload_holder' cannot be None", YPYServiceError)
    def test_decode_invalid_3(self):
        self.codec.decode(None, None)

    def test_xml_encode_decode(self):
        self.provider.encoding = EncodingFormat.XML
        r_1 = self._get_runner_entity()
        payload = self.codec.encode(self.provider, r_1)
        entity = self.codec.decode(self.provider, payload)

        self._compare_entities(r_1, entity)
        self.assertEqual(payload, self.codec.encode(self.provider, entity))

    def test_xml_encode_decode_dict(self):
        self.provider.encoding = EncodingFormat.XML
        r_1 = self._get_runner_entity()
        r_entity = {'ydktest-sanity':r_1}
        payload = self.codec.encode(self.provider, r_entity)
        entity = self.codec.decode(self.provider, payload)
        for module in entity:
            self._compare_entities(r_entity[module], entity[module])
        self.assertEqual(payload, self.codec.encode(self.provider, entity))

    def test_xml_decode_oc_pattern(self):
        self.provider.encoding = EncodingFormat.XML
        obj_A = oc_pattern.OcA()
        obj_A.a = 'Hello'

        entity = self.codec.decode(self.provider, self._xml_oc_pattern_payload)

        self.assertEqual(obj_A.a, entity.a)

    # JSON

    def test_json_encode_1(self):
        self.provider.encoding = EncodingFormat.JSON
        entity = self._get_runner_entity()
        payload = self.codec.encode(self.provider, entity)
        self.assertEqual(self._json_runner_payload, payload)

    def test_json_encode_2(self):
        self.provider.encoding = EncodingFormat.JSON
        from ydk.models.ydktest.ydktest_sanity import YdkEnumTest
        r_1 = ysanity.Runner.Ytypes.BuiltInT()
        r_1.enum_value = YdkEnumTest.local

        payload = self.codec.encode(self.provider, r_1)
        self.assertEqual(self._json_enum_payload_1, payload)

    def test_json_decode_1(self):
        self.provider.encoding = EncodingFormat.JSON
        entity = self.codec.decode(self.provider, self._json_runner_payload)
        self.assertEqual(self._json_runner_payload,
                         self.codec.encode(self.provider, entity))

    def test_json_encode_decode(self):
        self.provider.encoding = EncodingFormat.JSON
        runner = self._get_runner_entity()
        payload = self.codec.encode(self.provider, runner)
        entity = self.codec.decode(self.provider, payload)

        self._compare_entities(runner, entity)
        self.assertEqual(payload, self.codec.encode(self.provider, entity))

    def test_json_encode_decode_dict(self):
        self.provider.encoding = EncodingFormat.JSON
        entity = self._get_runner_entity()
        entity_holder = {'ydktest-sanity': entity}
        payload_holder = self.codec.encode(self.provider, entity_holder)
        entities = self.codec.decode(self.provider, payload_holder)
        for module in entities:
            self._compare_entities(entities[module], entities[module])
            self.assertEqual(payload_holder[module],
                             self.codec.encode(self.provider, entities[module]))

    @unittest.skip('encodes to "oc-pattern:a": "(!error!)"')
    def test_json_encode_oc_pattern(self):
        self.provider.encoding = EncodingFormat.JSON
        obj_A = oc_pattern.OcA()
        obj_A.a = 'Hello'
        obj_A.b.b = 'Hello'

        self.assertEqual(self.codec.encode(self.provider, obj_A),
                         self._json_oc_pattern_payload)

    def test_json_decode_oc_pattern(self):
        self.provider.encoding = EncodingFormat.JSON
        entity = self.codec.decode(self.provider, self._json_oc_pattern_payload)
        self.assertEqual(entity.a.get(), 'Hello')
        self.assertEqual(entity.b.b.get(), 'Hello')


if __name__ == '__main__':
    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(SanityYang)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
