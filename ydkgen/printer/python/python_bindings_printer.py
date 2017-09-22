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

from ydkgen.api_model import Bits, Class, Enum, Package
from ydkgen.common import get_rst_file_name, get_property_name

from .deviation_printer import DeviationPrinter
from .import_test_printer import ImportTestPrinter
from .module_printer import ModulePrinter
from .module_meta_printer import ModuleMetaPrinter
from .namespace_printer import NamespacePrinter
from .init_file_printer import InitPrinter
from ..doc import DocPrinter
from ..tests import TestPrinter
from ydkgen.printer.language_bindings_printer import LanguageBindingsPrinter, _EmitArgs


class PythonBindingsPrinter(LanguageBindingsPrinter):

    def __init__(self, ydk_root_dir, bundle, generate_tests, one_class_per_module, sort_clazz):
        super(PythonBindingsPrinter, self).__init__(ydk_root_dir, bundle, generate_tests, one_class_per_module, sort_clazz)

    def print_files(self):
        self._print_init_file(self.models_dir)
        self._print_yang_ns_file()
        self._print_modules()
        self._print_import_tests_file()
        self._print_deviate_file()

        # Sub package
        if self.sub_dir != '':
            self._print_nmsp_declare_init(self.ydk_dir)
            self._print_nmsp_declare_init(os.path.join(self.ydk_dir, 'models'))
            self._print_nmsp_declare_init(self.models_dir)

        # RST Documentation
        if self.ydk_doc_dir is not None:
            self._print_python_rst_ydk_models()
        return ()

    def _print_modules(self):
        only_modules = [package.stmt for package in self.packages]
        size = len(only_modules)

        for index, package in enumerate(self.packages):
            self._print_module(index, package, size)

    def _print_module(self, index, package, size):
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
            module_dir = self.initialize_output_directory(self.models_dir)

        meta_dir = self.initialize_output_directory(module_dir + '/_meta')
        test_output_dir = self.initialize_output_directory(
            '%s/%s' % (self.test_dir, sub))


        if self.one_class_per_module:
            path = os.path.join(self.models_dir, package.name)
            self.initialize_output_directory(path, True)
            self._print_init_file(path)

            extra_args = {'one_class_per_module': self.one_class_per_module,
                          'identity_subclasses': self.identity_subclasses}
            self.print_file(get_python_module_file_name(path, package),
                            emit_module,
                            _EmitArgs(self.ypy_ctx, package, extra_args))

            self._print_python_modules(package, index, path, size, sub)
        else:
            # RST Documentation
            self._print_python_module(package, index, module_dir, size, sub)

        self._print_meta_module(package, meta_dir)
        if self.generate_tests:
            self._print_tests(package, test_output_dir)
        if self.ydk_doc_dir is not None:
            self._print_python_rst_module(package)

    def _print_python_rst_module(self, package):
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

    def _print_python_rst_ydk_models(self):
        if self.ydk_doc_dir is None:
            return
        packages = [p for p in self.packages if len(p.owned_elements) > 0]

        self.print_file(get_table_of_contents_file_name(self.ydk_doc_dir),
                        emit_table_of_contents,
                        _EmitArgs(self.ypy_ctx, packages, (self.bundle_name, self.bundle_version)))

    def _print_python_modules(self, element, index, path, size, sub):
        for c in [clazz for clazz in element.owned_elements if isinstance(clazz, Class)]:
            if not c.is_identity():
                self._print_python_module(c, index, os.path.join(path, get_property_name(c, c.iskeyword)), size, sub)

    def _print_python_module(self, package, index, path, size, sub):
        if self.one_class_per_module:
            self.initialize_output_directory(path, True)
            self._print_init_file(path)

        self._print_init_file(path)

        package.parent_pkg_name = sub
        extra_args = {'one_class_per_module': self.one_class_per_module,
                      'sort_clazz': self.sort_clazz,
                      'identity_subclasses': self.identity_subclasses}
        self.print_file(get_python_module_file_name(path, package),
                        emit_module,
                        _EmitArgs(self.ypy_ctx, package, extra_args))

        if self.one_class_per_module:
            self._print_python_modules(package, index, path, size, sub)

    def _print_meta_module(self, package, path):
        self._print_init_file(path)
        extra_args = {'one_class_per_module': self.one_class_per_module,
                      'sort_clazz': self.sort_clazz}
        self.print_file(get_meta_module_file_name(path, package),
                        emit_meta,
                        _EmitArgs(self.ypy_ctx, package, extra_args))

    def _print_tests(self, package, path):
        self._print_init_file(self.test_dir)
        empty = self.is_empty_package(package)
        if not empty:
            self.print_file(get_test_module_file_name(path, package),
                            emit_test_module,
                            _EmitArgs(self.ypy_ctx, package, self.identity_subclasses))

    def _print_yang_ns_file(self):
        packages = self.packages + self.deviation_packages

        self.print_file(get_yang_ns_file_name(self.models_dir),
                        emit_yang_ns,
                        _EmitArgs(self.ypy_ctx, packages, self.one_class_per_module))

    def _print_deviate_file(self):
        self._print_nmsp_declare_init(self.deviation_dir)
        for package in self.deviation_packages:
            self.print_file(get_meta_module_file_name(self.deviation_dir, package),
                            emit_deviation,
                            _EmitArgs(self.ypy_ctx, package, self.sort_clazz))

    def _print_import_tests_file(self):
        self.print_file(get_import_test_file_name(self.test_dir),
                        emit_importests,
                        _EmitArgs(self.ypy_ctx, self.packages))

    def _print_init_file(self, path):
        file_name = get_init_file_name(path)
        if not os.path.isfile(file_name):
            self.print_file(file_name)

    def _print_nmsp_declare_init(self, path):
        file_name = get_init_file_name(path)
        self.print_file(file_name,
                        emit_nmsp_declare_init,
                        _EmitArgs(self.ypy_ctx, self.packages))

    def _print_nmsp_augment_finder_init(self, path, is_meta=False):
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
    if isinstance(package, Package):
        return '%s/%s.py' % (path, package.name)
    else:
        return '%s/%s.py' % (path, get_property_name(package, package.iskeyword))


def get_meta_module_file_name(path, package):
    return '%s/_%s.py' % (path, package.name)


def get_test_module_file_name(path, package):
    return '%s/test_%s.py' % (path, package.stmt.arg.replace('-', '_'))


def emit_yang_ns(ctx, packages, one_class_per_module):
    NamespacePrinter(ctx, one_class_per_module).print_output(packages)


def emit_importests(ctx, packages):
    ImportTestPrinter(ctx).print_import_tests(packages)


def emit_module_documentation(ctx, named_element, identity_subclasses):
    DocPrinter(ctx, 'py').print_module_documentation(named_element, identity_subclasses)


def emit_table_of_contents(ctx, packages, extra_args):
    DocPrinter(ctx, 'py').print_table_of_contents(packages, extra_args[0], extra_args[1])


def emit_module(ctx, package, extra_args):
    ModulePrinter(ctx, extra_args).print_output(package)


def emit_test_module(ctx, package, identity_subclasses):
    TestPrinter(ctx, 'py').print_tests(package, identity_subclasses)


def emit_meta(ctx, package, extra_args):
    ModuleMetaPrinter(ctx, extra_args['one_class_per_module'], extra_args['sort_clazz']).print_output(package)


def emit_deviation(ctx, package, sort_clazz):
    DeviationPrinter(ctx, sort_clazz).print_deviation(package)


def emit_nmsp_declare_init(ctx, package):
    InitPrinter(ctx).print_nmsp_declare_init(package)


def emit_nmsp_augment_finder_init(ctx, package, is_meta):
    InitPrinter(ctx).print_nmsp_augment_finder_init(package, is_meta)
