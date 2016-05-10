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
from ydkgen.api_model import Class, Bits
from ydkgen.common import sort_classes_at_same_level
from ydkgen.printer.file_printer import FilePrinter


class HeaderPrinter(FilePrinter):
    def __init__(self, ctx):
        super(HeaderPrinter, self).__init__(ctx)

    def print_header(self, package):
        self._print_include_guard_header(package)
        self._print_imports(package)
        self.ctx.writeln('namespace ydk {')
        sub = package.get_py_mod_name()[len('ydk.models.'): package.get_py_mod_name().rfind('.')]
        self.ctx.writeln('namespace {0}'.format(sub + ' {'))
        self.ctx.bline()

    def _print_imports(self, package):
        self._print_common_imports(package)
        self._print_unique_imports(package)

    def _print_common_imports(self, package):
        self.ctx.writeln('#include <memory>')
        self.ctx.writeln('#include <vector>')
        self.ctx.writeln('#include <string>')
        self.ctx.writeln('#include "ydk/entity.h"')
        self.ctx.writeln('#include "ydk/types.h"')
        self.ctx.bline()

    def _print_unique_imports(self, package):
        if len(package.imported_types()) == 0:
            return
        imports_to_print = []
        for imported_type in package.imported_types():
            import_stmt = '#include "{0}"'.format(imported_type.get_cpp_header_name())
            if import_stmt in imports_to_print:
                continue
            else:
                imports_to_print.append(import_stmt)
        imports_to_print = sorted(imports_to_print)
        for import_to_print in imports_to_print:
            self.ctx.writeln('%s' % import_to_print)
        self.ctx.bline()

    def print_trailer(self, package):
        self.ctx.bline()
        self.ctx.writeln('}')
        self.ctx.writeln('}')
        self._print_include_guard_trailer(package)
        self.ctx.bline()

    def print_body(self, package):
        self._print_classes([clazz for clazz in package.owned_elements if isinstance(clazz, Class)])

    def _print_classes(self, clazzes):
        sorted_classes = sort_classes_at_same_level(clazzes)
        for clazz in sorted_classes:
            self._print_class(clazz)

    def _print_class(self, clazz):
        self._print_class_header(clazz)
        self._print_class_body(clazz)
        self._print_class_trailer(clazz)

    def _print_class_header(self, clazz):
        parents = 'object'
        if len(clazz.extends) > 0:
            parents = ', '.join([sup.qualified_cpp_name() for sup in clazz.extends])
            self.ctx.writeln('class ' + clazz.name + ' : public ' + parents + ' {')
        else:
            self.ctx.writeln('class ' + clazz.name + ' : public Entity {')
        self.ctx.lvl_inc()

    def _print_class_body(self, clazz):
        child_classes = [nested_class for nested_class in clazz.owned_elements if isinstance(nested_class, Class)]
        self.ctx.writeln('public:')
        self.ctx.lvl_inc()
        self.ctx.writeln(clazz.name + '();')
        self.ctx.lvl_dec()
        self.ctx.bline()
        if len(child_classes) > 0:
            self._print_classes(child_classes)

        if clazz.is_identity() and len(clazz.extends) == 0:
            self.ctx.bline()
        else:
            # self._print_class_inits_properties(clazz)
            properties = clazz.properties()
            self.ctx.writeln('public:')
            self.ctx.lvl_inc()
            for prop in properties:
                self._print_class_inits_property(prop)
            self.ctx.lvl_dec()
        self.ctx.bline()

    def _print_class_inits_property(self, prop):
        if prop.is_many:
            self._print_class_inits_is_many(prop)
        else:
            self._print_class_inits_unique(prop)

    def _print_class_inits_is_many(self, prop):
        if isinstance(prop.property_type, Class):
            self.ctx.writeln('std::vector< std::unique_ptr<%s> > %s;' % (prop.property_type.qualified_cpp_name(), prop.name))
        else:
            self.ctx.writeln('std::vector<std::string> %s;' % (prop.name))

    def _print_class_inits_unique(self, prop):
        if isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
            self.ctx.writeln('std::unique_ptr<%s> %s;' % (prop.property_type.qualified_cpp_name(), prop.name,))
            # instantiate the class only if it is not a presence class
            '''stmt = prop.property_type.stmt
            if stmt.search_one('presence') is None:
                self.ctx.writeln('self.%s = %s()' %
                                 (prop.name, prop.property_type.qn()))
                self.ctx.writeln('self.%s.parent = self' % (prop.name))
            else:
                self.ctx.writeln('self.%s = None' % (prop.name,))'''
        elif isinstance(prop.property_type, Bits):
            '''self.ctx.writeln('self.%s = %s()' %
                             (prop.name, prop.property_type.qn()))'''
        else:
            type_str=''
            from pyang.types import PathTypeSpec, BinaryTypeSpec, BooleanTypeSpec, Decimal64TypeSpec, \
                                    RangeTypeSpec, EmptyTypeSpec, IntTypeSpec, LengthTypeSpec, PatternTypeSpec, \
                                    StringTypeSpec
            type_stmt = prop.stmt.search_one('type')
            target_type_stmt = type_stmt
            if type_stmt is None:
                return
            type_spec = type_stmt.i_type_spec
            while isinstance(type_spec, PathTypeSpec):
                if not hasattr(type_spec, 'i_target_node'):
                    return None
                target_type_stmt = type_spec.i_target_node.search_one('type')
                type_spec = target_type_stmt.i_type_spec
            if isinstance(type_spec, BinaryTypeSpec):
                type_str = 'std::string'
            elif isinstance(type_spec, BooleanTypeSpec):
                type_str = 'bool'
            elif isinstance(type_spec, Decimal64TypeSpec):
                type_str='int64'
            elif isinstance(type_spec, EmptyTypeSpec):
                type_str = 'Empty'
            elif isinstance(type_spec, IntTypeSpec):
                type_str = 'int'
            elif isinstance(type_spec, LengthTypeSpec):
                type_str = 'std::string'

            elif isinstance(type_spec, PatternTypeSpec):
                type_str = 'std::string'

            elif isinstance(type_spec, RangeTypeSpec):
                type_str = 'std::string'

            elif isinstance(type_spec, StringTypeSpec):
                type_str = 'std::string'

            else:
                type_str = 'std::string'
            self.ctx.writeln('%s %s;' % (type_str, prop.name,))

    def _print_class_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.writeln('};')
        self.ctx.bline()

    def _print_include_guard_header(self, package):
        self.ctx.writeln('#ifndef _{0}_'.format(package.name.upper()))
        self.ctx.writeln('#define _{0}_'.format(package.name.upper()))
        self.ctx.bline()

    def _print_include_guard_trailer(self, package):
        self.ctx.bline()
        self.ctx.writeln('#endif /* _{0}_ */'.format(package.name.upper()))
