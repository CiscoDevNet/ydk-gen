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
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either exitess or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------
"""
util.py

Utility functions for test case generator.
"""
import rstr

from pyang.types import (PathTypeSpec, LengthTypeSpec, IntTypeSpec,
                         Decimal64TypeSpec, RangeTypeSpec, UnionTypeSpec,
                         IdentityrefTypeSpec, EmptyTypeSpec)
from ydkgen.api_model import Class

STR_LIMIT = 47


def has_terminal_nodes(prop):
    # has leaf or leaflist
    ptype = prop.property_type
    for p in ptype.properties():
        if is_terminal_prop(p):
            return True
    return False


def is_config_field(prop):
    ptype = prop.property_type
    return ptype.is_config()


def is_leafref_prop(prop):
    return (isinstance(prop.property_type, PathTypeSpec) and
            prop.stmt.i_leafref_ptr is not None)


def is_mandatory_prop(prop):
    mandatory = prop.stmt.search_one('mandatory')
    return mandatory is not None and mandatory.arg == 'true'


def is_leaflist_prop(prop):
    return prop.stmt.keyword == 'leaf-list'


def is_presence_prop(prop):
    return prop.stmt.search_one('presence') is not None


def is_terminal_prop(prop):
    return not is_class_prop(prop)


def is_empty_prop(prop):
    return isinstance(prop.property_type, EmptyTypeSpec)


def is_class_prop(prop):
    return is_class_element(prop.property_type)


def is_identity_prop(prop):
    return is_identity_element(prop.property_type)


def is_decimal64_prop(prop):
    return isinstance(prop.property_type, Decimal64TypeSpec)


def is_path_prop(prop):
    return isinstance(prop.property_type, PathTypeSpec)


def is_union_prop(prop):
    return isinstance(prop.property_type, UnionTypeSpec)


def is_union_type_spec(type_spec):
    return isinstance(type_spec, UnionTypeSpec)


def is_identityref_type_spec(type_spec):
    return isinstance(type_spec, IdentityrefTypeSpec)


def is_identity_element(element):
    return isinstance(element, Class) and element.is_identity()


def is_class_element(element):
    return isinstance(element, Class) and not element.is_identity()


def is_list_element(element):
    return element.stmt.keyword == 'list'


def is_ref_prop(prop):
    return (is_leafref_prop(prop) or is_identityref_prop(prop))


def is_identityref_prop(prop):
    return (isinstance(prop.property_type, Class) and
            prop.property_type.is_identity() and
            prop.stmt.i_leafref_ptr is not None)


def get_length_limits(length_type):
    assert isinstance(length_type, LengthTypeSpec)
    lengths = []
    for low, high in length_type.lengths:
        low = '0' if low == 'min' else low
        high = STR_LIMIT if high == 'max' else high
        lengths.append((low, high))
    return lengths


def get_range_limits(range_type):
    assert isinstance(range_type, RangeTypeSpec)
    ranges = []
    for low, high in range_type.ranges:
        low = _get_range_min(range_type) if low == 'min' else low
        high = _get_range_max(range_type) if high == 'max' else high
        ranges.append((low, high))
    return ranges


def _get_range_min(range_type):
    if isinstance(range_type, IntTypeSpec):
        return range_type.base.min
    elif isinstance(range_type, Decimal64TypeSpec):
        return range_type.base.min.s


def _get_range_max(range_type):
    if isinstance(range_type, IntTypeSpec):
        return range_type.base.max
    elif isinstance(range_type, Decimal64TypeSpec):
        return range_type.base.max.s


def get_string(low=None, high=None, pattern=None):
    low = 0 if low is None else low
    high = STR_LIMIT if high is None else high
    if pattern is None or _is_match_all(pattern):
        pattern = r'[0-9a-zA-Z]'
    return rstr.xeger(pattern).rstrip("\\\"")


def _is_match_all(pattern):
    return pattern in ('[^\*].*', '\*')


def get_typedef_stmt(type_stmt):
    while all([hasattr(type_stmt, 'i_typedef') and
               type_stmt.i_typedef is not None]):
        type_stmt = type_stmt.i_typedef.search_one('type')
    return type_stmt


def get_qn(element, language):
    qn = ''
    if language == 'py':
        qn = element.qn()
    elif language == 'cpp':
        qn = element.fully_qualified_cpp_name()
    return qn


def get_nmsp_sep(language):
    sep = ''
    if language == 'py':
        sep = '.'
    elif language == 'cpp':
        sep = '::'
    return sep


