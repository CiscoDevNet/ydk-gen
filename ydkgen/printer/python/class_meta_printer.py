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
class_meta_printer.py

 YANG model driven API, class emitter.

"""
from ydkgen.api_model import Class, Enum, Property
from ydkgen.common import sort_classes_at_same_level, get_module_name
from ydkgen.printer.meta_data_util import get_meta_info_data
from .enum_printer import EnumPrinter


class ClassMetaPrinter(object):

    def __init__(self, ctx, one_class_per_module, sort_clazz):
        self.ctx = ctx
        self.is_rpc = False
        self.sort_clazz = sort_clazz
        self.one_class_per_module = one_class_per_module

    def print_output(self, unsorted_classes):
        ''' This arranges the classes at the same level
            so that super references are printed before
            the subclassess'''
        sorted_classes = sort_classes_at_same_level(unsorted_classes, self.sort_clazz)

        for clazz in sorted_classes:
            self.print_class_meta(clazz)

    def print_parents(self, unsorted_classes):
        ''' This arranges the classes at the same level
            so that super references are printed before
            the subclassess'''
        sorted_classes = sort_classes_at_same_level(unsorted_classes, self.sort_clazz)

        for clazz in sorted_classes:
            self._print_meta_parents(clazz)

    def _print_meta_parents(self, clazz):
        nested_classes = sort_classes_at_same_level(
            [nested_class for nested_class in clazz.owned_elements if isinstance(nested_class, Class)],
            self.sort_clazz)
        self.print_parents(nested_classes)
        for nested_class in nested_classes:
            self.ctx.writeln('_meta_table[\'%s\'][\'meta_info\'].parent =_meta_table[\'%s\'][\'meta_info\']' % (
                nested_class.qn(), clazz.qn()))

    def print_class_meta(self, clazz):
        if clazz.is_rpc():
            self.is_rpc = True

        self.print_output(
            [nested_class for nested_class in clazz.owned_elements if isinstance(nested_class, Class)])
        enumz = []
        enumz.extend([nested_enum for nested_enum in clazz.owned_elements if isinstance(
            nested_enum, Enum)])
        for nested_enumz in sorted(enumz, key=lambda e: e.name):
            EnumPrinter(self.ctx).print_enum_meta(nested_enumz)
        self._print_meta_member(clazz)

    def _print_meta_member(self, clazz):
        self.ctx.writeln('\'%s\' : {' % (clazz.qn()))
        self.ctx.lvl_inc()
        self.ctx.writeln("'meta_info' : _MetaInfoClass('%s'," % clazz.qn())
        self.ctx.lvl_inc()
        if clazz.is_grouping():
            self.ctx.writeln('True, ')
        else:
            self.ctx.writeln('False, ')
        self.ctx.writeln('[')

        prop_list = []
        if self.is_rpc:
            prop_list = [p for p in clazz.owned_elements if isinstance(p, Property)]
        else:
            prop_list = clazz.properties()

        for prop in prop_list:
            meta_info_data = get_meta_info_data(
                prop, prop.property_type, prop.stmt.search_one('type'), 'py', self.one_class_per_module)
            self.print_meta_class_member(meta_info_data, self.ctx)

        '''
        class _MetaInfoClass(object):

    def __init__(
            self,
            name,
            is_abstract,
            meta_info_class_members,
            module_name,
            yang_name,
            namespace,
            pmodule_name):
        '''
        self.ctx.writeln('],'),
        module_name = "%s" % get_module_name(clazz.stmt)
        self.ctx.writeln("'%s'," % module_name)
        self.ctx.writeln("'%s'," % clazz.stmt.arg)
        if clazz.is_grouping():
            self.ctx.writeln('None,')
        else:
            self.ctx.writeln("_yang_ns._namespaces['%s']," % module_name)
        self.ctx.lvl_dec()
        self.ctx.writeln("'%s'" % clazz.get_py_mod_name(self.one_class_per_module))
        self.ctx.writeln('),')

        self.ctx.lvl_dec()
        self.ctx.writeln('},')

    def print_meta_class_member(self, meta_info_data, ctx):
        if meta_info_data is None:
            return

        name = meta_info_data.name
        mtype = meta_info_data.mtype
        ptype = meta_info_data.ptype
        pmodule_name = meta_info_data.pmodule_name
        clazz_name = meta_info_data.clazz_name
        prange = meta_info_data.prange
        pattern = meta_info_data.pattern
        presentation_name = meta_info_data.presentation_name
        max_elements = meta_info_data.max_elements
        min_elements = meta_info_data.min_elements

        ctx.writeln("_MetaInfoClassMember('%s', %s, \'%s\' , %s, %s, " %
                    (name, mtype, ptype, pmodule_name, clazz_name))
        ctx.lvl_inc()
        ctx.writeln("%s, %s, " % (str(prange), str(pattern)))
        ctx.write("'''")
        if meta_info_data.comment is not None:
            for line in meta_info_data.comment.split('\n'):
                ctx.writeln('%s' % line)
        ctx.writeln("''',")
        ctx.writeln("'%s'," % presentation_name)

        if len(meta_info_data.children) > 0:
            ctx.writeln(
                "'%s', %s, [" % (meta_info_data.module_name, meta_info_data.is_key))
            ctx.lvl_inc()
            for child_meta_info_data in meta_info_data.children:
                self.print_meta_class_member(child_meta_info_data, ctx)
            ctx.lvl_dec()
            ctx.write(']')
        else:
            ctx.write("'%s', %s" %
                      (meta_info_data.module_name, meta_info_data.is_key))

        if max_elements or min_elements:
            if max_elements:
                ctx.str(", max_elements=%s" % max_elements)
            if min_elements:
                ctx.str(", min_elements=%s),\n" % min_elements)
            else:
                ctx.str("),\n")
        else:
            ctx.str('),\n')

        ctx.lvl_dec()
