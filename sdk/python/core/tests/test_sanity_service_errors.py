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

import re
import sys
import unittest

from ydk.services import CRUDService, Datastore, ExecutorService, CodecService, NetconfService
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.providers import NetconfServiceProvider, CodecServiceProvider
from ydk.types import Empty, EncodingFormat
from ydk.errors import YServiceError

from test_utils import assert_with_error
from test_utils import ParametrizedTestCase
from test_utils import get_device_info

from ydk.models.ydktest import ietf_netconf

def _get_runner_entity():
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

_entity_pattern = "'provider' and 'entity_holder' cannot be None"
_payload_pattern = "'provider' and 'payload_holder' cannot be None"

class SanityCodec(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.codec = CodecService()
        self.provider = CodecServiceProvider(type=EncodingFormat.XML)

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
</runner>"""

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

    @assert_with_error(_entity_pattern, YServiceError)
    def test_encode_invalid_1(self):
        self.codec.encode(self.provider, None)

    @assert_with_error(_entity_pattern, YServiceError)
    def test_encode_invalid_2(self):
        self.codec.encode(None, _get_runner_entity())

    @assert_with_error(_entity_pattern, YServiceError)
    def test_encode_invalid_3(self):
        self.codec.encode(None, None)

    @assert_with_error(_payload_pattern, YServiceError)
    def test_decode_invalid_1(self):
        self.codec.decode(None, self._enum_payload_2)

    @assert_with_error(_payload_pattern, YServiceError)
    def test_decode_invalid_2(self):
        self.codec.decode(self.provider, None)

    @assert_with_error(_payload_pattern, YServiceError)
    def test_decode_invalid_3(self):
        self.codec.decode(None, None)

class SanityCRUD(unittest.TestCase):

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
        cls.crud = CRUDService()

    def setUp(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    _error_pattern_entity = "'provider' and 'entity' cannot be None"
    _error_pattern_filter = "provider cannot be None"

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_create_invalid_1(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number8 = 0
        self.crud.create(None, runner)

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_create_invalid_2(self):
        self.crud.create(self.ncc, None)

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_create_invalid_3(self):
        self.crud.create(None, None)

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_delete_invalid_1(self):
        runner = ysanity.Runner()
        self.crud.delete(None, runner)

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_delete_invalid_2(self):
        self.crud.delete(self.ncc, None)

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_delete_invalid_3(self):
        self.crud.delete(None, None)

    @assert_with_error(_error_pattern_filter, YServiceError)
    def test_crud_read_invalid_1(self):
        runner_read = ysanity.Runner()
        self.crud.read(None, runner_read)

    @assert_with_error(_error_pattern_filter, YServiceError)
    def test_crud_read_invalid_3(self):
        self.crud.read(None, None)

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_update_invalid_1(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.bool_value = True
        self.crud.update(None, runner)

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_update_invalid_2(self):
        self.crud.update(self.ncc, None)

    @assert_with_error(_error_pattern_entity, YServiceError)
    def test_crud_update_invalid_3(self):
        self.crud.update(None, None)


class SanityExecutor(unittest.TestCase):

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
        cls.executor = ExecutorService()
        cls.codec = CodecService()
        cls.codec_provider = CodecServiceProvider(type=EncodingFormat.XML)

    def setUp(self):
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def test_execute_rpc_invalid_1(self):
        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'

        edit_rpc = ietf_netconf.EditConfig()
        edit_rpc.input.target.candidate = Empty()
        runner_xml = self.codec.encode(self.codec_provider, runner)
        edit_rpc.input.config = runner_xml
        try:
            self.executor.execute_rpc(None, edit_rpc)
        except YServiceError as err:
            expected_msg = "provider and entity cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_execute_rpc_invalid_2(self):
        try:
            self.executor.execute_rpc(self.ncc, None)
        except YServiceError as err:
            expected_msg = "provider and entity cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_execute_rpc_invalid_3(self):
        try:
            self.executor.execute_rpc(None, None)
        except YServiceError as err:
            expected_msg = "provider and entity cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

def _create_runner():
    runner = ysanity.Runner()
    runner.one.number = 1
    runner.one.name = 'runner:one:name'
    return runner

class SanityNetconf(unittest.TestCase):

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
        cls.netconf_service = NetconfService()

    def setUp(self):
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def test_copy_config_invalid_1(self):
        try:
            self.netconf_service.copy_config(self.ncc, target=None, source=Datastore.running)
        except YServiceError as err:
            expected_msg = "provider, target, and source/source_config cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_copy_config_invalid_2(self):
        try:
            self.netconf_service.copy_config(self.ncc, target=Datastore.candidate, source=None)
        except YServiceError as err:
            expected_msg = "provider, target, and source/source_config cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_copy_config_invalid_3(self):
        try:
            self.netconf_service.copy_config(self.ncc, target=None, source=None)
        except YServiceError as err:
            expected_msg = "provider, target, and source/source_config cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_copy_config_invalid_4(self):
        try:
            self.netconf_service.copy_config(
                self.ncc, target=Datastore.candidate, source=Datastore.running, with_defaults_option=1)
        except TypeError as err:
            expected_msg = "copy_config() got an unexpected keyword argument 'with_defaults_option'"
            self.assertTrue(expected_msg in err.args[0])
        else:
            raise Exception('YServiceError not raised')

    def test_delete_config_invalid(self):
        try:
            self.netconf_service.delete_config(self.ncc, target=None)
        except YServiceError as err:
            expected_msg = "provider and target cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_edit_config_invalid_1(self):
        try:
            runner = _create_runner()
            self.netconf_service.edit_config(self.ncc, None, runner)
        except YServiceError as err:
            expected_msg = "provider, target, and config cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_edit_config_invalid_2(self):
        try:
            self.netconf_service.edit_config(self.ncc, Datastore.candidate, None)
        except YServiceError as err:
            expected_msg = "provider, target, and config cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_edit_config_invalid_3(self):
        try:
            self.netconf_service.edit_config(self.ncc, None, None)
        except YServiceError as err:
            expected_msg = "provider, target, and config cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_edit_config_invalid_4(self):
        try:
            runner = _create_runner()
            self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner, default_operation=1)
        except YServiceError as err:
#             expected_msg = """incompatible function arguments. The following argument types are supported:
#     1. \(self: ydk_.services.NetconfService, provider: ydk_.providers.NetconfServiceProvider, target: ydk_.services.Datastore, config: ydk_.types.Entity, default_operation: (unicode|str)=[u]?'', test_option: (unicode|str)=[u]?'', error_option: (unicode|str)=[u]?''\) -> bool
#     2. \(self: ydk_.services.NetconfService, provider: ydk_.providers.NetconfServiceProvider, target: ydk_.services.Datastore, config: List\[ydk_.types.Entity\], default_operation: (unicode|str)=[u]?'', test_option: (unicode|str)=[u]?'', error_option: (unicode|str)=[u]?''\) -> bool
#
# Invoked with: <ydk_.services.NetconfService object at [0-9a-z]+>, <ydk.providers.netconf_provider.NetconfServiceProvider object at [0-9a-z]+>, Datastore.candidate, <ydk.models.ydktest.ydktest_sanity.Runner object at [0-9a-z]+>, 1, '', ''"""
#             res = re.match(expected_msg, err.message.strip())
#             self.assertIsNotNone(res)
            self.assertTrue("incompatible function arguments" in err.message)
        else:
            raise Exception('YServiceError not raised')

    def test_edit_config_invalid_5(self):
        try:
            runner = _create_runner()
            self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner, error_option=1)
        except YServiceError as err:
#             expected_msg = """incompatible function arguments. The following argument types are supported:
#     1. \(self: ydk_.services.NetconfService, provider: ydk_.providers.NetconfServiceProvider, target: ydk_.services.Datastore, config: ydk_.types.Entity, default_operation: (unicode|str)=[u]?'', test_option: (unicode|str)=[u]?'', error_option: (unicode|str)=[u]?''\) -> bool
#     2. \(self: ydk_.services.NetconfService, provider: ydk_.providers.NetconfServiceProvider, target: ydk_.services.Datastore, config: List\[ydk_.types.Entity\], default_operation: (unicode|str)=[u]?'', test_option: (unicode|str)=[u]?'', error_option: (unicode|str)=[u]?''\) -> bool
#
# Invoked with: <ydk_.services.NetconfService object at [0-9a-z]+>, <ydk.providers.netconf_provider.NetconfServiceProvider object at [0-9a-z]+>, Datastore.candidate, <ydk.models.ydktest.ydktest_sanity.Runner object at [0-9a-z]+>, '', '', 1"""
#             res = re.match(expected_msg, err.message.strip())
#             self.assertIsNotNone(res)
            self.assertTrue("incompatible function arguments" in err.message)
        else:
            raise Exception('YServiceError not raised')

    def test_edit_config_invalid_6(self):
        try:
            runner = _create_runner()
            self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner, test_option=1)
        except YServiceError as err:
#             expected_msg = """incompatible function arguments. The following argument types are supported:
#     1. \(self: ydk_.services.NetconfService, provider: ydk_.providers.NetconfServiceProvider, target: ydk_.services.Datastore, config: ydk_.types.Entity, default_operation: (unicode|str)=[u]?'', test_option: (unicode|str)=[u]?'', error_option: (unicode|str)=[u]?''\) -> bool
#     2. \(self: ydk_.services.NetconfService, provider: ydk_.providers.NetconfServiceProvider, target: ydk_.services.Datastore, config: List\[ydk_.types.Entity\], default_operation: (unicode|str)=[u]?'', test_option: (unicode|str)=[u]?'', error_option: (unicode|str)=[u]?''\) -> bool
#
# Invoked with: <ydk_.services.NetconfService object at [0-9a-z]+>, <ydk.providers.netconf_provider.NetconfServiceProvider object at [0-9a-z]+>, Datastore.candidate, <ydk.models.ydktest.ydktest_sanity.Runner object at [0-9a-z]+>, '', 1, ''"""
#             res = re.match(expected_msg, err.message.strip())
#             self.assertIsNotNone(res)
            self.assertTrue("incompatible function arguments" in err.message)
        else:
            raise Exception('YServiceError not raised')

    def test_get_config_invalid_1(self):
        try:
            runner = _create_runner()
            get_filter = ysanity.Runner()

            self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
            self.netconf_service.get_config(self.ncc, None, get_filter)
        except YServiceError as err:
            expected_msg = 'provider and source cannot be None'
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_get_config_invalid_2(self):
        try:
            runner = _create_runner()
            get_filter = ysanity.Runner()

            self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
            self.netconf_service.get_config(self.ncc, Datastore.candidate, get_filter, with_defaults_option=1)
        except TypeError as err:
            expected_msg = "get_config() got an unexpected keyword argument 'with_defaults_option'"
            self.assertTrue(expected_msg in err.args[0])
        else:
            raise Exception('TypeError not raised')

    def test_get_invalid(self):
        try:
            runner = _create_runner()
            get_filter = ysanity.Runner()

            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
            self.assertEqual(True, op)

            op = self.netconf_service.discard_changes(self.ncc)
            self.assertEqual(True, op)

            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
            self.assertEqual(True, op)

            op = self.netconf_service.commit(self.ncc)
            self.assertEqual(True, op)

            result = self.netconf_service.get(self.ncc, get_filter, with_defaults_option=1)
            self.assertEqual(is_equal(runner, result), True)
        except TypeError as err:
            expected_msg = "get() got an unexpected keyword argument 'with_defaults_option'"
            self.assertTrue(expected_msg in err.args[0])
        else:
            raise Exception('TypeError not raised')

    def test_lock_invalid(self):
        try:
            self.netconf_service.lock(self.ncc, None)
        except YServiceError as err:
            expected_msg = "provider and target cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_unlock_invalid(self):
        try:
            self.netconf_service.lock(self.ncc, Datastore.candidate)
            self.netconf_service.unlock(self.ncc, None)
        except YServiceError as err:
            expected_msg = "provider and target cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')

    def test_validate_invalid(self):
        try:
            self.netconf_service.validate(self.ncc)
        except YServiceError as err:
            expected_msg = "provider and source/source_config cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YServiceError not raised')


if __name__ == '__main__':
    device, non_demand, common_cache, timeout = get_device_info()

    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    for testCase in [SanityCRUD, SanityExecutor, SanityNetconf, SanityCodec]:
        suite.addTest(ParametrizedTestCase.parametrize(
            testCase,
            device=device,
            non_demand=non_demand,
            common_cache=common_cache,
            timeout=timeout))
    res=unittest.TextTestRunner(verbosity=2).run(suite)
    # sys.exit expects an integer, will throw libc++ abi error if use:
    # ret = res.wasSuccessful() # <-- ret is a bool
    # sys.exit(ret)
    if not res.wasSuccessful():
        sys.exit(1)
    else:
        sys.exit(0)
