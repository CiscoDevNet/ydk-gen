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

class ClassGetChildrenPrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_class_get_children(self, clazz, children):
        self.ctx.writeln('std::map<std::string, Entity*> & %s::get_children()' % clazz.qualified_cpp_name())
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        for child in children:
            self._print_class_get_child(child)
        self.ctx.writeln('return children;')
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_class_get_child(self, child):
        if child.is_many:
            self._print_class_get_child_many(child)
        else:
            self._print_class_get_child_unique(child)
        self.ctx.bline()

    def _print_class_get_child_many(self, child):
        self.ctx.writeln('for (auto const & c : %s)' % child.name)
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        self.ctx.writeln('if(children.find(c->get_segment_path()) == children.end())')
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        self.ctx.writeln('children[c->get_segment_path()] = c.get();')
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.lvl_dec()
        self.ctx.writeln('}')

    def _print_class_get_child_unique(self, child):
        self.ctx.writeln('if(children.find("%s") == children.end())' % child.stmt.arg)
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        self.ctx.writeln('if(%s != nullptr)' % child.name)
        self.ctx.writeln('{')
        self.ctx.lvl_inc()
        self.ctx.writeln('children["%s"] = %s.get();' % (child.stmt.arg, child.name))
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
