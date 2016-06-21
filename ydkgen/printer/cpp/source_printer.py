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
from ydkgen.api_model import Class
from ydkgen.common import sort_classes_at_same_level
from ydkgen.printer.file_printer import FilePrinter


class SourcePrinter(FilePrinter):
    def __init__(self, ctx):
        super(SourcePrinter, self).__init__(ctx)

    def print_header(self, package):
        self.ctx.writeln('#include "ydk/make_unique.h"')
        self.ctx.bline()
        self.ctx.writeln('#include "{0}.h"'.format(package.name))
        self.ctx.bline()
        self.ctx.writeln('namespace ydk {')
        sub = package.get_py_mod_name()[len('ydk.models.'): package.get_py_mod_name().rfind('.')]
        self.ctx.writeln('namespace {0}'.format(sub + ' {'))
        self.ctx.bline()

    def print_trailer(self, package):
        self.ctx.bline()
        self.ctx.writeln('}')
        self.ctx.writeln('}')
        self.ctx.bline()

    def print_body(self, package):
        self._print_classes([clazz for clazz in package.owned_elements if isinstance(clazz, Class)])

    def _print_classes(self, clazzes):
        sorted_classes = sort_classes_at_same_level(clazzes)
        for clazz in sorted_classes:
            self._print_class(clazz)

    def _print_class(self, clazz):
        self._print_classes([nested_class for nested_class in clazz.owned_elements if isinstance(nested_class, Class)])

        self._print_class_body(clazz)

    def _print_class_body(self, clazz):
        self.ctx.writeln(clazz.qualified_cpp_name() + '::' + clazz.name + '() {')
        self.ctx.lvl_inc()
        if clazz.is_identity() and len(clazz.extends) == 0:
            self.ctx.bline()
        else:
            properties = clazz.properties()
            for prop in properties:
                self._print_class_inits_property(prop)

        self.ctx.bline()
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_class_inits_property(self, prop):
        if not prop.is_many:
            self._print_class_inits_unique(prop)

    def _print_class_inits_unique(self, prop):
        if isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
            self.ctx.writeln('%s = std::make_unique<%s>();' % (prop.name, prop.property_type.qualified_cpp_name()))

