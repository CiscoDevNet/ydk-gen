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
from ydkgen.api_model import Class, Enum, Bits
from .. import utils


class FixtureBuilder(object):

    def __init__(self, lang, identity_subclasses):
        self.lang = lang
        self.identity_subclasses = identity_subclasses

    def get_imports(self, package, builder):
        self.bundle_name = package.bundle_name
        imports = set()
        for imp in package.imported_types():
            imports.add(self._get_import_stmt(imp))

        for element in package.owned_elements:
            if isinstance(element, (Class, Enum, Bits)):
                imports.add(self._get_import_stmt(element))
        for element in package.owned_elements:
            if utils.is_class_element(element) and element.is_config():
                self._add_refclass_imports(element, imports)

        for identity in builder.derived_identities:
            imports.add(self._get_import_stmt(identity))

        return imports

    def _add_refclass_imports(self, clazz, imports):
        for prop in clazz.properties():
            self._add_prop_imports(prop, imports)

        for element in clazz.owned_elements:
            if isinstance(element, Class) and element.is_config():
                self._add_refclass_imports(element, imports)

    def _add_prop_imports(self, prop, imports):
        if utils.is_identity_prop(prop):
            identity = prop.property_type
            imports.add(self._get_import_stmt(identity))
            self._add_identity_import(id(identity), imports)
        elif utils.is_path_prop(prop):
            leafref_ptr = prop.stmt.i_leafref_ptr
            if leafref_ptr is not None:
                ptr, _ = prop.stmt.i_leafref_ptr
                ref_class = ptr.parent.i_class
                imports.add(self._get_import_stmt(ref_class))
        elif utils.is_union_prop(prop):
            ptype = prop.property_type
            self._add_union_imports(ptype, imports)

    def _add_union_imports(self, type_spec, imports):
        for type_stmt in type_spec.types:
            type_stmt = utils.get_typedef_stmt(type_stmt)
            if hasattr(type_stmt, 'i_type_spec'):
                type_spec = type_stmt.i_type_spec
                if utils.is_union_type_spec(type_spec):
                    self._add_union_imports(type_spec, imports)
                elif utils.is_identityref_type_spec(type_spec):
                    identity = type_spec.base.i_identity.i_class
                    imports.add(self._get_import_stmt(identity))
                    self._add_identity_import(id(identity), imports)

    def _add_identity_import(self, identity_id, imports):
        for subclass in self.identity_subclasses[identity_id]:
            imports.add(self._get_import_stmt(subclass))
            if id(subclass) in self.identity_subclasses:
                self._add_identity_import(id(subclass), imports)

    def _get_import_stmt(self, imp_type):
        if self.lang == 'py':
            mod_name = imp_type.get_py_mod_name()
            class_name = imp_type.qn().split('.')[0]
            return self.py_fmt.format(mod_name, class_name)
        elif self.lang == 'cpp':
            header_name = imp_type.get_cpp_header_name()
            bundle_name = imp_type.get_package().bundle_name
            if bundle_name != self.bundle_name:
                return self.cpp_fmt.format(header_name)
            else:
                return self.cpp_bundle_fmt.format(bundle_name, header_name)

    @property
    def py_fmt(self):
        return 'from {0} import {1}'

    @property
    def cpp_fmt(self):
        return '#include "{}"'

    @property
    def cpp_bundle_fmt(self):
        return '#include "ydk_{0}/{1}"'

