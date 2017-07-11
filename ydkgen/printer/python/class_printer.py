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
from ydkgen.api_model import Class, Package, Enum, Bits
from ydkgen.common import sort_classes_at_same_level
from ydkgen.printer.file_printer import FilePrinter

# from .bits_printer import BitsPrinter
from .class_docstring_printer import ClassDocstringPrinter
from .class_inits_printer import ClassInitsPrinter, ClassSetAttrPrinter
from .class_has_data_printer import ClassHasDataPrinter
from .class_get_entity_path_printer import GetEntityPathPrinter, GetSegmentPathPrinter
from .class_get_child_by_name_printer import ClassGetChildByNamePrinter
from .class_set_value_printer import ClassSetYLeafPrinter
from .enum_printer import EnumPrinter



class ClassPrinter(FilePrinter):

    def __init__(self, ctx, sort_clazz, module_namespace_lookup):
        super(ClassPrinter, self).__init__(ctx)
        self.sort_clazz = sort_clazz
        self.module_namespace_lookup = module_namespace_lookup

    def print_body(self, unsorted_classes):
        ''' This arranges the classes at the same level
            so that super references are printed before
            the subclassess'''
        sorted_classes = sort_classes_at_same_level(unsorted_classes, self.sort_clazz)

        for clazz in sorted_classes:
            self._print_class(clazz)

    def _print_class(self, clazz):
        self._print_class_header(clazz)
        self._print_class_body(clazz)
        self._print_class_trailer(clazz)

    def _print_class_header(self, clazz):
        self._print_class_declaration(clazz)
        self._print_class_docstring(clazz)
        self._print_class_attributes(clazz)

    def _print_class_body(self, clazz):
        leafs = []
        children = []
        self._get_class_members(clazz, leafs, children)
        self._print_class_inits(clazz, leafs, children)
        self._print_class_setattr(clazz, leafs)
        self._print_child_enums(clazz)
        self._print_child_classes(clazz)
        self._print_class_functions(clazz, leafs, children)

    def _print_class_functions(self, clazz, leafs, children):
        if clazz.is_identity():
            return
        self._print_class_has_data(clazz, leafs, children)
        self._print_class_has_operation(clazz, leafs, children)
        self._print_class_get_segment_path(clazz)
        self._print_class_get_entity_path(clazz, leafs)
        self._print_class_get_child_by_name(clazz, children)
        self._print_class_set_value(clazz, leafs)
        self._print_class_clone_ptr(clazz)

    def _print_child_enums(self, parent):
        enumz = []
        enumz.extend([nested_enum for nested_enum in parent.owned_elements if isinstance(
            nested_enum, Enum)])

        for nested_enumz in sorted(enumz, key=lambda e: e.name):
            self._print_enum(nested_enumz)

    def _print_child_bits(self, parent):
        bits = []
        bits.extend(
            [nested_bit for nested_bit in parent.owned_elements if isinstance(nested_bit, Bits)])

        for bit in sorted(bits, key=lambda b: b.name):
            self._print_bits(bit)

    def _print_child_classes(self, parent):
        self.print_body(
            [nested_class for nested_class in parent.owned_elements if isinstance(nested_class, Class)])

    def _print_class_trailer(self, clazz):
        self.ctx.lvl_dec()

    def _print_class_declaration(self, clazz):
        self.ctx.bline()

        parents = 'Entity'
        if clazz.is_identity():
            parents = 'Identity'
        elif len(clazz.extends) > 0:
            parents = ' ,'.join([sup.qn() for sup in clazz.extends])

        self.ctx.writeln("class %s(%s):" % (clazz.name, parents))

    def _print_class_docstring(self, clazz):
        ClassDocstringPrinter(self.ctx).print_output(clazz)

    def _print_class_attributes(self, clazz):
        module = clazz.stmt
        if not clazz.stmt.keyword == 'module':
            module = clazz.stmt.i_module
        self.ctx.writeln("_prefix = '%s'" % module.i_prefix)
        revision_stmt = module.search_one('revision')
        if revision_stmt is not None:
            self.ctx.writeln("_revision = '%s'" % revision_stmt.arg)
        self.ctx.bline()

    def _get_class_members(self, clazz, leafs, children):
        for prop in clazz.properties():
            ptype = prop.property_type
            if isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
                children.append(prop)
            elif ptype is not None:
                leafs.append(prop)

    def _print_class_inits(self, clazz, leafs, children):
        ClassInitsPrinter(self.ctx, self.module_namespace_lookup).print_output(clazz, leafs, children)

    def _print_class_setattr(self, clazz, leafs):
        ClassSetAttrPrinter(self.ctx).print_setattr(clazz, leafs)

    def _print_class_has_data(self, clazz, leafs, children):
        ClassHasDataPrinter(self.ctx).print_class_has_data(clazz, leafs, children)

    def _print_class_has_operation(self, clazz, leafs, children):
        ClassHasDataPrinter(self.ctx).print_class_has_operation(clazz, leafs, children)

    def _print_class_get_segment_path(self, clazz):
        GetSegmentPathPrinter(self.ctx).print_output(clazz)

    def _print_class_get_entity_path(self, clazz, leafs):
        GetEntityPathPrinter(self.ctx).print_output(clazz, leafs)

    def _print_class_get_child_by_name(self, clazz, children):
        ClassGetChildByNamePrinter(self.ctx).print_class_get_child_by_name(clazz, children)

    def _print_class_set_value(self, clazz, leafs):
        ClassSetYLeafPrinter(self.ctx).print_class_set_value(clazz, leafs)

    def _print_class_clone_ptr(self, clazz):
        if clazz.owner is not None and isinstance(clazz.owner, Package):
            self.ctx.writeln('def clone_ptr(self):')
            self.ctx.lvl_inc()
            self.ctx.writeln('self._top_entity = %s()' % clazz.qn())
            self.ctx.writeln('return self._top_entity')
            self.ctx.lvl_dec()

    def _print_bits(self, bits):
        BitsPrinter(self.ctx).print_bits(bits)

    def _print_enum(self, enum_class):
        EnumPrinter(self.ctx).print_enum(enum_class, False)
