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
  meta_data_util.py 
  
  YANG model driven API, python emitter.
"""

from ydkgen.api_model import Class, Enum, Bits
from ydkgen.builder import TypesExtractor
from ydkgen.common import convert_to_reStructuredText, get_module_name
from pyang import types
from pyang.error import EmitError
from pyang.types import BinaryTypeSpec, BooleanTypeSpec, Decimal64TypeSpec, EmptyTypeSpec, \
    IntTypeSpec, LengthTypeSpec, PatternTypeSpec, RangeTypeSpec, StringTypeSpec, PathTypeSpec, \
    BitsTypeSpec, IdentityrefTypeSpec, UnionTypeSpec, TypeSpec


class MetaInfoData:

    '''
    Meta info for a property
    '''

    def __init__(self, prop):
        self.name = prop.stmt.arg
        self.mtype = ''
        self.ptype = ''
        self.prange = []
        self.pattern = []
        self.presentation_name = "%s" % prop.name
        self.module_name = "%s" % get_module_name(prop.stmt)
        self.pmodule_name = None
        self.clazz_name = None
        self.is_many = prop.is_many
        self.doc_link = None
        self.children = []
        self.comment = prop.comment
        self.is_key = prop.is_key()
        self.max_elements = prop.max_elements
        self.min_elements = prop.min_elements


def get_class_docstring(clazz):
    class_description = ''
    if clazz.comment is not None:
        class_description = clazz.comment

    properties_description = []
    for prop in clazz.properties():
        prop_comment = ''
        if prop.comment is not None:
            prop_comment = prop.comment.replace('\n', ' ')
            if prop_comment.endswith('.'):
                prop_comment = prop_comment[:-1]
        meta_info_data = get_meta_info_data(
            prop, prop.property_type, prop.stmt.search_one('type'))
        if meta_info_data is None:
            continue
        doc_link = meta_info_data.doc_link

        prop_restriction = get_property_restriction(meta_info_data)

        properties_description.append('.. attribute:: %s\n\n' % (prop.name))

        properties_description.append('\t%s\n' % (
            convert_to_reStructuredText(prop_comment)))

        properties_description.append('\t**type**\: %s\n\n' % doc_link)

        if prop_restriction is not None and len(prop_restriction) > 0:
            properties_description.append('\t%s\n\n' % prop_restriction)

        if clazz.stmt.search_one('presence'):
            properties_description.append(add_presence_property_docstring(clazz))

    return convert_to_reStructuredText(class_description) + '\n\n' + ''.join(properties_description)


def add_presence_property_docstring(clazz):
    description = []
    description.append(".. attribute:: %s\n\n" % ("_is_presence"))
    description.append("\tIs present if this instance represents presence container else not\n")
    description.append("\t**type**\: bool\n\n")


    return ''.join(description)


def get_enum_class_docstring(enumz):
    enumz_description = ''
    if enumz.comment is not None:
        enumz_description = enumz.comment

    enumz_description = "%s\n\n\n" % (enumz.name) + enumz_description

    literals_description = []
    for enum_literal in enumz.literals:
        literals_description.append(".. data:: %s = %s\n" % (enum_literal.name, enum_literal.value))
        if enum_literal.comment is not None:
            for line in enum_literal.comment.split("\n"):
                literals_description.append("\t%s\n\n" % line)

    return ''.join([convert_to_reStructuredText(enumz_description)] + ['\n\n'] + literals_description)


def get_property_restriction(meta_info_data):
    prop_restriction = ''

    if len(meta_info_data.pattern) > 0:
        prop_restriction = '**pattern:** {0}'.format(
            convert_to_reStructuredText(meta_info_data.pattern[0]))
    else:
        if len(meta_info_data.prange) > 0:
            prop_restriction = '**range:** {0}'. \
                format(convert_to_reStructuredText(
                    format_range_string(meta_info_data.prange)))

    return prop_restriction


def format_range_string(ranges):
    range_string = ''
    for min_limit, max_limit in ranges:
        if len(range_string) > 0:
            range_string += ' | '
        if max_limit is not None:
            range_string += "{0}..{1}".format(min_limit, max_limit)
        else:
            range_string += str(min_limit)
    return range_string


def get_meta_info_data(prop, property_type, type_stmt):
    """ Gets an instance of MetaInfoData that has the useful information about the property.

        Args:
            prop: The property
            property_type : The type under consideration
            type_stmt : The type stmt currently under consideration 
    """
    clazz = prop.owner
    meta_info_data = MetaInfoData(prop)
    types_extractor = TypesExtractor()
    target_type_stmt = type_stmt

    if isinstance(property_type, Class):
        meta_info_data.pmodule_name = "'%s'" % property_type.get_py_mod_name()
        meta_info_data.clazz_name = "'%s'" % property_type.qn()

        meta_info_data.doc_link = ':py:class:`%s <%s.%s>`' % (
            property_type.name, property_type.get_py_mod_name(), property_type.qn())

        if prop.is_many:
            meta_info_data.mtype = 'REFERENCE_LIST'
            meta_info_data.doc_link = 'list of %s' % meta_info_data.doc_link
        elif property_type.is_identity():
            meta_info_data.mtype = 'REFERENCE_IDENTITY_CLASS'
        else:
            meta_info_data.mtype = 'REFERENCE_CLASS'
        # if the class is local use just the local name
        if property_type in clazz.owned_elements:
            meta_info_data.ptype = property_type.name
        else:
            meta_info_data.ptype = property_type.qn()

    elif isinstance(property_type, Enum):
        meta_info_data.pmodule_name = "'%s'" % property_type.get_py_mod_name()
        meta_info_data.clazz_name = "'%s'" % property_type.qn()
        meta_info_data.doc_link = ':py:class:`%s <%s.%s>`' % (
            property_type.name, property_type.get_py_mod_name(), property_type.qn())

        meta_info_data.mtype = 'REFERENCE_ENUM_CLASS'
        if prop.is_many:
            meta_info_data.mtype = 'REFERENCE_LEAFLIST'
            meta_info_data.doc_link = 'list of %s' % meta_info_data.doc_link

        if prop.property_type in clazz.owned_elements:
            meta_info_data.ptype = property_type.name
        else:
            meta_info_data.ptype = property_type.qn()

    elif isinstance(property_type, Bits):
        meta_info_data.pmodule_name = "'%s'" % property_type.get_py_mod_name()
        meta_info_data.clazz_name = "'%s'" % property_type.qn()
        meta_info_data.doc_link = ':py:class:`%s <%s.%s>`' % (
            property_type.name, property_type.get_py_mod_name(), property_type.qn())

        meta_info_data.mtype = 'REFERENCE_BITS'
        if prop.is_many:
            meta_info_data.mtype = 'REFERENCE_LEAFLIST'
            meta_info_data.doc_link = 'list of %s' % meta_info_data.doc_link

        if prop.property_type in clazz.owned_elements:
            meta_info_data.ptype = property_type.name
        else:
            meta_info_data.ptype = property_type.qn()

    else:
        if prop.stmt.keyword == 'leaf-list':
            meta_info_data.mtype = 'REFERENCE_LEAFLIST'
            meta_info_data.doc_link = 'list of '
        elif prop.stmt.keyword == 'anyxml':
            meta_info_data.mtype = 'ANYXML_CLASS'
            meta_info_data.doc_link = 'anyxml'
            meta_info_data.ptype = 'object'
            return meta_info_data
        else:
            meta_info_data.mtype = 'ATTRIBUTE'
            meta_info_data.doc_link = ''

        type_spec = type_stmt.i_type_spec

        while isinstance(type_spec, PathTypeSpec):
            if not hasattr(type_spec, 'i_target_node'):
                return None
            target_type_stmt = type_spec.i_target_node.search_one('type')
            type_spec = target_type_stmt.i_type_spec

        if isinstance(type_spec, BinaryTypeSpec):
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += meta_info_data.ptype
        elif isinstance(type_spec, BitsTypeSpec):
            # This can happen in a Union
            raise EmitError('Illegal Code path')
        elif isinstance(type_spec, BooleanTypeSpec):
            meta_info_data.ptype = 'bool'
            meta_info_data.doc_link += meta_info_data.ptype
        elif isinstance(type_spec, Decimal64TypeSpec):
            meta_info_data.ptype = 'Decimal64'
            meta_info_data.prange.append(
                ('%s' % type_spec.min.s, '%s' % type_spec.max.s))
            # ' :ref:`Decimal64 <ydk_models_types_Decimal64>`'
            meta_info_data.doc_link += ':py:class:`Decimal64 <ydk.types.Decimal64>`'
        elif isinstance(type_spec, EmptyTypeSpec):
            meta_info_data.ptype = 'Empty'
            # ' :ref:`Empty <ydk_models_types_Empty>`'
            meta_info_data.doc_link += ':py:class:`Empty <ydk.types.Empty>`'
        elif isinstance(prop.property_type, Enum):
            raise EmitError('Illegal Code path')
        elif isinstance(type_spec, IdentityrefTypeSpec):
            raise EmitError('Illegal Code path')
        elif isinstance(type_spec, IntTypeSpec):
            meta_info_data.ptype = 'int'
            meta_info_data.doc_link += meta_info_data.ptype
            meta_info_data.prange.append((type_spec.min, type_spec.max))
        elif isinstance(type_spec, LengthTypeSpec):
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += meta_info_data.ptype
            meta_info_data.prange = get_length_limits(type_spec)

        elif isinstance(type_spec, PathTypeSpec):
            raise EmitError('Illegal Code path')
        elif isinstance(type_spec, PatternTypeSpec):
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += meta_info_data.ptype
            while hasattr(target_type_stmt, 'i_typedef') and target_type_stmt.i_typedef is not None:
                target_type_stmt = target_type_stmt.i_typedef.search_one(
                    'type')
            pattern = target_type_stmt.search_one('pattern')
            if pattern is not None:
                meta_info_data.pattern.append(pattern.arg.encode('ascii'))
        elif isinstance(type_spec, RangeTypeSpec):
            meta_info_data.ptype = get_range_base_type_name(type_spec)
            meta_info_data.prange = get_range_limits(type_spec)
            meta_info_data.doc_link += meta_info_data.ptype

        elif isinstance(type_spec, StringTypeSpec):
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += meta_info_data.ptype
        elif isinstance(type_spec, UnionTypeSpec):
            # validate against all the data types
            meta_info_data.mtype = 'REFERENCE_UNION'
            meta_info_data.ptype = 'str'
            meta_info_data.property_type = type_spec
            if len(type_spec.types) > 0:
                meta_info_data.doc_link += 'one of { '
                for contained_type_stmt in type_spec.types:
                    enum_type_stmt = types_extractor.get_enum_type_stmt(contained_type_stmt)
                    bits_type_stmt = types_extractor.get_bits_type_stmt(contained_type_stmt)
                    union_type_stmt = types_extractor.get_union_type_stmt(contained_type_stmt)
                    contained_property_type = contained_type_stmt.i_type_spec
                    if isinstance(contained_property_type, IdentityrefTypeSpec):
                        contained_property_type = contained_property_type.base.i_identity.i_class
                    elif enum_type_stmt is not None:
                        # this is an enumeration
                        if not hasattr(enum_type_stmt, 'i_enum'):
                            raise EmitError('Failure to get i_enum')
                        contained_property_type = enum_type_stmt.i_enum
                    elif bits_type_stmt is not None:
                        # bits
                        contained_property_type = bits_type_stmt.i_bits
                    elif union_type_stmt is not None:
                        contained_property_type = union_type_stmt
                    child_meta_info_data = get_meta_info_data(
                        prop, contained_property_type, contained_type_stmt)
                    meta_info_data.children.append(child_meta_info_data)
                    if meta_info_data.doc_link[-1:] != ' ':
                        meta_info_data.doc_link += ' | '
                    meta_info_data.doc_link += child_meta_info_data.doc_link
                meta_info_data.doc_link += ' }'

        elif isinstance(type_spec, TypeSpec) and type_spec.name == 'instance-identifier':
            # Treat as string
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += meta_info_data.ptype
        else:
            raise EmitError('Illegal path')
    return meta_info_data


def get_length_limits(length_type):
    assert isinstance(length_type, LengthTypeSpec)
    prange = []
    for m_min, m_max in length_type.lengths:
        pmin = None
        pmax = None
        if m_min == 'min':
            pmin = '0'
        else:
            pmin = m_min
        if m_max == 'max':
            pmax = '18446744073709551615'
        else:
            pmax = m_max
        prange.append((pmin, pmax))
    return prange


def get_range_limits(range_type):
    prange = []
    base_type = get_range_base_type_spec(range_type)
    if isinstance(base_type, IntTypeSpec):
        for m_min, m_max in range_type.ranges:
            pmin = None
            pmax = None
            if m_min == 'min':
                pmin = range_type.base.min
            else:
                if m_min is not None:
                    pmin = m_min

            if m_max == 'max':
                pmax = range_type.base.max
            else:
                if m_max is not None:
                    pmax = m_max
            if types.yang_type_specs['uint64'].max == pmax:
                prange.append((long(pmin), long(pmax)))
            prange.append((pmin, pmax))
    elif isinstance(base_type, Decimal64TypeSpec):
        for m_min, m_max in range_type.ranges:
            pmin = None
            pmax = None
            if m_min == 'min':
                pmin = range_type.base.min.s
            else:
                if m_min is not None:
                    pmin = str(m_min)
            if m_max == 'max':
                pmax = range_type.base.max.s
            else:
                if m_max is not None:
                    pmax = str(m_max)
            prange.append(('%s' % pmin, '%s' % pmax))
    return prange


def get_range_base_type_name(range_type):
    base_type = get_range_base_type_spec(range_type)
    if isinstance(base_type, IntTypeSpec):
        ptype = 'int'
        for m_min, m_max in range_type.ranges:
            pmax = None
            if m_max == 'max':
                pmax = range_type.base.max
            else:
                if m_max is not None:
                    pmax = m_max
            if types.yang_type_specs['uint64'].max == pmax:
                ptype = 'long'
    elif isinstance(base_type, Decimal64TypeSpec):
        ptype = 'Decimal64'
    else:
        raise EmitError('Unknown range type')
    return ptype


def get_range_base_type_spec(range_type):
    if isinstance(range_type.base, RangeTypeSpec):
        return get_range_base_type_spec(range_type.base)
    return range_type.base
