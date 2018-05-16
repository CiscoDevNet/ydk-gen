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

from ydkgen.common import get_qualified_yang_name


class ClassGetChildPrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_class_get_child(self, clazz, children):
        self._print_class_get_child_header(clazz)
        self._print_class_get_child_body(children)
        self._print_class_get_child_trailer(clazz)

    def _print_class_get_child_header(self, clazz):
        self.ctx.writeln('std::shared_ptr<Entity> %s::get_child_by_name(const std::string & child_yang_name, const std::string & segment_path)' % clazz.qualified_cpp_name())
        self.ctx.writeln('{')
        self.ctx.lvl_inc()

    def _print_class_get_child_body(self, children):
        for child in children:
            self._print_class_get_child(child)
            self.ctx.bline()

    def _print_class_get_child(self, child):
        self.ctx.writeln('if(child_yang_name == "%s")' % get_qualified_yang_name(child))
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        if child.is_many:
            self._print_class_get_child_many(child)
        else:
            self._print_class_get_child_unique(child)
        self.ctx.lvl_dec()
        self.ctx.writeln('}')

    def _print_class_get_child_many(self, child):
        self.ctx.writeln('auto c = std::make_shared<%s>();' % (child.property_type.qualified_cpp_name()))
        self.ctx.writeln('c->parent = this;')
        self.ctx.writeln('%s.append(c);' % child.name)
        self.ctx.writeln('return c;')

    def _print_class_get_child_unique(self, child):
        self.ctx.writeln('if(%s == nullptr)' % child.name)
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        self.ctx.writeln('%s = std::make_shared<%s>();' % (child.name, child.property_type.qualified_cpp_name()))
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.writeln('return %s;' % child.name)

    def _print_class_get_child_trailer(self, clazz):
        self.ctx.writeln('return nullptr;')
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()
