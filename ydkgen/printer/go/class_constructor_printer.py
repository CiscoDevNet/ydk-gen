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

 prints Go class constructor

"""
from functools import reduce

from ydkgen.api_model import Bits, Class, DataType, Enum
from ydkgen.builder import TypesExtractor
from pyang.types import Decimal64TypeSpec, PathTypeSpec, UnionTypeSpec
from ydkgen.printer.meta_data_util import get_class_docstring, get_meta_info_data, format_range_string
from .function_printer import FunctionPrinter

class ClassConstructorPrinter(FunctionPrinter):
    def __init__(self, ctx, clazz, leafs, identity_subclasses):
        super(ClassConstructorPrinter, self).__init__(ctx, clazz, leafs)
        self.identity_subclasses = identity_subclasses

    def print_function_header(self):
        self._print_docstring()
        self.ctx.writeln('type %s struct {' % self.clazz.qualified_go_name())
        self.ctx.lvl_inc()

    def print_function_body(self):
        self.ctx.writeln('parent types.Entity')
        self.ctx.writeln('YFilter types.YFilter')
        self._print_inits()

    def _print_docstring(self):
        self.ctx.writeln('//////////////////////////////////////////////////////////////////////////')
        self.ctx.writeln('// %s' % self.clazz.qualified_go_name())
        self.ctx.writeln('//////////////////////////////////////////////////////////////////////////')
        lines = []
        # class comment

        if self.clazz.comment is not None:
            lines.extend(self.clazz.comment.split('\n'))

        # presence statement
        if self.clazz.stmt.search_one('presence') is not None:
            lines.append('This type is a presence type.')

        for l in lines:
            self.ctx.writeln('// %s' % l)
        self.ctx.writeln('//////////////////////////////////////////////////////////////////////////')

    def _print_inits(self):
        self._print_leaf_inits()
        self._print_children_inits()

    def _print_leaf_inits(self):
        index = 0
        while index < len(self.leafs):
            self.ctx.bline()
            prop = self.leafs[index]
            index += 1
            leaf_comments = []

            leaf_name = prop.go_name()
            type_name = get_type_name(prop.property_type)

            if prop.is_many:
                declaration_stmt = '%s []interface{}' % leaf_name
            else:
                declaration_stmt = '%s interface{}' % leaf_name

            comments = self._get_attribute_comment(prop, type_name)
            self.ctx.writelns(comments)
            self.ctx.bline()
            self.ctx.writeln(declaration_stmt)

    def _print_children_inits(self):
        for prop in self.clazz.properties():
            if not prop.is_many:
                self._print_child_inits_unique(prop)
            elif prop.stmt.keyword != 'anyxml':
                self._print_child_inits_many(prop)

    def _print_child_inits_unique(self, prop):
        if (isinstance(prop.property_type, Class)
            and not prop.property_type.is_identity()):
            self.ctx.bline()

            comments = []
            self._add_comment_on_prop(prop, comments)
            self.ctx.writelns(self._get_formatted_comment(comments))
            self.ctx.bline()

            self.ctx.writeln('%s %s' % (
                prop.go_name(), prop.qualified_go_name()))

    def _print_child_inits_many(self, prop):
        if (prop.is_many and isinstance(prop.property_type, Class)
            and not prop.property_type.is_identity()):
            self.ctx.bline()

            comments = self._get_attribute_comment(prop)
            self.ctx.writelns(comments)
            self.ctx.bline()

            self.ctx.writeln('%s []%s' % (
                prop.go_name(), prop.qualified_go_name()))
            # self.ctx.writeln('%s []interface{}' % (prop.go_name()))

    def _get_formatted_comment(self, comments):
        comments = ' '.join(comments)
        comments = comments.split(' ')
        line = ''
        result = []
        for word in comments:
            _line = '%s %s' % (line, word)
            if line == '':
                line = word
            elif len(_line) > 75:
                result.append('// %s' % line)
                line = word
            else:
                line = _line
        if line != '':
            result.append('// %s' % line)
        return result

    def _get_attribute_comment(self, prop, type_name=None):
        properties_description = []
        meta_info_data = self._get_meta_info_data(prop)
        self._add_comment_on_key(prop, properties_description)
        self._add_comment_on_prop(prop, properties_description)
        self._add_comment_on_presence(prop, properties_description)
        # self._add_comment_on_expected_type(prop, properties_description, type_name, meta_info_data)

        # More Comments
        properties_description.append('The type is')
        properties_description.extend(self._get_type_doc(meta_info_data, 1))
        formatted_comment = self._get_formatted_comment(properties_description)
        return formatted_comment

    def _get_meta_info_data(self, prop):
        property_type = prop.property_type
        type_stmt = prop.stmt.search_one('type')

        id_subclasses = None
        if (hasattr(property_type, 'is_identity') and property_type.is_identity()):
            id_subclasses = self.identity_subclasses

        meta_info_data = get_meta_info_data(
            prop, property_type, type_stmt, 'go', identity_subclasses=id_subclasses)

        if type(prop.property_type) in (Class, Enum, Bits):
            meta_info_data.clazz_name = "'%s'" % property_type.qualified_go_name()

        if isinstance(prop.property_type, Class):
            if id_subclasses is None:
                meta_info_data.doc_link = prop.qualified_go_name()
            else:
                meta_info_data.doc_link = self._get_many_docstring(id_subclasses, property_type)
                meta_info_data.doc_link_description = 'one of the following: '
            if prop.is_many:
                meta_info_data.doc_link = 'slice of %s' % meta_info_data.doc_link
        else:
            meta_info_data.doc_link = get_type_name(property_type)
            if prop.stmt.keyword == 'leaf-list':
                meta_info_data.doc_link = 'slice of %s' % get_type_name(property_type)
            elif prop.stmt.keyword == 'anyxml':
                return meta_info_data

            type_spec = type_stmt.i_type_spec
            if isinstance(type_spec, PathTypeSpec):
                if prop.stmt.i_leafref_ptr is not None:
                    ref_class = prop.stmt.i_leafref_ptr[0].parent.i_class
                    ref_prop = prop.stmt.i_leafref_ptr[0].i_property
                    ref_path = '%s.%s' % (ref_class.get_package().name, ref_prop.qualified_go_name())
                    meta_info_data.target_of_leafref = ref_path
            elif isinstance(type_spec, UnionTypeSpec):
                pairs = zip(meta_info_data.children, type_spec.types)
                for child, ptype in pairs:
                    if child.mtype == 'REFERENCE_ENUM_CLASS':
                        child.doc_link = 'enumeration %s' % (child.ptype)
        return meta_info_data

    def _get_many_docstring(self, id_subclasses, property_type):
        doc_link = []
        pkey = id(property_type)
        if pkey in id_subclasses:
            for subclass in id_subclasses[pkey]:
                doc_link.append(subclass.go_name())
                if id(subclass) in id_subclasses:
                    subclass_doc_link = [x for x in self._get_many_docstring(
                        id_subclasses, subclass) if x not in doc_link]
                    doc_link.extend(subclass_doc_link)
        else:
            doc_link.append(property_type.go_name())
        return doc_link

    def _add_comment_on_key(self, prop, description):
        if prop in self.clazz.get_key_props():
            description.append('This attribute is a key.')

    def _add_comment_on_prop(self, prop, description):
        if prop.comment not in (None, ''):
            comment = prop.comment.split('\n')
            comment = ' '. join(comment)
            if not comment.endswith('.'):
                comment = '%s.' % comment
            description.append(comment)

    def _add_comment_on_presence(self, prop, description):
        if prop not in self.leafs and prop.property_type.stmt.search_one('presence') is not None:
            description.append('This attribute is a presence node.')

    def _add_comment_on_expected_type(self, prop, description, type_name, meta_info_data):
        if type_name is not None:
            description.append('Expected type is %s.' % type_name)
        # else:
        #     target = meta_info_data.doc_link
        #     if isinstance(meta_info_data.doc_link, list):
        #         doc_link = map(lambda l: '%s' % l, meta_info_data.doc_link)
        #         target = '%s%s' % (meta_info_data.doc_link_description, ''.join(doc_link))

        #     if (prop in self.clazz.properties() and prop.is_many) or prop in self.leafs:
        #         description.append('The type is %s.' % target.lstrip())

    def _get_type_doc(self, meta_info_data, type_depth):
        properties_description = []
        if len(meta_info_data.children) > 0:
            pass
            if type_depth == 1:
                properties_description.append('one of the following types:')
            temp = [' '.join(self._get_type_doc(c, type_depth + 1)) for c in meta_info_data.children]
            temp = reduce(lambda a, b: '%s, or %s' % (a, b), temp)
            properties_description.append('%s.' % temp)
        else:
            self._add_comment_on_restriction(properties_description, meta_info_data, type_depth)
            self._add_comment_on_leaf_ref_source(properties_description, meta_info_data)
            self._add_comment_on_mandatory(properties_description, meta_info_data)
            # # self._add_comment_on_presence(description, meta_info_data)
            self._add_comment_on_units(properties_description, meta_info_data)
            self._add_comment_on_default_value(properties_description, meta_info_data)
            self._add_comment_on_status(properties_description, meta_info_data)

        return properties_description

    def _add_comment_on_restriction(self, description, meta_info_data, depth):
        target = meta_info_data.doc_link
        if isinstance(meta_info_data.doc_link, list):
            doc_link = map(lambda l: '%s' % l, meta_info_data.doc_link)
            target = '%s%s' % (meta_info_data.doc_link_description, ''.join(doc_link))
        target = target.lstrip()

        prop_restriction = self._get_property_restriction(meta_info_data)
        if prop_restriction is not None:
            target = '%s %s' % (target, prop_restriction)
        if depth == 1:
            target = '%s.' % target

        description.append(target)

    def _get_property_restriction(self, meta_info_data):
        prop_restriction = None

        if len(meta_info_data.pattern) > 0:
            prop_restriction = 'with pattern: {0}'.format(meta_info_data.pattern[0])
        else:
            if len(meta_info_data.prange) > 0:
                restriction = format_range_string(meta_info_data.prange)
                if meta_info_data.ptype in ('str', 'string'):
                    prop_restriction = 'with length: {0}'.format(restriction)
                else:
                    prop_restriction = 'with range: {0}'.format(restriction)

        return prop_restriction

    def _add_comment_on_leaf_ref_source(self, description, meta_info_data):
        if len(meta_info_data.target_of_leafref) > 0:
            description.append('Refers to %s' % (meta_info_data.target_of_leafref))

    def _add_comment_on_mandatory(self, description, meta_info_data):
        if meta_info_data.mandatory:
            description.append('This attribute is mandatory.')

    # todo: duplicate / not sure if this is needed
    # def _add_comment_on_presence(self, description, meta_info_data):
    #     pass

    def _add_comment_on_units(self, description, meta_info_data):
        if len(meta_info_data.units) > 0:
            description.append('Units are %s.' % meta_info_data.units)

    def _add_comment_on_default_value(self, description, meta_info_data):
        if len(meta_info_data.default_value) > 0:
            description.append('The default value is %s.' % meta_info_data.default_value)

    # todo: untested
    def _add_comment_on_status(self, description, meta_info_data):
        pass
        # if len(meta_info_data.status) > 0:
            # properties_description.append('The status is %s' % meta_info_data.status)

def get_type_name(prop_type):
    if prop_type.name in ('str', 'string'):
        return 'string'
    if prop_type.name in ('bool', 'boolean'):
        return 'bool'
    elif prop_type.name == 'leafref':
        return 'string'
    elif prop_type.name == 'decimal64':
        return 'string'
    elif prop_type.name == 'union':
        return 'string'
    elif prop_type.name == 'binary':
        return 'string'
    elif prop_type.name == 'instance-identifier':
        return 'string'
    elif isinstance(prop_type, Bits):
        return 'map[string]bool'
    elif isinstance(prop_type, Class) and prop_type.is_identity():
        return 'interface{}'
    elif prop_type.name == 'leafref':
        return 'interface{}'
    elif isinstance(prop_type, Enum):
        return prop_type.go_name()
    elif isinstance(prop_type, DataType):
        return 'string'
    return 'interface{}'

