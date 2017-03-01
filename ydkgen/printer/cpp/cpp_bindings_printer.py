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

from ydkgen.api_model import Class, Enum
from ydkgen.builder import MultiFileBuilder, MultiFileHeader, MultiFileSource
from ydkgen.common import get_rst_file_name
from ydkgen.printer.language_bindings_printer import LanguageBindingsPrinter, _EmitArgs

from .header_printer import HeaderPrinter
from .source_printer import SourcePrinter
from .entity_lookup_printer import EntityLookUpPrinter
from .test_case_cmake_file_printer import CMakeListsPrinter
from ..doc import DocPrinter
from ..tests import TestPrinter


class CppBindingsPrinter(LanguageBindingsPrinter):

    def __init__(self, ydk_root_dir, bundle_name, bundle_version, generate_tests, sort_clazz):
        super(CppBindingsPrinter, self).__init__(ydk_root_dir, bundle_name, bundle_version, generate_tests, sort_clazz)
        self.source_files = []
        self.header_files = []

    def print_files(self):
        only_modules = [package.stmt for package in self.packages]
        size = len(only_modules)

        for index, package in enumerate(self.packages):
            self._print_module(index, package, size)

        self._print_entity_lookup_files(self.packages, self.models_dir)

        # RST documentation
        self._print_cpp_rst_toc()
        if self.generate_tests:
            self._print_cmake_file(self.packages, self.bundle_name, self.test_dir)
        return (self.source_files, self.header_files)

    def _print_module(self, index, package, size):
        print('Processing %d of %d %s' % (index + 1, size, package.stmt.pos.ref))
        # Skip generating module for empty modules
        if len(package.owned_elements) == 0:
            return
        builder = MultiFileBuilder(package, self.classes_per_source_file, self.sort_clazz)
        self._print_header_file(package, builder.multi_file_data, self.models_dir)
        self._print_source_file(package, builder.multi_file_data, self.models_dir)
        self._print_cpp_rst_doc(package)
        if self.generate_tests:
            self._print_tests(package, self.test_dir)

    def _print_header_file(self, package, multi_file_data, path):
        hp = HeaderPrinter(self.ypy_ctx,
                           self.identity_subclasses)
        for multi_file_header in [x for x in multi_file_data.multi_file_list if isinstance(x, MultiFileHeader)]:
            hp.print_output(
                            package,
                            multi_file_header,
                            path
                            )
            if not multi_file_header.fragmented:
                self.header_files.append(multi_file_header.file_name)

    def _print_source_file(self, package, multi_file_data, path):
        sp = SourcePrinter(self.ypy_ctx, self.bundle_name)
        for multi_file_source in [x for x in multi_file_data.multi_file_list if isinstance(x, MultiFileSource)]:
            sp.print_output(
                            package,
                            multi_file_source,
                            path
                            )
            file_name = multi_file_source.file_name
            if multi_file_source.fragmented:
                file_name = os.path.join('fragmented', file_name)
            self.source_files.append(file_name)

    def _print_entity_lookup_files(self, packages, path):
        self.print_file(get_entity_lookup_source_file_name(path),
                        emit_entity_lookup_source,
                        _EmitArgs(self.ypy_ctx, packages, self.bundle_name))
        self.print_file(get_entity_lookup_header_file_name(path),
                        emit_entity_lookup_header,
                        _EmitArgs(self.ypy_ctx, packages, self.bundle_name))

    def _print_tests(self, package, path):
        empty = self.is_empty_package(package)
        if not empty:
            self.print_file(get_testcase_file_name(path, package),
                            emit_test_cases,
                            _EmitArgs(self.ypy_ctx, package, self.identity_subclasses))

    def _print_cmake_file(self, packages, bundle_name, path):
        args = {'bundle_name': bundle_name,
                'identity_subclasses': self.identity_subclasses}
        self.print_file(get_testcase_cmake_file_name(path),
                        emit_cmake_file,
                        _EmitArgs(self.ypy_ctx, packages, args))

    def _print_cpp_rst_doc(self, package):
        if self.ydk_doc_dir is None:
            return

        def _walk_n_print(named_element, p):
            self.print_file(get_cpp_doc_file_name(p, named_element),
                            emit_cpp_doc,
                            _EmitArgs(self.ypy_ctx, named_element, self.identity_subclasses))

            for owned_element in named_element.owned_elements:
                if isinstance(owned_element, (Class, Enum)):
                    _walk_n_print(owned_element, p)

        _walk_n_print(package, self.ydk_doc_dir)

    def _print_cpp_rst_toc(self):
        if self.ydk_doc_dir is None:
            return
        packages = [p for p in self.packages if len(p.owned_elements) > 0]

        self.print_file(get_table_of_contents_file_name(self.ydk_doc_dir),
                        emit_table_of_contents,
                        _EmitArgs(self.ypy_ctx, packages, (self.bundle_name, self.bundle_version)))


def get_entity_lookup_source_file_name(path):
    return '%s/generated_entity_lookup.cpp' % (path)


def get_entity_lookup_header_file_name(path):
    return '%s/generated_entity_lookup.hpp' % (path)


def get_table_of_contents_file_name(path):
    return '%s/ydk.models.rst' % path


def get_testcase_file_name(path, package):
    return '%s/test_%s.cpp' % (path, package.stmt.arg.replace('-', '_'))


def get_testcase_cmake_file_name(path):
    return '%s/CMakeLists.txt' % path


def get_cpp_doc_file_name(path, named_element):
    return '%s/%s.rst' % (path, get_rst_file_name(named_element))


def emit_header(ctx, package, extra_args):
    HeaderPrinter(ctx, extra_args[0], extra_args[1]).print_output(package)


def emit_entity_lookup_source(ctx, packages, bundle_name):
    EntityLookUpPrinter(ctx).print_source(packages, bundle_name)


def emit_entity_lookup_header(ctx, packages, bundle_name):
    EntityLookUpPrinter(ctx).print_header(bundle_name)


def emit_cpp_doc(ctx, named_element, identity_subclasses):
    DocPrinter(ctx, 'cpp').print_module_documentation(named_element, identity_subclasses)


def emit_table_of_contents(ctx, packages, extra_args):
    DocPrinter(ctx, 'cpp').print_table_of_contents(packages, extra_args[0], extra_args[1])


def emit_test_cases(ctx, package, identity_subclasses):
    TestPrinter(ctx, 'cpp').print_tests(package, identity_subclasses)


def emit_cmake_file(ctx, packages, args):
    CMakeListsPrinter(ctx).print_cmakelists_file(packages, args)
