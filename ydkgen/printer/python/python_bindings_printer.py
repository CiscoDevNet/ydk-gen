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

'''
   YDK PY converter
   
'''


import os

from ydkgen.api_model import Class, Enum
from ydkgen.common import get_rst_file_name

from .bits_printer import BitsPrinter
from .class_meta_printer import ClassMetaPrinter
from .class_printer import ClassPrinter
from .deviation_printer import DeviationPrinter
from .doc_printer import DocPrinter
from .enum_printer import EnumPrinter
from .import_test_printer import ImportTestPrinter
from .module_printer import ModulePrinter
from .module_meta_printer import ModuleMetaPrinter
from .test_case_printer import TestCasePrinter
from .namespace_printer import NamespacePrinter
from ydkgen.printer.language_bindings_printer import LanguageBindingsPrinter, _EmitArgs


class PythonBindingsPrinter(LanguageBindingsPrinter):

    def __init__(self, ydk_root_dir):
        super(PythonBindingsPrinter, self).__init__(ydk_root_dir)

    def print_files(self):
        self.print_modules()
        self.print_init_file(self.models_dir)
        self.print_yang_ns_file()
        self.print_import_tests_file()
        self.print_deviate_file()

        # RST Documentation
        if self.ydk_doc_dir is not None:
            self.print_python_rst_ydk_models()

    def print_modules(self):
        only_modules = [package.stmt for package in self.packages]
        size = len(only_modules)

        for index, package in enumerate(self.packages):
            self.print_module(index, package, size)

    def print_module(self, index, package, size):
        print 'Processing %d of %d %s' % (index + 1, size, package.stmt.pos.ref)

        py_mod_name = package.get_py_mod_name()
        sub = py_mod_name[len('ydk.models.'): py_mod_name.rfind('.')]

        module_dir = self.initialize_output_directory(
            '%s/%s' % (self.models_dir, sub))
        meta_dir = self.initialize_output_directory(module_dir + '/_meta')
        test_output_dir = self.initialize_output_directory(
            '%s/%s' % (self.test_dir, sub))

        # RST Documentation
        self.print_python_module(package, index, module_dir, size, sub)
        self.print_meta_module(package, meta_dir)
        self.print_test_module(package, test_output_dir)
        if self.ydk_doc_dir is not None:
            self.print_python_rst_module(package)

    def print_python_rst_module(self, package):
        if self.ydk_doc_dir is None:
            return
        # Skip generating documentation for empty modules
        if len(package.owned_elements) == 0:
            return

        def _walk_n_print(named_element, p):
            self.print_file(get_python_module_documentation_file_name(p, named_element),
                            emit_module_documentation,
                            _EmitArgs(self.ypy_ctx, named_element))

            for owned_element in named_element.owned_elements:
                if isinstance(owned_element, Class) or isinstance(owned_element, Enum):
                    _walk_n_print(owned_element, p)

        _walk_n_print(package, self.ydk_doc_dir)

    def print_python_rst_ydk_models(self):
        if self.ydk_doc_dir is None:
            return
        packages = [p for p in self.packages if len(p.owned_elements) > 0]

        self.print_file(get_table_of_contents_file_name(self.ydk_doc_dir),
                        emit_table_of_contents,
                        _EmitArgs(self.ypy_ctx, packages))

    def print_python_module(self, package, index, path, size, sub):
        self.print_init_file(path)

        package.parent_pkg_name = sub
        self.print_file(get_python_module_file_name(path, package),
                        emit_module,
                        _EmitArgs(self.ypy_ctx, package))

    def print_meta_module(self, package, path):
        self.print_init_file(path)
        self.print_file(get_meta_module_file_name(path, package),
                        emit_meta,
                        _EmitArgs(self.ypy_ctx, package))

    def print_test_module(self, package, path):
        self.print_file(get_test_module_file_name(path, package),
                        emit_test_module,
                        _EmitArgs(self.ypy_ctx, package))

    def print_yang_ns_file(self):
        packages = self.packages + self.deviation_packages
        self.print_file(get_yang_ns_file_name(self.models_dir),
                        emit_yang_ns,
                        _EmitArgs(self.ypy_ctx, packages))

    def print_deviate_file(self):
        self.print_init_file(self.deviation_dir)
        for package in self.deviation_packages:
            self.print_file(get_meta_module_file_name(self.deviation_dir, package),
                            emit_deviation,
                            _EmitArgs(self.ypy_ctx, package))

    def print_import_tests_file(self):
        self.print_file(get_import_test_file_name(self.test_dir),
                        emit_importests,
                        _EmitArgs(self.ypy_ctx, self.packages))

    def print_init_file(self, path):
        file_name = get_init_file_name(path)
        if not os.path.isfile(file_name):
            self.print_file(file_name)


def get_init_file_name(path):
    return path + '/__init__.py'


def get_yang_ns_file_name(path):
    return path + '/_yang_ns.py'


def get_import_test_file_name(path):
    return path + '/import_tests.py'


def get_python_module_documentation_file_name(path, named_element):
    return '%s/%s.rst' % (path, get_rst_file_name(named_element))


def get_table_of_contents_file_name(path):
    return '%s/ydk.models.rst' % path


def get_python_module_file_name(path, package):
    return '%s/%s.py' % (path, package.name)


def get_meta_module_file_name(path, package):
    return '%s/_%s.py' % (path, package.name)


def get_test_module_file_name(path, package):
    return '%s/%sTest.py' % (path, package.stmt.arg.replace('-', '_'))


def emit_yang_ns(ctx, packages):
    NamespacePrinter(ctx).print_output(packages)


def emit_importests(ctx, packages):
    ImportTestPrinter(ctx).print_import_tests(packages)


def emit_module_documentation(ctx, named_element):
    DocPrinter(ctx).print_module_documentation(named_element)


def emit_table_of_contents(ctx, packages):
    DocPrinter(ctx).print_table_of_contents(packages)


def emit_module(ctx, package):
    ModulePrinter(ctx).print_output(package)


def emit_test_module(ctx, package):
    TestCasePrinter(ctx).print_testcases(package)


def emit_meta(ctx, package):
    ModuleMetaPrinter(ctx).print_output(package)


def emit_deviation(ctx, package):
    DeviationPrinter(ctx).print_deviation(package)

