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

from __future__ import absolute_import
import unittest
from ydk.providers import NetconfServiceProvider
from ydk.path import CodecService
from ydk.types import EncodingFormat


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider('127.0.0.1', 'admin', 'admin', 12022)
        self.root_schema = self.ncc.get_root_schema()
        self.codec = CodecService()

    def _delete_runner(self):
        runner = self.root_schema.create("ydktest-sanity:runner")
        xml = self.codec.encode(runner, EncodingFormat.XML, True)
        create_rpc = self.root_schema.rpc("ydk:delete")
        create_rpc.input().create("entity", xml)
        create_rpc(self.ncc)

    def tearDown(self):
        self._delete_runner()

    def test_leafs(self):
        leaf_path_values = [("ytypes/built-in-t/number8", "2"),
                            ("ytypes/built-in-t/number16", "-32"),
                            ("ytypes/built-in-t/number32", "19"),
                            ("ytypes/built-in-t/number64", "-9223372036854775808"),
                            ("ytypes/built-in-t/u_number32", "2"),
                            ("ytypes/built-in-t/embeded-enum", "zero"),
                            ("ytypes/built-in-t/younion", "none"),
                            ("ytypes/built-in-t/enum-llist[.='local']", ""),
                            ("ytypes/built-in-t/enum-llist[.='remote']", ""),
                            ("ytypes/built-in-t/younion-recursive", "18")]
        runner = self.root_schema.create("ydktest-sanity:runner")
        for (leaf_path, leaf_value) in leaf_path_values:
            runner.create(leaf_path, leaf_value)

        xml = self.codec.encode(runner, EncodingFormat.XML, True)
        create_rpc = self.root_schema.rpc("ydk:create")
        create_rpc.input().create("entity", xml)
        create_rpc(self.ncc)

        runner_filter = self.root_schema.create("ydktest-sanity:runner")
        xml_filter = self.codec.encode(runner_filter, EncodingFormat.XML, False)
        read_rpc = self.root_schema.rpc("ydk:read")
        read_rpc.input().create("filter", xml_filter)
        runner_read = read_rpc(self.ncc)
        xml_read = self.codec.encode(runner_read, EncodingFormat.XML, True)
        self.maxDiff = None
        self.assertEqual(xml, xml_read)


if __name__ == '__main__':
    import sys

    suite = unittest.TestLoader().loadTestsFromTestCase(SanityTest)
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
