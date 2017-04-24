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

prints python classes

"""
from ydkgen.api_model import Bits


class ClassSetYLeafPrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_class_set_value(self, clazz, leafs):
        self._print_class_set_value_header(clazz)
        self._print_class_set_value_body(leafs)
        self._print_class_set_value_trailer(clazz)

    def _print_class_set_value_header(self, clazz):
        self.ctx.writeln('def set_value(self, value_path, value):')
        self.ctx.lvl_inc()

    def _print_class_set_value_body(self, leafs):
        for leaf in leafs:
            self._print_class_set_values(leaf)

        if len(leafs) == 0:
            self.ctx.writeln('pass')

    def _print_class_set_values(self, leaf):
        self.ctx.writeln('if(value_path == "%s"):' % (leaf.stmt.arg))
        self.ctx.lvl_inc()
        if(isinstance(leaf.property_type, Bits)):
            if leaf.is_many:
                self.ctx.writeln('bits_value = Bits()')
                self.ctx.writeln('bits_value[value] = True')
                self.ctx.writeln('self.%s.append(bits_value)' % leaf.name)
            else:
                self.ctx.writeln('self.%s[value] = True' % leaf.name)
        elif(leaf.is_many):
            self.ctx.writeln('self.%s.append(value)' % leaf.name)
        else:
            self.ctx.writeln('self.%s = value' % leaf.name)
        self.ctx.lvl_dec()

    def _print_class_set_value_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.bline()

