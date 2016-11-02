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
from ydkgen.api_model import Class, Package
from ydkgen.common import sort_classes_at_same_level
from ydkgen.printer.file_printer import FilePrinter

from .class_constructor_printer import ClassConstructorPrinter
from .class_has_data_printer import ClassHasDataPrinter
from .class_get_children_printer import ClassGetChildrenPrinter
from .class_set_child_printer import ClassSetChildPrinter
from .class_set_value_printer import ClassSetValuePrinter
from .enum_printer import EnumPrinter
from .get_entity_path_printer import GetEntityPathPrinter, GetSegmentPathPrinter


class SourcePrinter(FilePrinter):
    def __init__(self, ctx, sort_clazz):
        super(SourcePrinter, self).__init__(ctx)
        self.sort_clazz = sort_clazz

    def print_header(self, package):
        self.ctx.bline()
        self.ctx.writeln('#include <sstream>')
        self.ctx.writeln('#include <iostream>')
        self.ctx.writeln('#include <algorithm>')
        self.ctx.writeln('#include "{0}.hpp"'.format(package.name))
        self.ctx.bline()
        self.ctx.writeln('namespace ydk {')
        self.ctx.writeln('namespace %s {' % package.name)
        self.ctx.bline()

    def print_trailer(self, package):
        self.ctx.bline()
        self.ctx.writeln('}')
        self.ctx.writeln('}')
        self.ctx.bline()

    def print_body(self, package):
        self._print_classes([clazz for clazz in package.owned_elements if isinstance(clazz, Class)])
        self.ctx.bline()
        self._print_enums(package)
        # self._print_bits(package)

    def _print_classes(self, clazzes):
        sorted_classes = sort_classes_at_same_level(clazzes, self.sort_clazz)
        for clazz in sorted_classes:
            self._print_class(clazz)

    def _print_class(self, clazz):
        self._print_classes([nested_class for nested_class in clazz.owned_elements if isinstance(nested_class, Class)])
        self._print_class_body(clazz)

    def _print_class_body(self, clazz):
        leafs = []
        children = []
        self._get_class_members(clazz, leafs, children)
        self._print_class_constructor(clazz, leafs, children)
        self._print_class_destructor(clazz)
        self._print_class_method_definitions(clazz, leafs, children)

    def _print_class_method_definitions(self, clazz, leafs, children):
        if clazz.is_identity():
            return
        self._print_class_has_data(clazz, leafs, children)
        self._print_class_get_segment_path(clazz)
        self._print_class_get_path(clazz, leafs)
        self._print_class_set_child(clazz, children)
        self._print_class_get_children(clazz, children)
        self._print_class_set_value(clazz, leafs)
        self._print_clone_ptr_method(clazz, leafs)

    def _print_class_destructor(self, clazz):
        self.ctx.writeln(clazz.qualified_cpp_name() + '::~' + clazz.name + '()')
        self.ctx.writeln('{')
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_clone_ptr_method(self, clazz, leafs):
        if clazz.owner is not None and isinstance(clazz.owner, Package):
            self.ctx.writeln('std::unique_ptr<Entity> %s::clone_ptr()' % clazz.qualified_cpp_name())
            self.ctx.writeln('{')
            self.ctx.lvl_inc()
            self.ctx.writeln('return std::make_unique<%s>();' % clazz.qualified_cpp_name())
            self.ctx.lvl_dec()
            self.ctx.writeln('}')

    def _get_class_members(self, clazz, leafs, children):
        for prop in clazz.properties():
            ptype = prop.property_type
            if isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
                children.append(prop)
            elif ptype is not None:
                leafs.append(prop)

    def _print_class_get_children(self, clazz, children):
        ClassGetChildrenPrinter(self.ctx).print_class_get_children(clazz, children)

    def _print_class_constructor(self, clazz, leafs, children):
        ClassConstructorPrinter(self.ctx).print_constructor(clazz, leafs, children)

    def _print_class_has_data(self, clazz, leafs, children):
        ClassHasDataPrinter(self.ctx).print_class_has_data(clazz, leafs, children)

    def _print_class_get_segment_path(self, clazz):
        GetSegmentPathPrinter(self.ctx).print_output(clazz)

    def _print_class_get_path(self, clazz, leafs):
        GetEntityPathPrinter(self.ctx).print_output(clazz, leafs)

    def _print_class_set_child(self, clazz, children):
        ClassSetChildPrinter(self.ctx, get_path).print_class_set_child(clazz, children)

    def _print_class_set_value(self, clazz, leafs):
        ClassSetValuePrinter(self.ctx, get_path).print_class_set_value(clazz, leafs)

    def _print_enums(self, package):
        EnumPrinter(self.ctx).print_enum_to_string_funcs(package)


def get_path(prop):
        path = prop.stmt.arg
        current_owner = prop.owner
        while(current_owner is not None) and not isinstance(current_owner, Package):
            if current_owner.owner is not None and isinstance(current_owner.owner, Package):
                path = '/' + current_owner.owner.stmt.arg + ':' + current_owner.stmt.arg + '/' + path
            else:
                path = current_owner.stmt.arg + '/' + path
            current_owner = current_owner.owner
        return path
