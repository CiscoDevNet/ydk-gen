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

"""test_netconf_operations.py
test YFilter
"""
from __future__ import absolute_import

import sys
import unittest

from ydk.errors import YServiceProviderError
from ydk.models.ydktest import ydktest_sanity as ysanity
from ydk.providers import NetconfServiceProvider
from ydk.services import CRUDService
from ydk.filters import YFilter

from test_utils import assert_with_error
from test_utils import ParametrizedTestCase
from test_utils import get_device_info


test_create_pattern = """<\?xml version="1.0" encoding="UTF-8"\?>
<rpc-reply xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="[0-9]+">
  <rpc-error>
    <error-type>application</error-type>
    <error-tag>data-exists</error-tag>
    <error-severity>error</error-severity>
    <error-path xmlns:ydkut="http://cisco.com/ns/yang/ydktest-sanity" xmlns:nc="urn:ietf:params:xml:ns:netconf:base:1.0">
    /nc:rpc/nc:edit-config/nc:config/ydkut:runner/ydkut:one-list/ydkut:ldata\[ydkut:number='1'\]
  </error-path>
    <error-info>
      <bad-element>ldata</bad-element>
    </error-info>
  </rpc-error>
</rpc-reply>"""
test_delete_pattern = """<\?xml version="1.0" encoding="UTF-8"\?>
<rpc-reply xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="[0-9]+">
  <rpc-error>
    <error-type>application</error-type>
    <error-tag>data-missing</error-tag>
    <error-severity>error</error-severity>
    <error-path xmlns:ydkut="http://cisco.com/ns/yang/ydktest-sanity" xmlns:nc="urn:ietf:params:xml:ns:netconf:base:1.0">
    /nc:rpc/nc:edit-config/nc:config/ydkut:runner/ydkut:one-list/ydkut:ldata\[ydkut:number='1'\]
  </error-path>
    <error-info>
      <bad-element>ldata</bad-element>
    </error-info>
  </rpc-error>
</rpc-reply>"""
test_delete_leaf_pattern = """<\?xml version="1.0" encoding="UTF-8"\?>
<rpc-reply xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="[0-9]+">
  <rpc-error>
    <error-type>application</error-type>
    <error-tag>data-missing</error-tag>
    <error-severity>error</error-severity>
    <error-path xmlns:ydkut="http://cisco.com/ns/yang/ydktest-sanity" xmlns:nc="urn:ietf:params:xml:ns:netconf:base:1.0">
    /nc:rpc/nc:edit-config/nc:config/ydkut:runner/ydkut:ytypes/ydkut:built-in-t/ydkut:number8
  </error-path>
    <error-info>
      <bad-element>number8</bad-element>
    </error-info>
  </rpc-error>
</rpc-reply>"""
test_delete_leaflist_pattern = """<\?xml version="1.0" encoding="UTF-8"\?>
<rpc-reply xmlns="urn:ietf:params:xml:ns:netconf:base:1.0" message-id="[0-9]+">
  <rpc-error>
    <error-type>application</error-type>
    <error-tag>data-missing</error-tag>
    <error-severity>error</error-severity>
    <error-path xmlns:ydkut="http://cisco.com/ns/yang/ydktest-sanity" xmlns:nc="urn:ietf:params:xml:ns:netconf:base:1.0">
    /nc:rpc/nc:edit-config/nc:config/ydkut:runner/ydkut:ytypes/ydkut:built-in-t/ydkut:enum-llist\[.="local"\]
  </error-path>
    <error-info>
      <bad-element>enum-llist</bad-element>
    </error-info>
  </rpc-error>
</rpc-reply>"""


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
        cls.crud = CRUDService()

    def setUp(self):
        crud = CRUDService()
        runner = ysanity.Runner()
        crud.delete(self.ncc, runner)

    def test_replace(self):
        runner_create = ysanity.Runner()
        runner_create.ytypes.built_in_t.number8 = 10
        self.crud.create(self.ncc, runner_create)

        runner_empty = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_empty)

        self.assertEqual(runner_create, runner_read)

        # REPLACE
        runner = runner_create
        runner.ytypes.built_in_t.number8 = 25
        runner.yfilter = YFilter.replace
        self.crud.update(self.ncc, runner)

        runner_read = self.crud.read(self.ncc, runner_empty)
        self.assertEqual(runner, runner_read)

    @assert_with_error(test_create_pattern, YServiceProviderError)
    def test_create(self):
        runner = ysanity.Runner()
        e_1 = ysanity.Runner.OneList.Ldata()
        e_2 = ysanity.Runner.OneList.Ldata()
        e_1.number = 1
        e_1.name = 'foo'
        e_1.yfilter = YFilter.create
        e_2.number = 2
        e_2.name = 'bar'
        e_2.yfilter = YFilter.create
        runner.one_list.ldata.extend([e_1, e_2])
        self.crud.update(self.ncc, runner)

        # CREATE AGAIN WITH ERROR
        self.crud.update(self.ncc, runner)

    @assert_with_error(test_delete_pattern, YServiceProviderError)
    def test_delete(self):
        runner = ysanity.Runner()
        e_1 = ysanity.Runner.OneList.Ldata()
        e_2 = ysanity.Runner.OneList.Ldata()
        e_1.number = 1
        e_1.name = 'foo'
        e_1.yfilter = YFilter.create
        e_2.number = 2
        e_2.name = 'bar'
        e_2.yfilter = YFilter.create
        runner.one_list.ldata.extend([e_1, e_2])
        self.crud.update(self.ncc, runner)

        # DELETE
        runner = ysanity.Runner()
        e_1 = ysanity.Runner.OneList.Ldata()
        e_1.number = 1
        e_1.yfilter = YFilter.delete
        runner.one_list.ldata.append(e_1)
        self.crud.update(self.ncc, runner)

        # DELETE AGAIN WITH ERROR
        self.crud.update(self.ncc, runner)

    def test_remove(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number8 = 25
        runner.yfilter = YFilter.merge
        self.crud.update(self.ncc, runner)

        # REMOVE
        runner = ysanity.Runner()
        runner.yfilter = YFilter.remove
        self.crud.update(self.ncc, runner)

        # REMOVE AGAIN WITH NO ERROR
        self.crud.update(self.ncc, runner)

    def test_merge(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number8 = 25
        self.crud.create(self.ncc, runner)

        # MERGE
        runner.ytypes.built_in_t.number8 = 32
        runner.yfilter = YFilter.merge
        self.crud.update(self.ncc, runner)

        runner_empty = ysanity.Runner()
        runner_read = self.crud.read(self.ncc, runner_empty)
        self.assertEqual(runner, runner_read)

    @assert_with_error(test_delete_leaf_pattern, YServiceProviderError)
    def test_delete_leaf(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.number8 = 10
        self.crud.create(self.ncc, runner)

        # DELETE
        runner.ytypes.built_in_t.number8 = YFilter.delete
        self.crud.update(self.ncc, runner)

        # DELETE AGAIN WITH ERROR
        self.crud.update(self.ncc, runner)

    @assert_with_error(test_delete_leaflist_pattern, YServiceProviderError)
    def test_delete_leaflist(self):
        runner = ysanity.Runner()
        runner.ytypes.built_in_t.enum_llist.append(ysanity.YdkEnumTest.local)
        self.crud.create(self.ncc, runner)

        # DELETE
        runner.ytypes.built_in_t.enum_llist = YFilter.delete
        self.crud.update(self.ncc, runner)

        # DELETE AGAIN WITH ERROR
        self.crud.update(self.ncc, runner)


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
