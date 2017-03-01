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
 import_test_printer.py 
 
 YANG model driven API, python emitter.

"""
import abc
from ydkgen.api_model import Class


class _Stack:

    def __init__(self):
        self.items = []

    def isEmpty(self):
        return self.items == []

    def push(self, item):
        self.items.append(item)

    def pop(self):
        return self.items.pop()

    def peek(self):
        return self.items[len(self.items) - 1]

    def size(self):
        return len(self.items)


class FilePrinter(object):
    def __init__(self, ctx):
        self.ctx=ctx
        self._start_tab = _Stack()

    def _start_tab_leak_check(self):
        self._start_tab.push(self.ctx.lvl)

    def _check_tab_leak(self):
        end_tab = self.ctx.lvl
        if self._start_tab.pop() != end_tab:
            raise Exception('Tab leak !!!')

    def print_output(self,packages):
        self._start_tab_leak_check()
        self.print_header(packages)
        self.print_body(packages)
        self.print_trailer(packages)
        self._check_tab_leak()

    def print_header(self,packages):
        pass

    @abc.abstractmethod
    def print_body(self, packages):
        pass

    def print_trailer(self, packages):
        pass

    def is_derived_identity(self, package, identity):
        for ne in package.owned_elements:
            if isinstance(ne, Class) and ne.is_identity():
                for ext in ne.extends:
                    if ext == identity:
                        return True
        return False

    def _print_include_guard_header(self, include_guard):
        self.ctx.writeln('#ifndef {0}'.format(include_guard))
        self.ctx.writeln('#define {0}'.format(include_guard))
        self.ctx.bline()

    def _print_include_guard_trailer(self, include_guard):
        self.ctx.bline()
        self.ctx.writeln('#endif /* {0} */'.format(include_guard))
