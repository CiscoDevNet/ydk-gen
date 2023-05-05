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
# This file has been modified by Yan Gorelik, YDK Solutions.
# All modifications in original under CiscoDevNet domain
# introduced since October 2019 are copyrighted.
# All rights reserved under Apache License, Version 2.0.
# ------------------------------------------------------------------

"""
   YDK PY converter
"""

import os
import sys
import shutil
from distutils import dir_util

from ydkgen.api_model import Bits, Class, Enum, Package, get_property_name
from ydkgen.common import get_rst_file_name

from .import_test_printer import ImportTestPrinter
from .module_printer import ModulePrinter
from .module_meta_printer import ModuleMetaPrinter
from .namespace_printer import NamespacePrinter
from .init_file_printer import InitPrinter
from ..doc import DocPrinter
from ..tests import TestPrinter
from ydkgen.printer.language_bindings_printer import LanguageBindingsPrinter, _EmitArgs

import logging

logger = logging.getLogger('ydkgen')


class PythonBindingsPrinter(LanguageBindingsPrinter):

    def __init__(self, ydk_root_dir, bundle, generate_tests, one_class_per_module):
        if sys.version_info > (3,):
            super().__init__(ydk_root_dir, bundle, generate_tests, one_class_per_module)
        else:
            super(PythonBindingsPrinter, self).__init__(ydk_root_dir, bundle, generate_tests, one_class_per_module)
        self.bundle = bundle
        self.bundle_name = bundle.name
        self.bundle_version = bundle.str_version
        self.generate_meta = False

    def print_files(self):
        self._print_init_file(self.models_dir)
        self._print_yang_ns_file()
        self._print_modules()
        self._print_import_tests_file()

        # Sub package
        if self.sub_dir != '':
            self._print_nmsp_declare_init_files()

        # RST Documentation
        if self.ydk_doc_dir is not None:
            self._print_python_rst_ydk_models()

        # YANG models
        self._copy_yang_files()
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
            logger.debug("    Skipping module, because it does not contain top level containers")
            return

        sub = package.sub_name
        test_output_dir = self.initialize_output_directory(self.test_dir)

        if self.one_class_per_module:
            path = os.path.join(self.models_dir, package.name)
            self.initialize_output_directory(path, True)
            self._print_init_file(path)

            extra_args = {'one_class_per_module': self.one_class_per_module,
                          'identity_subclasses': self.identity_subclasses,
                          'generate_meta': self.generate_meta,
                          'module_namespace_lookup': self.module_namespace_lookup}
            self.print_file(get_python_module_file_name(path, package),
                            emit_module,
                            _EmitArgs(self.ypy_ctx, package, extra_args))

            self._print_python_modules(package, index, path, size, sub)
        else:
            # RST Documentation
            self._print_python_module(package, index, self.models_dir, size, sub)

        if self.generate_meta:
            meta_dir = self.initialize_output_directory(self.models_dir + '/_meta')
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
                      'generate_meta': self.generate_meta,
                      'identity_subclasses': self.identity_subclasses,
                      'module_namespace_lookup' : self.module_namespace_lookup}
        python_module_file_name = get_python_module_file_name(path, package)
        logger.debug("    Printing python module %s" % python_module_file_name)
        self.print_file(python_module_file_name,
                        emit_module,
                        _EmitArgs(self.ypy_ctx, package, extra_args))

        if self.one_class_per_module:
            self._print_python_modules(package, index, path, size, sub)

    def _print_meta_module(self, package, path):
        self._print_init_file(path)
        extra_args = {'one_class_per_module': self.one_class_per_module,
                      'identity_subclasses': self.identity_subclasses}
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
        packages = self.packages

        self.print_file(get_yang_ns_file_name(self.models_dir),
                        emit_yang_ns,
                        _EmitArgs(self.ypy_ctx, packages, (self.bundle_name, self.one_class_per_module)))

    def _print_import_tests_file(self):
        self.print_file(get_import_test_file_name(self.test_dir),
                        emit_importests,
                        _EmitArgs(self.ypy_ctx, self.packages))

    def _print_init_file(self, path):
        file_name = get_init_file_name(path)
        if not os.path.isfile(file_name):
            self.print_file(file_name)

    def _print_nmsp_declare_init_files(self):
        self._print_nmsp_declare_init(self.ydk_dir)
        self._print_nmsp_declare_init(os.path.join(self.ydk_dir, 'models'))
        self._print_nmsp_declare_init(self.models_dir)

    def _print_nmsp_declare_init(self, path):
        file_name = get_init_file_name(path)
        self.print_file(file_name,
                        emit_nmsp_declare_init,
                        _EmitArgs(self.ypy_ctx, self.packages))

    def _copy_yang_files(self):
        yang_files_dir = os.path.sep.join([self.models_dir, '_yang'])
        os.mkdir(yang_files_dir)
        dir_util.copy_tree(self.bundle.resolved_models_dir, yang_files_dir)
        _copy_yang_files_from_subdirectories(yang_files_dir)


def _copy_yang_files_from_subdirectories(yang_files_dir):
    subdirs = [os.path.join(yang_files_dir, o) for o in os.listdir(yang_files_dir) if os.path.isdir(os.path.join(yang_files_dir, o))]
    for subdir in subdirs:
        files = os.listdir(subdir)
        for file in files:
            file_path = os.path.join(subdir, file)
            if os.path.isfile(file_path):
                shutil.copy(file_path, yang_files_dir)


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


def emit_yang_ns(ctx, packages, extra_args):
    bundle_name = extra_args[0]
    one_class_per_module = extra_args[1]
    NamespacePrinter(ctx, one_class_per_module).print_output(packages, bundle_name)


def emit_importests(ctx, packages):
    ImportTestPrinter(ctx).print_import_tests(packages)


def emit_module_documentation(ctx, named_element, identity_subclasses):
    DocPrinter(ctx, 'py').print_module_documentation(named_element, identity_subclasses)


def emit_table_of_contents(ctx, packages, extra_args):
    bundle_name, bundle_version = extra_args
    DocPrinter(ctx, 'py', bundle_name, bundle_version).print_table_of_contents(packages)


def emit_module(ctx, package, extra_args):
    ModulePrinter(ctx, extra_args).print_output(package)


def emit_test_module(ctx, package, identity_subclasses):
    TestPrinter(ctx, 'py').print_tests(package, identity_subclasses)


def emit_meta(ctx, package, extra_args):
    ModuleMetaPrinter(ctx, extra_args['one_class_per_module'],
                      extra_args['identity_subclasses']).print_output(package)


def emit_nmsp_declare_init(ctx, package):
    InitPrinter(ctx).print_nmsp_declare_init(package)
