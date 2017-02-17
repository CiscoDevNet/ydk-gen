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
test_printer.py

Test printer.
"""

from ydkgen.common import iscppkeyword

from ydkgen.builder import TestBuilder
from ydkgen.builder import FixtureBuilder
from .test_fixture_printer import FixturePrinter
from ydkgen.common import get_top_class, get_element_path, get_path_sep, get_obj_name, \
                    get_qn, is_reference_prop, is_terminal_prop, is_empty_prop, \
                    is_identity_prop, is_decimal64_prop

_IGNORE_TESTS = set({'ietf_netconf_acm'})


class TestPrinter(FixturePrinter):
    """Test printer."""

    def __init__(self, ctx, lang):
        super(TestPrinter, self).__init__(ctx, lang)

    def print_tests(self, package, identity_subclasses):
        """Print all test case."""
        self.package = package
        self.identity_subclasses = identity_subclasses
        test_builder = TestBuilder(self.lang, identity_subclasses)
        fixture_builder = FixtureBuilder(self.lang, identity_subclasses)
        test_builder.build_test(package)
        imports = fixture_builder.get_imports(package, test_builder)
        self.print_fixture_head(package, imports)
        if package.name not in _IGNORE_TESTS:
            self._print_test_case(package, imports, test_builder)
        self.print_fixture_tail(package)

    def _print_test_case(self, package, imports, test_builder):
        """Print a single test case."""
        for test_case in test_builder.test_cases:
            stmts = test_case.stmts
            test_name = test_case.test_name
            clazz = test_case.clazz
            top_classes = list(test_case.ref_top_classes.values())
            self._print_test_case_header(test_name)
            self._print_test_case_body(stmts, clazz, top_classes)
            self._print_test_case_trailer()

    def _print_test_case_body(self, stmts, clazz, top_classes):
        self._print_test_case_requisites(stmts)
        self._print_test_case_crud_stmts(stmts, clazz, top_classes)
        self._print_test_case_cleanup(clazz, top_classes)
        self._print_test_case_compare(clazz)

    def _print_test_case_requisites(self, stmts):
        self._print_requsite_declarations(stmts)
        self._print_requisite_stmts(stmts)
        self._print_unadjust_leaflist_append_stmts(stmts)
        self._print_requisite_reference_stmts(stmts)
        self._print_requisite_adjustments(stmts)
        self._print_requisite_leaflist_adjusted(stmts)

    def _print_requsite_declarations(self, stmts):
        for path, val in stmts.declaration_stmts.items():
            self._write_end(self.declaration_fmt.format(path, val))

    def _print_unadjust_leaflist_append_stmts(self, stmts):
        for path, val in stmts.unadjusted_leaflist_appends:
            self._write_end(self.leaflist_append_fmt.format(path, val))

    def _print_requisite_stmts(self, stmts):
        sorted_paths = sorted(list(stmts.append_stmts.keys()) +
                              list(stmts.assignment_stmts.keys()))
        for path in sorted_paths:
            if path in stmts.append_stmts:
                self._print_requisite_list_append(stmts, path)
            elif path in stmts.assignment_stmts:
                self._print_requisite_assignment(stmts, path)

    def _print_requisite_list_append(self, stmts, path):
        val = stmts.append_stmts[path]
        self._print_requisite_list_parent_pointer(path, val)
        self._write_end(self.append_fmt.format(path, val))

    def _print_requisite_list_parent_pointer(self, path, val):
        # parent pointer is set by YList append method in Python,
        # no need to print
        if self.lang == 'cpp' and self.sep in path:
            parent = self.sep.join(path.split(self.sep)[:-1])
            parent_path = self.sep.join([val, 'parent'])
            self._write_end(self.cpp_leaf_fmt.format(parent_path, parent))

    def _print_requisite_assignment(self, stmts, path):
        val = stmts.assignment_stmts[path]
        fmt = self.get_assignment_fmt(path)
        self._write_end(fmt.format(path, val))

    def _print_requisite_reference_stmts(self, stmts):
        for path in sorted(stmts.reference_stmts):
            val = stmts.reference_stmts[path]
            self._write_end(self.ref_fmt.format(path, val))

    def _print_requisite_adjustments(self, stmts):
        for path in sorted(stmts.adjustment_stmts):
            val = stmts.adjustment_stmts[path]
            self._write_end(self.ref_fmt.format(path, val))

        for path in sorted(stmts.reference_adjustment_stmts):
            val = stmts.reference_adjustment_stmts[path]
            self._write_end(self.ref_fmt.format(path, val))

    def _print_requisite_leaflist_adjusted(self, stmts):
        for path, val in stmts.adjusted_leaflist_appends.items():
            self._write_end(self.leaflist_append_fmt.format(path, val))

    def _print_test_case_crud_stmts(self, stmts, clazz, top_classes):
        for top_class in top_classes:
            self._print_crud_create_stmts(top_class)

        top_class = get_top_class(clazz)

        self._print_crud_create_stmts(top_class)
        self._print_crud_read_stmts(top_class)

    def _print_crud_create_stmts(self, top_class):
        top_obj_name = get_obj_name(top_class)
        self._print_logging('Creating {}...'.format(top_obj_name))
        fmt = self._get_crud_fmt('create')
        self._write_end(fmt.format(top_obj_name))

    def _print_crud_read_stmts(self, top_class):
        top_obj_name = get_obj_name(top_class)
        read_obj_name = '{}_read'.format(top_obj_name)
        filter_obj_name = '{}_filter'.format(top_obj_name)
        qn = get_qn(self.lang, top_class)
        self._print_logging('Reading {}...'.format(top_obj_name))
        self._write_end(self.declaration_fmt.format(filter_obj_name, qn))

        fmt = self._get_crud_fmt('read')
        stmt = fmt.format(filter_obj_name)
        fmt = self.read_ret_fmt
        if self.lang == 'py':
            self._write_end(fmt.format(read_obj_name, stmt))
        elif self.lang == 'cpp':
            self._write_end('auto read_unique_ptr = {}'.format(stmt))
            self._write_end('CHECK( read_unique_ptr != nullptr)')
            self._write_end(fmt.format(read_obj_name, qn, 'read_unique_ptr'))

    def _print_test_case_cleanup(self, clazz, top_classes):
        self._print_crud_delete_stmts(clazz)
        for clazz in top_classes:
            self._print_crud_delete_stmts(clazz)

    def _print_crud_delete_stmts(self, clazz):
        top_class = get_top_class(clazz)
        top_obj_name = get_obj_name(top_class)
        fmt = self._get_crud_fmt('delete')
        self._print_logging('Deleting {}...'.format(top_obj_name))
        self._write_end(fmt.format(top_obj_name))

    def _print_test_case_compare(self, clazz):
        self._print_logging('Comparing leaf/leaf-lists...')
        for prop in clazz.properties():
            if is_reference_prop(prop) or is_terminal_prop(prop):
                # unable to compare empty
                # read object will not be assigned to Empty() automatically
                if not is_empty_prop(prop):
                    self._print_compare_stmt(prop)

    def _print_compare_stmt(self, prop):
        if is_identity_prop(prop) or is_decimal64_prop(prop):
            # unable to compare decimal64 in Python
            # unable to compare identity in C++ and Python
            return
        lhs = self._get_element_path(prop)
        top_class_name, path = lhs.split(self.sep, 1)
        top_class_name = '{}_read'.format(top_class_name)
        rhs = self.sep.join([top_class_name, path])
        self._write_end(self.compare_fmt.format(lhs, rhs))

    def _print_test_case_header(self, test_name):
        if self.lang == 'py':
            self._writeln('def test_{}s(self):'.format(test_name))
        elif self.lang == 'cpp':
            self._writeln('TEST_CASE_METHOD( ConnectionFixture, "{}_{}_test" )'.format(self.package.name, test_name))
            self._writeln('{')
            self._lvl_inc()
        self._lvl_inc()

    def _print_test_case_trailer(self):
        self._lvl_dec()
        if self.lang == 'py':
            self._bline()
        elif self.lang == 'cpp':
            self._lvl_dec()
            self._writeln('}')
            self._bline()

    def _print_logging(self, msg):
        self._bline()
        if self.lang == 'py':
            self._write_end('logger.info("{}")'.format(msg))

    def get_assignment_fmt(self, path):
        fmt = '{} = {}'
        if self.sep not in path and self.lang == 'cpp':
            fmt = 'auto {} = {}'
        return fmt

    def _get_crud_fmt(self, oper):
        if self.lang == 'py':
            fmt = 'self.crud.{}(self.ncc, {{}})'.format(oper)
        elif self.lang == 'cpp':
            if iscppkeyword(oper):
                oper = '{}_'.format(oper)
            fmt = 'm_crud.{}(*m_provider, *{{}})'.format(oper)
        return fmt

    @property
    def declaration_fmt(self):
        fmt = '{} = {}()'
        if self.lang == 'cpp':
            fmt = 'auto {} = std::make_unique<{}>()'
        return fmt

    @property
    def leaflist_append_fmt(self):
        fmt = '{}.append({})'
        if self.lang == 'cpp':
            fmt = '{}.append(std::move({}))'
        return fmt

    @property
    def append_fmt(self):
        fmt = '{}.append({})'
        if self.lang == 'cpp':
            fmt = '{}.emplace_back(std::move({}))'
        return fmt

    @property
    def cpp_leaf_fmt(self):
        return '{} = {}.get()'

    @property
    def ref_fmt(self):
        fmt = '{} = {}'
        if self.lang == 'cpp':
            fmt = '{} = {}.get()'
        return fmt

    @property
    def compare_fmt(self):
        fmt = 'self.assertEqual({}, {})'
        if self.lang == 'cpp':
            fmt = 'CHECK( {} == {} )'
        return fmt

    @property
    def read_ret_fmt(self):
        fmt = '{} = {}'
        if self.lang == 'cpp':
            fmt = 'auto {} = dynamic_cast<{}*>({}.get())'
        return fmt

    def _get_element_path(self, element):
        return get_element_path(self.lang, element)

    @property
    def sep(self):
        return get_path_sep(self.lang)
