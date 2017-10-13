#  ----------------------------------------------------------------
# Copyright 2017 Cisco Systems
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
class_enum_printer.py

 prints Go enum class method

"""

class EnumPrinter(object):

    def __init__(self, ctx):
        self.ctx = ctx

    def print_enum(self, enum_class):
        self._print_enum_header(enum_class)
        self._print_enum_body(enum_class)
        self._print_enum_trailer(enum_class)

    def _print_enum_header(self, enum_class):
        self._print_docstring(enum_class)
        self.ctx.writeln('type %s string' % enum_class.qualified_go_name())
        self.ctx.bline()
        self.ctx.writeln('const (')
        self.ctx.lvl_inc()

    def _print_enum_body(self, enum_class):
        enum_class_prefix = enum_class.qualified_go_name()
        for enum_literal in enum_class.literals:
            self._print_enum_literal(enum_class_prefix, enum_literal)

    def _print_enum_literal(self, enum_class_prefix, enum_literal):
        name = enum_literal.name
        self.ctx.writeln('%s_%s %s = "%s"' % (enum_class_prefix,
                                            name,
                                            enum_class_prefix,
                                            enum_literal.stmt.arg))

    def _print_enum_trailer(self, enum_class):
        self.ctx.lvl_dec()
        self.ctx.writeln(')')
        self.ctx.bline()

    def _print_docstring(self, enum_class):
        self.ctx.writeln('//////////////////////////////////////////////////////////////////////////')
        self.ctx.writeln('// %s' % enum_class.qualified_go_name())
        self.ctx.writeln('//////////////////////////////////////////////////////////////////////////')
        enum_class_prefix = enum_class.qualified_go_name()

        if enum_class.comment is not None:
            for c in enum_class.comment.split('\n'):
                self.ctx.writeln('// %s' % c)

        for l in enum_class.literals:
            self.ctx.writeln('// %s_%s:' % (enum_class_prefix, l.go_name()))
            if l.comment is not None:
                length = len(enum_class_prefix) + len(l.go_name()) + 2
                for c in l.comment.split('\n'):
                    self.ctx.writeln('// %s%s' % (' ' * length, c))
        self.ctx.writeln('//////////////////////////////////////////////////////////////////////////')
