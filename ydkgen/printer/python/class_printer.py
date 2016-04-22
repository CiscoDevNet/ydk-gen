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
class_printer.py 
 
 YANG model driven API, class emitter.
 
"""
from .class_common_path_printer import ClassCommonPathPrinter
from .class_docstring_printer import ClassDocstringPrinter
from .class_has_data_printer import ClassHasDataPrinter
from .class_inits_printer import ClassInitsPrinter
from .class_is_config_printer import ClassIsConfigPrinter
from ydkgen.common import sort_classes_at_same_level



class ClassPrinter(object):

    def __init__(self, ctx, parent):
        self.ctx = ctx
        self.parent = parent

    def print_classes(self, unsorted_classes):
        ''' This arranges the classes at the same level 
            so that super references are printed before 
            the subclassess'''
        sorted_classes = sort_classes_at_same_level(unsorted_classes)

        for clazz in sorted_classes:
            self.print_class(clazz)

    def print_class(self, clazz):
        self.parent._start_tab_leak_check()
        self._print_class_header(clazz)
        self._print_class_body(clazz)
        self._print_class_trailer(clazz)
        self.parent._check_tab_leak()

    def _print_class_header(self, clazz):
        self._print_class_declaration(clazz)
        self._print_class_docstring(clazz)
        self._print_class_attributes(clazz)

    def _print_class_body(self, clazz):
        self._print_class_inits(clazz)

        self.parent.print_child_enums(clazz)
        self.parent.print_child_bits(clazz)
        self.parent.print_child_classes(clazz)

        if not clazz.is_identity() and not clazz.is_grouping():
            self._print_common_path_functions(clazz)
            self._print_is_config_function(clazz)
            self._print_has_data_functions(clazz)

    def _print_class_trailer(self, clazz):
        self.ctx.writeln('@staticmethod')
        self.ctx.writeln('def _meta_info():')
        self.ctx.lvl_inc()
        self.ctx.writeln('from %s import _%s as meta' % (
            clazz.get_meta_py_mod_name(), clazz.get_package().name))
        self.ctx.writeln(
            "return meta._meta_table['%s']['meta_info']" % clazz.qn())
        self.ctx.lvl_dec()
        self.ctx.bline()
        self.ctx.lvl_dec()

    def _print_class_declaration(self, clazz):
        self.ctx.bline()
        parents = 'object'
        if len(clazz.extends) > 0:
            parents = ' ,'.join([sup.qn() for sup in clazz.extends])

        self.ctx.writeln("class %s(%s):" % (clazz.name, parents))

    def _print_class_docstring(self, clazz):
        ClassDocstringPrinter(self.ctx, self.parent).print_output(clazz)

    def _print_class_attributes(self, clazz):
        module = clazz.stmt
        if not clazz.stmt.keyword == 'module':
            module = clazz.stmt.i_module
        self.ctx.writeln("_prefix = '%s'" % module.i_prefix)
        revision_stmt = module.search_one('revision')
        if revision_stmt is not None:
            self.ctx.writeln("_revision = '%s'" % revision_stmt.arg)
        self.ctx.bline()

    def _print_class_inits(self, clazz):
        ClassInitsPrinter(self.ctx, self.parent).print_output(clazz)

    def _print_is_config_function(self, clazz):
        ClassIsConfigPrinter(self.ctx, self.parent).print_output(clazz)

    def _print_has_data_functions(self, clazz):
        ClassHasDataPrinter(self.ctx, self.parent).print_output(clazz)

    def _print_common_path_functions(self, clazz):
        ClassCommonPathPrinter(self.ctx, self.parent).print_output(clazz)
