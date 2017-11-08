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

 prints Go classes

"""
from ydkgen.api_model import Class, Enum, snake_case
from ydkgen.common import sort_classes_at_same_level

from .function_printer import FunctionPrinter
from .class_constructor_printer import ClassConstructorPrinter
from .class_enum_printer import EnumPrinter
from .class_has_data_printer import ClassDataFilterPrinter
from .class_get_entity_path_printer import GetEntityPathPrinter, GetSegmentPathPrinter
from .class_get_child_printer import ClassGetChildPrinter
from .class_get_children_printer import ClassGetChildrenPrinter
from .class_set_value_printer import ClassSetValuePrinter


class ClassPrinter(object):
    def __init__(self, ctx, bundle_name, identity_subclasses):
        self.ctx = ctx
        self.bundle_name = bundle_name
        self.identity_subclasses = identity_subclasses
        self.enum_printer = EnumPrinter(ctx)

    def print_output(self, clazz):
        leafs = []
        children = []
        self._get_class_members(clazz, leafs, children)
        self._print_class_constructor(clazz, leafs, children)
        self._print_class_method_definitions(clazz, leafs, children)
        self._print_child_classes(clazz)
        self._print_child_enums(clazz)

    def _print_class_constructor(self, clazz, leafs, children):
        ClassConstructorPrinter(self.ctx, clazz, leafs, self.identity_subclasses).print_all()

    def _print_class_method_definitions(self, clazz, leafs, children):
        self._print_class_has_data(clazz, leafs, children)
        self._print_class_get_filter(clazz)
        self._print_class_set_filter(clazz)
        self._print_class_get_segment_path(clazz)
        self._print_class_get_entity_path(clazz, leafs)
        self._print_class_get_child(clazz, leafs, children)
        self._print_class_get_children(clazz, leafs, children)
        self._print_class_set_value(clazz, leafs)
        self._print_bundle_name_function(clazz)
        self._print_yang_name_function(clazz)
        self._print_yang_models_function(clazz)
        self._print_capabilities_lookup_function(clazz)
        self._print_set_parent_function(clazz)
        self._print_get_parent_function(clazz)
        self._print_get_parent_yang_name_function(clazz)

    def _get_class_members(self, clazz, leafs, children):
        for prop in clazz.properties():
            ptype = prop.property_type
            if isinstance(ptype, Class) and not ptype.is_identity():
                children.append(prop)
            elif ptype is not None:
                leafs.append(prop)

    # HasDataOrFilter
    def _print_class_has_data(self, clazz, leafs, children):
        fp = ClassDataFilterPrinter(self.ctx, clazz, leafs, children)
        fp.print_all()

    # GetFilter
    def _print_class_get_filter(self, clazz):
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper('GetFilter', return_type='yfilter.YFilter')
        fp.ctx.writeln('return %s.YFilter' % fp.class_alias)
        fp.print_function_trailer()

    # SetFilter
    def _print_class_set_filter(self, clazz):
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper('SetFilter', args='yfilter yfilter.YFilter')
        fp.ctx.writeln('%s.YFilter = yfilter' % fp.class_alias)
        fp.print_function_trailer()

    # GetSegmentPath
    def _print_class_get_segment_path(self, clazz):
        fp = GetSegmentPathPrinter(self.ctx, clazz)
        fp.print_all()

    # GetEntityPath
    def _print_class_get_entity_path(self, clazz, leafs):
        fp = GetEntityPathPrinter(self.ctx, clazz, leafs)
        fp.print_all()

    # GetChildByName
    def _print_class_get_child(self, clazz, leafs, children):
        fp = ClassGetChildPrinter(self.ctx, clazz, leafs, children)
        fp.print_all()

    # GetChildren
    def _print_class_get_children(self, clazz, leafs, children):
        fp = ClassGetChildrenPrinter(self.ctx, clazz, leafs, children)
        fp.print_all()

    # SetValue
    def _print_class_set_value(self, clazz, leafs):
        fp = ClassSetValuePrinter(self.ctx, clazz, leafs)
        fp.print_all()

    # GetBundleName
    def _print_bundle_name_function(self, clazz):
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper('GetBundleName', return_type='string')
        fp.ctx.writeln('return "%s"' % self.bundle_name.lower())
        fp.print_function_trailer()

    # GetYangName
    def _print_yang_name_function(self, clazz):
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper('GetYangName', return_type='string')
        fp.ctx.writeln('return "%s"' % fp.class_alias)
        fp.print_function_trailer()

    # GetBundleYangModelsLocation
    def _print_yang_models_function(self, clazz):
        bundle_name = snake_case(self.bundle_name)
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper('GetBundleYangModelsLocation', return_type='string')
        fp.ctx.writeln('return %s.GetModelsPath()' % bundle_name)
        fp.print_function_trailer()

    # GetAugmentCapabilitiesFunction
    def _print_capabilities_lookup_function(self, clazz):
        bundle_name = snake_case(self.bundle_name)
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper(
            'GetAugmentCapabilitiesFunction', return_type='types.AugmentCapabilitiesFunction')
        fp.ctx.writeln("return %s.%sAugmentLookupTables" % (bundle_name, bundle_name.title()))
        fp.print_function_trailer()

    # SetParent
    def _print_set_parent_function(self, clazz):
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper('SetParent', args='parent types.Entity')
        fp.ctx.writeln('%s.parent = parent' % fp.class_alias)
        fp.print_function_trailer()

    # GetParent
    def _print_get_parent_function(self, clazz):
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper('GetParent', return_type='types.Entity')
        fp.ctx.writeln('return %s.parent' % fp.class_alias)
        fp.print_function_trailer()

    # GetYangName
    def _print_get_parent_yang_name_function(self, clazz):
        fp = FunctionPrinter(self.ctx, clazz)
        fp.print_function_header_helper('GetParentYangName', return_type='string')
        fp.ctx.writeln('return "%s"' % clazz.owner.stmt.arg)
        fp.print_function_trailer()

    def _print_child_classes(self, parent):
        unsorted_classes = [nested_class for nested_class in parent.owned_elements if isinstance(nested_class, Class)]
        sorted_classes = sort_classes_at_same_level(unsorted_classes)

        for clazz in sorted_classes:
            cp = ClassPrinter(self.ctx, self.bundle_name, self.identity_subclasses)
            cp.print_output(clazz)

    def _print_child_enums(self, parent):
        enumz = []
        enumz.extend([nested_enum for nested_enum in parent.owned_elements
                        if isinstance(nested_enum, Enum)])

        for nested_enum in sorted(enumz, key=lambda e: e.name):
            self.enum_printer.print_enum(nested_enum)
