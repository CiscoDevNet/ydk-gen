from .printer import Printer


class FixturePrinter(Printer):
    def __init__(self, ctx, lang,
                 address='localhost', username='admin',
                 password='admin', port=1222):
        super(FixturePrinter, self).__init__(ctx, lang)
        self.address = address
        self.username = username
        self.password = password
        self.port = port

    def print_fixture_head(self, package, imports):
        self._print_imports(package, imports)
        self._print_common_stmts()
        self._bline(num=2)
        if self.lang == 'py':
            self._print_py_fixture(package)
        elif self.lang == 'cpp':
            self._print_cpp_fixture()

    def print_fixture_tail(self, package):
        if self.lang == 'py':
            self._print_py_main_block(package)
        elif self.lang == 'cpp':
            self._writeln('BOOST_AUTO_TEST_SUITE_END()')

    def _print_imports(self, package, imports):
        if self.lang == 'py':
            self._print_py_common_imports()
        elif self.lang == 'cpp':
            self._print_cpp_common_imports(package)

        for imp in imports:
            self._writeln(imp)

    def _print_py_common_imports(self):
        self._writeln("import sys")
        self._writeln("import logging")
        self._writeln("import unittest")
        self._writeln('')
        self._writeln("from ydk.services import CRUDService")
        self._writeln("from ydk.types import Decimal64, Empty")
        self._writeln("from ydk.providers import NetconfServiceProvider")

    def _print_cpp_common_imports(self, package):
        macro = package.name.title().replace('_', '')
        self._writeln('#define BOOST_TEST_MODULE {}Test'.format(macro))
        self._writeln('')
        self._writeln('#include "boost/log/trivial.hpp"')
        self._writeln('#include "boost/test/unit_test.hpp"')
        self._writeln('')
        self._writeln('#include "ydk/crud_service.hpp"')
        self._writeln('#include "ydk/netconf_provider.hpp"')

    def _print_common_stmts(self):
        if self.lang == 'py':
            self._print_py_common_stmts()
        elif self.lang == 'cpp':
            self._print_cpp_common_stmts()

    def _print_py_common_stmts(self):
        self._writeln('')
        self._writeln("logger = logging.getLogger('ydk')")
        self._writeln("# logger.setLevel(logging.DEBUG)")
        self._writeln('# handler = logging.StreamHandler()')
        self._writeln('# logger.addHandler(handler)')

    def _print_cpp_common_stmts(self):
        self._writeln('')
        self._writeln('using namespace ydk;')

    def _print_py_fixture(self, package):
        pkg_name = package.name
        self._writeln('class {}Test(unittest.TestCase):'.format(pkg_name))
        self._lvl_inc()
        self._print_py_setup_class()
        self._print_py_teardown_class()

    def _print_py_setup_class(self):
        self._bline()
        self._writeln('@classmethod')
        self._writeln('def setUpClass(cls):')
        self._lvl_inc()
        self._writeln("cls.ncc = NetconfServiceProvider("
                      "address='{}', username='{}', "
                      "password='{}', port={})"
                      .format(self.address, self.username,
                              self.password, self.port))
        self._writeln('cls.crud = CRUDService()')
        self._lvl_dec()

    def _print_py_teardown_class(self):
        self._bline()
        self._writeln('@classmethod')
        self._writeln('def tearDownClass(cls):')
        self._lvl_inc()
        self._writeln('cls.ncc.close()')
        self._bline()
        self._lvl_dec()

    def _print_cpp_fixture(self):
        self._print_cpp_connection_fixture()
        self._bline()
        self._writeln('BOOST_FIXTURE_TEST_SUITE( s, ConnectionFixture )')
        self._bline()
        self._writeln('BOOST_AUTO_TEST_CASE( empty_test_place_holder ) {}')

    def _print_cpp_connection_fixture(self):
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
                      "(\"{}\", \"{}\", \"{}\", {});"
                      .format(self.address, self.username,
                              self.password, self.port))
        self._lvl_dec()
        self._writeln('}')
        self._writeln('~ConnectionFixture() {}')
        self._writeln('CrudService m_crud;')
        self._writeln("std::unique_ptr<NetconfServiceProvider> "
                      "m_provider;")
        self._lvl_dec()
        self._writeln('};')

    def _print_py_main_block(self, package):
        self._lvl_dec()
        self._bline()
        self._writeln("if __name__ == '__main__':")
        self._lvl_inc()
        self._writeln("loader = unittest.TestLoader()")
        self._writeln("suite = loader.""loadTestsFromTestCase({}Test)"
                      .format(package.name))
        self._writeln("runner = unittest.TextTestRunner(verbosity=2)")
        self._writeln("ret = not runner.run(suite).wasSuccessful()")
        self._writeln("sys.exit(ret)")
        self._lvl_dec()
