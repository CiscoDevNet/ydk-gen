#  ----------------------------------------------------------------
# Copyright 2017 Cisco Systems
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

"""test_on_demand_downloading.py
sanity test for on demand module downloading:
    - Parse and download new modules from xml payload

"""
from __future__ import absolute_import

import sys
import tempfile
import unittest

from ydk.path import Repository
from ydk.types import EncodingFormat
from ydk.providers import CodecServiceProvider
from ydk.providers import NetconfServiceProvider
from ydk.services import CRUDService
from ydk.services import CodecService

from ydk.models.augmentation import ietf_aug_base_1, ydktest_aug_ietf_5

from test_utils import ParametrizedTestCase
from test_utils import get_device_info


AUGMENTED_XML_PAYLOAD = """<cpython xmlns="http://cisco.com/ns/yang/ietf-aug-base-1">
  <doc>
    <aug-5-identityref xmlns:yaug-five="http://cisco.com/ns/yang/yaug-five">yaug-five:derived-aug-identity</aug-5-identityref>
    <disutils>
      <four-aug-list xmlns="http://cisco.com/ns/yang/yaug-four">
        <enabled>true</enabled>
      </four-aug-list>
    </disutils>
    <ydktest-aug-4 xmlns="http://cisco.com/ns/yang/yaug-four">
      <aug-four>aug four</aug-four>
    </ydktest-aug-4>
    <ydktest-aug-1 xmlns="http://cisco.com/ns/yang/yaug-one">
      <aug-one>aug one</aug-one>
    </ydktest-aug-1>
    <ydktest-aug-2 xmlns="http://cisco.com/ns/yang/yaug-two">
      <aug-two>aug two</aug-two>
    </ydktest-aug-2>
  </doc>
  <lib>
    <ydktest-aug-4 xmlns="http://cisco.com/ns/yang/yaug-four">
      <ydktest-aug-nested-4>
        <aug-four>aug four</aug-four>
      </ydktest-aug-nested-4>
    </ydktest-aug-4>
    <ydktest-aug-1 xmlns="http://cisco.com/ns/yang/yaug-one">
      <ydktest-aug-nested-1>
        <aug-one>aug one</aug-one>
      </ydktest-aug-nested-1>
    </ydktest-aug-1>
    <ydktest-aug-2 xmlns="http://cisco.com/ns/yang/yaug-two">
      <ydktest-aug-nested-2>
        <aug-two>aug two</aug-two>
      </ydktest-aug-nested-2>
    </ydktest-aug-2>
  </lib>
</cpython>"""

AUGMENTED_JSON_PAYLOAD = """{
  "ietf-aug-base-1:cpython": {
    "doc": {
      "disutils": {
        "ydktest-aug-ietf-4:four-aug-list": {
          "enabled": true
        }
      },
      "ydktest-aug-ietf-4:ydktest-aug-4": {
        "aug-four": "aug four"
      },
      "ydktest-aug-ietf-1:ydktest-aug-1": {
        "aug-one": "aug one"
      },
      "ydktest-aug-ietf-2:ydktest-aug-2": {
        "aug-two": "aug two"
      },
      "ydktest-aug-ietf-5:aug-5-identityref": "ydktest-aug-ietf-5:derived-aug-identity"
    },
    "lib": {
      "ydktest-aug-ietf-4:ydktest-aug-4": {
        "ydktest-aug-nested-4": {
          "aug-four": "aug four"
        }
      },
      "ydktest-aug-ietf-1:ydktest-aug-1": {
        "ydktest-aug-nested-1": {
          "aug-one": "aug one"
        }
      },
      "ydktest-aug-ietf-2:ydktest-aug-2": {
        "ydktest-aug-nested-2": {
          "aug-two": "aug two"
        }
      }
    }
  }
}"""

class SanityYang(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        tmp_dir = tempfile.mkdtemp()
        repo = Repository(tmp_dir)

        cls.ncc_empty_repo = NetconfServiceProvider(
            repo,
            cls.hostname,
            cls.username,
            cls.password,
            cls.port,
            cls.protocol,
            cls.on_demand,
            cls.timeout)
        cls.ncc = NetconfServiceProvider(
            cls.hostname,
            cls.username,
            cls.password,
            cls.port,
            cls.protocol,
            cls.on_demand,
            cls.common_cache,
            cls.timeout)
        cls.crud = CRUDService()

        cls.codec_provider = CodecServiceProvider()
        cls.codec = CodecService()

    def test_on_demand_downloading(self):
        # create augmentation configuration
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

        self.crud.read(self.ncc_empty_repo, ietf_aug_base_1.Cpython())

    def test_on_demand_loading_xml(self):
        self.codec_provider.encoding = EncodingFormat.XML
        entity1 = self.codec.decode(self.codec_provider, AUGMENTED_XML_PAYLOAD)
        self.assertEqual(entity1.lib.ydktest_aug_4.ydktest_aug_nested_4.aug_four, "aug four")


    def test_on_demand_loading_json(self):
        self.codec_provider.encoding = EncodingFormat.JSON
        entity1 = self.codec.decode(self.codec_provider, AUGMENTED_JSON_PAYLOAD)
        self.assertEqual(entity1.doc.aug_5_identityref, ydktest_aug_ietf_5.DerivedAugIdentity())



if __name__ == '__main__':
    device, non_demand, common_cache, timeout = get_device_info()

    suite = unittest.TestSuite()
    suite.addTest(ParametrizedTestCase.parametrize(
        SanityYang,
        device=device,
        non_demand=non_demand,
        common_cache=common_cache,
        timeout=timeout))
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
