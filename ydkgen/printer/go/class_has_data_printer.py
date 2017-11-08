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

 prints Go class methods

"""

from .function_printer import FunctionPrinter

class ClassDataFilterPrinter(FunctionPrinter):
    def __init__(self, ctx, clazz, leafs, children):
        super(ClassDataFilterPrinter, self).__init__(ctx, clazz, leafs, children)

    def print_function_header(self):
        self.print_function_header_helper('HasDataOrFilter', return_type='bool')

    def print_function_body(self):
        iter_stmt = 'for _, %s := range {0}.%s {{'.format(self.class_alias)

        a_stmt =  'if(&leaf != nil) {'
        for leaf in self.leafs:
            if leaf.is_many:
                self._print_many(iter_stmt % ('leaf', leaf.go_name()), a_stmt)

        a_stmt =  'if child.HasDataOrFilter() {'
        for child in self.children:
            if child.is_many:
                self._print_many(iter_stmt % ('child', child.go_name()), a_stmt)

        conditions = self._get_conditions()
        if len(conditions) == 0:
            self.ctx.writeln('return false')
        else:
            self.ctx.writeln('return %s' % ' ||\n\t'.join(conditions))

    def _get_conditions(self):
        conditions = []

        conditions.append('{}.YFilter != yfilter.NotSet'.format(self.class_alias))

        line = '{0}.%s != nil'.format(self.class_alias)
        conditions.extend([line % l.go_name() for l in self.leafs if not l.is_many])

        line = '{0}.{{0}}.HasDataOrFilter()'.format(self.class_alias)
        conditions.extend([line.format(c.go_name()) for c in self.children if not c.is_many])

        return conditions

    def _print_many(self, iter_statement, access_statement):
        self.ctx.writeln(iter_statement);   self.ctx.lvl_inc()
        self.ctx.writeln(access_statement); self.ctx.lvl_inc()
        self.ctx.writeln('return true');    #self.ctx.lvl_dec(); self.ctx.lvl_dec()
        # self.ctx.writeln('}')
        self.ctx.lvl_dec(); self.ctx.writeln('}')
        self.ctx.lvl_dec(); self.ctx.writeln('}')
