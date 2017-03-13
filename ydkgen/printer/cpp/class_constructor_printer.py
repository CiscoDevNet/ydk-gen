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
from ydkgen.api_model import Bits, Class, DataType, Enum


class ClassConstructorPrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_constructor(self, clazz, leafs, children):
        self._print_class_constructor_header(clazz, leafs, children)
        self._print_class_constructor_body(clazz, leafs, children)
        self._print_class_constructor_trailer()

    def _print_class_constructor_header(self, clazz, leafs, children):
        self.ctx.writeln(clazz.qualified_cpp_name() + '::' + clazz.name + '()')
        self.ctx.lvl_inc()
        if clazz.is_identity():
            self.ctx.writeln(' : Identity("%s:%s")' % (clazz.module.arg, clazz.stmt.arg))
        else:
            self._print_class_inits(clazz, leafs, children)
        self.ctx.lvl_dec()
        self.ctx.writeln('{')
        self.ctx.lvl_inc()

    def _print_class_constructor_body(self, clazz, leafs, children):
        self._print_init_children(children)
        if not clazz.is_identity():
            self.ctx.writeln('yang_name = "%s"; yang_parent_name = "%s";' % (clazz.stmt.arg, clazz.owner.stmt.arg))

    def _print_init_children(self, children):
        for child in children:
            if child.is_many or child.stmt.search_one('presence') is not None:
                continue
            self.ctx.writeln('%s->parent = this;' % child.name)
            self.ctx.bline()

    def _print_class_constructor_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_class_inits(self, clazz, leafs, children):
        if len(leafs) > 0:
            self.ctx.writeln(':')
            index = 0
            while index < len(leafs):
                prop = leafs[index]
                leaf_name = ''
                if prop.stmt.i_module.arg != clazz.stmt.i_module.arg:
                    leaf_name = prop.stmt.i_module.arg + ':' + prop.stmt.arg
                else:
                    leaf_name = prop.stmt.arg
                self.ctx.writeln('%s{YType::%s, "%s"}%s' % (prop.name,
                            get_type_name(prop.property_type), leaf_name, (',' if index != len(leafs) - 1 else '')))
                index += 1

        init_stmts = []
        for child in children:
            if not child.is_many:
                if (child.stmt.search_one('presence') is None):
                    init_stmts.append('%s(std::make_shared<%s>())' % (child.name, child.property_type.qualified_cpp_name()))
                else:
                    init_stmts.append('%s(nullptr) // presence node' % (child.name))
        if len(init_stmts) > 0:
            if len(leafs) == 0:
                self.ctx.writeln(':')
            else:
                self.ctx.writeln('\t,')
            self.ctx.writeln('\n\t,'.join(init_stmts))


def get_type_name(prop_type):
    if prop_type.name == 'string':
        return 'str'
    elif prop_type.name == 'leafref':
        return 'str'
    elif prop_type.name == 'decimal64':
        return 'str'
    elif prop_type.name == 'union':
        return 'str'
    elif prop_type.name == 'binary':
        return 'str'
    elif prop_type.name == 'instance-identifier':
        return 'str'
    elif isinstance(prop_type, Bits):
        return 'bits'
    elif isinstance(prop_type, Class) and prop_type.is_identity():
        return 'identityref'
    elif isinstance(prop_type, Enum):
        return 'enumeration'
    elif isinstance(prop_type, DataType):
        return 'str'
    return prop_type.name
