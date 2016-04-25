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
class_common_path_printer.py 
 
 Printer for the common path method.
 
"""
from ydkgen.api_model import Class
from ydkgen.common import get_module_name


class ClassCommonPathPrinter(object):

    """
        Print Common Class Path Printer

        :attribute ctx The printer context
        :attribute parent The parent printer object

    """

    def __init__(self, ctx, parent):
        self.ctx = ctx
        self.parent = parent

    def print_output(self, clazz):
        """
            Print the _common_path method for the clazz.

            :param `api_model.Class` clazz The class object.

        """
        self._print_common_path_functions_header(clazz)
        self._print_common_path_functions_body(clazz)
        self._print_common_path_functions_trailer(clazz)

    def _print_common_path_functions_header(self, clazz):
        self.ctx.writeln('@property')
        self.ctx.writeln('def _common_path(self):')
        self.ctx.lvl_inc()

    def _print_common_path_functions_body(self, clazz):
        owners = []
        owner_key_props = []
        current_owner = clazz.owner
        while current_owner != None and isinstance(current_owner, Class):
            owners.append(current_owner)
            owner_key_props.extend(current_owner.get_key_props())
            current_owner = current_owner.owner

        common_path = ''
        if len(owners) == 0 or len(owner_key_props) == 0:
            segments = ''
            for owner in reversed(owners):
                segments = "%s/%s:%s" % (segments,
                                         owner.module.arg, owner.stmt.arg)
            common_path = "return '%s" % segments
        else:
            self._print_common_path_validation_error(
                'parent', 'parent is not set . Cannot derive path.')
            common_path = "return self.parent._common_path +'"

        common_path = "%s/%s:%s" % (common_path,
                                    clazz.module.arg, clazz.stmt.arg)
        predicates = ''
        key_props = clazz.get_key_props()
        for key_prop in key_props:
            self._print_common_path_validation_error(
                key_prop.name, 'Key property {0} is None'.format(key_prop.name))
            predicates = "%s[%s:%s = ' + str(self.%s) + ']" % (predicates, get_module_name(key_prop.stmt),
                                                               key_prop.stmt.arg, key_prop.name)
        common_path = "%s%s'" % (common_path, predicates)

        self.ctx.bline()
        self.ctx.writeln(common_path)

    def _print_common_path_validation_error(self, prop, error_message):
        self.ctx.writeln('if self.{0} is None:'.format(prop))
        self.ctx.lvl_inc()
        self.ctx.writeln(
            "raise YPYDataValidationError('{0}')".format(error_message))
        self.ctx.lvl_dec()

    def _print_common_path_functions_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.bline()
