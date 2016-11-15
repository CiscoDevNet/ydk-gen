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
            self.ctx.writeln('children["%s"] = %s.get();' % (child.stmt.arg, child.name))
            self.ctx.bline()

    def _print_class_constructor_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_class_inits(self, clazz, leafs, children):
        children_init = ''
        if len(leafs) > 0:
            self.ctx.writeln(': \n\t%s' % ',\n\t '.join('%s{YType::%s, "%s"}' % (prop.name, get_type_name(prop.property_type), prop.stmt.arg) for prop in leafs))
            children_init = ', '
        else:
            children_init = ':\n\t '
        chs = [prop for prop in children if (not prop.is_many and (prop.stmt.search_one('presence') is None))]
        children_init += '\t%s' % (',\n '.join('\t%s(std::make_unique<%s>())' % (prop.name, prop.property_type.qualified_cpp_name()) for prop in chs))
        if len(chs) > 0:
            self.ctx.writeln('%s' % (children_init))


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
