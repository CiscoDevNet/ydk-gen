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
from __future__ import print_function


import os

from ydkgen.api_model import Bits, Class, Enum
from ydkgen.common import get_rst_file_name

from .deviation_printer import DeviationPrinter
from .import_test_printer import ImportTestPrinter
from .module_printer import ModulePrinter
from .module_meta_printer import ModuleMetaPrinter
from .test_case_printer import TestCasePrinter
from .namespace_printer import NamespacePrinter
from .init_file_printer import InitPrinter
from ..doc import DocPrinter
from ydkgen.printer.language_bindings_printer import LanguageBindingsPrinter, _EmitArgs


class PythonBindingsPrinter(LanguageBindingsPrinter):

    def __init__(self, ydk_root_dir, bundle_name, generate_tests, sort_clazz):
        super(PythonBindingsPrinter, self).__init__(ydk_root_dir, bundle_name, generate_tests, sort_clazz)

    def print_files(self):
        self.print_init_file(self.models_dir)
        self.print_yang_ns_file()
        self.print_modules()
        self.print_import_tests_file()
        self.print_deviate_file()

        # Sub package
        if self.sub_dir != '':
            self.print_nmsp_declare_init(self.ydk_dir)
            self.print_nmsp_declare_init(self.models_dir)
            self.print_nmsp_augment_finder_init(self.sub_dir)
            self.print_nmsp_augment_finder_init(os.path.join(self.sub_dir, '_meta'), True)

        # RST Documentation
        if self.ydk_doc_dir is not None:
            self.print_python_rst_ydk_models()

    def print_modules(self):
        only_modules = [package.stmt for package in self.packages]
        size = len(only_modules)

        for index, package in enumerate(self.packages):
            self.print_module(index, package, size)

    def print_module(self, index, package, size):
        print('Processing %d of %d %s' % (index + 1, size, package.stmt.pos.ref))

        # Skip generating module for empty modules
        if len(package.owned_elements) == 0:
            return

        sub = package.sub_name

        if package.aug_bundle_name != '':
            package.augments_other = True
            module_dir = self.initialize_output_directory(
                '%s/%s/%s' % (self.models_dir, self.bundle_name, '_aug'))
        else:
            module_dir = self.initialize_output_directory(
                '%s/%s' % (self.models_dir, sub))

        meta_dir = self.initialize_output_directory(module_dir + '/_meta')
        test_output_dir = self.initialize_output_directory(
            '%s/%s' % (self.test_dir, sub))

        # RST Documentation
        self.print_python_module(package, index, module_dir, size, sub)
        self.print_meta_module(package, meta_dir)
        if self.generate_tests:
            self.print_test_module(package, test_output_dir)
        if self.ydk_doc_dir is not None:
            self.print_python_rst_module(package)

    def print_python_rst_module(self, package):
        if self.ydk_doc_dir is None:
            return

        def _walk_n_print(named_element, p):
            self.print_file(get_python_module_documentation_file_name(p, named_element),
                            emit_module_documentation,
                            _EmitArgs(self.ypy_ctx, named_element, self.identity_subclasses))

            for owned_element in named_element.owned_elements:
                if any((isinstance(owned_element, Bits),
                        isinstance(owned_element, Class),
                        isinstance(owned_element, Enum))):
                    _walk_n_print(owned_element, p)

        _walk_n_print(package, self.ydk_doc_dir)

    def print_python_rst_ydk_models(self):
        if self.ydk_doc_dir is None:
            return
        packages = [p for p in self.packages if len(p.owned_elements) > 0]

        self.print_file(get_table_of_contents_file_name(self.ydk_doc_dir),
                        emit_table_of_contents,
                        _EmitArgs(self.ypy_ctx, packages, self.bundle_name))

    def print_python_module(self, package, index, path, size, sub):
        self.print_init_file(path)

        package.parent_pkg_name = sub
        extra_args = {'sort_clazz': False,
                      'identity_subclasses': self.identity_subclasses}
        self.print_file(get_python_module_file_name(path, package),
                        emit_module,
                        _EmitArgs(self.ypy_ctx, package, extra_args))

    def print_meta_module(self, package, path):
        self.print_init_file(path)
        self.print_file(get_meta_module_file_name(path, package),
                        emit_meta,
                        _EmitArgs(self.ypy_ctx, package, self.sort_clazz))

    def print_test_module(self, package, path):
        self.print_init_file(self.test_dir)
        self.print_file(get_test_module_file_name(path, package),
                        emit_test_module,
                        _EmitArgs(self.ypy_ctx, package, self.identity_subclasses))

    def print_yang_ns_file(self):
        packages = self.packages + self.deviation_packages
        target_dir = self.models_dir if self.sub_dir == '' else self.sub_dir

        self.print_file(get_yang_ns_file_name(target_dir),
                        emit_yang_ns,
                        _EmitArgs(self.ypy_ctx, packages))

    def print_deviate_file(self):
        self.print_nmsp_declare_init(self.deviation_dir)
        for package in self.deviation_packages:
            self.print_file(get_meta_module_file_name(self.deviation_dir, package),
                            emit_deviation,
                            _EmitArgs(self.ypy_ctx, package, self.sort_clazz))

    def print_import_tests_file(self):
        self.print_file(get_import_test_file_name(self.test_dir),
                        emit_importests,
                        _EmitArgs(self.ypy_ctx, self.packages))

    def print_init_file(self, path):
        file_name = get_init_file_name(path)
        if not os.path.isfile(file_name):
            self.print_file(file_name)

    def print_nmsp_declare_init(self, path):
        file_name = get_init_file_name(path)
        self.print_file(file_name,
                        emit_nmsp_declare_init,
                        _EmitArgs(self.ypy_ctx, self.packages))

    def print_nmsp_augment_finder_init(self, path, is_meta=False):
        file_name = get_init_file_name(path)
        self.print_file(file_name,
                        emit_nmsp_augment_finder_init,
                        _EmitArgs(self.ypy_ctx, self.packages, is_meta))


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


def emit_module_documentation(ctx, named_element, identity_subclasses):
    DocPrinter(ctx, 'py').print_module_documentation(named_element, identity_subclasses)


def emit_table_of_contents(ctx, packages, bundle_name):
    DocPrinter(ctx, 'py').print_table_of_contents(packages, bundle_name)


def emit_module(ctx, package, extra_args):
    ModulePrinter(ctx, extra_args).print_output(package)


def emit_test_module(ctx, package, identity_subclasses):
    TestCasePrinter(ctx).print_testcases(package, identity_subclasses)


def emit_meta(ctx, package, sort_clazz):
    ModuleMetaPrinter(ctx, sort_clazz).print_output(package)


def emit_deviation(ctx, package, sort_clazz):
    DeviationPrinter(ctx, sort_clazz).print_deviation(package)


def emit_nmsp_declare_init(ctx, package):
    InitPrinter(ctx).print_nmsp_declare_init(package)


def emit_nmsp_augment_finder_init(ctx, package, is_meta):
    InitPrinter(ctx).print_nmsp_augment_finder_init(package, is_meta)
