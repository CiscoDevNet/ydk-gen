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
test_cases_builder.py

Build an individual test case within a test program file.
"""
from ydkgen import api_model as atypes
from ydkgen.common import is_pkg_element, get_top_class, get_obj_name, get_qn, \
                        is_presence_element, is_list_element, is_mandatory_element, \
                        is_class_prop, is_class_element, is_config_prop, \
                        is_reference_prop, is_terminal_prop, is_leaflist_prop, \
                        is_union_prop, get_element_path

from .test_value_builder import ValueBuilder, BitsValue, IdentityValue


# Default value need to be set
_DEFAULT_VALUES = {
    'isis.instances.instance[0].instance_name': 'DEFAULT',
    'isis.instances.instance[0].running': 'Empty()',
    'policy_name': 'PASS-ALL',
    'route_policy_name': 'PASS-ALL',
    'system.radius.server[0].name': 'udp',
}

# Nodes listed below represent leafref values should be set by default.
# Leafref L1 needs to be set if L1 --> L2 and L2 is L1's children,
# or children's children etc,.
_DEFAULT_LEAFREFS = set({
    'afi_safi[0].afi_safi_name',
    'vlan[0].vlan_id',
    'destination_group[0].group_id',
    'destination_group[0].destinations.destination[0].destination_address',
    'destination_group[0].destinations.destination[0].destination_port',
    'sensor_group[0].sensor_group_id',
    'sensor_path[0].path',
    'subscription[0].subscription_id',
    'sensor_profile[0]->sensor_group'
})

# Enable afi-safi for some nodes.
_RELATIVE_DEFAULTS = {
    'afi_safi': 'config/enabled'
}


class Statements(object):
    """Glob class for test case statements.

    Atttributes:
        - append_stmts(dict): List append statements:
            - Python: list_object.append(list_item)
            - C++: list_object.emplace_back(std::move(list_item));
        - reference_stmts(dict): Reference statements:
            - Python: object_bob = object_alice
            - C++: object_bob = object_alice.get();
        - adjustment_stmts(dict): Adjustment statements:
            - Python: object_bob = object_alice
            - C++: object_bob = object_alice.get();
        - assignment_stmts(dict): Assignment statements:
            - Python: object_bob = object_alice
            - C++: auto object_bob = object_alice
        - declaration_stmts(dict): Declaration statements:
            - Python: object_bob = ObjectBobClass()
            - C++ : auto object_bob = std::make_unique<ObjectBobClass>();
        - leaflist_append_stmts(dict): leaf-list append statements:
            - Python: leaf_list_object.append(leaf_list_item)
            - C++: leaf_list_object.append(std::move(leaf_list_item));
        - reference_adjustment_stmts(dict): Adjustment statements
            for references:
            - Python: object_bob = object_alice
            - C++: object_bob = object_alice.get();
        - asjusted_leaflist_appends(dict): Adjustment statements
            for leaf-lists:
            - Python: leaf_list_object.append(leaf_list_item)
            - C++: leaf_list_object.append(std::move(leaf_list_item));
        - key_properties(dict): Set to record list key properties already set.
    """

    def __init__(self):
        self.append_stmts = {}
        self.reference_stmts = {}
        self.adjustment_stmts = {}
        self.assignment_stmts = {}
        self.declaration_stmts = {}
        self.leaflist_append_stmts = {}
        self.reference_adjustment_stmts = {}
        self.adjusted_leaflist_appends = {}
        self.key_properties = set()

    def add_append(self, path, val):
        self.append_stmts[path] = val

    def add_assignment(self, path, val):
        self.assignment_stmts[path] = val

    def add_declaration(self, path, val):
        self.declaration_stmts[path] = val

    def add_leaflist_append(self, path, val):
        self.leaflist_append_stmts[path] = val

    def add_adjustment(self, path, val):
        self.adjustment_stmts[path] = val

    def add_reference_adjustment(self, path, val):
        self.reference_adjustment_stmts[path] = val

    def add_key_prop(self, key_prop):
        self.key_properties.add(key_prop)

    def add_reference(self, path, reference_path):
        if reference_path in self.reference_stmts:
            self.reference_adjustment_stmts[path] = reference_path
        else:
            self.reference_stmts[path] = reference_path

    @property
    def unadjusted_leaflist_appends(self):
        for path in sorted(self.leaflist_append_stmts):
            val = self.leaflist_append_stmts[path]
            if any((val in self.assignment_stmts,
                    val in self.reference_stmts)):
                self.adjusted_leaflist_appends[path] = val
            else:
                yield path, val


class TestCasesBuilder(ValueBuilder):
    """Build independent test case for each test program file.

    Arrtibutes:
        stmts (Statements): test case statements.
        test_name (str): current test case name.
        clazz (ydkgen.api_model.Class): test case target container.
            Each test case in test program file set attributes for target
            class and its requisites. Then CRUD create, read, and delete
            will performed on target class. And finally its attributes are
            checked against object being read.
        ref_top_classes (ydkgen.api_model.Class): Requisite classes referenced
            in different package.
        derived_identities (ydkgen.api_model.Class): Identity classes
            referenced in same package or difference package.
    """

    def __init__(self, lang, identity_subclasses):
        super(TestCasesBuilder, self).__init__(lang, identity_subclasses)
        self.stmts = Statements()
        self.test_name = ''
        self.clazz = None
        self.ref_top_classes = {}
        self.derived_identities = set()

    def build_test_case(self, clazz):
        """Build a single test case."""
        self.clazz = clazz
        self.test_name = clazz.qn().lower().replace('.', '_')
        top_class = get_top_class(clazz)
        self._add_declaration_stmt(top_class)
        self._add_requisite_stmts(clazz)
        self._add_mandatory_stmts(top_class)
        self._add_list_stmts(clazz)
        for prop in clazz.properties():
            self._add_prop_stmts(prop)

    def _add_declaration_stmt(self, element):
        """Add declaration statements."""
        obj_name = get_obj_name(element)

        if isinstance(element, (atypes.Bits, atypes.Class)):
            value = get_qn(self.lang, element)
            self.stmts.add_declaration(obj_name, value)
        else:
            # add dec stmt for bits value
            value = self._get_value(element)
            self.stmts.add_assignment(obj_name, value)
            if isinstance(value, BitsValue):
                value = value.type_spec
                self.stmts.add_declaration(obj_name, value)

    def _add_requisite_stmts(self, clazz):
        """Add requisite statements.

        Requisite statements are statements for current statement that:
            - is a YANG list node
                - is a YANG key node for this node
            - is a YANG mandatory node
            - is a YANG presence node
            - is a node should be replaced with default value in _DEFAULTS
            - is a node referenced by its ancestors, which has been listed
              in _DEFAULT_LEAFREFS
        """
        while not is_pkg_element(clazz):
            self._add_requisite_clazz_stmts(clazz)
            for prop in clazz.properties():
                self._add_requisite_prop_stmts(prop)

            clazz = clazz.owner

    def _add_requisite_clazz_stmts(self, clazz):
        """Add requisite statements for a YANG container."""
        if is_presence_element(clazz):
            self._add_presence_clazz_stmts(clazz)
        if is_list_element(clazz):
            self._add_list_stmts(clazz)

    def _add_requisite_prop_stmts(self, prop):
        """Add requisite statements for a YANG leaf or leaf-list."""
        self._add_default_stmts(prop)
        self._add_relative_default_stmts(prop)
        if is_mandatory_element(prop):
            self._add_prop_stmts(prop)
        elif is_presence_element(prop):
            self._add_presence_prop_stmts(prop)

    def _add_presence_clazz_stmts(self, clazz):
        """Add requisite statements for presence container."""
        self._add_declaration_stmt(clazz)
        self._add_assignment_stmt(clazz)

    def _add_presence_prop_stmts(self, prop):
        """Add requisite statements for presence leaf or leaf-list."""
        self._add_declaration_stmt(prop.property_type)
        self._add_assignment_stmt(prop)

    def _add_mandatory_stmts(self, clazz):
        """Add requisite statements for mandatory nodes."""
        for prop in clazz.properties():
            if is_class_prop(prop):
                self._add_mandatory_stmts(prop.property_type)
            if is_mandatory_element(prop):
                self._add_requisite_stmts(prop.owner)
                self._add_prop_stmts(prop)

    def _add_assignment_stmt(self, element):
        """Add assignment statements."""
        ptype = self._get_element_ptype(element)
        path = self._get_element_path(element)
        if path not in self.stmts.declaration_stmts:
            obj_name = get_obj_name(ptype)
            if is_class_element(ptype):
                obj_name = self.assignment_fmt.format(obj_name)
            self.stmts.add_assignment(path, obj_name)

    def _add_list_stmts(self, clazz):
        """Add list statements as well as its requisite statements."""
        while not is_pkg_element(clazz):
            if all((is_list_element(clazz),
                    not is_pkg_element(clazz.owner))):
                self._add_declaration_stmt(clazz)
                self._add_list_key_stmts(clazz)
                self._add_append_stmt(clazz)

            clazz = clazz.owner

    def _add_list_key_stmts(self, clazz):
        """Add list key statements."""
        for key_prop in clazz.get_key_props():
            if key_prop not in self.stmts.key_properties:
                self.stmts.add_key_prop(key_prop)
                self._add_prop_stmts(key_prop)

    def _add_prop_stmts(self, prop):
        """Add property statements."""
        if is_config_prop(prop):
            if is_reference_prop(prop):
                self._add_reference_stmts(prop)
                self._add_requisite_prop_stmts(prop)
            elif is_terminal_prop(prop):
                self._add_terminal_prop_stmts(prop)

    def _add_reference_stmts(self, prop):
        """Add reference statements and its requisites."""
        refprop, refclass = self._get_reference_prop(prop)
        top_class = get_top_class(prop)
        top_refclass = get_top_class(refprop)
        if top_class != top_refclass:
            top_refclass_name = get_qn(self.lang, top_refclass)
            self.ref_top_classes[top_refclass_name] = top_refclass
            self._add_mandatory_stmts(top_refclass)
            self._add_declaration_stmt(top_refclass)
        # addjust the key value in leafref path
        self._add_leafref_path_key_stmts(prop)
        self._add_list_stmts(refclass)
        self._add_prop_stmts(refprop)
        self._add_reference_stmt(prop, refprop)

    def _add_reference_stmt(self, prop, refprop):
        """Add reference statements."""
        path = self._get_element_path(prop)
        refpath = self._get_element_path(refprop)
        value = self._get_value(refprop)
        if is_leaflist_prop(prop):
            self.stmts.add_leaflist_append(path, refpath)
        else:
            self.stmts.add_reference(path, refpath)
        if is_reference_prop(refprop):
            prop = refprop
            refprop, _ = self._get_reference_prop(prop)
            self._add_reference_stmt(prop, refprop)
        else:
            self._add_requisite_stmts(refprop.owner)
            self.stmts.add_assignment(refpath, value)

    def _add_terminal_prop_stmts(self, prop):
        """Add test case statements for leaf or leaf-list."""
        path = self._get_element_path(prop)
        if is_leaflist_prop(prop):
            path = '{}[0]'.format(path)
            value = self._get_value(prop)
            if isinstance(value, BitsValue):
                return
                # # ConfD internal error for leaflist of bits
                # path, value = self._render_bits_value(path, value.val)
                # self.stmts.add_assignment(path, value)
            self._add_declaration_stmt(prop)
            self._add_leaflist_append_stmts(prop)
        else:
            value = self._get_value(prop)
            if isinstance(value, BitsValue):
                if self.lang == 'py' and is_union_prop(prop):
                    # add additional dec, assignment stmt
                    # for UnionType contains Bits
                    dec_obj = self._get_element_path(value.type_spec)
                    dec = get_qn(self.lang, value.type_spec)
                    self.stmts.add_assignment(path, dec_obj)
                    self.stmts.add_declaration(dec_obj, dec)

                path, value = self._render_bits_value(path, value.val)

            self.stmts.add_assignment(path, value)

    def _add_leaflist_append_stmts(self, element):
        """Add leaf-list append statements."""
        path = self._get_element_path(element)
        obj_name = get_obj_name(element)
        self.stmts.add_leaflist_append(path, obj_name)

    def _add_append_stmt(self, element):
        """Add list append statements."""
        path = self._get_element_path(element)
        obj_name = get_obj_name(element)
        self.stmts.add_append(path, obj_name)

    def _add_default_stmts(self, prop):
        """Add default statements."""
        prop_path = self._get_element_path(prop)
        default_path = prop_path.replace('->', '.')
        for seg in _DEFAULT_LEAFREFS:
            if all((default_path.endswith(seg),
                    is_reference_prop(prop))):
                self._add_default_reference_stmts(prop)

    def _add_default_reference_stmts(self, prop):
        """Add default reference statements."""
        refprop, refclass = self._get_reference_prop(prop)
        path = self._get_element_path(prop)
        refpath = self._get_element_path(refprop)
        self.stmts.add_adjustment(path, refpath)
        if is_reference_prop(refprop):
            self._add_default_reference_stmts(refprop)

    def _add_relative_default_stmts(self, prop):
        """Add relative default statements."""
        path = _RELATIVE_DEFAULTS.get(prop.name, None)
        if path is None:
            return
        path, curr_prop = path.split('/'), prop
        for seg in path:
            for prop in curr_prop.property_type.owned_elements:
                if isinstance(prop, atypes.Property) and prop.name == seg:
                    curr_prop = prop
                    break

        self._add_terminal_prop_stmts(curr_prop)

    def _add_leafref_path_key_stmts(self, prop):
        """Add leafref statements if the reference path has predicate needs
        to be set.
        """
        orig_refstmt, _ = prop.stmt.i_leafref_ptr
        orig_refprop = orig_refstmt.i_property
        path_type_spec = prop.stmt.i_leafref
        if path_type_spec is None:
            return
        plist = path_type_spec.i_path_list
        _, pspec_list, _, _ = path_type_spec.path_spec
        if len(pspec_list) > len(plist):
            idx = 0
            for _, pstmt in plist:
                idx += 1
                pspec, pspec_list = pspec_list[0], pspec_list[1:]
                if pstmt.keyword == 'list':
                    if len(pspec) == 4 and pspec[0] == 'predicate':
                        pspec, pspec_list = pspec_list[0], pspec_list[1:]
                        _, identifier, up, dn = pspec
                        if up == 0:
                            # absolute path
                            continue

                        path_prop = self._get_path_predicate_prop(prop, up, dn)
                        # need to adjust value assigned according to predicate
                        if is_reference_prop(path_prop):
                            path = self._get_element_path(orig_refprop,
                                                          length=idx)
                            if isinstance(identifier, tuple):
                                _, identifier = identifier
                            path = self.path_sep.join([path, identifier])
                            value = self._get_element_path(path_prop)
                            self.stmts.add_adjustment(path, value)
                        elif is_terminal_prop(path_prop):
                            self._add_terminal_prop_stmts(path_prop)

    def _get_path_predicate_prop(self, prop, up, dn):
        """Get path predicate property."""
        stmt = prop.stmt
        while up:
            up -= 1
            stmt = stmt.parent

        for node in dn:
            for child in stmt.i_children:
                if child.arg == node:
                    stmt = child
                    break

        return stmt.i_property

    def _get_value(self, prop, default=None):
        """Get value based on property."""
        prop_path = self._get_element_path(prop)
        default = None
        if prop_path in _DEFAULT_VALUES:
            default = _DEFAULT_VALUES[prop_path]
        elif prop.name in _DEFAULT_VALUES:
            default = _DEFAULT_VALUES[prop.name]
        value = self.get_prop_value(prop, default=default)
        if isinstance(value, IdentityValue):
            self.derived_identities.add(value.identity)
            value = value.val
        return value

    def _get_element_path(self, element, length=None):
        """Get assignment path for element."""
        return get_element_path(self.lang, element, length)

    def _get_element_ptype(self, element):
        """Get element property type."""
        ptype = element
        if isinstance(element, atypes.Property):
            ptype = element.property_type
        return ptype

    def _get_reference_prop(self, prop):
        """Get reference property from prop, it traces one level."""
        ref, _ = prop.stmt.i_leafref_ptr
        refprop, refclass = ref.i_property, ref.parent.i_class
        return refprop, refclass

    def _render_bits_value(self, path, value):
        """Set bit value to true."""
        path = '{}["{}"]'.format(path, value)
        value = 'True'
        if self.lang == 'cpp':
            value = 'true'
        return path, value

    @property
    def assignment_fmt(self):
        fmt = '{}'
        if self.lang == 'cpp':
            fmt = 'std::move({})'
        return fmt
