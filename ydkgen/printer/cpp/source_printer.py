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
source_printer.py

 prints C++ classes

"""
from ydkgen.builder import MultiFileSource
from ydkgen.printer import MultiFilePrinter

from .class_source_printer import ClassSourcePrinter
from .class_enum_printer import EnumPrinter


class SourcePrinter(MultiFilePrinter):
    def __init__(self, ctx, bundle_name):
        super(SourcePrinter, self).__init__(ctx)
        self.enum_printer = EnumPrinter(self.ctx)
        self.bundle_name = bundle_name

    def print_body(self, multi_file):
        assert isinstance(multi_file, MultiFileSource)
        for clazz in multi_file.class_list:
            self._print_class(clazz)

    def print_extra(self, package, multi_file):
        assert isinstance(multi_file, MultiFileSource)
        self._print_enums(package, multi_file.class_list)

    def print_header(self, package, multi_file):
        assert isinstance(multi_file, MultiFileSource)
        self.ctx.bline()
        self.ctx.writeln('#include <sstream>')
        self.ctx.writeln('#include <iostream>')
        self.ctx.writeln('#include <ydk/entity_util.hpp>')
        self.ctx.writeln('#include "bundle_info.hpp"')
        self.ctx.writeln('#include "generated_entity_lookup.hpp"')
        self.ctx.writeln('#include "{0}"'.format(multi_file.file_name.replace('.cpp', '.hpp')))
        for header_import in multi_file.imports:
            self.ctx.writeln(header_import)
        self.ctx.bline()
        self.ctx.writeln('namespace ydk {')
        self.ctx.writeln('namespace %s {' % package.name)
        self.ctx.bline()

    def print_trailer(self, package, multi_file):
        assert isinstance(multi_file, MultiFileSource)
        self.ctx.bline()
        self.ctx.writeln('}')
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_class(self, clazz):
        ClassSourcePrinter(self.ctx, self.bundle_name).print_output(clazz)

    def _print_enums(self, package, classes):
        self.enum_printer.print_enum_to_string_funcs(package, classes)
