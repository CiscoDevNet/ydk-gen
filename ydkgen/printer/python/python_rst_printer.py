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
from pyang.error import EmitError

"""
python_rst_printer.py

Print rst documents for the generated Python api
"""

from ydkgen.api_model import Class, Enum, Package
from ydkgen.common import get_rst_file_name
from ydkgen.printer.meta_data_util import get_class_docstring, get_enum_class_docstring


class PythonRstPrinter(object):

    def __init__(self, ctx, parent):
        self.ctx = ctx
        self.parent = parent

    def print_rst_file(self, named_element):
        if isinstance(named_element, Enum):
            self._print_enum_rst(named_element)
        elif isinstance(named_element, Class):
            self._print_class_rst(named_element)
        elif isinstance(named_element, Package):
            self._print_package_rst(named_element)
        else:
            raise EmitError('Unrecognized named_element')

    def print_ydk_models_rst(self, packages):
        lines = []
        title = 'YDK Model API'
        lines.append(title)
        lines.append('=' * len(title))
        lines.append('')
        lines.append('.. toctree::')

        self.ctx.lvl_inc()
        lines.append('%s:maxdepth: 1\n' % self.ctx.get_indent())

        for package in packages:
            line = '%s <%s>' % (package.name, get_rst_file_name(package))
            lines.append('%s%s' % (self.ctx.get_indent(), line))

        self.ctx.lvl_dec()
        self.ctx.writelns(lines)

    def _write_toctree(self, named_element, lines):
        # lines = []
        lines.append('%s.. toctree::' % self.ctx.get_indent())
        
        self.ctx.lvl_inc()
        lines.append('%s:maxdepth: 1\n' % self.ctx.get_indent())

        owned_elements = named_element.owned_elements
        owned_elements.reverse()
        
        for elem in owned_elements:
            if isinstance(elem, Class) or isinstance(elem, Enum):
                lines.append('%s%s <%s>' % (self.ctx.get_indent(), elem.name, get_rst_file_name(elem)))
            # if isinstance(elem, Enum):
                # from pdb import set_trace; set_trace()
                # print 'hi'
        lines.append('\n')
        self.ctx.lvl_dec()
        # self.ctx.writelns(lines)

    def _get_class_hierarchy(self, clazz):
        parent_list = []
        parent = clazz
        while isinstance(parent, Class):
            parent_list.append(parent)
            parent = parent.owner

        clazz_hierarchy = ['Class Hierarchy \:']
        if len(parent_list) > 0:
            for parent in reversed(parent_list):
                if not clazz_hierarchy[0][-1:] == ':':
                    clazz_hierarchy.append(' \>')

                clazz_hierarchy.append(' :py:class:`%s <%s.%s>`' % (
                    parent.name, parent.get_py_mod_name(), parent.qn()))

            return ''.join(clazz_hierarchy)
        else:
            return None

    def _print_class_rst(self, clazz):
        class_docstring = get_class_docstring(clazz)

        # Title
        lines = []
        lines.append(clazz.name)
        lines.append('=' * len(clazz.name))
        lines.append('\n')
        # self.ctx.writelns(lines)

        # TOC Tree
        self._write_toctree(clazz, lines)

        # lines = []
        lines.append('\n')
        lines.append('.. py:currentmodule:: %s' %
                         (clazz.get_py_mod_name()))
        lines.append('\n')

        # Class Header
        lines.append('.. py:class:: %s' % (clazz.qn()))
        lines.append('\n')

        # self.ctx.writelns(lines)
        self.ctx.lvl_inc()

        # Bases
        # lines = []
        bases = ['%s:class:`object`' % self.ctx.get_indent()]
        if clazz.extends:
            for item in clazz.extends:
                bases.append('%s:class:`%s`' % (self.ctx.get_indent(), item.name))
        lines.append('%sBases: %s' % (self.ctx.get_indent(), ', '.join(bases)))
        lines.append('\n')

        # Class Hierarchy
        if not clazz.is_identity() and not clazz.is_grouping():
            clazz_hierarchy = self._get_class_hierarchy(clazz)
            if clazz_hierarchy is not None:
                lines.append('%s%s' % (self.ctx.get_indent(), clazz_hierarchy))
                lines.append('\n')

        # Presence Container
        lines.append('\n')
        if clazz.stmt.search_one('presence') is not None:
            line = """This class is a :ref:`presence class<presence-class>`"""
            lines.append('%s%s' % (self.ctx.get_indent(), line))
            lines.append('\n')

        # Doc String
        if len(class_docstring) > 0:
            for line in class_docstring.split('\n'):
                if line.strip() != '':
                    lines.append('%s%s' % (self.ctx.get_indent(), line))
                    lines.append('\n')

        if not clazz.is_identity() and not clazz.is_grouping():
            # Config Method
            lines.append('%s.. method:: is_config()\n' % (self.ctx.get_indent()))
            self.ctx.lvl_inc()
            lines.append("%sReturns True if this instance \
                represents config data else returns False" % (self.ctx.get_indent()))
            self.ctx.lvl_dec()
            lines.append('\n')

        self.ctx.lvl_dec()
        self.ctx.writelns(lines)

    def _print_package_rst(self, package):
        # Header
        lines = []
        line = package.name
        if package.stmt.keyword == 'module':
            line = '%s module' % line
        lines.append(line)
        lines.append('=' * len(line))
        lines.append('\n')
        
        self._write_toctree(package, lines)

        lines.append('\n')
        lines.append('.. py:module:: %s.%s' %
                         (package.get_py_mod_name(), package.name))
        lines.append('\n')
        lines.append('%s' % package.name)
        lines.append('\n')

        if package.comment is not None:
            lines.append(package.comment)

        self.ctx.writelns(lines)

    def _print_enum_rst(self, enumz):
        # from pdb import set_trace; set_trace()
        lines = []
        # Title
        line = enumz.name
        lines.append(line)
        lines.append('=' * len(line))
        lines.append('\n')

        lines.append('.. py:currentmodule:: %s' %
                         (enumz.get_py_mod_name()))
        lines.append('\n')

        lines.append('.. py:class:: %s' % (enumz.qn()))
        lines.append('\n')

        self.ctx.lvl_inc()

        # Bases
        bases = ['%s:class:`enum.Enum`' % (self.ctx.get_indent())]
        lines.append('%sBases: %s' % (self.ctx.get_indent(), ', '.join(bases)))
        lines.append('\n')

        enumz_docstring = get_enum_class_docstring(enumz)

        if len(enumz_docstring):
            for line in enumz_docstring.split('\n'):
                if line.strip() != '':
                    lines.append('%s%s' % (self.ctx.get_indent(), line))
                    lines.append('\n')

        self.ctx.lvl_dec()
        self.ctx.writelns(lines)
