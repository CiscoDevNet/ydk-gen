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

class ClassHasDataPrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_class_has_data(self, clazz, leafs, children):
        conditions = [ '%s.is_set' % (prop.name) for prop in leafs if not prop.is_many]
        conditions.extend([('(%s !=  nullptr && %s->has_data())' % (prop.name, prop.name)) for prop in children if not prop.is_many])
        self._print_function_header(clazz, 'has_data')
        self.ctx.writeln('if (is_presence_container) return true;')
        for child in children:
            if child.is_many:
                self._print_class_has_many(child, 'for (std::size_t index=0; index<%s.len(); index++)', 'if(%s[index]->has_data())' % child.name)
        for leaf in leafs:
            if leaf.is_many:
                self._print_class_has_many(leaf, 'for (auto const & leaf : %s.getYLeafs())', 'if(leaf.is_set)')
        if len(conditions) == 0:
            self.ctx.writeln('return false;')
        else:
            self.ctx.writeln('return %s;' % '\n\t|| '.join(conditions))
        self._print_function_trailer()

    def print_class_has_operation(self, clazz, leafs, children):
        conditions = ['is_set(yfilter)']
        conditions.extend([ 'ydk::is_set(%s.yfilter)' % (prop.name) for prop in leafs])
        conditions.extend([('(%s !=  nullptr && %s->has_operation())' % (prop.name, prop.name)) for prop in children if not prop.is_many])
        self._print_function_header(clazz, 'has_operation')
        for child in children:
            if child.is_many:
                self._print_class_has_many(child, 'for (std::size_t index=0; index<%s.len(); index++)', 'if(%s[index]->has_operation())' % child.name)
        for leaf in leafs:
            if leaf.is_many:
                self._print_class_has_many(leaf, 'for (auto const & leaf : %s.getYLeafs())', 'if(is_set(leaf.yfilter))')

        self.ctx.writeln('return %s;' % '\n\t|| '.join(conditions))
        self._print_function_trailer()

    def _print_function_header(self, clazz, function_name):
        self.ctx.writeln('bool %s::%s() const' % (clazz.qualified_cpp_name(), function_name))
        self.ctx.writeln('{')
        self.ctx.lvl_inc()

    def _print_function_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_class_has_many(self, child, iter_statement, access_statement):
        self.ctx.writeln(iter_statement % child.name)
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        self.ctx.writeln(access_statement)
        self.ctx.lvl_inc()
        self.ctx.writeln('return true;')
        self.ctx.lvl_dec()
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
