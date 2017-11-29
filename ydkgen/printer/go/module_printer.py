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
module_printer.py

 prints Go classes

"""
from ydkgen.api_model import Class, Enum, Bits
from .class_printer import ClassPrinter
from .class_identity_printer import IdentityPrinter
from .class_enum_printer import EnumPrinter

from ydkgen.printer.file_printer import FilePrinter
from ydkgen.common import convert_to_reStructuredText


class ModulePrinter(FilePrinter):

    def __init__(self, ctx, bundle_name, identity_subclasses):
        super(ModulePrinter, self).__init__(ctx)
        self.bundle_name = bundle_name
        self.identity_subclasses = identity_subclasses
        self.class_printer = ClassPrinter(ctx, bundle_name, identity_subclasses)
        self.identity_printer = IdentityPrinter(ctx, bundle_name, identity_subclasses)
        self.enum_printer = EnumPrinter(ctx)

    def print_header(self, package):
        self._print_package_description(package)
        self.ctx.writeln('package %s' % package.name)
        self.ctx.bline()
        self._print_imports(package)
        self._print_init(package)

    def print_body(self, package):
        for elem in package.owned_elements:
            self._print_element(elem)

    def print_extra(self, package):
        pass

    def print_trailer(self, package):
        pass

    def _print_package_description(self, package):
        comment = package.stmt.search_one('description')
        if comment is not None:
            comment = comment.arg
            for line in comment.split('\n'):
                self.ctx.writeln("// %s" % convert_to_reStructuredText(line))

    def _print_element(self, elem):
        if isinstance(elem, Enum):
            self.enum_printer.print_enum(elem)
        elif isinstance(elem, Bits):
            pass
        elif elem.is_identity():
            self.identity_printer.print_identity(elem)
        else:
            self.class_printer.print_output(elem)

    def _print_imports(self, package):
        self.ctx.writeln('import (')
        self.ctx.lvl_inc()
        self._print_static_imports(package)
        self.ctx.lvl_dec()
        self.ctx.writeln(')')
        self.ctx.bline()

    def _print_init(self, package):
        self.ctx.writeln('func init() {')
        self.ctx.lvl_inc()
        for e in package.owned_elements:
            ns = package.stmt.search_one('namespace')
            if ns is not None and isinstance(e, Class) and not e.is_identity():
                self.ctx.writeln('ydk.RegisterEntity("{{{} {}}}", reflect.TypeOf({}{{}}))'.format(ns.arg, e.stmt.arg, e.go_name()))
                self.ctx.writeln('ydk.RegisterEntity("{}:{}", reflect.TypeOf({}{{}}))'.format(package.stmt.arg, e.stmt.arg, e.go_name()))
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_static_imports(self, package):
        if self._has_bits(package):
            self.ctx.writeln('"strings"')
        has_top_entity = False
        for c in package.owned_elements:
            if isinstance(c, Class) and not c.is_identity():
                has_top_entity = True
                self.ctx.writeln('"fmt"')
                break
        if has_top_entity:
            self.ctx.writeln('"github.com/CiscoDevNet/ydk-go/ydk"')
            self.ctx.writeln('"github.com/CiscoDevNet/ydk-go/ydk/types"')
            self.ctx.writeln('"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"')
            self.ctx.writeln('"github.com/CiscoDevNet/ydk-go/ydk/models/{}"'.format(self.bundle_name))
            self.ctx.writeln('"reflect"')

    def _has_bits(self, element):
        for e in element.owned_elements:
            if isinstance(e, Bits) or self._has_bits(e):
                return True
        return False
