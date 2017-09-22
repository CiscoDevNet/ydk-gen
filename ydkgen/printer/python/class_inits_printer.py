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
class_inits_printer.py

 __init__ printer

"""
from ydkgen.api_model import Bits, Class, Package
from ydkgen.common import get_property_name


class ClassInitsPrinter(object):

    def __init__(self, ctx, one_class_per_module):
        self.ctx = ctx
        self.one_class_per_module = one_class_per_module

    def print_output(self, clazz):
        self._print_class_inits_header(clazz)
        self._print_class_inits_body(clazz)
        self._print_class_inits_trailer(clazz)

    def _print_class_inits_header(self, clazz):
        self.ctx.writeln('def __init__(self):')
        self.ctx.lvl_inc()
        for super_class in clazz.extends:
            self.ctx.writeln('%s.__init__(self)' % super_class.qn())
        if self.one_class_per_module:
            self._print_children_imports(clazz)

    def _print_class_inits_body(self, clazz):
        if clazz.is_identity() and len(clazz.extends) == 0:
            self.ctx.writeln('pass')
        else:
            self._print_class_inits_properties(clazz)
            self.print_class_is_rpc(clazz)

    def _print_children_imports(self, clazz):
        for prop in clazz.properties():
            if isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
                self.ctx.writeln('from .%s import %s' % (get_property_name(prop.property_type, prop.property_type.iskeyword), get_property_name(prop.property_type, prop.property_type.iskeyword)))
                self.ctx.writeln('self.__class__.%s = %s.%s' % (prop.property_type.name, get_property_name(prop.property_type, prop.property_type.iskeyword), prop.property_type.name))
        self.ctx.bline()

    def _print_class_inits_properties(self, clazz):
        # first the parent prop
        if not isinstance(clazz.owner, Package):
            self.ctx.writeln('self.parent = None')
        properties = clazz.properties()
        if clazz.stmt.search_one('presence'):
            self._print_presence_property(clazz)

        if len(properties) == 0 and not clazz.is_rpc() and len(clazz.extends) == 0 and isinstance(clazz.owner, Package):
                self.ctx.writeln('pass')
        else:
            for prop in properties:
                self._print_class_inits_property(prop, clazz)

    def _print_class_inits_property(self, prop, clazz):
        if prop.is_many:
            self._print_class_inits_is_many(prop)
        else:
            self._print_class_inits_unique(prop, clazz)

    def _print_class_inits_is_many(self, prop):
        if prop.stmt.keyword == 'list':
            self.ctx.writeln('self.%s = YList()' % prop.name)
            self.ctx.writeln('self.%s.parent = self' % prop.name)
            self.ctx.writeln("self.%s.name = '%s'" % (prop.name, prop.name))
        elif prop.stmt.keyword == 'leaf-list':
            self.ctx.writeln('self.%s = YLeafList()' % prop.name)
            self.ctx.writeln('self.%s.parent = self' % prop.name)
            self.ctx.writeln("self.%s.name = '%s'" % (prop.name, prop.name))
        else:
            self.ctx.writeln('self.%s = []' % prop.name)

    def _print_class_inits_unique(self, prop, clazz):
        if isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
            # instantiate the class only if it is not a presence class
            stmt = prop.property_type.stmt
            if stmt.search_one('presence') is None:
                if self.one_class_per_module:
                    self.ctx.writeln('self.%s = %s.%s()' % (prop.name, get_property_name(prop.property_type, prop.property_type.iskeyword), prop.property_type.name))
                else:
                    self.ctx.writeln('self.%s = %s()' %
                                     (prop.name, prop.property_type.qn()))
                    self.ctx.writeln('self.%s.parent = self' % (prop.name))
            else:
                self.ctx.writeln('self.%s = None' % (prop.name,))
        elif isinstance(prop.property_type, Bits):
            if self.one_class_per_module:
                self.ctx.writeln('self.%s = %s.%s()' %
                                 (prop.name, clazz.name, prop.property_type.name))
            else:
                self.ctx.writeln('self.%s = %s()' %
                                 (prop.name, prop.property_type.qn()))
        else:
            self.ctx.writeln('self.%s = None' % (prop.name,))

    def print_class_is_rpc(self, clazz):
        if clazz.is_rpc():
            self.ctx.bline()
            self.ctx.writeln('self.is_rpc = True')

    def _print_class_inits_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.bline()

    def _print_presence_property(self, clazz):
        self.ctx.writeln('self.%s = %s' % ('_is_presence', 'True'))
