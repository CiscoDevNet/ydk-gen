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
class_path_printer.py

 Printer for class methods

"""
from ydkgen.api_model import Package
from ydkgen.common import has_list_ancestor, is_top_level_class

class GetSegmentPathPrinter(object):

    """
        Print get_segment_path method

        :attribute ctx The printer context

    """

    def __init__(self, ctx):
        self.ctx = ctx

    def print_output(self, clazz):
        """
            Print the get_entity_path method for the clazz.

            :param `api_model.Class` clazz The class object.

        """
        self._print_get_ydk_segment_path_body(clazz)

    def _print_get_ydk_segment_path_body(self, clazz):
        path='"'
        if clazz.owner is not None:
            if isinstance(clazz.owner, Package):
                path+= clazz.owner.stmt.arg + ':'
            elif clazz.owner.stmt.i_module.arg != clazz.stmt.i_module.arg:
                path+=clazz.stmt.i_module.arg + ':'

        path+= clazz.stmt.arg
        path+='"'
        predicates = ''
        insert_token = ' + '

        key_props = clazz.get_key_props()
        for key_prop in key_props:
            predicates += insert_token
            
            predicates += '"['
            if key_prop.stmt.i_module.arg != clazz.stmt.i_module.arg:
                predicates += key_prop.stmt.i_module.arg
                predicates += ':'
            
            predicates += key_prop.stmt.arg + '='
            
            predicates += "'"
                
            predicates +='"'

            predicates += insert_token
            
            predicates += ('str(self.%s)') % key_prop.name + insert_token

            predicates += '"'
                
            predicates += "'"
                
            predicates += ']"'

        path = '%s%s' % (path, predicates)


        self.ctx.writeln("self._segment_path = lambda: %s" % path)

    def _print_get_ydk_segment_path_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.bline()


class GetAbsolutePathPrinter(object):

    """

        :attribute ctx The printer context

    """

    def __init__(self, ctx):
        self.ctx = ctx

    def print_output(self, clazz, leafs):
        """

            :param `api_model.Class` clazz The class object.

        """
        if not is_top_level_class(clazz) and not has_list_ancestor(clazz):
            self._print_absolute_path_body(clazz, leafs)

    def _print_absolute_path_body(self, clazz, leafs):
        parents = []
        p = clazz
        while p is not None and not isinstance(p, Package):
            if p != clazz:
                parents.append(p)
            p = p.owner

        parents.reverse()
        path = ''
        for p in parents:
            if len(path) == 0:
                path += p.owner.stmt.arg
                path += ':'
                path += p.stmt.arg
            else:
                path += '/'
                if p.stmt.i_module.arg != p.owner.stmt.i_module.arg:
                    path += p.stmt.i_module.arg
                    path += ':'
                path += p.stmt.arg
        slash = ""
        if len(path) > 0:
            slash = "/"
        path = "%s%s" % (path, slash)
        self.ctx.writeln('self._absolute_path = lambda: "%s%%s" %% self._segment_path()' % path)
