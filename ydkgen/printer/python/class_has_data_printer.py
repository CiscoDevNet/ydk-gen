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
class_has_data_printer.py

 Printer for the _has_data method.

"""
from ydkgen.api_model import Bits, Class


class ClassHasDataPrinter(object):

    def __init__(self, ctx):
        self.ctx = ctx

    def print_class_has_data(self, clazz, leafs, children):
        self._print_has_data_header()
        self._print_has_data_body(clazz, leafs, children)
        self._print_function_trailer()

    def print_class_has_operation(self, clazz, leafs, children):
        self._print_has_operation_header()
        self._print_has_operation_body(clazz, leafs, children)
        self._print_function_trailer()

    def _print_has_data_header(self):
        self.ctx.writeln('def has_data(self):')
        self.ctx.lvl_inc()

    def _print_has_operation_header(self):
        self.ctx.writeln('def has_operation(self):')
        self.ctx.lvl_inc()

    def _print_has_data_body(self, clazz, leafs, children):
        conditions = self._init_has_data_conditions(leafs, children)
        for child in children:
            if child.is_many:
                iter_stmt = 'for c in self.%s:'
                access_stmt = 'if (c.has_data()):'
                self._print_class_has_many(child, iter_stmt, access_stmt)
        for leaf in leafs:
            if leaf.is_many:
                iter_stmt = 'for leaf in self.%s.getYLeafs():'
                access_stmt = 'if (leaf.operation != YFilter.not_set):'
                self._print_class_has_many(leaf, iter_stmt, access_stmt)
        
        if len(conditions) == 0:
            self.ctx.writeln('return False')
        elif len(conditions) == 1:
            self.ctx.writeln('return %s' % ''.join(conditions))
        else:
            self.ctx.lvl_inc()
            tab = '\n%s' % self.ctx.tab()
            self.ctx.lvl_dec()
            self.ctx.writeln('return (%s%s)' % (tab, tab.join(conditions)))

    def _print_has_operation_body(self, clazz, leafs, children):
        conditions = self._init_has_operation_conditions(leafs, children)
        for child in children:
            if child.is_many:
                iter_stmt = 'for c in self.%s:'
                access_stmt = 'if (c.has_operation()):'
                self._print_class_has_many(child, iter_stmt, access_stmt)
        for leaf in leafs:
            if leaf.is_many:
                iter_stmt = 'for leaf in self.%s.getYLeafs():'
                access_stmt = 'if (leaf.is_set):'
                self._print_class_has_many(leaf, iter_stmt, access_stmt)
        
        if len(conditions) == 1:
            self.ctx.writeln('return %s' % ''.join(conditions))
        else:
            self.ctx.lvl_inc()
            tab = '\n%s' % self.ctx.tab()
            self.ctx.lvl_dec()
            self.ctx.writeln('return (%s%s)' % (tab, tab.join(conditions)))

    def _print_function_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.bline()

    def _init_has_data_conditions(self, leafs, children):
        conditions = [ 'self.%s.is_set' % (prop.name) for prop in leafs if not prop.is_many]
        conditions.extend([ ('(self.%s is not None and self.%s.has_data())' 
            % (prop.name, prop.name)) for prop in children if not prop.is_many ])
        
        if len(conditions) > 0:
            temp = ' or,'.join(conditions)
            conditions = temp.split(',')

        return conditions

    def _init_has_operation_conditions(self, leafs, children):
        conditions = ['self.operation != YFilter.not_set']
        conditions.extend([ 'self.%s.operation != YFilter.not_set' 
            % (prop.name) for prop in leafs])
        conditions.extend([('(self.%s is not None and self.%s.has_operation())' 
            % (prop.name, prop.name)) for prop in children if not prop.is_many])

        if len(conditions) > 0:
            temp = ' or,'.join(conditions)
            conditions = temp.split(',')

        return conditions

    def _print_class_has_many(self, child, iter_statement, access_statement):
        self.ctx.writeln(iter_statement % child.name)
        self.ctx.lvl_inc()
        self.ctx.writeln(access_statement)
        self.ctx.lvl_inc()
        self.ctx.writeln('return True')
        self.ctx.lvl_dec()
        self.ctx.lvl_dec()
