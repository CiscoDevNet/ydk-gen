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
  _types_extractor.py

 Extractor for types
"""

from pyang.types import EnumerationTypeSpec, BitsTypeSpec, UnionTypeSpec, PathTypeSpec, \
    IdentityrefTypeSpec


class TypesExtractor(object):
    def __init__(self):
        self.get_enum_type_stmt = lambda stmt: self._get_type_stmt(stmt, EnumerationTypeSpec)
        self.get_identity_ref_type_stmt = lambda stmt: self._get_type_stmt(
            stmt, IdentityrefTypeSpec)
        self.get_bits_type_stmt = lambda stmt: self._get_type_stmt(stmt, BitsTypeSpec)
        self.get_union_type_stmt = lambda stmt: self._get_type_stmt(stmt, UnionTypeSpec)

    def _get_type_stmt(self, stmt, typeSpec):
        if stmt.keyword == 'type':
            type_stmt = stmt
        else:
            type_stmt = stmt.search_one('type')

        if hasattr(type_stmt, 'i_typedef') and type_stmt.i_typedef is not None:
            typedef_stmt = type_stmt.i_typedef
            return self._get_type_stmt(typedef_stmt, typeSpec)
        elif hasattr(type_stmt, 'i_type_spec'):
            type_spec = type_stmt.i_type_spec
            while isinstance(type_spec, PathTypeSpec):
                if not hasattr(type_spec, 'i_target_node'):
                    return None
                type_stmt = type_spec.i_target_node.search_one('type')
                type_spec = type_stmt.i_type_spec
                if hasattr(type_stmt, 'i_typedef') and type_stmt.i_typedef is not None:
                    typedef_stmt = type_stmt.i_typedef
                    return self._get_type_stmt(typedef_stmt, typeSpec)

            if isinstance(type_spec, typeSpec):
                return type_stmt
            else:
                return None
        else:
            return None
