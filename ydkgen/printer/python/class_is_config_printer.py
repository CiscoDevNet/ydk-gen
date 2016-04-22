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
class_is_config_printer.py 
 
 YANG model driven API, class emitter.
 
"""
from ydkgen.api_model import Class


class ClassIsConfigPrinter(object):

    def __init__(self, ctx, parent):
        self.ctx = ctx
        self.parent = parent

    def print_output(self, clazz):
        # prints a function for the entity clazz that
        # allows the external user to determine whether
        # this is a configuration clazz or not
        self._print_is_config_function_header(clazz)
        self._print_is_config_function_body(clazz)
        self._print_is_config_function_trailer(clazz)

    def _print_is_config_function_header(self, clazz):
        self.ctx.writeln('def is_config(self):')
        self.ctx.lvl_inc()
        self.ctx.writeln(
            "''' Returns True if this instance represents config data else returns False '''")

    def _print_is_config_function_body(self, clazz):
        if hasattr(clazz.stmt, 'i_config'):
            is_config = clazz.stmt.i_config
            if is_config is None:
                # do we have a parent
                if clazz.is_grouping() or isinstance(clazz.owner, Class):
                    self._print_is_config_function_non_config(clazz)
                else:
                    self.ctx.writeln('return True')
            else:
                self.ctx.writeln('return %s' % str(is_config))
        else:
            owner = clazz.owner
            if isinstance(owner, Class):
                self._print_is_config_function_non_config(clazz)

    def _print_is_config_function_non_config(self, clazz):
        self.ctx.writeln('if self.parent is None:')
        self.ctx.lvl_inc()
        self.ctx.writeln(
            "raise YPYError('Parent reference is needed to determine if entity has configuration data')")
        self.ctx.lvl_dec()
        self.ctx.writeln('return self.parent.is_config()')

    def _print_is_config_function_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.bline()
