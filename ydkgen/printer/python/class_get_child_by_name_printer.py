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

 prints python classes

"""


class ClassGetChildByNamePrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_class_get_child_by_name(self, clazz, children):
        self._print_class_get_child_header(clazz)
        self._print_class_get_child_body(children)
        self._print_class_get_child_trailer(clazz)

    def _print_class_get_child_header(self, clazz):
        self.ctx.writeln('def get_child_by_name(self, child_yang_name, segment_path):')
        self.ctx.lvl_inc()

    def _print_class_get_child_body(self, children):
        self._print_class_get_child_common()
        for child in children:
            self._print_class_get_child(child)
            self.ctx.bline()

    def _print_class_get_child_common(self):
        self.ctx.writeln('child = self._get_child_by_seg_name([child_yang_name, segment_path])')
        self.ctx.writeln('if child is not None:')
        self.ctx.lvl_inc()
        self.ctx.writeln('return child')
        self.ctx.lvl_dec()
        self.ctx.bline()

    def _print_class_get_child(self, child):
        self.ctx.writeln('if (child_yang_name == "%s"):' % (child.stmt.arg))
        self.ctx.lvl_inc()
        if child.is_many:
            self._print_class_get_child_many(child)
        else:
            self._print_class_get_child_unique(child)
        self.ctx.lvl_dec()

    def _print_class_get_child_many(self, child):
        self.ctx.writeln('for c in self.%s:' % child.name)
        self.ctx.lvl_inc()
        self.ctx.writeln('segment = c.get_segment_path()')
        self.ctx.writeln('if (segment_path == segment):')
        self.ctx.lvl_inc()
        self.ctx.writeln('return c')
        self.ctx.lvl_dec()
        self.ctx.lvl_dec()
        self.ctx.writeln('c = %s()' % (child.property_type.qn()))
        self.ctx.writeln('c.parent = self')
        self.ctx.writeln('local_reference_key = "ydk::seg::%s" % segment_path')
        self.ctx.writeln('self._local_refs[local_reference_key] = c')
        self.ctx.writeln('self.%s.append(c)' % child.name)
        self.ctx.writeln('return c')

    def _print_class_get_child_unique(self, child):
        self.ctx.writeln('if (self.%s is None):' % child.name)
        self.ctx.lvl_inc()
        self.ctx.writeln('self.%s = %s()' % (child.name, child.property_type.qn()))
        self.ctx.writeln('self.%s.parent = self' % child.name)
        self.ctx.writeln('self._children_name_map["%s"] = "%s"' % (child.name, child.stmt.arg))
        self.ctx.lvl_dec()
        self.ctx.writeln('return self.%s' % child.name)

    def _print_class_get_child_trailer(self, clazz):
        self.ctx.writeln('return None')
        self.ctx.lvl_dec()
        self.ctx.bline()
