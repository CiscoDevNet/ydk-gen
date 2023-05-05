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
   YDK GO converter

'''

from __future__ import print_function

import os
import sys
from distutils.file_util import copy_file
from distutils.dir_util import mkpath

from ydkgen.api_model import Class, Enum
from ydkgen.common import get_rst_file_name
from ydkgen.printer.language_bindings_printer import LanguageBindingsPrinter, _EmitArgs

from .module_printer import ModulePrinter
from .generated_package_methods_printer import GeneratedPackageMethodsPrinter
from ..doc import DocPrinter


class GoBindingsPrinter(LanguageBindingsPrinter):

    def __init__(self, ydk_root_dir, bundle, generate_tests, one_class_per_module):
        if sys.version_info > (3,):
            super().__init__(ydk_root_dir, bundle, generate_tests, one_class_per_module)
        else:
            super(GoBindingsPrinter, self).__init__(ydk_root_dir, bundle, generate_tests, one_class_per_module)

    def print_files(self):
        only_modules = [package.stmt for package in self.packages]
        size = len(only_modules)

        for index, package in enumerate(self.packages):
            self._print_module(index, package, size)

        # generated entity lookup
        path = self.models_dir
        generated_entity_lookup_file_name = '%s/generated_package_methods.go' % path
        with open(generated_entity_lookup_file_name, 'w+') as file_descriptor:
            self.ypy_ctx.fd = file_descriptor
            gelp = GeneratedPackageMethodsPrinter(self.ypy_ctx, self.bundle_name, self.packages)
            gelp.print_output()

        # RST documentation
        self._print_go_rst_toc()

        # if self.generate_tests:
        #     self._print_cmake_file(self.packages, self.bundle_name, self.test_dir)
        # return self.source_files

    def _print_module(self, index, package, size):
        print('Processing %d of %d %s' % (index + 1, size, package.stmt.pos.ref))
        # Skip generating module for empty modules
        if len(package.owned_elements) == 0:
            return

        # Set up module directory
        module_dir = self.initialize_output_directory(self.models_dir)

        # Generate go module
        self._print_go_module(package, module_dir)

        # RST documentation
        self._print_go_rst_doc(package)

        # YANG models
        self._print_yang_files()

        # if self.generate_tests:
        #     self._print_tests(package, self.test_dir)

    def _print_go_module(self, package, path):
        path = '%s/%s' % (path, package.name)
        self.initialize_output_directory(path)

        go_module_file_name = '%s/%s.go' % (path, package.name)
        with open(go_module_file_name, 'w+') as file_descriptor:
            self.ypy_ctx.fd = file_descriptor
            mp = ModulePrinter(self.ypy_ctx, self.bundle_name, self.identity_subclasses)
            mp.print_output(package)

    def _print_go_rst_toc(self):
        if self.ydk_doc_dir is None:
            return
        packages = [p for p in self.packages if len(p.owned_elements) > 0]

        self.print_file(get_table_of_contents_file_name(self.ydk_doc_dir),
                        emit_table_of_contents,
                        _EmitArgs(self.ypy_ctx, packages, (self.bundle_name, self.bundle_version)))

    def _print_go_rst_doc(self, package):
        if self.ydk_doc_dir is None:
            return

        def _walk_n_print(named_element, p):
            self.print_file(get_go_doc_file_name(p, named_element),
                            emit_go_doc,
                            _EmitArgs(self.ypy_ctx, named_element, (self.identity_subclasses, self.bundle_name)))

            for owned_element in named_element.owned_elements:
                if isinstance(owned_element, (Class, Enum)):
                    _walk_n_print(owned_element, p)

        _walk_n_print(package, self.ydk_doc_dir)

    def _print_yang_files(self):
        yang_files_dir = os.path.sep.join([self.models_dir, '_yang'])
        mkpath(yang_files_dir)
        copy_tree(self.bundle.resolved_models_dir, yang_files_dir)

def get_table_of_contents_file_name(path):
    return '%s/ydk.models.rst' % path

def get_go_doc_file_name(path, named_element):
    return '%s/%s.rst' % (path, get_rst_file_name(named_element))

def emit_table_of_contents(ctx, packages, extra_args):
    bundle_name, bundle_version = extra_args
    DocPrinter(ctx, 'go', bundle_name, bundle_version).print_table_of_contents(packages)

def emit_go_doc(ctx, named_element, extra_args):
    identity_subclasses, bundle_name = extra_args
    DocPrinter(ctx, 'go', bundle_name).print_module_documentation(named_element, identity_subclasses)

def copy_tree(src, dst):
    names = os.listdir(src)
    outputs = []

    for n in names:
        src_name = os.path.join(src, n)
        dst_name = os.path.join(dst, n)

        if os.path.isdir(src_name):
            outputs.extend(copy_tree(src_name, dst))
        elif dst_name.endswith('.yang'):
            copy_file(src_name, dst_name)
            outputs.append(dst_name)

    return outputs
