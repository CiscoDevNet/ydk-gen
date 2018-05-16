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
header_printer.py

 prints C++ classes

"""
from pyang.types import UnionTypeSpec

from ydkgen.api_model import Class, Enum, Package
from ydkgen.builder import TypesExtractor
from pyang.types import PathTypeSpec
from ydkgen.common import has_list_ancestor, is_top_level_class


class ClassMembersPrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_class_members(self, clazz):
        self._print_constructor_destructor(clazz)
        self._print_class_method_declarations(clazz)
        self._print_class_value_members(clazz)

    def print_class_children_members(self, clazz):
        self._print_class_child_members(clazz)
        self._print_class_enums_forward_declarations(clazz)

    def _print_class_method_declarations(self, clazz):
        if clazz.is_identity():
            return
        self._print_common_method_declarations(clazz)
        self._print_top_level_entity_functions(clazz)

    def _print_constructor_destructor(self, clazz):
        self.ctx.writeln('public:')
        self.ctx.lvl_inc()
        self.ctx.writeln(clazz.name + '();')
        self.ctx.writeln('~' + clazz.name + '();')
        self.ctx.bline()

    def _print_common_method_declarations(self, clazz):
        self.ctx.writeln('bool has_data() const override;')
        self.ctx.writeln('bool has_operation() const override;')
        self.ctx.writeln('std::vector<std::pair<std::string, ydk::LeafData> > get_name_leaf_data() const override;')
        self.ctx.writeln('std::string get_segment_path() const override;')
        self.ctx.writeln('std::shared_ptr<ydk::Entity> get_child_by_name(const std::string & yang_name, const std::string & segment_path) override;')
        self.ctx.writeln('void set_value(const std::string & value_path, const std::string & value, const std::string & name_space, const std::string & name_space_prefix) override;')
        self.ctx.writeln('void set_filter(const std::string & value_path, ydk::YFilter yfliter) override;')
        self.ctx.writeln('std::map<std::string, std::shared_ptr<ydk::Entity>> get_children() const override;')
        self.ctx.writeln('bool has_leaf_or_child_of_name(const std::string & name) const override;')
        if not is_top_level_class(clazz) and not has_list_ancestor(clazz):
            self.ctx.writeln('std::string get_absolute_path() const override;')

    def _print_top_level_entity_functions(self, clazz):
        if clazz.owner is not None and isinstance(clazz.owner, Package):
            self.ctx.writeln('std::shared_ptr<ydk::Entity> clone_ptr() const override;')
            self.ctx.writeln('ydk::augment_capabilities_function get_augment_capabilities_function() const override;')
            self.ctx.writeln('std::string get_bundle_yang_models_location() const override;')
            self.ctx.writeln('std::string get_bundle_name() const override;')
            self.ctx.writeln('std::map<std::pair<std::string, std::string>, std::string> get_namespace_identity_lookup() const override;')

    def _print_class_value_members(self, clazz):
        if clazz.is_identity():
            self.ctx.lvl_dec()
            return
        self.ctx.bline()
        self._print_value_members(clazz)
        self.ctx.lvl_dec()

    def _print_value_members(self, clazz):
        for leaf in _get_leafs(clazz):
            self._print_value_member(leaf, 'ydk::YLeaf', '')
        for leaf in _get_leaf_lists(clazz):
            self._print_value_member(leaf, 'ydk::YLeafList', ' list of ')

    def _print_value_member(self, leaf, leaf_type, description):
        if isinstance(leaf.property_type, UnionTypeSpec):
            union_types = self._get_union_types(leaf)
            if len(union_types) == 1:
                self.ctx.writeln('%s %s; //type:%s %s' % (leaf_type, leaf.name, description, list(union_types)[0]))
            else:
                self.ctx.writeln('%s %s; //type:%s one of %s' % (leaf_type, leaf.name, description, ', '.join(union_types)))
        elif isinstance(leaf.property_type, PathTypeSpec):
            self._print_leafref(leaf, leaf_type, description)
        else:
            self.ctx.writeln('%s %s; //type:%s %s' % (leaf_type, leaf.name, description, leaf.property_type.name))

    def _get_union_types(self, union_leaf):
        union_type = union_leaf.property_type
        contained_types = set()
        for contained_type_stmt in union_type.types:
            contained_property_type = TypesExtractor().get_property_type(contained_type_stmt)
            if isinstance(contained_property_type, UnionTypeSpec):
                contained_types.update(self._get_union_types(contained_property_type))
            elif isinstance(contained_property_type, PathTypeSpec):
                contained_types.add('%s' % _get_leafref_comment(union_leaf))
            else:
                contained_types.add(contained_type_stmt.i_type_spec.name)
        return contained_types

    def _print_leafref(self, leaf, leaf_type, description):
        self.ctx.writeln('//type:%s %s' % (description, _get_leafref_comment(leaf)))
        self.ctx.writeln('%s %s;' % (leaf_type, leaf.name))

    def _print_class_child_members(self, clazz):
        if clazz.is_identity() and len(clazz.extends) == 0:
            return
        class_inits_properties = _get_children(clazz)
        if len(class_inits_properties) > 0:
            self.ctx.lvl_inc()
            self.ctx.writelns(class_inits_properties)
            self.ctx.lvl_dec()

    def _print_class_enums_forward_declarations(self, clazz):
        self.ctx.lvl_inc()
        for child in clazz.owned_elements:
            if isinstance(child, Enum):
                self.ctx.writeln('class %s;' % child.name)
        self.ctx.lvl_dec()


def _get_leafs(clazz):
    leafs = []
    for child in clazz.owned_elements:
        if child.stmt.keyword in ('leaf', 'anyxml'):
            leafs.append(child)
    return leafs


def _get_leaf_lists(clazz):
    leaf_lists = []
    for child in clazz.owned_elements:
        if child.stmt.keyword == 'leaf-list':
            leaf_lists.append(child)
    return leaf_lists


def _get_leafref_comment(leaf):
    if leaf.stmt.i_leafref_ptr is not None:
        reference_class = leaf.stmt.i_leafref_ptr[0].parent.i_class
        reference_prop = leaf.stmt.i_leafref_ptr[0].i_property
        return ('%s (refers to %s::%s)' %
                         (reference_prop.property_type.name,
                          reference_class.fully_qualified_cpp_name(),
                          reference_prop.name))


def _get_class_inits_unique(prop):
    if isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
        presence_stmt = ''
        if prop.property_type.stmt.search_one('presence') is not None:
            presence_stmt = ' // presence node'
        return 'std::shared_ptr<%s> %s;%s' % (prop.property_type.fully_qualified_cpp_name(), prop.name, presence_stmt)


def _get_class_inits_many(prop):
    if prop.is_many and isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
        return 'ydk::YList %s;' % (prop.name)


def _get_children(clazz):
    class_inits_properties = []
    for prop in clazz.properties():
        result = None
        if prop.stmt.keyword == 'anyxml':
            pass
        elif not prop.is_many:
            result = _get_class_inits_unique(prop)
        else:
            result = _get_class_inits_many(prop)
        if result is not None:
            class_inits_properties.append(result)
    if class_inits_properties:
        class_inits_properties.append('')
    return class_inits_properties
