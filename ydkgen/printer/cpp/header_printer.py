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
header_printer.py

 prints C++ classes

"""

from ydkgen.api_model import Class
from ydkgen.builder import MultiFileHeader
from ydkgen.printer import MultiFilePrinter

from .class_members_printer import ClassMembersPrinter
from .class_enum_printer import EnumPrinter


class HeaderPrinter(MultiFilePrinter):
    def __init__(self, ctx, identity_subclasses):
        super(HeaderPrinter, self).__init__(ctx)
        self.enum_printer = EnumPrinter(self.ctx)
        self.identity_subclasses = identity_subclasses

    def print_body(self, multi_file):
        assert isinstance(multi_file, MultiFileHeader)
        for clazz in multi_file.class_list:
            self._print_class(clazz)

    def print_extra(self, package, multi_file):
        assert isinstance(multi_file, MultiFileHeader)
        self._print_enums(package, multi_file.class_list, multi_file.file_name, (not multi_file.fragmented))

    def print_header(self, package, multi_file):
        assert isinstance(multi_file, MultiFileHeader)
        self.p = package
        self._print_include_guard_header(multi_file.include_guard)
        self._print_imports(package, multi_file.imports)
        self.ctx.writeln('namespace ydk {')
        self.ctx.writeln('namespace %s {' % package.name)
        self.ctx.bline()

    def print_trailer(self, package, multi_file):
        assert isinstance(multi_file, MultiFileHeader)
        self.ctx.bline()
        self.ctx.writeln('}')
        self.ctx.writeln('}')
        self._print_include_guard_trailer(multi_file.include_guard)
        self.ctx.bline()

    def _print_imports(self, package, imports_to_print):
        self._print_common_imports(package)
        self._print_unique_imports(package, imports_to_print)

    def _print_common_imports(self, package):
        self.ctx.writeln('#include <memory>')
        self.ctx.writeln('#include <vector>')
        self.ctx.writeln('#include <string>')
        self.ctx.writeln('#include <ydk/types.hpp>')
        self.ctx.writeln('#include <ydk/errors.hpp>')
        self.ctx.bline()

    def _print_unique_imports(self, package, imports_to_print):
        if len(package.imported_types()) == 0 and len(imports_to_print) == 0:
            return
        for imported_type in package.imported_types():
            if all((id(imported_type) in self.identity_subclasses,
                    self.is_derived_identity(package, imported_type))):
                import_stmt = '#include "{0}"'.format(imported_type.get_cpp_header_name())
                imports_to_print.add(import_stmt)
        imports_to_print = sorted(imports_to_print)
        for import_to_print in imports_to_print:
            self.ctx.writeln('%s' % import_to_print)
        self.ctx.bline()

    def _print_class(self, clazz):
        self._print_class_header(clazz)
        self._print_class_body(clazz)
        self._print_class_trailer(clazz)

    def _print_class_header(self, clazz):
        parents = 'Entity'
        if isinstance(clazz.owner, Class):
            self.ctx.bline()
        class_name = clazz.qualified_cpp_name()
        if len(clazz.extends) > 0:
            parents = ', '.join([sup.fully_qualified_cpp_name() for sup in clazz.extends])
            if clazz.is_identity():
                parents += ', virtual Identity'
            self.ctx.writeln('class ' + class_name + ' : public ' + parents)
        elif clazz.is_identity():
            self.ctx.writeln('class ' + class_name + ' : public virtual Identity')
        else:
            self.ctx.writeln('class ' + class_name + ' : public Entity')
        self.ctx.writeln('{')
        self.ctx.lvl_inc()

    def _print_class_body(self, clazz):
        members_printer = ClassMembersPrinter(self.ctx)
        members_printer.print_class_members(clazz)
        self._print_forward_declarations(clazz)
        members_printer.print_class_children_members(clazz)

    def _print_class_trailer(self, clazz):
        self.ctx.bline()
        self.ctx.lvl_dec()
        self.ctx.writeln('}; // ' + clazz.qualified_cpp_name())
        self.ctx.bline()

    def _print_forward_declarations(self, clazz):
        child_classes = [nested_class for nested_class in clazz.owned_elements if isinstance(nested_class, Class)]
        if len(child_classes) == 0:
            return
        self.ctx.bline()
        self.ctx.lvl_inc()
        for child in child_classes:
            self._print_forward_declaration(child)
        self.ctx.lvl_dec()

    def _print_forward_declaration(self, clazz):
        self.ctx.writeln('class ' + clazz.name + '; //type: ' + clazz.qualified_cpp_name())

    def _print_enums(self, package, classes, file_name, reset_enum_lookup):
        self.enum_printer.print_enum_declarations(package, classes, file_name, reset_enum_lookup)
