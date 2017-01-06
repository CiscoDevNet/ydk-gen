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
import rstr

from collections import namedtuple
from random import choice, randint, getrandbits
from pyang import types as ptypes
from ydkgen import api_model as atypes
from ydkgen.builder import TypesExtractor

from .. import utils

_LOW = 0
_HIGH = 33
BitsValue = namedtuple('BitsValue', 'val type_spec')
IdentityValue = namedtuple('IdentityValue', 'val identity')


class ValueBuilder(object):
    def __init__(self, lang, identity_subclasses):
        self.lang = lang
        self.identity_subclasses = identity_subclasses
        self.types_extractor = TypesExtractor()
        self.type_spec = None
        self.type_stmt = None

    def get_prop_value(self, prop, default=None):
        self._set_prop_type(prop)
        prop_value = None

        if isinstance(self.type_spec, atypes.Bits):
            prop_value = self._handle_bits()
        elif utils.is_identity_element(self.type_spec):
            prop_value = self._handle_identity()
        elif isinstance(self.type_spec, atypes.Enum):
            prop_value = self._handle_enum()
        elif isinstance(self.type_spec, ptypes.BinaryTypeSpec):
            prop_value = self._handle_binary()
        elif isinstance(self.type_spec, ptypes.BooleanTypeSpec):
            prop_value = self._handle_boolean(prop)
        elif isinstance(self.type_spec, ptypes.Decimal64TypeSpec):
            prop_value = self._handle_decimal64()
        elif isinstance(self.type_spec, ptypes.EmptyTypeSpec):
            prop_value = self._handle_empty()
        elif isinstance(self.type_spec, ptypes.LengthTypeSpec):
            prop_value = self._handle_length()
        elif isinstance(self.type_spec, ptypes.IntTypeSpec):
            prop_value = self._handle_int()
        elif isinstance(self.type_spec, ptypes.PatternTypeSpec):
            prop_value = self._handle_pattern(prop)
        elif isinstance(self.type_spec, ptypes.RangeTypeSpec):
            prop_value = self._handle_range()
        elif isinstance(self.type_spec, ptypes.StringTypeSpec):
            prop_value = self._handle_string(default=default)

        return prop_value

    def _handle_bits(self):
        val = choice(list(self.type_spec._dictionary.keys()))
        return BitsValue(val=val, type_spec=self.type_spec)

    def _handle_identity(self):
        identities = set()
        self._collect_identities(self.type_spec, identities)
        identity = choice(list(identities))
        identity_value = '{}()'.format(utils.get_qn(self.lang, identity))
        return IdentityValue(val=identity_value, identity=identity)

    def _handle_enum(self):
        literal = choice(self.type_spec.literals)
        qn = utils.get_qn(self.lang, self.type_spec)
        return self.nmsp_sep.join([qn, literal.name])

    def _handle_binary(self):
        random_string = self._get_string(0)
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

        if self.lang == 'cpp':
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
        if decimal64.endswith('.'):
            decimal64 += '0'
        decimal64 = '"{}{}"'.format(minus, decimal64)

        if self.lang == 'cpp':
            decimal64 = 'std::string{{{}}}'.format(decimal64)
        return 'Decimal64({})'.format(decimal64)

    def _handle_empty(self):
        return 'Empty()'

    def _handle_length(self):
        low, high = choice(self._get_length_limits(self.type_spec))
        self.type_spec = self.type_spec.base
        self.type_stmt = utils.get_typedef_stmt(self.type_stmt)
        return self._handle_string(low=low, high=high)

    def _handle_int(self, low=None, high=None):
        low = 0 if low is None else low
        high = low if high is None else high
        return self._handle_int_range(low, high)

    def _handle_int_range(self, low, high):
        int_value = randint(low, high)
        range_stmt = self.type_stmt.search_one('range')
        if range_stmt is not None:
            if hasattr(range_stmt, 'i_type_spec'):
                self.type_stmt = range_stmt
                self.type_spec = range_stmt.i_type_spec
                int_value = self._handle_range()
            else:
                low, high = choice(range_stmt.arg.split('|')).split('..')
                low, high = self._render_range(low, high)
                int_value = randint(low, high)

        return int_value

    def _handle_pattern(self, prop):
        # YANG regular expression syntax (XSD) is different from Python,
        # need to use hard code value
        patterns = self._collect_patterns()

        pattern = None
        for p in patterns:
            if '(%[\\p{N}\\p{L}]+)?' in p.arg:
                pattern = p.arg.replace('(%[\\p{N}\\p{L}]+)?', '')
                break

        if pattern is None:
            pattern = choice(patterns).arg

        pattern_value = self._get_string(pattern=pattern)

        if 'ipv4' in self.type_stmt.arg:
            pattern_value = '10.0.0.1'
        if 'ipv6' in self.type_stmt.arg:
            pattern_value = '2001:db8::ff:2'
        if 'password' in self.type_stmt.arg:
            pattern_value = '$0$password'
        if 'domain-name' in self.type_stmt.arg:
            pattern_value = 'domain.name'
        if 'masklength_range' in prop.name:
            pattern_value = '21..24'
        if prop.name in ('ip_prefix', 'address_prefix', 'fec_address'):
            pattern_value = '10.0.0.1/32'

        pattern_value = pattern_value.strip(':').lower()

        return self._render_string(pattern_value)

    def _handle_range(self):
        low, high = choice(self._get_range_limits(self.type_spec))
        self.type_spec = self.type_spec.base
        self.type_stmt = utils.get_typedef_stmt(self.type_stmt)
        if isinstance(self.type_spec, ptypes.IntTypeSpec):
            return self._handle_int(low=low, high=high)
        elif isinstance(self.type_spec, ptypes.Decimal64TypeSpec):
            return self._handle_decimal64(low=low, high=high)

    def _handle_string(self, default=None, low=None, high=None, pattern=None):
        string_value = self._get_string(low, high)
        string_value = default if default is not None else string_value
        return self._render_string(string_value)

    def _render_string(self, value):
        # remove null or control characters
        value = ''.join(c for c in value if ord(c) >= 32)
        if self.lang == 'py':
            return '"""%s"""' % value
        elif self.lang == 'cpp':
            return 'R"(%s)"' % value

    def _set_prop_type(self, prop):
        type_spec = prop.property_type
        type_stmt = prop.stmt.search_one('type')

        if isinstance(type_spec, ptypes.PathTypeSpec):
            type_stmt = self._get_path_target_type_stmt(type_stmt)
            type_spec = type_stmt.i_type_spec

        if isinstance(type_spec, ptypes.UnionTypeSpec):
            type_spec, type_stmt = self._get_union_type_spec(type_spec)

        self.type_spec = type_spec
        self.type_stmt = type_stmt

    def _get_path_target_type_stmt(self, type_stmt):
        type_spec = type_stmt.i_type_spec
        while all([isinstance(type_spec, ptypes.PathTypeSpec),
                   hasattr(type_spec, 'i_target_node')]):
            type_stmt = type_spec.i_target_node.search_one('type')
            type_spec = type_stmt.i_type_spec
        return type_stmt

    def _get_union_type_spec(self, orig_type_spec):
        type_spec = orig_type_spec
        while isinstance(type_spec, ptypes.UnionTypeSpec):
            type_stmt = choice(type_spec.types)
            type_spec = self.types_extractor.get_property_type(type_stmt)
            if hasattr(type_spec, 'i_type_spec'):
                type_spec = type_spec.i_type_spec

        return type_spec, type_stmt

    def _collect_patterns(self):
        patterns = []
        type_stmt = self.type_stmt
        while all([hasattr(type_stmt, 'i_typedef') and
                   type_stmt.i_typedef is not None]):
            patterns.extend(type_stmt.search('pattern'))
            type_stmt = type_stmt.i_typedef.search_one('type')
        patterns.extend(type_stmt.search('pattern'))

        return patterns

    def _collect_identities(self, identity, identities):
        identity_id = id(identity)
        if identity_id in self.identity_subclasses:
            derived_identities = set(self.identity_subclasses[identity_id])
            identities |= derived_identities
            for identity in derived_identities:
                self._collect_identities(identity, identities)

    def _get_string(self, low=None, high=None, pattern=None):
        low, high = self._render_range(low, high)
        if pattern is None or utils.is_match_all(pattern):
            pattern = r'[0-9a-zA-Z]'
        return rstr.xeger(pattern).rstrip("\\\"")

    def _get_length_limits(self, length_type):
        lengths = []
        for low, high in length_type.lengths:
            low, high = self._render_range(low, high)
            lengths.append((low, high))
        return lengths

    def _get_range_limits(self, range_type):
        ranges = []
        for low, high in range_type.ranges:
            low = self._get_range_min(range_type) if low == 'min' else low
            high = self._get_range_max(range_type) if high == 'max' else high
            ranges.append((low, high))
        return ranges

    def _get_range_min(self, range_type):
        range_min = range_type.base.min
        if isinstance(range_type, ptypes.Decimal64TypeSpec):
            range_min = range_min.s
        return range_min

    def _get_range_max(self, range_type):
        range_max = range_type.base.max
        if isinstance(range_type, ptypes.Decimal64TypeSpec):
            range_max = range_max.s
        return range_max

    def _render_range(self, low, high):
        if low in (None, 'min'):
            low = _LOW
        low = int(low)
        if high in (None, 'max'):
            high = low
        high = int(high)
        return low, high

    @property
    def nmsp_sep(self):
        sep = '.'
        if self.lang == 'cpp':
            sep = '::'
        return sep
