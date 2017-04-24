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
 printer_context.py

 YANG model driven API, common definitions.
"""


class PrinterContext(object):

    """
        Print Context.

        Used to encapsulate information needed by the printers.
    """

    def __init__(self):
        self.fd = None
        self.lvl = 0
        # internal
        self.all_classes = []
        self.class_list = []
        self.class_name = ''
        self.class_stack = []
        self.comment = False
        self.contact = ''
        self.depth = 0
        self.env = None
        self.first = True
        self.group_list = []
        self.groupings = {}
        self.idx = 0
        self.idx_stack = []
        self.import_enum = []
        self.imports = {}
        self.loader = None
        self.local_group_list = []
        self.meta = True
        self.module = None
        self.module_name = ''
        self.namespace = ''
        self.ns = []
        self.organization = ''
        self.prefix = ''
        self.printer = None
        self.revision = ''
        self.rpc = []
        self.tab_size = 4
        self.target = ''
        self.templates = None
        self.types = []
        self.uses = []

    def str(self, msg):
        self.fd.write(msg)

    def tab(self, lvl=None):
        if lvl is None:
            lvl = self.lvl
        if lvl > 0:
            fmt = '%%%ds' % (lvl * self.tab_size)
        else:
            return ''
        return fmt % ' '

    def write(self, msg):
        if self.lvl > 0:
            fmt = '%%%ds' % (self.lvl * self.tab_size)
            self.fd.write(fmt % ' ')
        self.fd.write(msg)

    def writeln(self, msg, tab=0):
        if self.lvl + tab > 0:
            fmt = '%%%ds' % ((self.lvl + tab) * self.tab_size)
            self.fd.write(fmt % ' ')
        self.fd.write(msg)
        self.fd.write('\n')

    def writelns(self, lines, tab=0):
        indent = ''
        if self.lvl + tab > 0:
            indent = ' ' * ((self.lvl + tab) * self.tab_size)

        fmt = '\n%s' % indent
        lines = fmt.join(lines)
        self.fd.write('%s%s' % (indent, lines))

    def get_indent(self):
        indent = ''
        if self.lvl > 0:
            indent = ' ' * (self.lvl * self.tab_size)
        return indent

    def bline(self):
        self.fd.write('\n')

    def lvl_inc(self, tab=1):
        self.lvl += tab

    def lvl_dec(self, tab=1):
        self.lvl -= tab

    def push_idx(self):
        self.idx_stack.append(self.idx)

    def pop_idx(self):
        self.idx = self.idx_stack.pop()

    def push_class(self):
        self.class_stack.append(self.class_list)
        self.class_list = []

    def pop_class(self):
        self.class_list = self.class_stack.pop()
