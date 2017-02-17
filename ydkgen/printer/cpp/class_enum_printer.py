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
 enum_printer.py

 YANG model driven API, C++ emitter.

"""


from ydkgen.api_model import Enum

class EnumPrinter(object):

    def __init__(self, ctx):
        self.ctx = ctx
        self.printed_enums = {}

    def print_enum_declarations(self, package, classes, file_name, reset_enum_lookup):
        self._print_declarations(package, file_name)
        for clazz in classes:
            self._print_declarations(clazz, file_name)
        if reset_enum_lookup:
            self.printed_enums = {}
                
    def _print_declarations(self, clazz, file_name):
        self._print_enum_declarations(self._get_enums(clazz, file_name))

    def print_enum_to_string_funcs(self, package, classes):
        self._print_to_string_funcs(package)
        for clazz in classes:
            self._print_to_string_funcs(clazz)

    def _print_to_string_funcs(self, clazz):
        self._print_enums(self._get_enums(clazz))
                
    def _get_enums(self, element, file_name=''):
        enums = []
        for child in element.owned_elements:
            if isinstance(child, Enum) and not child.name in self.printed_enums:
                enums.append(child)
                self.printed_enums[child.name] = child
        return enums

    def _print_enums(self, enums):
        for enum in enums:
            self._print_enum_to_string_func(enum)

    def _print_enum_declarations(self, enums):
        for enum in enums:
            self._print_enum_declaration(enum)

    def _print_enum_declaration(self, enum_class):
        assert isinstance(enum_class, Enum)
        self._print_enum_header(enum_class)
        self._print_enum_body(enum_class)
        self._print_enum_trailer(enum_class)

    def _print_enum_header(self, enum_class):
        self.ctx.writeln('class %s : public Enum' % enum_class.qualified_cpp_name())
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        self.ctx.writeln('public:')
        self.ctx.lvl_inc()

    def _print_enum_body(self, enum_class):
        self._print_enum_literals(enum_class)

    def _print_enum_literals(self, enum_class):
        for enum_literal in enum_class.literals:
            self._print_enum_literal(enum_literal)

    def _print_enum_literal(self, enum_literal):
        self.ctx.writeln('static const Enum::YLeaf %s;' % (enum_literal.name))

    def _print_enum_trailer(self, enum_class):
        self.ctx.lvl_dec()
        self.ctx.lvl_dec()
        self.ctx.bline()
        self.ctx.writeln('};')
        self.ctx.bline()

    def _print_enum_to_string_func(self, enum_class):
        for enum_literal in enum_class.literals:
            self._print_enum_literal_to_string(enum_class, enum_literal)
        self.ctx.bline()

    def _print_enum_literal_to_string(self, enum_class, enum_literal):
        self.ctx.writeln('const Enum::YLeaf %s::%s {%s, "%s"};' % (enum_class.qualified_cpp_name(), enum_literal.name, enum_literal.value, enum_literal.stmt.arg))

