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

import ydk.types as ytypes
import unittest

from ydk.services import CRUDService, ExecutorService
from ydk.services.meta_service import MetaService
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.models.ydktest import ydktest_sanity_types as ysanity_types
from ydk.models.ydktest import ydktest_types as y_types
from ydk.providers import NetconfServiceProvider, NativeNetconfServiceProvider
from ydk.types import Empty, DELETE, Decimal64
from compare import is_equal
from ydk.errors import YPYServiceError
try:
    from ydk.models.ietf import ietf_netconf
except:
    pass

from ydk.models.ydktest.ydktest_sanity import YdkEnumTestEnum, YdkEnumIntTestEnum

class SanityCodec(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        from ydk.providers import CodecServiceProvider
        from ydk.services import CodecService
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

class SanityCrud(unittest.TestCase):
    PROVIDER_TYPE = "non-native"
    @classmethod
    def setUpClass(self):
        if SanityCrud.PROVIDER_TYPE == "native":
            self.ncc = NativeNetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        else:
            self.ncc = NetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        self.crud = CRUDService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        print "\nIn method", self._testMethodName
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def tearDown(self):
        runner = ysanity.Runner()
        self.crud.delete(self.ncc, runner)

    def _create_runner(self):
        runner = ysanity.Runner()
        runner.ytypes = runner.Ytypes()
        runner.ytypes.built_in_t = runner.ytypes.BuiltInT()
        return runner

    def test_crud_create_invalid_1(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.number8 = 0
            self.crud.create(None, runner)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_crud_create_invalid_2(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.number8 = 0
            self.crud.create(self.ncc, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_crud_create_invalid_3(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.number8 = 0
            self.crud.create(None, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_crud_delete_invalid_1(self):
        try:
            runner = self._create_runner()
            self.crud.delete(None, runner)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

    def test_crud_delete_invalid_2(self):
        try:
            self.crud.delete(self.ncc, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

    def test_crud_delete_invalid_3(self):
        try:
            self.crud.delete(None, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

    def test_crud_read_invalid_1(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.bool_value = True
            self.crud.create(self.ncc, runner)
            # Read into Runner2
            runner1 = ysanity.Runner()
            self.crud.read(None, runner1)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'read_filter' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

    def test_crud_read_invalid_2(self):
        try:
            self.crud.read(self.ncc, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'read_filter' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

    def test_crud_read_invalid_3(self):
        try:
            self.crud.read(None, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'read_filter' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

    def test_crud_update_invalid_1(self):
        try:
            runner = self._create_runner()
            runner.ytypes.built_in_t.bool_value = True
            self.crud.create(self.ncc, runner)

            # Read into Runner2
            runner1 = ysanity.Runner()
            runner1 = self.crud.read(self.ncc, runner1)

            # Compare runners
            result = is_equal(runner, runner1)
            self.assertEqual(result, True)

            runner = self._create_runner()
            runner.ytypes.built_in_t.bool_value = False
            self.crud.update(None, runner)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

    def test_crud_update_invalid_2(self):
        try:
            self.crud.update(self.ncc, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

    def test_crud_update_invalid_3(self):
        try:
            self.crud.update(None, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not Raised')

class SanityExecutor(unittest.TestCase):
    PROVIDER_TYPE = "non-native"
    @classmethod
    def setUpClass(self):
        if SanityExecutor.PROVIDER_TYPE == "native":
            self.ncc = NativeNetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        else:
            self.ncc = NetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        self.executor = ExecutorService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        print "\nIn method", self._testMethodName
        from ydk.services import CRUDService
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def tearDown(self):
        pass

    def test_execute_rpc_invalid_1(self):
        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'

        edit_rpc = ietf_netconf.EditConfigRpc()
        edit_rpc.input.target.candidate = Empty()
        edit_rpc.input.config = runner
        try:
            op = self.executor.execute_rpc(None, edit_rpc)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'rpc' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_execute_rpc_invalid_2(self):
        try:
            op = self.executor.execute_rpc(self.ncc, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'rpc' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_execute_rpc_invalid_3(self):
        try:
            op = self.executor.execute_rpc(None, None)
        except YPYServiceError as err:
            expected_msg = "'provider' and 'rpc' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

class SanityMeta(unittest.TestCase):
    PROVIDER_TYPE = "non-native"
    @classmethod
    def setUpClass(self):
        if SanityMeta.PROVIDER_TYPE == "native":
            self.ncc = NativeNetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        else:
            self.ncc = NetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        print "\nIn method", self._testMethodName
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def tearDown(self):
        pass

    def test_normalize_meta_invalid_1(self):
        try:
            runner = ysanity.Runner()
            MetaService.normalize_meta(None, runner)
        except YPYServiceError as err:
            expected_msg = "'capabilities' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_normalize_meta_invalid_2(self):
        try:
            MetaService.normalize_meta(self.ncc._get_capabilities(), None)
        except YPYServiceError as err:
            expected_msg = "'capabilities' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_normalize_meta_invalid_3(self):
        try:
            runner = ysanity.Runner()
            MetaService.normalize_meta(None, None)
        except YPYServiceError as err:
            expected_msg = "'capabilities' and 'entity' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

class SanityNetconf(unittest.TestCase):
    PROVIDER_TYPE = "non-native"
    @classmethod
    def setUpClass(self):
        from ydk.providers import NetconfServiceProvider
        from ydk.services import NetconfService

        if SanityNetconf.PROVIDER_TYPE == "native":
            self.ncc = NativeNetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        else:
            self.ncc = NetconfServiceProvider(
                address='127.0.0.1',
                username='admin',
                password='admin',
                protocol='ssh',
                port=12022)
        self.netconf_service = NetconfService()

    @classmethod
    def tearDownClass(self):
        self.ncc.close()

    def setUp(self):
        from ydk.services import CRUDService
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

        print '\nIn method', self._testMethodName + ':'

    def tearDown(self):
        pass

    def _create_runner(self):
        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner:one:name'
        return runner

    def test_copy_config_invalid_1(self):
        try:
            from ydk.services import Datastore
            op = self.netconf_service.copy_config(self.ncc, target=None, source=Datastore.running)
        except YPYServiceError as err:
            expected_msg = "'target' and 'source' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_copy_config_invalid_2(self):
        try:
            from ydk.services import Datastore
            op = self.netconf_service.copy_config(self.ncc, target=Datastore.candidate, source=None)
        except YPYServiceError as err:
            expected_msg = "'target' and 'source' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_copy_config_invalid_3(self):
        try:
            op = self.netconf_service.copy_config(self.ncc, target=None, source=None)
        except YPYServiceError as err:
            expected_msg = "'target' and 'source' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_copy_config_invalid_4(self):
        try:
            from ydk.services import Datastore
            op = self.netconf_service.copy_config(
                self.ncc, target=Datastore.candidate, source=Datastore.running, with_defaults_option=1)
        except YPYServiceError as err:
            expected_msg = "optional arg 'with_defaults_option' must be of type ietf_netconf_with_defaults.WithDefaultsModeEnum"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_delete_config_invalid(self):
        try:
            op = self.netconf_service.delete_config(self.ncc, target=None)
        except YPYServiceError as err:
            expected_msg = "'target' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_edit_config_invalid_1(self):
        try:
            runner = self._create_runner()
            op = self.netconf_service.edit_config(self.ncc, None, runner)
        except YPYServiceError as err:
            expected_msg = "'target' and 'config' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_edit_config_invalid_2(self):
        try:
            from ydk.services import Datastore
            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, None)
        except YPYServiceError as err:
            expected_msg = "'target' and 'config' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_edit_config_invalid_3(self):
        try:
            op = self.netconf_service.edit_config(self.ncc, None, None)
        except YPYServiceError as err:
            expected_msg = "'target' and 'config' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_edit_config_invalid_4(self):
        try:
            from ydk.services import Datastore
            runner = self._create_runner()
            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner, default_operation=1)
        except YPYServiceError as err:
            expected_msg = "optional arg 'default_operation' must be of type ietf_netconf.EditConfigRpc.Input.DefaultOperationEnum"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_edit_config_invalid_5(self):
        try:
            from ydk.services import Datastore
            runner = self._create_runner()
            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner, error_option=1)
        except YPYServiceError as err:
            expected_msg = "optional arg 'error_option' must be of type ietf_netconf.EditConfigRpc.Input.ErrorOptionEnum"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_edit_config_invalid_6(self):
        try:
            from ydk.services import Datastore
            runner = self._create_runner()
            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner, test_option=1)
        except YPYServiceError as err:
            expected_msg = "optional arg 'test_option' must be of type ietf_netconf.EditConfigRpc.Input.TestOptionEnum"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_get_config_invalid_1(self):
        try:
            from ydk.services import Datastore
            runner = self._create_runner()
            get_filter = ysanity.Runner()

            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
            result = self.netconf_service.get_config(self.ncc, None, get_filter)
        except YPYServiceError as err:
            expected_msg = "'source' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_get_config_invalid_2(self):
        try:
            from ydk.services import Datastore
            runner = self._create_runner()
            get_filter = ysanity.Runner()

            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
            result = self.netconf_service.get_config(self.ncc, Datastore.candidate, get_filter, with_defaults_option=1)
        except YPYServiceError as err:
            expected_msg = "optional arg 'with_defaults_option' must be of type ietf_netconf_with_defaults.WithDefaultsModeEnum"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_get_invalid(self):
        try:
            from ydk.services import Datastore
            runner = self._create_runner()
            get_filter = ysanity.Runner()

            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
            self.assertIn('ok', op)

            op = self.netconf_service.discard_changes(self.ncc)
            self.assertIn('ok', op)

            op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
            self.assertIn('ok', op)

            op = self.netconf_service.commit(self.ncc)
            self.assertIn('ok', op)

            result = self.netconf_service.get(self.ncc, get_filter, with_defaults_option=1)
            self.assertEqual(is_equal(runner, result), True)
        except YPYServiceError as err:
            expected_msg = "optional arg 'with_defaults_option' must be of type ietf_netconf_with_defaults.WithDefaultsModeEnum"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_lock_invalid(self):
        try:
            op = self.netconf_service.lock(self.ncc, None)
        except YPYServiceError as err:
            expected_msg = "'target' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_unlock_invalid(self):
        try:
            from ydk.services import Datastore
            op = self.netconf_service.lock(self.ncc, Datastore.candidate)
            op = self.netconf_service.unlock(self.ncc, None)
        except YPYServiceError as err:
            expected_msg = "'target' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_validate_invalid(self):
        try:
            op = self.netconf_service.validate(self.ncc)
        except YPYServiceError as err:
            expected_msg = "'source' and 'config' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

    def test_something(self):
        try:
            op = self.netconf_service.validate(self.ncc)
        except YPYServiceError as err:
            expected_msg = "'source' and 'config' cannot be None"
            self.assertEqual(err.message, expected_msg)
        else:
            raise Exception('YPYServiceError not raised')

if __name__ == '__main__':
    import sys
    if len(sys.argv) > 1:
        provider_type = sys.argv.pop()
        SanityCrud.PROVIDER_TYPE = provider_type
        SanityExecutor.PROVIDER_TYPE = provider_type
        SanityMeta.PROVIDER_TYPE = provider_type
        SanityNetconf.PROVIDER_TYPE = provider_type
    unittest.main()
