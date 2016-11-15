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

 prints C++ classes

"""
from ydkgen.api_model import Bits


class ClassSetValuePrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_class_set_value(self, clazz, leafs):
        self._print_class_set_value_header(clazz)
        self._print_class_set_value_body(leafs)
        self._print_class_set_value_trailer(clazz)

    def _print_class_set_value_header(self, clazz):
        self.ctx.writeln('void %s::set_value(const std::string & value_path, std::string value)' % clazz.qualified_cpp_name())
        self.ctx.writeln('{')
        self.ctx.lvl_inc()

    def _print_class_set_value_body(self, leafs):
        for leaf in leafs:
            self._print_class_set_values(leaf)

    def _print_class_set_values(self, leaf):
        self.ctx.writeln('if(value_path == "%s")' % (leaf.stmt.arg))
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        if(isinstance(leaf.property_type, Bits)):
            if leaf.is_many:
                self.ctx.writeln('Bits bits_value{};')
                self.ctx.writeln('bits_value[value] = true;')
                self.ctx.writeln('%s.append(bits_value);' % leaf.name)
            else:
                self.ctx.writeln('%s[value] = true;' % leaf.name)
        elif(leaf.is_many):
            self.ctx.writeln('%s.append(value);' % leaf.name)
        else:
            self.ctx.writeln('%s = value;' % leaf.name)
        self.ctx.lvl_dec()
        self.ctx.writeln('}')

    def _print_class_set_value_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

