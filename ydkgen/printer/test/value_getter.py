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
property_handler.py

Generate and return property value.
"""
import sys
import base64
from collections import namedtuple
from random import choice, randint, getrandbits

from pyang.types import (BinaryTypeSpec, BooleanTypeSpec, Decimal64TypeSpec,
                         EmptyTypeSpec, IntTypeSpec, LengthTypeSpec,
                         PathTypeSpec, PatternTypeSpec, RangeTypeSpec,
                         StringTypeSpec, UnionTypeSpec)
from ydkgen.api_model import Bits, Enum
from ydkgen.builder import TypesExtractor
from .utils import (is_identity_element, get_length_limits, get_range_limits,
                    get_string, get_typedef_stmt, get_qn, get_nmsp_sep)

STR_LIMIT = 47
INT_LIMIT = 2147483647

BitsValue = namedtuple('BitsValue', 'val type_spec')


class ValueGetter(object):
    def __init__(self, identity_subclasses, language):
        self.language = language
        self.identity_subclasses = identity_subclasses
        self.types_extractor = TypesExtractor()
        self.type_spec = None
        self.type_stmt = None
        self.trans_map = dict.fromkeys(range(32))

    def __call__(self, prop):
        self._set_prop_type(prop)
        prop_value = None

        if isinstance(self.type_spec, Bits):
            prop_value = self._handle_bits()
        elif is_identity_element(self.type_spec):
            prop_value = self._handle_identity()
        elif isinstance(self.type_spec, Enum):
            prop_value = self._handle_enum()
        elif isinstance(self.type_spec, BinaryTypeSpec):
            prop_value = self._handle_binary()
        elif isinstance(self.type_spec, BooleanTypeSpec):
            prop_value = self._handle_boolean(prop)
        elif isinstance(self.type_spec, Decimal64TypeSpec):
            prop_value = self._handle_decimal64()
        elif isinstance(self.type_spec, EmptyTypeSpec):
            prop_value = self._handle_empty()
        elif isinstance(self.type_spec, LengthTypeSpec):
            prop_value = self._handle_length()
        elif isinstance(self.type_spec, IntTypeSpec):
            prop_value = self._handle_int()
        elif isinstance(self.type_spec, PatternTypeSpec):
            prop_value = self._handle_pattern(prop)
        elif isinstance(self.type_spec, RangeTypeSpec):
            prop_value = self._handle_range()
        elif isinstance(self.type_spec, StringTypeSpec):
            prop_value = self._handle_string()

        return prop_value

    def _handle_bits(self):
        val = choice(list(self.type_spec._dictionary.keys()))
        return BitsValue(val=val, type_spec=self.type_spec)

    def _handle_identity(self):
        identities = set()
        self._collect_identities(self.type_spec, identities)
        return '{}()'.format(get_qn(choice(list(identities)), self.language))

    def _handle_enum(self):
        literal = choice(self.type_spec.literals)
        qn = get_qn(self.type_spec, self.language)
        sep = get_nmsp_sep(self.language)
        return sep.join([qn, literal.name])

    def _handle_binary(self):
        random_string = get_string(0)
        if sys.version_info >= (3, 0):
            bin_string = base64.b64encode(str.encode(random_string)).decode()
        else:
            bin_string = base64.b64encode(random_string)
        return self._render_string(bin_string)

    def _handle_boolean(self, prop):
        boolean = str(bool(getrandbits(1)))
        # hard code to enable
        if prop.name == 'enabled':
            boolean = 'True'

        if self.language == 'cpp':
            boolean = boolean.lower()

        return boolean

    def _handle_decimal64(self, low=None, high=None):
        low = self.type_spec.min if low is None else low
        high = self.type_spec.max if high is None else high

        decimal64 = randint(low.value, high.value)
        minus = '-' if decimal64 < 0 else ''
        decimal64 = '{:=19d}'.format(decimal64)
        fractions = self.type_spec.fraction_digits
        decimal64 = '.'.join([decimal64[:-fractions], decimal64[-fractions:]])
        decimal64 = decimal64.strip('- 0')
        decimal64 = '"{}{}"'.format(minus, decimal64)
        if self.language == 'cpp':
            decimal64 = 'std::string{{{}}}'.format(decimal64)
        return 'Decimal64({})'.format(decimal64)

    def _handle_empty(self):
        return 'Empty()'

    def _handle_length(self):
        low, high = choice(get_length_limits(self.type_spec))
        self.type_spec = self.type_spec.base
        self.type_stmt = get_typedef_stmt(self.type_stmt)
        return self._handle_string(low=low, high=high)

    def _handle_int(self, low=None, high=None):
        if low is None:
            low = self.type_spec.min
        if high is None:
            high = self.type_spec.max
        if self.language == 'cpp':
            low, high = 0, STR_LIMIT
        return randint(low, high)

    def _handle_pattern(self, prop):
        patterns = self._collect_patterns()

        # hard code, regex YANG regex syntax(XSD) is different from Python
        pattern = None
        for p in patterns:
            if '(%[\\p{N}\\p{L}]+)?' in p.arg:
                pattern = p.arg.replace('(%[\\p{N}\\p{L}]+)?', '')
                break

        if pattern is None:
            pattern = choice(patterns).arg

        pattern_value = get_string(pattern=pattern)

        # hard code value
        if 'ipv4' in self.type_stmt.arg:
            pattern_value = '10.0.0.1'
        if 'ipv6' in self.type_stmt.arg:
            pattern_value = '2001:db8::ff:2'
        if 'masklength_range' in prop.name:
            pattern_value = '21..24'
        if 'ip_prefix' in prop.name:
            pattern_value = '10.0.0.1/32'

        pattern_value = pattern_value.strip(':').lower()

        return self._render_string(pattern_value)

    def _handle_range(self):
        low, high = choice(get_range_limits(self.type_spec))
        self.type_spec = self.type_spec.base
        self.type_stmt = get_typedef_stmt(self.type_stmt)
        if isinstance(self.type_spec, IntTypeSpec):
            return self._handle_int(low=low, high=high)
        elif isinstance(self.type_spec, Decimal64TypeSpec):
            return self._handle_decimal64(low=low, high=high)

    def _handle_string(self, low=None, high=None, pattern=None):
        string_value = get_string(low, high)
        return self._render_string(string_value)

    def _render_string(self, value):
        # remove null control or characters
        value = value.translate(self.trans_map)
        if self.language == 'py':
            return '"""%s"""' % value
        elif self.language == 'cpp':
            return 'R"(%s)"' % value

    def _set_prop_type(self, prop):
        type_spec = prop.property_type
        type_stmt = prop.stmt.search_one('type')

        if isinstance(type_spec, PathTypeSpec):
            type_stmt = self._get_path_target_type_stmt(type_stmt)
            type_spec = type_stmt.i_type_spec

        if isinstance(type_spec, UnionTypeSpec):
            type_spec, type_stmt = self._get_union_type_spec(type_spec)

        self.type_spec = type_spec
        self.type_stmt = type_stmt

    def _get_path_target_type_stmt(self, type_stmt):
        type_spec = type_stmt.i_type_spec
        while all([isinstance(type_spec, PathTypeSpec),
                   hasattr(type_spec, 'i_target_node')]):
            type_stmt = type_spec.i_target_node.search_one('type')
            type_spec = type_stmt.i_type_spec
        return type_stmt

    def _get_union_type_spec(self, orig_type_spec):
        type_spec = orig_type_spec
        while isinstance(type_spec, UnionTypeSpec):
            type_stmt = choice(type_spec.types)
            type_spec = self.types_extractor.get_property_type(type_stmt)
            if hasattr(type_spec, 'i_type_spec'):
                type_spec = type_spec.i_type_spec

        return type_spec, type_stmt

    def _collect_identities(self, identity, identities):
        identity_id = id(identity)
        if identity_id in self.identity_subclasses:
            derived_identities = set(self.identity_subclasses[identity_id])
            identities |= derived_identities
            for identity in derived_identities:
                self._collect_identities(identity, identities)

    def _collect_patterns(self):
        patterns = []
        type_stmt = self.type_stmt
        while all([hasattr(type_stmt, 'i_typedef') and
                   type_stmt.i_typedef is not None]):
            patterns.extend(type_stmt.search('pattern'))
            type_stmt = type_stmt.i_typedef.search_one('type')
        patterns.extend(type_stmt.search('pattern'))

        return patterns
