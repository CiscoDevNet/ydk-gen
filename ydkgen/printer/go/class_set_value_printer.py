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
source_printer.py

 prints Go class method

"""
from ydkgen.api_model import Bits
from .function_printer import FunctionPrinter
from ydkgen.common import get_qualified_yang_name


class ClassSetValuePrinter(FunctionPrinter):
    def __init__(self, ctx, clazz, leafs):
        super(ClassSetValuePrinter, self).__init__(ctx, clazz, leafs)

    def print_function_header(self):
        self.print_function_header_helper('SetValue', args='value_path string, value string')

    def print_function_body(self):
        for leaf in self.leafs:
            self.ctx.writeln('if value_path == "%s" {' % get_qualified_yang_name(leaf))
            self.ctx.lvl_inc()
            line = '{0}.{1} = value'
            if leaf.is_many:
                line = '{0}.{1} = append({0}.{1}, value)'
            self.ctx.writeln(line.format(self.class_alias, leaf.go_name()))
            self.ctx.lvl_dec()
            self.ctx.writeln('}')

    def _print_if_stmt(self, leaf):
        self.ctx.writeln('if value_path == "%s" {' % (leaf.stmt.arg))
        self.ctx.lvl_inc()
        line = '{0}.{1} = value'
        if leaf.is_many:
            line = '{0}.{1} = append({0}.{1}, value)'
        self.ctx.writeln(line.format(self.class_alias, leaf.go_name()))

        # todo: what is going on with bits?
        # if(isinstance(leaf.property_type, Bits)):
        #     if leaf.is_many:
        #         self.ctx.writeln('Bits bits_value{};')
        #         self.ctx.writeln('bits_value[value] = true;')
        #         self.ctx.writeln('%s.append(bits_value);' % leaf.name)
        #     else:
        #         self.ctx.writeln('%s[value] = true;' % leaf.name)
        # elif(leaf.is_many):
        #     self.ctx.writeln('%s.append(value);' % leaf.name)
        # else:
        #     self.ctx.writeln('%s.%s = value' % (clazz.stmt.arg, leaf.name))

        self.ctx.lvl_dec()
        self.ctx.writeln('}')
