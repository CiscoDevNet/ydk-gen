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
from ydkgen.api_model import Class, Enum
from ydkgen.common import get_rst_file_name
from ydkgen.printer.language_bindings_printer import LanguageBindingsPrinter, _EmitArgs

from .header_printer import HeaderPrinter
from .source_printer import SourcePrinter
from .entity_lookup_printer import EntityLookUpPrinter
from ..doc import DocPrinter


class CppBindingsPrinter(LanguageBindingsPrinter):

    def __init__(self, ydk_root_dir, bundle_name, generate_tests, sort_clazz):
        super(CppBindingsPrinter, self).__init__(ydk_root_dir, bundle_name, generate_tests, sort_clazz)

    def print_files(self):
        only_modules = [package.stmt for package in self.packages]
        size = len(only_modules)

        for index, package in enumerate(self.packages):
            self.print_module(index, package, size)

        self._print_entity_lookup_files(self.models_dir, self.packages)

        # RST documentation
        if self.ydk_doc_dir is not None:
            self._print_cpp_rst_toc()

    def print_module(self, index, package, size):

        print('Processing %d of %d %s' % (index + 1, size, package.stmt.pos.ref))
        # Skip generating module for empty modules
        if len(package.owned_elements) == 0:
            return

        self._print_header_file(package, self.models_dir)
        self._print_source_file(package, self.models_dir)
        if self.ydk_doc_dir is not None:
            self._print_cpp_rst_doc(package)

    def _print_header_file(self, package, path):
        self.print_file(get_header_file_name(path, package),
                        emit_header,
                        _EmitArgs(self.ypy_ctx, package, self.sort_clazz))

    def _print_source_file(self, package, path):
        self.print_file(get_source_file_name(path, package),
                        emit_source,
                        _EmitArgs(self.ypy_ctx, package, self.sort_clazz))

    def _print_entity_lookup_files(self, path, packages):
        self.print_file(get_entity_lookup_source_file_name(path),
                        emit_entity_lookup_source,
                        _EmitArgs(self.ypy_ctx, packages, self.bundle_name))

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
                        _EmitArgs(self.ypy_ctx, packages, self.bundle_name))


def get_source_file_name(path, package):
    return '%s/%s.cpp' % (path, package.name)


def get_header_file_name(path, package):
    return '%s/%s.hpp' % (path, package.name)


def get_entity_lookup_source_file_name(path):
    return '%s/generated_entity_lookup.cpp' % (path)


def get_table_of_contents_file_name(path):
    return '%s/ydk.models.rst' % path


def get_cpp_doc_file_name(path, named_element):
    return '%s/%s.rst' % (path, get_rst_file_name(named_element))


def emit_header(ctx, package, sort_clazz):
    HeaderPrinter(ctx, sort_clazz).print_output(package)


def emit_source(ctx, package, sort_clazz):
    SourcePrinter(ctx, sort_clazz).print_output(package)


def emit_entity_lookup_source(ctx, packages, bundle_name):
    EntityLookUpPrinter(ctx).print_source(packages, bundle_name)


def emit_cpp_doc(ctx, named_element, identity_subclasses):
    DocPrinter(ctx, 'cpp').print_module_documentation(named_element, identity_subclasses)


def emit_table_of_contents(ctx, packages, bundle_name):
    DocPrinter(ctx, 'cpp').print_table_of_contents(packages, bundle_name)
