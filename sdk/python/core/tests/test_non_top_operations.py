#  ----------------------------------------------------------------
# Copyright 2018 Cisco Systems
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

"""test_non_top_operations.py
Sanity test for ydktest-sanity.yang targetted specifically
to test support for non-top level objects in CRUD operations
"""

from __future__ import absolute_import
from __future__ import print_function

import unittest

from ydk.providers import NetconfServiceProvider
from ydk.services  import CRUDService, NetconfService
from ydk.filters import YFilter
from ydk.ext.services import Datastore

try:
    from ydk.models.ydktest.ydktest_sanity import Runner, ChildIdentity, Native
except ImportError:
    from ydk.models.ydktest.ydktest_sanity.runner.runner import Runner
    from ydk.models.ydktest.ydktest_sanity.native.native import Native
    from ydk.models.ydktest.ydktest_sanity.ydktest_sanity import ChildIdentity


class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(self):
        self.ncc = NetconfServiceProvider(
            "127.0.0.1",
            "admin",
            "admin",
            12022,
            )
        self.crud = CRUDService()

    def setUp(self):
        runner = Runner()
        self.crud.delete(self.ncc, runner)

    def test_delete_on_list_with_identitykey(self):
        a1 = Runner.OneList.IdentityList()
        a1.config.id = ChildIdentity()
        a1.id_ref = a1.config.id
        self.crud.create(self.ncc, a1)

        k = Runner.OneList.IdentityList()
        k.config.id = ChildIdentity()
        k.id_ref = k.config.id
        k.yfilter = YFilter.delete
        self.crud.update(self.ncc, k)

        runner_read = self.crud.read(self.ncc, Runner())
        self.assertIsNone(runner_read)

    def test_iden_list(self):
        # CREATE
        il = Runner.OneList.IdentityList()
        il.config.id = ChildIdentity()
        il.id_ref = ChildIdentity()
        self.crud.create(self.ncc, il)

        # READ & VALIDATE
        runner_filter = Runner()
        read_one = self.crud.read(self.ncc, runner_filter.one_list)
        self.assertIsNotNone(read_one)

        read_il = read_one.identity_list.get(ChildIdentity().to_string())
        self.assertIsNotNone(read_il)
        read_il.parent = None
        self.assertEqual(read_il, il)

        # DELETE & VALIDATE
        self.crud.delete(self.ncc, il)
        runner_read = self.crud.read(self.ncc, Runner())
        self.assertIsNone(runner_read)

    def test_crud_delete_container(self):
        # Build loopback configuration
        address = Native.Interface.Loopback.Ipv4.Address()
        address.ip = "2.2.2.2"
        address.netmask = "255.255.255.255"

        loopback = Native.Interface.Loopback()
        loopback.name = 2222
        loopback.ipv4.address.append(address)

        native = Native()
        native.interface.loopback.append(loopback)

        crud = CRUDService()
        result = crud.create(self.ncc, native)
        self.assertTrue(result)

        # Read ipv4 configuration
        native = Native()
        loopback = Native.Interface.Loopback()
        loopback.name = 2222
        native.interface.loopback.append(loopback)
        ipv4_config = crud.read(self.ncc, loopback.ipv4)
        self.assertIsNotNone(ipv4_config)
        self.assertEqual(ipv4_config.address['2.2.2.2'].netmask, "255.255.255.255")

        # Remove ipv4 configuration
        native = Native()
        loopback = Native.Interface.Loopback()
        loopback.name = 2222
        native.interface.loopback.append(loopback)
        result = crud.delete(self.ncc, loopback.ipv4)
        self.assertTrue(result)

        # Delete configuration
        native = Native()
        result = crud.delete(self.ncc, native)
        self.assertEqual(result, True)

    def test_netconf_delete_container(self):
        # Build loopback configuration
        address = Native.Interface.Loopback.Ipv4.Address()
        address.ip = "2.2.2.2"
        address.netmask = "255.255.255.255"

        loopback = Native.Interface.Loopback()
        loopback.name = 2222
        loopback.ipv4.address.append(address)

        native = Native()
        native.interface.loopback.append(loopback)

        ns = NetconfService()
        result = ns.edit_config(self.ncc, Datastore.candidate, native)
        self.assertTrue(result)

        # Read ipv4 configuration
        native = Native()
        loopback = Native.Interface.Loopback()
        loopback.name = 2222
        native.interface.loopback.append(loopback)
        ipv4_config = ns.get_config(self.ncc, Datastore.candidate, loopback.ipv4)
        self.assertIsNotNone(ipv4_config)
        self.assertEqual(ipv4_config.address['2.2.2.2'].netmask, "255.255.255.255")

        # Delete configuration
        result = ns.discard_changes(self.ncc)
        self.assertEqual(result, True)


if __name__ == '__main__':
    suite = unittest.TestSuite()
    testloader = unittest.TestLoader()
    testnames = testloader.getTestCaseNames(SanityTest)
    for name in testnames:
        suite.addTest(SanityTest(name))
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
