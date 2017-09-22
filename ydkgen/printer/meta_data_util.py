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
from ydkgen.api_model import Bits, Class, Enum, Package
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
        self.doc_link_description = ''
        self.children = []
        self.comment = prop.comment
        self.is_key = prop.is_key()
        self.max_elements = prop.max_elements
        self.min_elements = prop.min_elements
        self.target_of_leafref = ''
        self.mandatory = False
        self.is_presence = False
        self.units = ''
        self.default_value = ''
        self.status = ''


def get_class_docstring(clazz, language, identity_subclasses=None):
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

        id_subclasses = None
        if (hasattr(prop.property_type, 'is_identity') and
            prop.property_type.is_identity() and identity_subclasses is not None):
            id_subclasses = identity_subclasses

        meta_info_data = get_meta_info_data(
            prop, prop.property_type, prop.stmt.search_one('type'), language, False,
            identity_subclasses=id_subclasses)

        if meta_info_data is None:
            continue

        keys = clazz.get_key_props()
        attribute_title = prop.name
        if prop in keys:
            attribute_title = '%s  <key>' % attribute_title
        properties_description.append('.. attribute:: %s\n\n' % (attribute_title))

        properties_description.append('\t%s\n' % (
            convert_to_reStructuredText(prop_comment)))

        properties_description.extend(get_type_doc(meta_info_data, type_depth=1))

    if clazz.stmt.search_one('presence') and language == 'py':
        properties_description.append(add_presence_property_docstring(clazz))

    return convert_to_reStructuredText(class_description) + '\n\n' + ''.join(properties_description)


def get_type_doc(meta_info_data, type_depth):
    properties_description = []

    if len(meta_info_data.children) > 0:
        if type_depth == 1:
            properties_description.append('\t**type**\: one of the below types:\n\n')
        for child in meta_info_data.children:
            properties_description.extend(get_type_doc(child, type_depth + 1))
            properties_description.append('\n----\n')
    else:
        target = meta_info_data.doc_link
        if isinstance(meta_info_data.doc_link, list):
            doc_link = map(lambda l: '\n\n\t\t%s' % l, meta_info_data.doc_link)
            target = ''.join(doc_link)
        properties_description.append('\t**type**\: %s %s\n\n' %
            (meta_info_data.doc_link_description, target))

        prop_restriction = get_property_restriction(meta_info_data)
        if prop_restriction is not None and len(prop_restriction) > 0:
            properties_description.append('\t%s\n\n' % (prop_restriction))
        if len(meta_info_data.target_of_leafref) > 0:
            properties_description.append('\t**refers to**\: %s\n\n' % (meta_info_data.target_of_leafref))
        if meta_info_data.mandatory:
            properties_description.append('\t**mandatory**\: True\n\n')
        if meta_info_data.is_presence:
            properties_description.append('\t**presence node**\: True\n\n')
        if len(meta_info_data.units) > 0:
            properties_description.append('\t**units**\: %s\n\n' % meta_info_data.units)
        if len(meta_info_data.default_value) > 0:
            properties_description.append('\t**default value**\: %s\n\n' % meta_info_data.default_value)
        if len(meta_info_data.status) > 0:
            properties_description.append('\t**status**\: %s\n\n' % meta_info_data.status)

    return properties_description


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
            restriction = convert_to_reStructuredText(
                    format_range_string(meta_info_data.prange))
            if meta_info_data.ptype == 'str':
                prop_restriction = '**length:** {0}'.format(restriction)
            else:
                prop_restriction = '**range:** {0}'.format(restriction)

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


def get_meta_info_data(prop, property_type, type_stmt, language, one_class_per_module, identity_subclasses=None):
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

    mandatory = prop.stmt.search_one('mandatory')
    if mandatory is not None and mandatory.arg == 'true':
        meta_info_data.mandatory = True

    presence = prop.stmt.search_one('presence')
    if presence is not None:
        meta_info_data.is_presence = True

    units = prop.stmt.search_one('units')
    if units is not None:
        meta_info_data.units = units.arg

    status = prop.stmt.search_one('status')
    if status is not None:
        meta_info_data.status = status.arg

    default_value = prop.stmt.search_one('default')
    if default_value is not None:
        meta_info_data.default_value = default_value.arg

    if isinstance(property_type, Class):
        meta_info_data.pmodule_name = "'%s'" % property_type.get_py_mod_name(one_class_per_module)
        if one_class_per_module:
            if property_type.is_identity():
                meta_info_data.clazz_name = "'%s%s'" % (
                    (property_type.owner.name + '.') if (not isinstance(property_type.owner, Package)) else '',
                    property_type.name)
            else:
                meta_info_data.clazz_name = "'%s'" % property_type.name
        else:
            meta_info_data.clazz_name = "'%s'" % property_type.qn()

        if identity_subclasses is None:
            meta_info_data.doc_link = get_class_crossref_tag(
                property_type.name, property_type, language)
        else:
            meta_info_data.doc_link = _get_identity_docstring(
                identity_subclasses, property_type, language)
            meta_info_data.doc_link_description = 'one of the below types:'

        if prop.stmt.keyword == 'leaf-list':
            meta_info_data.mtype = 'REFERENCE_LEAFLIST'
            target = ''
            if isinstance(meta_info_data.doc_link, list):
                doc_link = map(lambda l: '\n\n\t\t%s' % l, meta_info_data.doc_link)
                target = ''.join(doc_link)
            meta_info_data.doc_link = target
            meta_info_data.doc_link = '\n\t\t' + _get_list_doc_link_tag(
                meta_info_data, 'doc_link', language, meta_info_data.mtype)
        elif prop.stmt.keyword == 'list':
            meta_info_data.mtype = 'REFERENCE_LIST'
            meta_info_data.doc_link_description = _get_list_doc_link_tag(
                meta_info_data, 'doc_link_description', language, meta_info_data.mtype)
        elif property_type.is_identity():
            meta_info_data.mtype = 'REFERENCE_IDENTITY_CLASS'
        else:
            meta_info_data.mtype = 'REFERENCE_CLASS'
        # if the class is local use just the local name
        if one_class_per_module or property_type in clazz.owned_elements:
            meta_info_data.ptype = property_type.name
        else:
            meta_info_data.ptype = property_type.qn()

    elif isinstance(property_type, Enum):
        meta_info_data.pmodule_name = "'%s'" % property_type.get_py_mod_name(one_class_per_module)
        if one_class_per_module:
            meta_info_data.clazz_name = "'%s%s'" % (
                (property_type.owner.name + '.') if (not isinstance(property_type.owner, Package)) else '',
                property_type.name)
        else:
            meta_info_data.clazz_name = "'%s'" % property_type.qn()

        meta_info_data.doc_link = get_class_crossref_tag(property_type.name,
                                                         property_type,
                                                         language)
        meta_info_data.mtype = 'REFERENCE_ENUM_CLASS'
        if prop.is_many:
            meta_info_data.mtype = 'REFERENCE_LEAFLIST'
            meta_info_data.doc_link = _get_list_doc_link_tag(
                meta_info_data, 'doc_link', language, meta_info_data.mtype)

        if one_class_per_module or prop.property_type in clazz.owned_elements:
            meta_info_data.ptype = property_type.name
        else:
            meta_info_data.ptype = property_type.qn()

    elif isinstance(property_type, Bits):
        meta_info_data.pmodule_name = "'%s'" % property_type.get_py_mod_name(one_class_per_module)
        if one_class_per_module:
            meta_info_data.clazz_name = "'%s%s'" % (
            (property_type.owner.name + '.') if (not isinstance(property_type.owner, Package)) else '',
            property_type.name)
        else:
            meta_info_data.clazz_name = "'%s'" % property_type.qn()
        meta_info_data.doc_link = get_bits_doc_link(property_type, language)
        meta_info_data.mtype = 'REFERENCE_BITS'
        if prop.is_many:
            meta_info_data.mtype = 'REFERENCE_LEAFLIST'
            meta_info_data.doc_link = _get_list_doc_link_tag(
                meta_info_data, 'doc_link', language, meta_info_data.mtype)

        if one_class_per_module or prop.property_type in clazz.owned_elements:
            meta_info_data.ptype = property_type.name
        else:
            meta_info_data.ptype = property_type.qn()

    else:
        if prop.stmt.keyword == 'leaf-list':
            meta_info_data.mtype = 'REFERENCE_LEAFLIST'
            meta_info_data.doc_link = _get_list_tag(language, meta_info_data.mtype)
        elif prop.stmt.keyword == 'anyxml':
            meta_info_data.mtype = 'ANYXML_CLASS'
            meta_info_data.doc_link = 'anyxml'
            meta_info_data.ptype = 'object'
            return meta_info_data
        else:
            meta_info_data.mtype = 'ATTRIBUTE'
            meta_info_data.doc_link = ''

        type_spec = type_stmt.i_type_spec

        if isinstance(type_spec, PathTypeSpec):
            if prop.stmt.i_leafref_ptr is not None:
                reference_class = prop.stmt.i_leafref_ptr[0].parent.i_class
                reference_prop = prop.stmt.i_leafref_ptr[0].i_property
                tag = get_class_crossref_tag(reference_prop.name,
                                             reference_class, language)
                meta_info_data.target_of_leafref = tag
        while isinstance(type_spec, PathTypeSpec):
            if not hasattr(type_spec, 'i_target_node'):
                return None
            target_type_stmt = type_spec.i_target_node.search_one('type')
            type_spec = target_type_stmt.i_type_spec

        if isinstance(type_spec, BinaryTypeSpec):
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += get_primitive_type_tag('str', language)
        elif isinstance(type_spec, BitsTypeSpec):
            # This can happen in a Union
            raise EmitError('Illegal Code path')
        elif isinstance(type_spec, BooleanTypeSpec):
            meta_info_data.ptype = 'bool'
            meta_info_data.doc_link += get_primitive_type_tag('bool', language)
        elif isinstance(type_spec, Decimal64TypeSpec):
            meta_info_data.ptype = 'Decimal64'
            meta_info_data.prange.append(
                ('%s' % str(type_spec.min.s), '%s' % str(type_spec.max.s)))
            meta_info_data.doc_link += get_primitive_type_tag('Decimal64', language)
        elif isinstance(type_spec, EmptyTypeSpec):
            meta_info_data.ptype = 'Empty'
            meta_info_data.doc_link += get_primitive_type_tag('Empty', language)
        elif isinstance(prop.property_type, Enum):
            raise EmitError('Illegal Code path')
        elif isinstance(type_spec, IdentityrefTypeSpec):
            raise EmitError('Illegal Code path')
        elif isinstance(type_spec, IntTypeSpec):
            meta_info_data.ptype = 'int'
            meta_info_data.doc_link += meta_info_data.ptype
            lower = str(type_spec.min)
            upper = str(type_spec.max)
            meta_info_data.prange.append((lower, upper))
        elif isinstance(type_spec, LengthTypeSpec):
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += get_primitive_type_tag('str', language)
            meta_info_data.prange = get_length_limits(type_spec)

        elif isinstance(type_spec, PathTypeSpec):
            raise EmitError('Illegal Code path')
        elif isinstance(type_spec, PatternTypeSpec):
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += get_primitive_type_tag('str', language)
            while hasattr(target_type_stmt, 'i_typedef') and target_type_stmt.i_typedef is not None:
                target_type_stmt = target_type_stmt.i_typedef.search_one('type')
            pattern = target_type_stmt.search_one('pattern')
            if pattern is not None:
                meta_info_data.pattern.append(pattern.arg.encode('ascii'))
        elif isinstance(type_spec, RangeTypeSpec):
            meta_info_data.ptype = get_range_base_type_name(type_spec)
            meta_info_data.prange = get_range_limits(type_spec)
            meta_info_data.doc_link += meta_info_data.ptype

        elif isinstance(type_spec, StringTypeSpec):
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += get_primitive_type_tag('str', language)
        elif isinstance(type_spec, UnionTypeSpec):
            # validate against all the data types
            meta_info_data.mtype = 'REFERENCE_UNION'
            meta_info_data.ptype = 'str'
            meta_info_data.property_type = type_spec
            for contained_type_stmt in type_spec.types:
                contained_property_type = types_extractor.get_property_type(contained_type_stmt)
                child_meta_info_data = get_meta_info_data(
                    prop, contained_property_type, contained_type_stmt, language, one_class_per_module,
                    identity_subclasses=identity_subclasses)
                meta_info_data.children.append(child_meta_info_data)

        elif isinstance(type_spec, TypeSpec) and type_spec.name == 'instance-identifier':
            # Treat as string
            meta_info_data.ptype = 'str'
            meta_info_data.doc_link += get_primitive_type_tag('str', language)
        else:
            raise EmitError('Illegal path')
    return meta_info_data


def _get_identity_docstring(identity_subclasses, property_type, language):
    doc_link = []
    if id(property_type) in identity_subclasses:
        for subclass in identity_subclasses[id(property_type)]:
            doc_link.append(
                get_class_crossref_tag(subclass.name, subclass, language)
            )
            if id(subclass) in identity_subclasses:
                doc_link.extend(x for x in _get_identity_docstring(identity_subclasses, subclass, language) if x not in doc_link)
    else:
        doc_link.append(
            get_class_crossref_tag(property_type.name, property_type, language)
            )
    return doc_link


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
                prange.append((str(pmin), str(pmax)))
            prange.append((str(pmin), str(pmax)))
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
            prange.append(('%s' % str(pmin), '%s' % str(pmax)))
    return prange


def get_range_base_type_name(range_type):
    base_type = get_range_base_type_spec(range_type)
    if isinstance(base_type, IntTypeSpec):
        ptype = 'int'
    elif isinstance(base_type, Decimal64TypeSpec):
        ptype = 'Decimal64'
    else:
        raise EmitError('Unknown range type')
    return ptype


def get_range_base_type_spec(range_type):
    if isinstance(range_type.base, RangeTypeSpec):
        return get_range_base_type_spec(range_type.base)
    return range_type.base


def get_primitive_type_tag(typ, language):
    TYPE_TAG_MAP = {
        ('py', 'int'): 'int',
        ('py', 'str'): 'str',
        ('py', 'bool'): 'bool',
        ('py', 'Decimal64'): ':py:class:`Decimal64<ydk.types.Decimal64>`',
        ('py', 'Empty'): ':py:class:`Empty<ydk.types.Empty>`',
        ('py', 'Enum'): ':py:class:`enum.Enum`',
        ('py', 'Identity'): ':py:class:`object`',

        ('cpp', 'int'): '``int``',
        ('cpp', 'str'): '``std::string``',
        ('cpp', 'bool'): '``bool``',
        ('cpp', 'Decimal64'): ':cpp:class:`Decimal64<ydk::Decimal64>`',
        ('cpp', 'Empty'): ':cpp:class:`Empty<ydk::Empty>`',
        ('cpp', 'Enum'): ':cpp:class:`Enum<ydk::Enum>`',
        ('cpp', 'Identity'): ':cpp:class:`Identity<ydk::Identity>`',
    }
    try:
        return TYPE_TAG_MAP[(language, typ)]
    except KeyError:
        raise EmitError("Invalid language, type combination for rst documentation\n"
                        "language = %s, type = %s" % (language, typ))


def get_tag_template(language, domain, crossref):
    TEMPLATES = {
        ('py', 'class', False): '.. py:class:: %s\n',
        ('py', 'module', False): '.. py:module:: %s.%s\n',
        ('py', 'currentmodule', False): '.. py:currentmodule:: %s\n',
        ('cpp', 'class', False): '.. cpp:class:: %s\n',

        ('py', 'class', True): ' :py:class:`%s <%s.%s>`',
        ('cpp', 'class', True): ' :cpp:class:`%s <%s>`',
    }
    try:
        return TEMPLATES[(language, domain, crossref)]
    except KeyError:
        raise EmitError("Invalid language, domain, crossref combination for"
                        "rst documentation\n"
                        "language = %s, type = %s" % (language, domain, crossref))


def _get_list_tag(language, mtype):
    if language == 'py':
        return 'list of '
    elif language == 'cpp':
        if mtype == 'REFERENCE_LEAFLIST':
            return ':cpp:class:`ValueList<ydk::ValueList>` of '
        else:
            return '``std::vector`` of '

def _get_list_doc_link_tag(meta_info_data, attribute, language, mtype):
    template = '%s %%s' % _get_list_tag(language, mtype)
    return template % (getattr(meta_info_data, attribute))


def get_py_module_tag(named_element):
    template = get_tag_template('py', 'module', False)
    return template % (named_element.get_py_mod_name(),
                       named_element.name)


def get_py_currentmodule_tag(named_element):
    template = get_tag_template('py', 'currentmodule', False)
    return template % (named_element.get_py_mod_name())


def get_class_tag(named_element, language):
    if language == 'py':
        template = get_tag_template('py', 'class', False)
        return template % named_element.qn()
    elif language == 'cpp':
        template = get_tag_template('cpp', 'class', False)
        return template % named_element.qualified_cpp_name()


def get_class_crossref_tag(name, named_element, language):
    if language == 'py':
        template = get_tag_template('py', 'class', True)
        return template % (name,
                           named_element.get_py_mod_name(),
                           named_element.qn())
    elif language == 'cpp':
        template = get_tag_template('cpp', 'class', True)
        return template % (name,
                           named_element.fully_qualified_cpp_name())


def get_bits_class_docstring(bitz):
    bitz_description = []
    if bitz.comment is not None:
        for line in bitz.comment.split('\n'):
            bitz_description.append(convert_to_reStructuredText(line))
    bitz_description.append('\n\t**Bits positions\:**\n')
    for pos, name in enumerate(bitz._dictionary):
        bitz_description.append("\t\t%s\: %s\n" % (name, pos))

    return ''.join(bitz_description)


def get_bits_doc_link(bitz, language):
    if language == 'py':
        return get_class_crossref_tag(bitz.name, bitz, language)
    elif language == 'cpp':
        return ':cpp:class:`Bits<ydk::Bits>`\n\t%s' % get_bits_class_docstring(bitz)


def get_langage_spec_tags(named_element, language):
    tags = []
    if language == 'py':
        if isinstance(named_element, Package):
            tags.append(get_py_module_tag(named_element))
        else:
            tags.append(get_py_currentmodule_tag(named_element))
    return tags


def get_class_bases(clazz, language):
    bases = []
    if language == 'py':
        bases.append(':class:`object`')
    if isinstance(clazz, Enum):
        bases.append(get_primitive_type_tag('Enum', language))
    elif clazz.is_identity():
        bases.append(get_primitive_type_tag('Identity', language))
    elif language == 'cpp':
        bases.append(':cpp:class:`Entity<ydk::Entity>`')

    if hasattr(clazz, 'extends'):
        for item in clazz.extends:
            bases.append(get_class_crossref_tag(item.name, item, language))
    return bases
