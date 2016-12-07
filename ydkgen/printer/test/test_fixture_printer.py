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
test_fixture_printer.py

Print test fixture.
"""
from ydkgen.api_model import Class, Enum, Bits
from .language_printer import LanguagePrinter
from .utils import (is_class_element, is_identity_prop, is_path_prop,
                    is_union_prop, get_typedef_stmt,
                    is_union_type_spec,
                    is_identityref_type_spec)


class TestFixturePrinter(LanguagePrinter):
    def __init__(self, ctx, language):
        super(TestFixturePrinter, self).__init__(ctx, language)
        self._package = None

    @property
    def package(self):
        return self._package

    @package.setter
    def package(self, package):
        self._package = package

    def _print_header(self):
        self._print_imports()
        self._print_fixture_header()

    def _print_trailer(self):
        if self.language == 'py':
            self._print_main_block()
        elif self.language == 'cpp':
            self._writeln('BOOST_AUTO_TEST_SUITE_END()')

    def _print_main_block(self):
        self._lvl_dec()
        self._bline()
        self._writeln("if __name__ == '__main__':")
        self._lvl_inc()
        self._writeln("loader = unittest.TestLoader()")
        self._writeln("suite = loader.""loadTestsFromTestCase({}Test)".format(
                      self.package.name))
        self._writeln("runner = unittest.TextTestRunner(verbosity=2)")
        self._writeln("ret = not runner.run(suite).wasSuccessful()")
        self._writeln("sys.exit(ret)")
        self._lvl_dec()

    def _print_imports(self):
        for stmt in self._common_import_stmts:
            self._writeln(stmt)

        for stmt in sorted(self._get_imports()):
            self._writeln(stmt)

        for stmt in self._common_stmts:
            self._writeln(stmt)

    def _print_fixture_header(self):
        self._bline(num=2)
        if self.language == 'py':
            pkg_name = self.package.name
            self._writeln('class {}Test(unittest.TestCase):'.format(pkg_name))
            self._lvl_inc()
            self._print_setup_class()
            self._print_teardown_class()
        elif self.language == 'cpp':
            self._print_connection_fixture()
            self._bline()
            self._writeln('BOOST_FIXTURE_TEST_SUITE( s, ConnectionFixture )')
            self._bline()
            self._writeln('BOOST_AUTO_TEST_CASE( empty_test_place_holder ) {}')

    def _print_setup_class(self):
        self._bline()
        self._writeln('@classmethod')
        self._writeln('def setUpClass(cls):')
        self._lvl_inc()
        self._writeln("cls.ncc = NetconfServiceProvider(address='127.0.0.1', "
                      "username='admin', password='admin', port=12022)")
        self._writeln('cls.crud = CRUDService()')
        self._lvl_dec()

    def _print_teardown_class(self):
        self._bline()
        self._writeln('@classmethod')
        self._writeln('def tearDownClass(cls):')
        self._lvl_inc()
        self._writeln('cls.ncc.close()')
        self._bline()
        self._lvl_dec()

    def _print_connection_fixture(self):
        self._writeln('struct ConnectionFixture')
        self._writeln('{')
        self._lvl_inc()
        self._writeln('ConnectionFixture()')
        self._writeln('{')
        self._lvl_inc()
        self._bline()
        self._writeln('m_crud = CrudService{};')
        self._writeln("m_provider = "
                      "std::make_unique<NetconfServiceProvider>"
                      "(\"127.0.0.1\", \"admin\", \"admin\", 12022);")
        self._lvl_dec()
        self._writeln('}')
        self._writeln('~ConnectionFixture() {}')
        self._writeln('CrudService m_crud;')
        self._writeln("std::unique_ptr<NetconfServiceProvider> "
                      "m_provider;")
        self._lvl_dec()
        self._writeln('};')

    def _print_test_case_header(self, clazz):
        test_name = clazz.qn().lower().replace('.', '_')
        if self.language == 'py':
            self._writeln('def test_%s(self):' % test_name)
        elif self.language == 'cpp':
            self._writeln('BOOST_AUTO_TEST_CASE( %s )' % test_name)
            self._writeln('{')
            self._lvl_inc()
        self._lvl_inc()

    def _print_test_case_trailer(self):
        self._lvl_dec()
        if self.language == 'py':
            self._bline()
        elif self.language == 'cpp':
            self._lvl_dec()
            self._writeln('}')

    def _print_logging(self, msg):
        self._bline()
        if self.language == 'py':
            self._write_end('logger.info("{}")'.format(msg))
        elif self.language == 'cpp':
            self._write_end('BOOST_LOG_TRIVIAL(trace) << "{}"'.format(msg))

    @property
    def _common_import_stmts(self):
        if self.language == 'py':
            yield "import sys"
            yield "import logging"
            yield "import unittest"
            yield ''
            yield "from ydk.services import CRUDService"
            yield "from ydk.types import Decimal64, Empty"
            yield "from ydk.providers import NetconfServiceProvider"
        elif self.language == 'cpp':
            macro = self.package.name.title().replace('_', '')
            yield '#define BOOST_TEST_MODULE {}Test'.format(macro)
            yield ''
            yield '#include "boost/log/trivial.hpp"'
            yield '#include "boost/test/unit_test.hpp"'
            yield ''
            yield '#include "ydk/crud_service.hpp"'
            yield '#include "ydk/netconf_provider.hpp"'

    @property
    def _common_stmts(self):
        if self.language == 'py':
            yield ''
            yield "logger = logging.getLogger('ydk')"
            yield "# logger.setLevel(logging.DEBUG)"
            yield '# handler = logging.StreamHandler()'
            yield '# logger.addHandler(handler)'
        elif self.language == 'cpp':
            yield ''
            yield 'using namespace ydk;'

    def _get_imports(self):
        unique_imps = set()
        for imp_type in self.package.imported_types():
            unique_imps.add(self._get_import_stmt(imp_type))

        for element in self.package.owned_elements:
            if isinstance(element, (Class, Enum, Bits)):
                unique_imps.add(self._get_import_stmt(element))

        for element in self.package.owned_elements:
            if is_class_element(element) and element.is_config():
                self._add_refclass_imports(element, unique_imps)
        return unique_imps

    def _add_refclass_imports(self, clazz, unique_imps):
        for prop in clazz.properties():
            self._add_prop_imports(prop, unique_imps)

        for element in clazz.owned_elements:
            if isinstance(element, Class) and element.is_config():
                self._add_refclass_imports(element, unique_imps)

    def _add_prop_imports(self, prop, unique_imps):
        if is_identity_prop(prop):
            identity = prop.property_type
            unique_imps.add(self._get_import_stmt(identity))
            self._add_identity_import(id(identity), unique_imps)
        elif is_path_prop(prop):
            leafref_ptr = prop.stmt.i_leafref_ptr
            if leafref_ptr is not None:
                ptr, _ = prop.stmt.i_leafref_ptr
                ref_class = ptr.parent.i_class
                unique_imps.add(self._get_import_stmt(ref_class))
        elif is_union_prop(prop):
            ptype = prop.property_type
            self._add_union_imports(ptype, unique_imps)

    def _add_union_imports(self, type_spec, unique_imps):
        for type_stmt in type_spec.types:
            type_stmt = get_typedef_stmt(type_stmt)
            if hasattr(type_stmt, 'i_type_spec'):
                type_spec = type_stmt.i_type_spec
                if is_union_type_spec(type_spec):
                    self._add_union_imports(type_spec, unique_imps)
                elif is_identityref_type_spec(type_spec):
                    identity = type_spec.base.i_identity.i_class
                    unique_imps.add(self._get_import_stmt(identity))
                    self._add_identity_import(id(identity), unique_imps)

    def _add_identity_import(self, identity_id, unique_imps):
        for subclass in self.identity_subclasses[identity_id]:
            unique_imps.add(self._get_import_stmt(subclass))
            if id(subclass) in self.identity_subclasses:
                self._add_identity_import(id(subclass), unique_imps)

    def _get_import_stmt(self, imp_type):
        if self.language == 'py':
            mod_name = imp_type.get_py_mod_name()
            class_name = imp_type.qn().split('.')[0]
            return 'from {0} import {1}'.format(mod_name, class_name)
        elif self.language == 'cpp':
            header_name = imp_type.get_cpp_header_name()
            bundle_name = imp_type.get_package().bundle_name
            return '#include "ydk_{0}/{1}"'.format(bundle_name, header_name)
