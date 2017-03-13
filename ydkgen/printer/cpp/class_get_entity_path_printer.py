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

class GetEntityPathPrinter(object):

    """
        Print get_entity_path method

        :attribute ctx The printer context

    """

    def __init__(self, ctx):
        self.ctx = ctx

    def print_output(self, clazz, leafs):
        """
            Print the get_entity_path method for the clazz.

            :param `api_model.Class` clazz The class object.

        """
        self._print_get_entity_path_header(clazz)
        self._print_get_entity_path_body(clazz, leafs)
        self._print_get_entity_path_trailer(clazz)

    def _print_get_entity_path_header(self, clazz):
        self.ctx.writeln('EntityPath %s::get_entity_path(Entity* ancestor) const' % clazz.qualified_cpp_name())
        self.ctx.writeln('{')
        self.ctx.lvl_inc()

    def _is_parent_needed_for_abs_path(self, clazz):
        c = clazz.owner
        parents = []

        while c is not None and not isinstance(c,Package):
            parents.append(c)
            c = c.owner

        for p in parents:
            key_props = p.get_key_props()
            if key_props is not None and len(key_props) > 0:
                return True
        return False

    def _print_get_entity_path_body(self, clazz, leafs):
        #can this class handle a nullptr
        #in which case it's absolute path can be determined

        self.ctx.writeln('std::ostringstream path_buffer;')

        if clazz.owner is not None and isinstance(clazz.owner, Package):
            # the ancestor is irrelevant here
            self.ctx.writeln('if (ancestor != nullptr)')
            self.ctx.writeln('{')
            self.ctx.lvl_inc()
            self.ctx.writeln('throw(YCPPInvalidArgumentError{"ancestor has to be nullptr for top-level node"});')
            self.ctx.lvl_dec()
            self.ctx.writeln('}')
            self.ctx.bline()
            self.ctx.writeln('path_buffer << get_segment_path();');
        else:
            #this is not a top level
            # is nullptr a valid parameter here
            self.ctx.writeln('if (ancestor == nullptr)')
            self.ctx.writeln('{')
            self.ctx.lvl_inc()

            if self._is_parent_needed_for_abs_path(clazz):
                self.ctx.writeln('throw(YCPPInvalidArgumentError{"ancestor cannot be nullptr as one of the ancestors is a list"});')
            else:
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
                self.ctx.writeln('path_buffer << "%s%s" << get_segment_path();' % (path, slash))

            self.ctx.lvl_dec()
            self.ctx.writeln('}')
            self.ctx.writeln('else')
            self.ctx.writeln('{')
            self.ctx.lvl_inc()

            self.ctx.writeln("path_buffer << get_relative_entity_path(this, ancestor, path_buffer.str());")

            self.ctx.lvl_dec()
            self.ctx.writeln('}')
            self.ctx.bline()

        self.ctx.writeln('std::vector<std::pair<std::string, LeafData> > leaf_name_data {};')
        self.ctx.bline()
        for prop in leafs:
            if not prop.is_many:
                self.ctx.writeln('if (%s.is_set || is_set(%s.operation)) leaf_name_data.push_back(%s.get_name_leafdata());' % (prop.name, prop.name, prop.name))
        self._print_get_entity_path_leaflists(leafs)
        self.ctx.bline()
        self.ctx.writeln('EntityPath entity_path {path_buffer.str(), leaf_name_data};')
        self.ctx.writeln('return entity_path;')

    def _print_get_entity_path_leaflists(self, leafs):
        leaf_lists = [leaf for leaf in leafs if leaf.is_many]
        self.ctx.bline()
        for leaf in leaf_lists:
            self.ctx.writeln('auto %s_name_datas = %s.get_name_leafdata();' % (leaf.name, leaf.name))
            self.ctx.writeln('leaf_name_data.insert(leaf_name_data.end(), %s_name_datas.begin(), %s_name_datas.end());' % (leaf.name, leaf.name))

    def _print_get_entity_path_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.bline()
        self.ctx.writeln('}')
        self.ctx.bline()

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
        self._print_get_ydk_segment_path_header(clazz)
        self._print_get_ydk_segment_path_body(clazz)
        self._print_get_ydk_segment_path_trailer(clazz)

    def _print_get_ydk_segment_path_header(self, clazz):
        self.ctx.writeln('std::string %s::get_segment_path() const' % clazz.qualified_cpp_name())
        self.ctx.writeln('{')
        self.ctx.lvl_inc()


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
        insert_token = ' <<'
        key_props = clazz.get_key_props()
        for key_prop in key_props:
            predicates += insert_token
            predicates += '"['
            if key_prop.stmt.i_module.arg != clazz.stmt.i_module.arg:
                predicates += key_prop.stmt.i_module.arg
                predicates += ':'
            predicates += key_prop.stmt.arg + '='
            predicates+= "'"
            predicates+='"'
            predicates += insert_token
            predicates += ('%s') % key_prop.name + insert_token
            predicates += '"'
            predicates += "'"
            predicates += ']"'

        self.ctx.writeln('std::ostringstream path_buffer;')
        self.ctx.writeln('path_buffer << %s%s;' % (path, predicates))
        self.ctx.bline()
        self.ctx.writeln('return path_buffer.str();')

    def _print_get_ydk_segment_path_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.bline()
        self.ctx.writeln('}')
        self.ctx.bline()

