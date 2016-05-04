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

    def _append(self, line):
        _line = '%s%s' % (self.ctx.get_indent(), line)
        self.lines.append(_line)

    def print_ydk_models_rst(self, packages):
        self.lines = []

        # Title
        title = 'YDK Model API'
        self._write_title(title)
        #TOC Tree
        self._write_toctree(packages, is_package=True)

        self.ctx.writelns(self.lines)
        del self.lines

    def _print_class_rst(self, clazz):
        self.lines = []
        # Title
        self._write_title(clazz.name)
        # TOC Tree
        self._write_toctree(clazz.owned_elements)
        self._append('\n')
        self._append('.. py:currentmodule:: %s\n' %
                         (clazz.get_py_mod_name()))

        self._append('.. py:class:: %s\n' % (clazz.qn()))
        self.ctx.lvl_inc()
        # Bases
        self._write_bases(clazz=clazz, is_class=True)
        # Class Hierarchy
        self._write_class_hierarchy(clazz)
        # Presence Container
        if clazz.stmt.search_one('presence') is not None:
            self._append('This class is a :ref:`presence class<presence-class>`\n')
        # Docstring
        self._write_class_docstring(clazz)
        # Config Method
        if not clazz.is_identity() and not clazz.is_grouping():
            self._write_class_config_method()
        self.ctx.lvl_dec()

        self.ctx.writelns(self.lines)
        del self.lines

    def _write_title(self, title):
        self._append(title)
        self._append('=' * len(title))
        self._append('\n')

    def _write_toctree(self, elements, is_package=False):
        self._append('.. toctree::')
        self.ctx.lvl_inc()
        self._append(':maxdepth: 1\n')

        if not is_package:
            elements.reverse()
            for elem in elements:
                if isinstance(elem, Class) or isinstance(elem, Enum):
                    self._append('%s <%s>' % (elem.name, get_rst_file_name(elem)))
        else:
            for elem in elements:
                self._append('%s <%s>' % (elem.name, get_rst_file_name(elem)))

        self._append('\n')
        self.ctx.lvl_dec()

    def _write_bases(self, clazz=None, is_class=False):
        bases = [':class:`%s`' % ('object' if is_class else 'enum.Enum')]
        if is_class and clazz.extends:
            for item in clazz.extends:
                bases.append(':class:`%s`' % (item.name))
        self._append('Bases: %s\n' % (', '.join(bases)))

    def _write_class_hierarchy(self, clazz):
        if not clazz.is_identity() and not clazz.is_grouping():
            clazz_hierarchy = self._get_class_hierarchy(clazz)
            if clazz_hierarchy is not None:
                self._append(clazz_hierarchy)
                self._append('\n\n')

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

    def _write_class_docstring(self, clazz):
        class_docstring = get_class_docstring(clazz)
        if len(class_docstring) > 0:
            for line in class_docstring.split('\n'):
                if line.strip() != '':
                    self._append(line)
                    self._append('\n')

    def _write_enum_docstring(self, enumz):
        enumz_docstring = get_enum_class_docstring(enumz)
        if len(enumz_docstring):
            for line in enumz_docstring.split('\n'):
                if line.strip() != '':
                    self._append(line)
                    self._append('\n')

    def _write_class_config_method(self):
        self._append('.. method:: is_config()\n')
        self.ctx.lvl_inc()
        self._append("Returns True if this instance \
            represents config data else returns False")
        self.ctx.lvl_dec()
        self._append('\n')

    def _print_package_rst(self, package):
        self.lines = []

        # Title
        line = package.name
        if package.stmt.keyword == 'module':
            line = '%s module' % line
        self._write_title(line)
        # TOC Tree
        self._write_toctree(package.owned_elements)
        self._append('\n')
        self._append('.. py:module:: %s.%s\n' %
                         (package.get_py_mod_name(), package.name))

        # Package Comment
        self._append('%s\n' % package.name)
        if package.comment is not None:
            self._append(package.comment)

        self.ctx.writelns(self.lines)

    def _print_enum_rst(self, enumz):
        self.lines = []
        # Title
        self._write_title(enumz.name)
        
        self._append('.. py:currentmodule:: %s\n' %
                         (enumz.get_py_mod_name()))
        self._append('.. py:class:: %s\n' % (enumz.qn()))
        self.ctx.lvl_inc()
        # Bases
        self._write_bases()
        # Docstring
        self._write_enum_docstring(enumz)

        self.ctx.lvl_dec()
        self.ctx.writelns(self.lines)
