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

import sys
import unittest

from ydk.errors import YInvalidArgumentError
from ydk.types  import EntityCollection, Filter, Config

from ydk.models.ydktest import ydktest_sanity as ysanity

from test_utils import assert_with_error

test_add_unsupported_pattern = '''Argument <(type|class) 'str'> is not supported by EntityCollection class; data ignored'''

class SanityTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        pass

    def test_entity_collection_empty(self):
        anydata = EntityCollection()
        self.assertEqual(len(anydata), 0)
        self.assertEqual(format(anydata), "Entities in EntityCollection: []")

    def test_config_add_del(self):
        runner = ysanity.Runner()
        native = ysanity.Native()

        config = Config()
        config.append(runner)
        self.assertEqual(len(config), 1)
        self.assertEqual(format(config), "Entities in Config: ['ydk.models.ydktest.ydktest_sanity.Runner']")

        config.append(native)
        self.assertEqual(len(config), 2)
        self.assertEqual(config.has_key(runner.path()), True)
        self.assertEqual(format(config), "Entities in Config: ['ydk.models.ydktest.ydktest_sanity.Runner', 'ydk.models.ydktest.ydktest_sanity.Native']")

        del config[native.path()]
        self.assertEqual(len(config), 1)
        self.assertEqual(config.has_key(native.path()), False)

        del config[runner]
        self.assertEqual(len(config), 0)

    def test_filter_list(self):
        runner = ysanity.Runner()
        native = ysanity.Native()

        read_filter = Filter(runner, native)

        self.assertEqual(read_filter.keys(), ['ydktest-sanity:runner', 'ydktest-sanity:native'])
        all_entities = read_filter.entities()
        self.assertEqual(format(all_entities[0]), 'ydk.models.ydktest.ydktest_sanity.Runner')
        self.assertEqual(format(all_entities[1]), 'ydk.models.ydktest.ydktest_sanity.Native')

        self.assertEqual(format(read_filter[runner]), 'ydk.models.ydktest.ydktest_sanity.Runner')
        self.assertEqual(format(read_filter[runner.path()]), 'ydk.models.ydktest.ydktest_sanity.Runner')
        read_filter.clear()

    def test_access_config_by_key(self):
        runner = ysanity.Runner()
        native = ysanity.Native()

        config = Config(runner, native)
        for key in config.keys():
            if key=='ydktest-sanity:runner':
                self.assertEqual(config[key].path(), 'ydktest-sanity:runner')
            else:
                self.assertEqual(config[key].path(), 'ydktest-sanity:native')
        config.clear()

    def test_access_config_by_item_no(self):
        runner = ysanity.Runner()
        native = ysanity.Native()

        config = Config([runner, native])
        self.assertEqual(format(config[0]), "ydk.models.ydktest.ydktest_sanity.Runner")
        self.assertEqual(format(config[1]), "ydk.models.ydktest.ydktest_sanity.Native")
        config.clear()

    def test_access_config_by_iter(self):
        runner = ysanity.Runner()
        native = ysanity.Native()

        config = Config(runner, native)
        for entity in config:
            print(entity)

        self.assertEqual(len(config), 2)
        config.clear()

    @assert_with_error(test_add_unsupported_pattern, YInvalidArgumentError)
    def test_add_unsupported(self):
        anydata = EntityCollection()
        anydata.append('native')
        self.assertEqual(len(anydata), 0)

    def test_init_delete(self):
        runner = ysanity.Runner()
        native = ysanity.Native()

        anydata = Config(runner)
        deleted = anydata.pop(0)
        self.assertEqual(deleted.__class__.__name__, "Runner")

        anydata = Config()
        anydata.append([runner, native])

        deleted = anydata.pop('ydktest-sanity:native')
        self.assertEqual(len(anydata), 1)
        self.assertEqual(deleted.__class__.__name__, "Native")

        deleted = anydata.pop('ydktest-sanity:native')
        self.assertEqual(deleted, None)

        deleted = anydata.pop(runner)
        self.assertEqual(deleted.__class__.__name__, "Runner")

if __name__ == '__main__':
    suite = unittest.TestSuite()
    testloader = unittest.TestLoader()
    testnames = testloader.getTestCaseNames(SanityTest)
    for name in testnames:
        suite.addTest(SanityTest(name))
    ret = not unittest.TextTestRunner(verbosity=2).run(suite).wasSuccessful()
    sys.exit(ret)
