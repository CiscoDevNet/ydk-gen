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
class_has_data_printer.py 
 
 Printer for the _has_data method.
 
"""
from ydkgen.api_model import Bits, Class


class ClassHasDataPrinter(object):

    def __init__(self, ctx):
        """
            _has_data() printer

            :attribute ctx The printer context

        """
        self.ctx = ctx

    def print_output(self, clazz):
        """
            prints the _has_data() function.

            prints a function in the entity clazz that can be queried
            to find if the element has any data to be be updated or created
            in its hierarchy.

        """
        self._print_has_data_functions_header(clazz)
        self._print_has_data_functions_body(clazz)
        self._print_has_data_functions_trailer(clazz)

    def _print_has_data_functions_header(self, clazz):
        self.ctx.writeln('def _has_data(self):')
        self.ctx.lvl_inc()

    def _print_has_data_functions_body(self, clazz):
        self._print_has_data_function('not self.is_config()', 'False')
        if clazz.stmt.search_one('presence') and len(clazz.properties()) == 0:
            self._print_has_data_function('self._is_presence')

        for prop in clazz.properties():
            if isinstance(prop.property_type, Class):
                if not prop.is_many:
                    if not prop.property_type.is_identity():
                        self._print_has_data_function(
                            'self.%s is not None and self.%s._has_data()' % (prop.name, prop.name))
                        self.ctx.bline()
                    else:
                        self._print_has_data_function(
                            'self.%s is not None' % prop.name)
                        self.ctx.bline()
                else:
                    self.ctx.writeln('if self.%s is not None:' % prop.name)
                    self.ctx.lvl_inc()
                    self.ctx.writeln('for child_ref in self.%s:' % prop.name)
                    self.ctx.lvl_inc()
                    self._print_has_data_function('child_ref._has_data()')
                    self.ctx.lvl_dec()
                    self.ctx.lvl_dec()
                    self.ctx.bline()
            else:
                if not prop.is_many:
                    self.ctx.writeln('if self.%s is not None:' % prop.name)
                    self.ctx.lvl_inc()
                    if isinstance(prop.property_type, Bits):
                        self._print_has_data_function(
                            'self.%s._has_data()' % prop.name)
                    else:
                        self.ctx.writeln('return True')
                    self.ctx.lvl_dec()
                    self.ctx.bline()
                else:
                    self.ctx.writeln('if self.%s is not None:' % prop.name)
                    self.ctx.lvl_inc()
                    self.ctx.writeln('for child in self.%s:' % prop.name)
                    self.ctx.lvl_inc()
                    self.ctx.writeln('if child is not None:')
                    self.ctx.lvl_inc()
                    if isinstance(prop.property_type, Bits):
                        self._print_has_data_function('child._has_data()')
                    else:
                        self.ctx.writeln('return True')
                    self.ctx.lvl_dec()
                    self.ctx.lvl_dec()
                    self.ctx.lvl_dec()
                    self.ctx.bline()

    def _print_has_data_function(self, if_condition, return_value='True'):
        self.ctx.writeln('if {0}:'.format(if_condition))
        self.ctx.lvl_inc()
        self.ctx.writeln('return ' + return_value)
        self.ctx.lvl_dec()

    def _print_has_data_functions_trailer(self, clazz):
        self.ctx.writeln('return False')
        self.ctx.lvl_dec()
        self.ctx.bline()
