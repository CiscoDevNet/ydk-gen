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
from ydkgen.printer.language_bindings_printer import LanguageBindingsPrinter, _EmitArgs

from .header_printer import HeaderPrinter
from .source_printer import SourcePrinter


class CppBindingsPrinter(LanguageBindingsPrinter):

    def __init__(self, ydk_root_dir):
        super(CppBindingsPrinter, self).__init__(ydk_root_dir)

    def print_files(self):
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

        self._print_header_file(package, module_dir)
        self._print_source_file(package, module_dir)

    def _print_header_file(self, package, path):
        self.print_file(get_header_file_name(path, package),
                        emit_header,
                        _EmitArgs(self.ypy_ctx, package))

    def _print_source_file(self, package, path):
        self.print_file(get_source_file_name(path, package),
                        emit_source,
                        _EmitArgs(self.ypy_ctx, package))


def get_source_file_name(path, package):
    return '%s/%s.cpp' % (path, package.name)


def get_header_file_name(path, package):
    return '%s/%s.h' % (path, package.name)


def emit_header(ctx, package):
    HeaderPrinter(ctx).print_output(package)


def emit_source(ctx, package):
    SourcePrinter(ctx).print_output(package)
