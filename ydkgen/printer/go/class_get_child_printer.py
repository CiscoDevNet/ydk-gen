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
from .function_printer import FunctionPrinter
from ydkgen.common import get_qualified_yang_name


class ClassGetChildPrinter(FunctionPrinter):
    def __init__(self, ctx, clazz, leafs, children):
        super(ClassGetChildPrinter, self).__init__(ctx, clazz, leafs, children)

    def print_function_header(self):
        self.print_function_header_helper(
            'GetChildByName', 'child_yang_name string, segment_path string', 'types.Entity')

    def print_function_body(self):
        for child in self.children:
            self._print_check_child(child)
        self.ctx.writeln('return nil')

    def _print_check_child(self, child):
        self.ctx.writeln('if child_yang_name == "%s" {' % get_qualified_yang_name(child))
        self.ctx.lvl_inc()
        if child.is_many:
            self._print_check_many(child)
        else:
            self.ctx.writeln('return &%s.%s' % (self.class_alias, child.go_name()))
        self.ctx.lvl_dec()
        self.ctx.writeln('}')

    def _print_check_many(self, child):
        self.ctx.writeln('for _, c := range %s.%s {' % (
            self.class_alias, child.go_name()))
        self.ctx.lvl_inc()
        self.ctx.writeln('if %s.GetSegmentPath() == segment_path {' % self.class_alias)
        self.ctx.lvl_inc();     self.ctx.writeln('return &c');
        self.ctx.lvl_dec();     self.ctx.writeln('}')
        self.ctx.lvl_dec();     self.ctx.writeln('}')
        self.ctx.writeln('child := %s{}' % (child.qualified_go_name()))

        list_name = '%s.%s' % (self.class_alias, child.go_name())
        self.ctx.writeln('{0} = append({0}, child)'.format(list_name))
        self.ctx.writeln('return &{0}[len({0})-1]'.format(list_name))
