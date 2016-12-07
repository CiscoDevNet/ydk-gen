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
test_case_printer.py

Print test cases.
"""
from ydkgen.api_model import Bits, Class, Package, Property

from .value_getter import ValueGetter, BitsValue
from .test_fixture_printer import TestFixturePrinter
from .utils import (has_terminal_nodes, is_config_field, is_class_prop,
                    is_ref_prop, is_mandatory_prop, is_leaflist_prop,
                    is_presence_prop, is_terminal_prop, is_list_element,
                    is_union_prop, is_identity_prop, is_decimal64_prop,
                    is_empty_prop)


class Statements(object):
    """class of path/value pair dictionaries"""

    def __init__(self):
        self._refs = {}
        self._appends = {}
        self._assignments = {}
        self._decs = {}
        self._adjustments = {}
        self._refadjustments = {}
        self._leaflist_appends = {}
        self.key_props = set()
        self.leafref_props = set()

    def add_leaflist_append(self, path, val):
        self._leaflist_appends[path] = val

    def add_assignment(self, path, val):
        self._assignments[path] = val

    def add_append(self, path, val):
        self._appends[path] = val

    def add_dec(self, path, val):
        self._decs[path] = val

    def add_ref(self, path, refpath):
        if refpath in self._refs:
            self._refadjustments[path] = refpath
        else:
            self._refs[path] = refpath

    def add_adjustment(self, path, val):
        self._adjustments[path] = val

    def add_refadjustment(self, path, val):
        self._refadjustments[path] = val

    def add_key_prop(self, key_prop):
        self.key_props.add(key_prop)


class TestCasePrinter(TestFixturePrinter):
    def __init__(self, ctx, language):
        super(TestCasePrinter, self).__init__(ctx, language)
        self.stmts = None

    def print_testcases(self, package, identity_subclasses):
        assert isinstance(package, Package)
        self.package = package
        self.ref_top_classes = {}
        self.identity_subclasses = identity_subclasses
        self._get_prop_value = ValueGetter(identity_subclasses, self.language)
        self._print_header()
        self._print_body()
        self._print_trailer()

    def _print_body(self):
        for elem in self.package.owned_elements:
            if isinstance(elem, Class) and not elem.is_identity():
                self._traverse_and_print(elem)

    def _traverse_and_print(self, clazz):
        for prop in clazz.properties():
            if is_class_prop(prop):
                ptype = prop.property_type
                self._traverse_and_print(ptype)
                if has_terminal_nodes(prop) and is_config_field(prop):
                    self._print_test_case(ptype)

    def _print_test_case(self, clazz):
        self.ref_top_classes = {}
        self._print_test_case_header(clazz)
        self._print_test_case_preamble(clazz)
        self._print_crud_test(clazz)
        self._print_test_case_cleanup(clazz)
        self._print_test_case_compare(clazz)
        self._print_test_case_trailer()

    def _print_test_case_preamble(self, clazz):
        self.stmts = Statements()
        self._add_preamble_stmts(clazz)
        self._print_preamble_stmts()

    def _add_preamble_stmts(self, clazz):
        top_class = self._get_top_class(clazz)
        self._add_dec_stmt(top_class)
        self._add_mandatory_stmts(clazz.owner)
        self._add_list_stmts(clazz)
        for prop in clazz.properties():
            self._add_prop_stmts(prop)

    def _add_mandatory_stmts(self, clazz):
        while not isinstance(clazz, Package):
            for prop in clazz.properties():
                if is_mandatory_prop(prop):
                    self._add_prop_stmts(prop)
                elif is_presence_prop(prop):
                    self._add_dec_stmt(prop.property_type)
                    self._add_assignment_stmt(prop)
                if is_list_element(clazz):
                    self._add_list_key_stmt(clazz)
                # hard code, enable afi-safi
                if prop.name == 'afi_safi':
                    self._add_relative_assignment(prop, 'config/enabled')

            clazz = clazz.owner

    def _add_relative_assignment(self, prop, path):
        path = path.split('/')
        curr_prop = prop
        for seg in path:
            for prop in curr_prop.property_type.owned_elements:
                if isinstance(prop, Property) and prop.name == seg:
                    curr_prop = prop
                    break
        self._add_terminal_prop_stmts(curr_prop)

    def _add_list_stmts(self, clazz):
        while not isinstance(clazz, Package):
            if is_list_element(clazz) and not isinstance(clazz.owner, Package):
                self._add_dec_stmt(clazz)
                self._add_list_key_stmt(clazz)
                self._add_append_stmt(clazz)
            clazz = clazz.owner

    def _add_list_key_stmt(self, clazz):
        for key_prop in clazz.get_key_props():
            if key_prop not in self.stmts.key_props:
                self.stmts.add_key_prop(key_prop)
                self._add_prop_stmts(key_prop)

    def _add_prop_stmts(self, prop):
        if is_ref_prop(prop):
            self._add_leafref_stmts(prop)
        elif is_terminal_prop(prop):
            self._add_terminal_prop_stmts(prop)

    def _add_terminal_prop_stmts(self, prop):
        path = self._get_assignment_path(prop)
        if is_leaflist_prop(prop):
            path = '{}[0]'.format(path)
            value = self._get_prop_value(prop)
            if isinstance(value, BitsValue):
                return
                # <-- ConfD internal error for leaflist of bits -->
                # path, value = self._handle_bits_value(path, value.val)
                # self.stmts.add_assignment(path, value)
                # <-- ConfD internal error for leaflist of bits -->
            self._add_dec_stmt(prop)
            self._add_leaflist_append_stmt(prop)
        else:
            value = self._get_prop_value(prop)
            if isinstance(value, BitsValue):
                if self.language == 'py' and is_union_prop(prop):
                    # add additional dec, assignment stmt
                    # for UnionType contains Bits
                    dec_obj = self._get_assignment_path(value.type_spec)
                    dec = self._get_qn(value.type_spec)
                    self.stmts.add_assignment(path, dec_obj)
                    self.stmts.add_dec(dec_obj, dec)

                path, value = self._handle_bits_value(path, value.val)

            self.stmts.add_assignment(path, value)

    def _add_leaflist_append_stmt(self, element):
        path = self._get_assignment_path(element)
        obj_name = self._get_obj_name(element)
        self.stmts.add_leaflist_append(path, obj_name)

    def _add_append_stmt(self, element):
        path = self._get_assignment_path(element)
        obj_name = self._get_obj_name(element)
        self.stmts.add_append(path, obj_name)

    def _add_leafref_stmts(self, prop):
        ref, _ = prop.stmt.i_leafref_ptr
        refprop, refclass = ref.i_property, ref.parent.i_class
        top_class = self._get_top_class(prop)
        top_refclass = self._get_top_class(refprop)
        if top_class != top_refclass:
            top_refclass_name = self._get_qn(top_refclass)
            self.ref_top_classes[top_refclass_name] = top_refclass
            self._add_dec_stmt(top_refclass)
        # addjust the key value in leafref path
        self._add_leafref_path_key_stmts(prop)
        self._add_list_stmts(refclass)
        self._add_prop_stmts(refprop)
        self._add_ref_stmt(prop, refprop)

    def _add_leafref_prop_stmts(self, prop, refprop):
        top_class = self._get_top_class(prop)
        top_refclass = self._get_top_class(refprop)
        if top_class != top_refclass:
            self._add_dec_stmt(top_refclass)
        self._add_ref_stmt(prop, refprop)

    def _add_leafref_path_key_stmts(self, prop):
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
                    pspec, pspec_list = pspec_list[0], pspec_list[1:]
                    if len(pspec) == 4 and pspec[0] == 'predicate':
                        _, identifier, up, dn = pspec
                        if up == 0:
                            # absolute path
                            continue

                        path_prop = self._get_path_predicate_prop(prop, up, dn)
                        # need to adjust value assigned according to predicate
                        if is_ref_prop(path_prop):
                            path = self._get_assignment_path(orig_refprop,
                                                             length=idx)
                            path = self.sep.join([path, identifier])
                            value = self._get_assignment_path(path_prop)
                            self.stmts.add_adjustment(path, value)
                        elif is_terminal_prop(path_prop):
                            self._add_terminal_prop_stmts(path_prop)

    def _get_path_predicate_prop(self, prop, up, dn):
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

    def _add_ref_stmt(self, prop, refprop):
        path = self._get_assignment_path(prop)
        refpath = self._get_assignment_path(refprop)
        value = self._get_prop_value(refprop)
        if is_leaflist_prop(prop):
            self.stmts.add_leaflist_append(path, refpath)
        else:
            self.stmts.add_ref(path, refpath)
        self.stmts.add_assignment(refpath, value)

    def _add_dec_stmt(self, element):
        obj_name = self._get_obj_name(element)
        if isinstance(element, (Bits, Class)):
            # class or identity
            value = self._get_qn(element)
            self.stmts.add_dec(obj_name, value)
        else:
            # add dec stmt for bits value
            value = self._get_prop_value(element)
            self.stmts.add_assignment(obj_name, value)
            if isinstance(value, BitsValue):
                value = value.type_spec
                self.stmts.add_dec(obj_name, value)

    def _add_assignment_stmt(self, prop):
        ptype = prop.property_type
        path = self._get_assignment_path(prop)
        obj_name = self._get_obj_name(ptype)
        if isinstance(ptype, Class) and self.language == 'cpp':
            obj_name = 'std::move({})'.format(obj_name)
        self.stmts.add_assignment(path, obj_name)

    def _print_preamble_stmts(self):
        self._print_preamble_decs()

        leaflist_appends = self._print_preamble_leaflist_unadjusted()
        self._print_preamble_list_appends()
        self._print_preamble_assignments()
        self._print_preamble_refs()
        self._print_preamble_adjustments()
        self._print_preamble_leaflist_adjusted(leaflist_appends)

    def _print_preamble_decs(self):
        for path in self.stmts._decs:
            val = self.stmts._decs[path]
            self._write_end(self.dec_fmt.format(path, val))

    def _print_preamble_leaflist_unadjusted(self):
        leaflist_appends = {}
        for path in sorted(self.stmts._leaflist_appends):
            val = self.stmts._leaflist_appends[path]
            if val in self.stmts._assignments:
                leaflist_appends[path] = val
            else:
                self._write_end(self.llist_append_fmt.format(path, val))
        return leaflist_appends

    def _print_preamble_list_appends(self):
        for path in sorted(self.stmts._appends):
            val = self.stmts._appends[path]
            self._print_preamble_list_parent_pointer(path, val)
            self._write_end(self.append_fmt.format(path, val))

    def _print_preamble_list_parent_pointer(self, path, val):
        # parent pointer is set by YList append method in Python
        if self.language == 'cpp' and self.sep in path:
            parent = self.sep.join(path.split(self.sep)[:-1])
            parent_path = self.sep.join([val, 'parent'])
            self._write_end(self.cpp_leaf_fmt.format(parent_path, parent))

    def _print_preamble_assignments(self):
        for path in sorted(self.stmts._assignments):
            val = self.stmts._assignments[path]
            fmt = self.get_assignment_fmt(path)
            self._write_end(fmt.format(path, val))

    def _print_preamble_refs(self):
        for path in sorted(self.stmts._refs):
            val = self.stmts._refs[path]
            self._write_end(self.ref_fmt.format(path, val))

    def _print_preamble_adjustments(self):
        for path in sorted(self.stmts._adjustments):
            val = self.stmts._adjustments[path]
            self._write_end(self.ref_fmt.format(path, val))

        for path in sorted(self.stmts._refadjustments):
            val = self.stmts._refadjustments[path]
            self._write_end(self.ref_fmt.format(path, val))

    def _print_preamble_leaflist_adjusted(self, leaflist_appends):
        for path in leaflist_appends:
            val = leaflist_appends[path]
            self._write_end(self.leaflist_append_fmt.format(path, val))

    def _print_crud_test(self, clazz):
        for ref_topclass_name in self.ref_top_classes:
            ref_top_class = self.ref_top_classes[ref_topclass_name]
            self._print_crud_create_stmt(ref_top_class)

        top_class = self._get_top_class(clazz)
        self._print_crud_create_stmt(top_class)
        self._print_crud_read_stmt(top_class)

    def _print_crud_create_stmt(self, top_class):
        top_obj_name = self._get_obj_name(top_class)
        self._print_logging('Creating {}...'.format(top_obj_name))
        fmt = self._get_crud_fmt('create')
        self._write_end(fmt.format(top_obj_name))

    def _print_crud_read_stmt(self, top_class):
        top_obj_name = self._get_obj_name(top_class)
        read_obj_name = '{}_read'.format(top_obj_name)
        filter_obj_name = '{}_filter'.format(top_obj_name)
        qn = self._get_qn(top_class)
        self._print_logging('Reading {}...'.format(top_obj_name))
        self._write_end(self.dec_fmt.format(filter_obj_name, qn))
        fmt = self._get_crud_fmt('read')
        stmt = fmt.format(filter_obj_name)
        fmt = self._get_ret_fmt()
        if self.language == 'py':
            self._write_end(fmt.format(read_obj_name, stmt))
        elif self.language == 'cpp':
            self._write_end('auto read_unique_ptr = {}'.format(stmt))
            self._write_end('BOOST_CHECK_EQUAL( read_unique_ptr != nullptr, true )')
            self._write_end(fmt.format(read_obj_name, qn, 'read_unique_ptr'))

    def _print_test_case_compare(self, clazz):
        self._print_logging('Comparing leaf/leaf-lists...')
        for prop in clazz.properties():
            if is_ref_prop(prop) or is_terminal_prop(prop):
                # unable to compare
                # read object will not be assigned to Empty() automatically
                if not is_empty_prop(prop):
                    self._print_compare_stmt(prop)

    def _print_compare_stmt(self, prop):
        if is_identity_prop(prop) or is_decimal64_prop(prop):
            # unable to compare decimal64 in Python
            # unable to compare identity in C++ and Python
            return
        lhs = self._get_assignment_path(prop)
        top_class_name, path = lhs.split(self.sep, 1)
        top_class_name = '{}_read'.format(top_class_name)
        rhs = self.sep.join([top_class_name, path])
        self._write_end(self.compare_fmt.format(lhs, rhs))

    def _print_test_case_cleanup(self, clazz):
        self._print_crud_delete_stmts(clazz)
        for top_ref_class in self.ref_top_classes:
            top_ref_class = self.ref_top_classes[top_ref_class]
            self._print_crud_delete_stmts(top_ref_class)

    def _print_crud_delete_stmts(self, clazz):
        top_class = self._get_top_class(clazz)
        top_obj_name = self._get_obj_name(top_class)
        fmt = self._get_crud_fmt('delete')
        self._print_logging('Deleting {}...'.format(top_obj_name))
        self._write_end(fmt.format(top_obj_name))

    def _get_top_class(self, clazz):
        while not isinstance(clazz.owner, Package):
            clazz = clazz.owner
        return clazz

    def _get_assignment_path(self, element, length=None):
        path = []
        while not isinstance(element, Package):
            seg = self._get_seg_name(element)
            if all((element.stmt.keyword == 'list', path,
                    not isinstance(element.owner, Package))):
                # list and leaf-list only has one element
                seg += '[0]'
            path.append(seg)
            element = element.owner

        if length is None:
            return self.sep.join(reversed(path))
        else:
            path = list(reversed(path))[:length]
            return self.sep.join(path)

    def _get_seg_name(self, element):
        seg_name = ''
        if isinstance(element.owner, Package):
            seg_name = element.name.lower()
        elif isinstance(element, Property):
            seg_name = element.name
        else:
            for prop in element.owner.properties():
                if prop.stmt == element.stmt:
                    seg_name = prop.name
        return seg_name

    def _handle_bits_value(self, path, value):
        path = '{}["{}"]'.format(path, value)
        value = ''
        if self.language == 'py':
            value = 'True'
        elif self.language == 'cpp':
            value = 'true'
        return path, value
