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
printer.py

Meta class for test printer.
"""


class Printer(object):
    """Meta class for test printer."""

    def __init__(self, ctx, lang):
        self.ctx = ctx
        self.lang = lang

    def _write_end(self, line):
        """Write with language specific ending."""
        if self.lang == 'py':
            self._writeln(line)
        elif self.lang == 'cpp':
            self._writeln('{};'.format(line))

    def _writeln(self, line):
        """Write line with new line character."""
        self.ctx.writeln(line)

    def _writelns(self, lines):
        """Write lines."""
        self.ctx.writelns(lines)

    def _bline(self, num=1):
        """Write blank lines."""
        while num > 0:
            self.ctx.bline()
            num -= 1

    def _write_comment(self, line):
        """Write langauge specific line comment."""
        if self.lang == 'py':
            self.ctx.writeln('# {}'.format(line))
        elif self.lang == 'cpp':
            self.ctx.writeln('// {}'.format(line))

    def _write_comments(self, line):
        """Write langauge specific block comments."""
        if self.lang == 'py':
            self.ctx.writeln('"""')
            self.ctx.writeln(line)
            self.ctx.writeln('"""')
        elif self.lang == 'cpp':
            self.ctx.writeln('/*')
            self.ctx.writeln(line)
            self.ctx.writeln('*/')

    def _lvl_inc(self):
        """Indent."""
        self.ctx.lvl_inc()

    def _lvl_dec(self):
        """Unindent."""
        self.ctx.lvl_dec()
