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
# This file has been modified by Yan Gorelik, YDK Solutions.
# All modifications in original under CiscoDevNet domain
# introduced since October 2019 are copyrighted.
# All rights reserved under Apache License, Version 2.0.
# ------------------------------------------------------------------

from __future__ import absolute_import

import sys
import unittest
import logging

from ydk.path.sessions import NetconfSession
from ydk.path import Codec
from ydk.types import EncodingFormat

from test_utils import ParametrizedTestCase
from test_utils import get_device_info


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.nc_session = NetconfSession(
            cls.hostname,
            cls.username,
            cls.password,
            cls.port,
            cls.protocol,
            cls.on_demand,
            cls.common_cache,
            cls.timeout)
        cls.root_schema = cls.nc_session.get_root_schema()
        cls.codec = Codec()

    def _delete_runner(self):
        runner = self.root_schema.create_datanode("ydktest-sanity:runner")
        xml = self.codec.encode(runner, EncodingFormat.XML, True)
        create_rpc = self.root_schema.create_rpc("ydk:delete")
        create_rpc.get_input_node().create_datanode("entity", xml)
        # RuntimeError: YCoreError: YCodecError:Schema node not found.. Path: input/config if invoked
        create_rpc(self.nc_session)

    def tearDown(self):
        # RuntimeError: YCoreError: YCodecError:Schema node not found.. Path: input/config if invoked
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
        runner = self.root_schema.create_datanode("ydktest-sanity:runner")
        for (leaf_path, leaf_value) in leaf_path_values:
            runner.create_datanode(leaf_path, leaf_value)

        xml = self.codec.encode(runner, EncodingFormat.XML, True)
        create_rpc = self.root_schema.create_rpc("ydk:create")
        create_rpc.get_input_node().create_datanode("entity", xml)
        create_rpc(self.nc_session)

        runner_filter = self.root_schema.create_datanode("ydktest-sanity:runner")
        xml_filter = self.codec.encode(runner_filter, EncodingFormat.XML, False)
        read_rpc = self.root_schema.create_rpc("ydk:read")
        read_rpc.get_input_node().create_datanode("filter", xml_filter)
        runner_read = read_rpc(self.nc_session)
        xml_read = self.codec.encode(runner_read, EncodingFormat.XML, True)
        self.maxDiff = None
        self.assertEqual(xml, xml_read)

    def test_rpcs(self):
        getc = self.root_schema.create_rpc("ietf-netconf:get-config")
        self.assertEqual(getc.has_output_node() , True)
        get = self.root_schema.create_rpc("ietf-netconf:get")
        self.assertEqual(get.has_output_node() , True)
        editc = self.root_schema.create_rpc("ietf-netconf:edit-config")
        self.assertEqual(editc.has_output_node() , False)
        val = self.root_schema.create_rpc("ietf-netconf:validate")
        self.assertEqual(val.has_output_node() , False)
        com = self.root_schema.create_rpc("ietf-netconf:commit")
        self.assertEqual(com.has_output_node() , False)
        lo = self.root_schema.create_rpc("ietf-netconf:lock")
        self.assertEqual(lo.has_output_node() , False)

    def test_codec(self):
        self.root_schema.create_datanode('ydktest-sanity:runner')
        self.root_schema.create_rpc('ietf-netconf-monitoring:get-schema')
        a = self.codec.decode(self.root_schema,
                         '''<runner xmlns="http://cisco.com/ns/yang/ydktest-sanity">
                     <ytypes>
                     <built-in-t>
                     <bits-value>disable-nagle auto-sense-speed</bits-value>
                     </built-in-t>
                     </ytypes>
                     </runner>''',
                     EncodingFormat.XML)
        self.assertNotEqual(a, None)

        pl2 = ''' <data xmlns="urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring">module xyz { } </data>'''
        d2 = self.codec.decode_rpc_output(self.root_schema, pl2, "/ietf-netconf-monitoring:get-schema", EncodingFormat.XML)
        self.assertNotEqual(d2 , None)
        x2 = self.codec.encode(d2, EncodingFormat.XML, False)
        self.assertEqual(
            x2, "<get-schema xmlns=\"urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring\"><data>module xyz { } </data></get-schema>")

    def test_get_schema(self):
        get_schema_rpc = self.root_schema.create_rpc("ietf-netconf-monitoring:get-schema")
        get_schema_rpc.get_input_node().create_datanode("identifier", "ydktest-sanity-types")

        res = get_schema_rpc(self.nc_session)

        xml = self.codec.encode(res, EncodingFormat.XML, False)
        self.assertNotEqual( len(xml), 0 )

    def test_get_running_config(self):
        get_config_rpc = self.root_schema.create_rpc("ietf-netconf:get-config")
        get_config_rpc.get_input_node().create_datanode("source/running")

        response = get_config_rpc(self.nc_session)

        all_nodes = response.get_children()
        self.assertEqual( len(all_nodes)>=2, True )

        xml = self.codec.encode(response, EncodingFormat.XML, True)
        self.assertNotEqual( len(xml), 0 )

    def test_anyxml(self):
        get_rpc = self.root_schema.create_rpc('ietf-netconf:get')
        get_rpc.get_input_node().create_datanode(
            'filter',
            '''<?xml version="1.0"?><bgp xmlns="http://openconfig.net/yang/bgp"/>''')
        datanode = get_rpc(self.nc_session)
        self.assertIsNotNone(datanode)

        get_rpc = self.root_schema.create_rpc('ietf-netconf:get')
        get_rpc.get_input_node().create_datanode(
            'filter',
            '''<?xml version="1.0"?>
            <bgp xmlns="http://openconfig.net/yang/bgp"/>''')
        datanode = get_rpc(self.nc_session)
        self.assertIsNotNone(datanode)

    def test_anyxml_action(self):
        expected='''<data xmlns="http://cisco.com/ns/yang/ydktest-action">
  <action-node>
    <test>xyz</test>
  </action-node>
</data>
'''
        native = self.root_schema.create_datanode("ydktest-sanity-action:data", "")
        a = native.create_action("action-node")
        a.create_datanode("test", "xyz")

        xml = self.codec.encode(native, EncodingFormat.XML, True)
        self.assertEqual(xml, expected)

        try:
            native(self.nc_session)
        except Exception as e:
            self.assertEqual(isinstance(e, RuntimeError), True)

    def test_path_codec_list(self):
        root_shema = self.nc_session.get_root_schema()

        runner = root_shema.create_datanode("ydktest-sanity:runner")
        runner.create_datanode("one/number", "2")

        native = root_shema.create_datanode("ydktest-sanity:native")
        native.create_datanode("version", '0.1.0')

        nodes_encode = [runner, native]
        xml_encode = self.codec.encode(nodes_encode, EncodingFormat.XML, True)

        root_node = self.codec.decode(root_shema, xml_encode, EncodingFormat.XML)
        node_decode = root_node.get_children()

        xml_decode = self.codec.encode(node_decode, EncodingFormat.XML, True)
        self.assertEqual(xml_encode, xml_decode)

    def test_rpc_get_schema_no_decode(self):
        rpc = self.root_schema.create_rpc("ietf-netconf-monitoring:get-schema")
        rpc.get_input_node().create_datanode("identifier", "ydktest-sanity-action")

        reply = self.nc_session.execute_netconf_operation(rpc)
        self.assertTrue(len(reply)>0)
        #print(reply)

    def test_create_datanode(self):
        datanode_path = "/ydktest-sanity:runner/two-key-list[first='first-key'][second='2']/property"
        datanode_value = "TWO-KEY-LIST PROPERTY"
        datanode = self.root_schema.create_datanode(datanode_path, datanode_value)

        self.assertEqual(datanode_path, datanode.get_path())
        self.assertEqual(datanode_value, datanode.get_value())


if __name__ == '__main__':
    device, non_demand, common_cache, timeout = get_device_info()

    suite = unittest.TestSuite()
    suite.addTest(ParametrizedTestCase.parametrize(
        SanityTest,
        device = device,
        non_demand = non_demand,
        common_cache = common_cache,
        timeout = timeout))
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
