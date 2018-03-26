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
from ydkgen.api_model import Class, Enum, Package, snake_case
from ydkgen.common import get_qualified_yang_name, sort_classes_at_same_level

from .function_printer import FunctionPrinter
from .class_constructor_printer import ClassConstructorPrinter
from .class_enum_printer import EnumPrinter


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
        self._print_class_get_entity_data(clazz, leafs, children)

    def _get_class_members(self, clazz, leafs, children):
        for prop in clazz.properties():
            ptype = prop.property_type
            if isinstance(ptype, Class) and not ptype.is_identity():
                children.append(prop)
            elif ptype is not None:
                leafs.append(prop)

    def _print_class_get_entity_data(self, clazz, leafs, children):
        fp = FunctionPrinter(self.ctx, clazz)
        data_alias = '%s.EntityData' % fp.class_alias
        bundle_name = snake_case(self.bundle_name)

        fp.print_function_header_helper('GetEntityData', return_type='*types.CommonEntityData')
        fp.ctx.writeln('%s.YFilter = %s.YFilter' % (data_alias, fp.class_alias))
        fp.ctx.writeln('%s.YangName = "%s"' % (data_alias, fp.clazz.stmt.arg))
        fp.ctx.writeln('%s.BundleName = "%s"' % (data_alias, self.bundle_name.lower()))
        fp.ctx.writeln('%s.ParentYangName = "%s"' % (data_alias, clazz.owner.stmt.arg))
        self._print_segment_path(fp, data_alias)
        fp.ctx.writeln('%s.CapabilitiesTable = %s.GetCapabilities()' % (data_alias, bundle_name))
        fp.ctx.writeln('%s.NamespaceTable = %s.GetNamespaces()' % (data_alias, bundle_name))
        fp.ctx.writeln('%s.BundleYangModelsLocation = %s.GetModelsPath()' % (data_alias, bundle_name))
        fp.ctx.bline()
        self._print_children(fp, children, data_alias)
        self._print_leafs(fp, leafs, data_alias)

        fp.ctx.writeln('return &(%s)' % data_alias)
        fp.print_function_trailer()

    @staticmethod
    def _print_segment_path(fp, data_alias):
        path = ['"']
        prefix = ''
        if fp.clazz.owner is not None:
            if isinstance(fp.clazz.owner, Package):
                prefix = '%s:' % fp.clazz.owner.stmt.arg
            elif fp.clazz.owner.stmt.i_module.arg != fp.clazz.stmt.i_module.arg:
                prefix = '%s:' % fp.clazz.stmt.i_module.arg
        path.append('%s%s"' % (prefix, fp.clazz.stmt.arg))

        key_props = fp.clazz.get_key_props()
        predicate = '"'
        for key_prop in key_props:
            prefix = ''
            if key_prop.stmt.i_module.arg != fp.clazz.stmt.i_module.arg:
                prefix = '%s:' % (key_prop.stmt.i_module.arg)

            predicate = '''{0}"[%s%s='"{0}fmt.Sprintf("%%v", %s.%s){0}"']"'''

            predicate = predicate.format(' + ') % (
                prefix, key_prop.stmt.arg, fp.class_alias, key_prop.go_name())
            path.append(predicate)
        fp.ctx.writeln('%s.SegmentPath = %s' % (data_alias, ''.join(path)))

    @staticmethod
    def _print_children(fp, children, data_alias):
        fp.ctx.writeln('%s.Children = make(map[string]types.YChild)' % data_alias)
        for child in children:
            path = get_qualified_yang_name(child)
            if child.is_many:
                fp.ctx.writeln('%s.Children["%s"] = types.YChild{"%s", nil}' % (
                    data_alias, path, child.go_name()))

                child_stmt = '%s.%s' % (fp.class_alias, child.go_name())
                fp.ctx.writeln('for i := range %s {' % (child_stmt))
                fp.ctx.lvl_inc()
                child_stmt = '%s[i]' % child_stmt
                fp.ctx.writeln('%s.Children[types.GetSegmentPath(&%s)] = types.YChild{"%s", &%s}' %(
                    data_alias, child_stmt, child.go_name(), child_stmt))
                fp.ctx.lvl_dec()
                fp.ctx.writeln('}')
            else:
                fp.ctx.writeln('%s.Children["%s"] = types.YChild{"%s", &%s.%s}' % (
                    data_alias, path, child.go_name(), fp.class_alias, child.go_name()))

    @staticmethod
    def _print_leafs(fp, leafs, data_alias):
        fp.ctx.writeln('%s.Leafs = make(map[string]types.YLeaf)' % data_alias)
        for leaf in leafs:
            fp.ctx.writeln('%s.Leafs["%s"] = types.YLeaf{"%s", %s.%s}' % (
                data_alias, leaf.stmt.arg, leaf.go_name(), fp.class_alias, leaf.go_name()))

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
