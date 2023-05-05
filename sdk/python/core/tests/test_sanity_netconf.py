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

"""test_sanity_netconf.py
sanity tests for netconf
"""
from __future__ import absolute_import
from __future__ import print_function

import sys
import unittest
import logging

from ydk.errors import YModelError, YError, YServiceError
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.models.ydktest import openconfig_bgp as openconfig

from ydk.providers import NetconfServiceProvider
from ydk.services  import NetconfService, Datastore
from ydk.services  import CRUDService
from ydk.services  import CodecService
from ydk.providers import CodecServiceProvider
from ydk.ext.types import EncodingFormat

from test_utils import ParametrizedTestCase
from test_utils import get_device_info

from ydk.types  import Filter, Config

class SanityNetconf(ParametrizedTestCase):

    @classmethod
    def setUpClass(cls):
        cls.ncc = NetconfServiceProvider(
            cls.hostname,
            cls.username,
            cls.password,
            cls.port,
            cls.protocol,
            not cls.on_demand,
            cls.common_cache,
            cls.timeout)
        cls.netconf_service = NetconfService()
        cls.logger = logging.getLogger("ydk")
        handler = logging.StreamHandler()
        formatter = logging.Formatter(("%(asctime)s - %(name)s - %(levelname)s - %(message)s"))
        handler.setFormatter(formatter)
        cls.logger.addHandler(handler)
        cls.logger.setLevel(logging.ERROR)

    def setUp(self):
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def tearDown(self):
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def test_edit_commit_get(self):
        one = ysanity.Runner.One()
        one.number = 1
        one.name = 'runner-one-name'

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, one)
        self.assertEqual(True, op)

        get_filter = ysanity.Runner().One()
        one_result = self.netconf_service.get_config(self.ncc, Datastore.candidate, get_filter)
        self.assertEqual(one, one_result)

        op = self.netconf_service.commit(self.ncc)
        self.assertEqual(True, op)

        one_result = self.netconf_service.get(self.ncc, get_filter)
        self.assertEqual(one, one_result)

    def test_lock_unlock(self):
        op = self.netconf_service.lock(self.ncc, Datastore.running)
        self.assertEqual(True, op)

        op = self.netconf_service.unlock(self.ncc, Datastore.running)
        self.assertEqual(True, op)

    # Failing - NetconfService glue code needed
    def test_lock_unlock_fail(self):
        op = self.netconf_service.lock(self.ncc, Datastore.candidate)
        self.assertEqual(True, op)

        try:
            op = self.netconf_service.unlock(self.ncc, Datastore.running)
        except Exception as e:
            self.assertIsInstance(e, YError)

        op = self.netconf_service.unlock(self.ncc, Datastore.candidate)

    def test_validate(self):
        op = self.netconf_service.validate(self.ncc, source=Datastore.candidate)
        self.assertEqual(True, op)

        runner = ysanity.Runner()
        runner.one.number = 1
        runner.one.name = 'runner-one-name'
        op = self.netconf_service.validate(self.ncc, source=runner)
        self.assertEqual(True, op)

    def test_validate_fail(self):
        # should have been handled by YDK local validation
        pass

    def test_commit_discard(self):
        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner-two-name'
        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
        self.assertEqual(True, op)

        op = self.netconf_service.discard_changes(self.ncc)
        self.assertEqual(True, op)

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
        self.assertEqual(True, op)

        op = self.netconf_service.commit(self.ncc)
        self.assertEqual(True, op)

        result = self.netconf_service.get(self.ncc, get_filter)
        self.assertEqual(runner, result)

    #@unittest.skip('No message id in cancel commit payload')
    def test_confirmed_commit(self):
        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner-two-name'
        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
        self.assertEqual(True, op)

        op = self.netconf_service.commit(self.ncc, confirmed=True, confirm_timeout=120)
        self.assertEqual(True, op)

        result = self.netconf_service.get(self.ncc, get_filter)
        self.assertEqual(runner, result)

        op = self.netconf_service.cancel_commit(self.ncc)
        self.assertEqual(True, op)

    def test_copy_config(self):
        op = self.netconf_service.copy_config(self.ncc, Datastore.candidate, Datastore.running)
        self.assertEqual(True, op)

        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner-two-name'
        get_filter = ysanity.Runner()

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, runner)
        self.assertEqual(True, op)

        op = self.netconf_service.copy_config(self.ncc, Datastore.running, Datastore.candidate)
        self.assertEqual(True, op)

        result = self.netconf_service.get_config(self.ncc, Datastore.running, get_filter)
        self.assertEqual(runner, result)

        runner.two.name = '%smodified' % runner.two.name

        op = self.netconf_service.copy_config(self.ncc, Datastore.running, runner)
        self.assertEqual(True, op)

        result = self.netconf_service.get_config(self.ncc, Datastore.running, get_filter)
        self.assertEqual(runner, result)

    def test_edit_get_config_list(self):
        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner-two-name'

        native = ysanity.Native()
        native.hostname = 'NewHostName'
        native.version = '0.1.0a'

        edit_filter = [runner, native]

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, edit_filter)
        self.assertEqual(True, op)

        get_filter = [ysanity.Runner(), ysanity.Native()]
        config = self.netconf_service.get_config(self.ncc, Datastore.candidate, get_filter)
        self.assertEqual(edit_filter, config)

#         codec_service = CodecService()
#         codec_provider = CodecServiceProvider()
#         codec_provider.encoding = EncodingFormat.XML
#         for entity in config:
#             xml_encode = codec_service.encode(codec_provider, entity)
#             print(xml_encode)

        op = self.netconf_service.discard_changes(self.ncc)

    def test_edit_get_config_collection(self):
        runner = ysanity.Runner()
        runner.two.number = 2
        runner.two.name = 'runner-two-name'

        native = ysanity.Native()
        native.hostname = 'NewHostName'
        native.version = '0.1.0a'

        edit_config = Config([runner, native])

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, edit_config)
        self.assertEqual(True, op)

        get_filter = Filter([ysanity.Runner(), ysanity.Native()])
        config = self.netconf_service.get_config(self.ncc, Datastore.candidate, get_filter)
        self.assertEqual(edit_config, config)

        op = self.netconf_service.discard_changes(self.ncc)

    def test_delete_config(self):
        pass
        # startup and candidate cannot be both enabled in ConfD
        # op = self.netconf_service.delete_config(self.ncc, Datastore.startup)
        # self.assertEqual(True, op)

    # Error not thrown by TCP client, YError is populated instead
    def test_delete_config_fail(self):
        found = False
        try:
            self.netconf_service.delete_config(self.ncc, Datastore.running)
        except (YError, YModelError):
            found = True
        self.assertEqual(found, True)

    # Failing - NetconfService glue code needed
    def test_copy_config_fail(self):
        self.assertRaises(YServiceError,
                          self.netconf_service.copy_config,
                          self.ncc,
                          target=123,
                          source=456)

    # Failing - NetconfService glue code needed
    def test_edit_config_fail(self):
        self.assertRaises(YServiceError,
                          self.netconf_service.edit_config,
                          self.ncc,
                          Datastore.startup,
                          Datastore.candidate)

    # Failing - NetconfService glue code needed
    def test_get_config_fail(self):
        runner = ysanity.Runner()
        self.assertRaises(YServiceError,
                          self.netconf_service.get_config,
                          self.ncc,
                          "invalid-input",
                          runner)

    # Failing - NetconfService glue code needed
    def test_lock_fail(self):
        self.assertRaises(YServiceError,
                          self.netconf_service.lock,
                          self.ncc,
                          "invalid-input")

    # Failing - NetconfService glue code needed
    def test_unlock_fail(self):
        self.assertRaises(YServiceError,
                          self.netconf_service.unlock,
                          self.ncc,
                          "invalid-input")

    def test_sanity_crud_read_interface(self):
        address = ysanity.Native.Interface.Loopback.Ipv4.Address();
        address.ip = "2.2.2.2"
        address.netmask = "255.255.255.255"

        loopback = ysanity.Native.Interface.Loopback()
        loopback.name = 2222
        loopback.ipv4.address.append(address)

        native = ysanity.Native()
        native.interface.loopback.append(loopback)

        crud = CRUDService()
        result = crud.create(self.ncc, native)
        self.assertEqual(result, True)

        native_read = ysanity.Native()
        interfaces = crud.read(self.ncc, native_read)

        codec_service = CodecService()
        codec_provider = CodecServiceProvider()
        codec_provider.encoding = EncodingFormat.XML

        xml_encode = codec_service.encode(codec_provider, interfaces)
        print('\n===== Printing entity: {}'.format(interfaces))
        print(xml_encode)

        # Delete configuration
        result = crud.delete(self.ncc, native)
        self.assertEqual(result, True)

    def test_crud_read_list(self):
        crud = CRUDService()

        # Build configuration of multiple objects
        native = ysanity.Native()
        native.hostname = 'NativeHost'
        native.version = '0.1.0'

        bgp = openconfig.Bgp()
        bgp.global_.config.as_ = 65001
        bgp.global_.config.router_id = "1.2.3.4"

        create_list = [native, bgp];

        # Configure device
        result = crud.create(self.ncc, create_list)
        self.assertEqual(result, True)

        # Read configuration
        native_filter = ysanity.Native()
        bgp_filter = openconfig.Bgp()
        filter_list = [native_filter, bgp_filter];

        read_list = crud.read(self.ncc, filter_list)
        self.assertEqual(isinstance(read_list, list), True)
        self.assertEqual(len(read_list), 2)

        # Delete configuration
        result = crud.delete(self.ncc, create_list)
        self.assertEqual(result, True)

    def test_crud_read_collection(self):
        crud = CRUDService()

        # Build configuration of multiple objects
        create_list = Config();

        native = ysanity.Native()
        native.hostname = 'NativeHost'
        native.version = '0.1.0'
        create_list.append(native)

        bgp = openconfig.Bgp()
        bgp.global_.config.as_ = 65001
        bgp.global_.config.router_id = "1.2.3.4"
        create_list.append(bgp)

        create_list = Config([native, bgp])

        # Configure device
        result = crud.create(self.ncc, create_list)
        self.assertEqual(result, True)

        # Read configuration
        read_filter = Filter([ysanity.Native(), openconfig.Bgp()]);
        read_config = crud.read(self.ncc, read_filter)
        self.assertEqual(isinstance(read_config, Config), True)
        self.assertEqual(len(read_config), 2)

        # Print configuration
        codec_service = CodecService()
        codec_provider = CodecServiceProvider()
        codec_provider.encoding = EncodingFormat.XML
        for entity in read_config:
            xml = codec_service.encode(codec_provider, entity)
            print('\n===== Printing entity: {}'.format(entity))
            print(xml)

        # Delete configuration
        result = crud.delete(self.ncc, create_list)
        self.assertEqual(result, True)

    def test_netconf_get_candidate_config(self):
        self.logger.setLevel(logging.ERROR)
        ns = NetconfService()
        config = ns.get_config(self.ncc, Datastore.candidate)
        self.assertNotEqual(len(config), 0)
        print("\n==== Retrieved entities:")
        for entity in config:
            print(entity.path())

    def test_netconf_get_running_config(self):
        self.logger.setLevel(logging.ERROR)
        ns = NetconfService()
        config = ns.get_config(self.ncc)
        self.assertNotEqual(len(config), 0)
        print("\n==== Retrieved entities:")
        for entity in config:
            print(entity.path())

    def test_crud_get_all_config(self):
        self.logger.setLevel(logging.ERROR)
        crud = CRUDService()
        config = crud.read_config(self.ncc)
        self.assertNotEqual(len(config), 0)
        print("\n==== Retrieved entities:")
        for entity in config:
            print(entity.path())

    def test_crud_get_all(self):
        self.logger.setLevel(logging.ERROR)
        crud = CRUDService()
        try:
            config = crud.read(self.ncc)
            self.assertNotEqual(len(config), 0)
            print("\n==== Retrieved entities:")
            for entity in config:
                print(entity.path())
        except YError as err:
            self.logger.error("Failed to get device state due to error: {}".format(err.message))

    def test_passive_interface(self):
        ospf = ysanity.Runner.One.Ospf()
        ospf.id = 22
        ospf.passive_interface.interface = "xyz"
        test = ysanity.Runner.One.Ospf.Test()
        test.name = "abc"
        ospf.test.append(test)

        op = self.netconf_service.edit_config(self.ncc, Datastore.candidate, ospf)
        self.assertEqual(True, op)

        ospf_filter = ysanity.Runner.One.Ospf()
        ospf_filter.id = 22
        ospf_result = self.netconf_service.get_config(self.ncc, Datastore.candidate, ospf_filter)
        self.assertIsNotNone(ospf_result)
        self.assertEqual(ospf, ospf_result)

        op = self.netconf_service.discard_changes(self.ncc)
        self.assertEqual(True, op)

if __name__ == '__main__':
    device, non_demand, common_cache, timeout = get_device_info()

    suite = unittest.TestSuite()
    suite.addTest(ParametrizedTestCase.parametrize(
        SanityNetconf,
        device=device,
        non_demand=non_demand,
        common_cache=common_cache,
        timeout=timeout))
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
