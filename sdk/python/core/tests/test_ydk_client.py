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
    test_ydk_client.py
        Unittest for DELETE object.
"""
from __future__ import print_function
from __future__ import absolute_import
import unittest
from compare import is_equal

from ydk.services import NetconfService
from ydk.providers import NativeNetconfServiceProvider, NetconfServiceProvider
from ydk.errors import YPYServiceProviderError, YPYErrorCode
from ydk.models.ydktest import ydktest_sanity as ysanity


class SanityTest(unittest.TestCase):
    PROVIDER_TYPE = "non-native"

    @classmethod
    def setUpClass(self):
        self.ydk_client = NativeNetconfServiceProvider(address='127.0.0.1',
                                                       username='admin',
                                                       password='admin',
                                                       protocol='ssh',
                                                       port=12022)
        self.netconf_service = NetconfService()

    @classmethod
    def tearDownClass(self):
        self.ydk_client.close()
        pass

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_payload(self):
        result = self.ydk_client.execute('''
            <rpc xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
            <edit-config>
            <target><candidate/></target>
            <config>
                <runner xmlns="http://cisco.com/ns/yang/ydktest-sanity"><ytypes><built-in-t><number8>12</number8></built-in-t></ytypes></runner>
            </config>
            </edit-config>
            </rpc>''', '')
        self.assertIn('ok', result)

    def test_server_error(self):
        try:
            result = self.ydk_client.execute('''
                <rpc xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
                <edit-config>
                <target><candidate/></target>
                <config>
                    <runner xmlns="http://invalid.com"></runner>
                </config>
                </edit-config>
                </rpc>''', '')
            self.assertIn('ok', result)
        except Exception as e:
            self.assertIsInstance(e, YPYServiceProviderError)
            msg = str(e)
            self.assertEqual(msg, 'Server rejected request.\n\terror-type: protocol\n\terror-tag: unknown-namespace\n\t'
                             'error-severity: error\n\terror-path: /rpc/edit-config/config\n\tbad-element: runner\n'
                             '\tbad-namespace: http://invalid.com')

    def test_compare_clients(self):
        ncc = NetconfServiceProvider(address='127.0.0.1',
                                     username='admin',
                                     password='admin',
                                     protocol='ssh',
                                     port=12022)
        import time
        start_time = time.time()
        native_result = self.netconf_service.get(self.ydk_client, None)
        native_end_time = time.time()
        ncc_result = self.netconf_service.get(ncc, None)
        ncc_end_time = time.time()
        print('Native client time taken: %s seconds' % (native_end_time - start_time))
        print('NCClient time taken: %s seconds' % (ncc_end_time - native_end_time))
        self.assertEqual(True, is_equal(native_result, ncc_result))


if __name__ == '__main__':
    import sys
    suite = unittest.TestLoader().loadTestsFromTestCase(SanityTest)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
