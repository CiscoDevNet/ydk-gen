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

"""test_restconf_provider.py
RestconfServiceProvider test
"""
from __future__ import absolute_import

import os, sys, unittest

from ydk.providers import NetconfServiceProvider
from test_utils import ParametrizedTestCase
from test_utils import get_device_info


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.ncc = NetconfServiceProvider(
            cls.hostname,
            cls.username,
            cls.password,
            cls.port,
            cls.protocol,
            cls.on_demand,
            cls.common_cache,
            cls.timeout)

    def test_get_session(self):
        session = self.ncc.get_session()
        self.assertEqual(session is not None, True)

    def test_get_encoding(self):
        encoding = self.ncc.get_encoding()
        self.assertEqual(encoding is not None, True)

    def test_get_capabilities(self):
        capabilities = self.ncc.get_capabilities()
        self.assertEqual(capabilities is not None, True)

    # todo: test more signatures, negative test cases
    def test_keybase_auth(self):
        dir_path = os.path.dirname(os.path.realpath(__file__))

        private = "%s/ssh_host_rsa_key" % dir_path
        public = "%s/ssh_host_rsa_key.pub" % dir_path
        provider = NetconfServiceProvider(
            address = self.hostname,
            username = self.username,
            private_key_path = private,
            public_key_path = public,
            port = self.port)

        session = provider.get_session()
        self.assertEqual(session is not None, True)

        encoding = provider.get_encoding()
        self.assertEqual(encoding is not None, True)

        capabilities = provider.get_capabilities()
        self.assertEqual(capabilities is not None, True)


if __name__ == '__main__':
    device, non_demand, common_cache, timeout = get_device_info()

    suite = unittest.TestSuite()
    suite.addTest(ParametrizedTestCase.parametrize(
            SanityTest,
            device=device,
            non_demand=non_demand,
            common_cache=common_cache,
            timeout=timeout))
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
