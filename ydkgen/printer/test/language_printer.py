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
test_printer.py

Printer wrapper class provide language specific functionalities.
"""
from ydkgen.api_model import Package
from ydkgen.common import iscppkeyword


class LanguagePrinter(object):
    def __init__(self, ctx, language):
        self.ctx = ctx
        self.language = language

    def _write_end(self, line):
        if self.language == 'py':
            self._writeln(line)
        elif self.language == 'cpp':
            self._writeln('{};'.format(line))

    def _writeln(self, line):
        self.ctx.writeln(line)

    def _writelns(self, lines):
        self.ctx.writelns(lines)

    def _bline(self, num=1):
        while num > 0:
            self.ctx.bline()
            num -= 1

    def _write_comment(self, line):
        if self.language == 'py':
            self.ctx.writeln('# {}'.format(line))
        elif self.language == 'cpp':
            self.ctx.writeln('// {}'.format(line))

    def _write_comments(self, line):
        if self.language == 'py':
            self.ctx.writeln('"""')
            self.ctx.writeln(line)
            self.ctx.writeln('"""')
        elif self.language == 'cpp':
            self.ctx.writeln('/*')
            self.ctx.writeln(line)
            self.ctx.writeln('*/')

    def _lvl_inc(self):
        self.ctx.lvl_inc()

    def _lvl_dec(self):
        self.ctx.lvl_dec()

    def _get_qn(self, element):
        qn = ''
        if self.language == 'py':
            qn = element.qn()
        elif self.language == 'cpp':
            qn = element.fully_qualified_cpp_name()
        return qn

    def _get_crud_fmt(self, oper):
        if self.language == 'py':
            fmt = 'self.crud.{}(self.ncc, {{}})'.format(oper)
        elif self.language == 'cpp':
            if iscppkeyword(oper):
                oper = '{}_'.format(oper)
            fmt = 'm_crud.{}(*m_provider, *{{}})'.format(oper)
        return fmt

    @property
    def dec_fmt(self):
        fmt = ''
        if self.language == 'py':
            fmt = '{} = {}()'
        elif self.language == 'cpp':
            fmt = 'auto {} = std::make_unique<{}>()'
        return fmt

    @property
    def read_ret_fmt(self):
        fmt = ''
        if self.language == 'py':
            fmt = '{} = {}'
        elif self.language == 'cpp':
            fmt = 'auto {} = dynamic_cast<{}*>({}.get())'
        return fmt

    @property
    def leaflist_append_fmt(self):
        fmt = ''
        if self.language == 'py':
            fmt = '{}.append({})'
        elif self.language == 'cpp':
            fmt = '{}.append(std::move({}))'
        return fmt

    @property
    def append_fmt(self):
        fmt = ''
        if self.language == 'py':
            fmt = '{}.append({})'
        elif self.language == 'cpp':
            fmt = '{}.emplace_back(std::move({}))'
        return fmt

    @property
    def cpp_leaf_fmt(self):
        return '{} = {}.get()'

    @property
    def sep(self):
        sep = ''
        if self.language == 'py':
            sep = '.'
        elif self.language == 'cpp':
            sep = '->'
        return sep

    def get_assignment_fmt(self, path):
        fmt = ''
        if self.sep not in path and self.language == 'cpp':
            fmt = 'auto {} = {}'
        else:
            fmt = '{} = {}'
        return fmt

    @property
    def ref_fmt(self):
        fmt = ''
        if self.language == 'cpp':
            fmt = '{} = {}.get()'
        else:
            fmt = '{} = {}'
        return fmt

    @property
    def compare_fmt(self):
        fmt = ''
        if self.language == 'py':
            fmt = 'self.assertEqual({}, {})'
        elif self.language == 'cpp':
            fmt = 'BOOST_CHECK_EQUAL( {} == {}, true )'
        return fmt

    def _get_obj_name(self, clazz):
        obj_names = []
        while not isinstance(clazz, Package):
            obj_name = clazz.name.lower()
            obj_names.append(obj_name)
            clazz = clazz.owner
        return '_'.join(reversed(obj_names))
