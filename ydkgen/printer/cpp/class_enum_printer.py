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

    def print_enum_declarations(self, element):
        self._print_enum_declarations(self._get_enums(element))
        for child in element.owned_elements:
            self.print_enum_declarations(child)

    def print_enum_to_string_funcs(self, element):
        self._print_enums(self._get_enums(element))
        for child in element.owned_elements:
            self.print_enum_to_string_funcs(child)

    def _get_enums(self, element):
        enums = []
        enum_name_map = {}
        for child in element.owned_elements:
            if isinstance(child, Enum) and not child.name in enum_name_map:
                enums.append(child)
                enum_name_map[child.name] = child
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
        self.ctx.writeln('static const Enum::Value %s;' % (enum_literal.name))

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
        self.ctx.writeln('const Enum::Value %s::%s {%s, "%s"};' % (enum_class.qualified_cpp_name(), enum_literal.name, enum_literal.value, enum_literal.name))

