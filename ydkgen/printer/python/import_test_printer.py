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
 import_test_printer.py

 YANG model driven API, python emitter.

"""

from ydkgen.api_model import Class, Enum, Package


class ImportTestPrinter(object):

    def __init__(self, ctx):
        self.ctx = ctx

    def print_import_tests(self, packages):
        self.ctx.bline()
        self.ctx.writeln('import unittest')
        self.ctx.bline()
        self.ctx.writeln('class ImportTest(unittest.TestCase):')
        self.ctx.lvl_inc()
        self.ctx.bline()

        def collect_types(element, typelist):
            sorted(types, key=lambda x: x.name)
            for e in types:
                typelist.append(e)
                collect_types(e, typelist)

        for package in packages:
            assert isinstance(package, Package)
            self.ctx.bline()
            self.ctx.writeln('def test_%s(self):' % package.name)
            self.ctx.lvl_inc()
            types = []
            types.extend([owned_element for owned_element in package.owned_elements if isinstance(
                owned_element, Class) or isinstance(owned_element, Enum)])
            if len(types) > 0:
                for defined_type in types:
                    self.ctx.writeln(
                        'from %s import %s' % (package.get_py_mod_name(), defined_type.qn()))
            else:
                self.ctx.writeln('pass')
            self.ctx.lvl_dec()
            self.ctx.bline()
        self.ctx.lvl_dec()
        self.ctx.bline()

        self.ctx.writeln("if __name__ == '__main__':")
        self.ctx.lvl_inc()
        self.ctx.writeln('unittest.main()')
        self.ctx.lvl_dec()
        self.ctx.bline()
