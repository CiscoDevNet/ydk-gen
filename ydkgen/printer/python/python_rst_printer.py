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
        title = 'YDK Model API'
        self.ctx.writeln(title)
        self.ctx.writeln('=' * len(title))
        self.ctx.bline()
        self.ctx.bline()
        self.ctx.writeln('.. toctree::')
        self.ctx.lvl_inc()
        self.ctx.writeln(':maxdepth: 1\n')

        for package in packages:
            line = '%s <%s>' % (package.name, get_rst_file_name(package))
            self.ctx.writeln(line)

        self.ctx.lvl_dec()

    def _write_toctree(self, named_element):
        self.ctx.writeln('.. toctree::')
        self.ctx.lvl_inc()
        self.ctx.writeln(':maxdepth: 1\n')
	owned_elements = named_element.owned_elements
        owned_elements.reverse()
        for c in owned_elements:
            if isinstance(c, Class) or isinstance(c, Enum):
                #self.writeln(' :py:class:`%s <%s.%s>`'%(c.name, c.get_py_mod_name(), c.qn()))
                self.ctx.writeln('%s <%s>' % (c.name, get_rst_file_name(c)))
        self.ctx.bline()
        self.ctx.lvl_dec()

    def _get_class_hierarchy(self, clazz):
        parent_list = []
        parent = clazz
        while isinstance(parent, Class):
            parent_list.append(parent)
            parent = parent.owner

        clazz_hierarchy = 'Class Hierarchy \:'
        if len(parent_list) > 0:
            for parent in reversed(parent_list):
                if not clazz_hierarchy[-1:] == ':':
                    clazz_hierarchy += ' \>'

                clazz_hierarchy += ' :py:class:`%s <%s.%s>`' % (
                    parent.name, parent.get_py_mod_name(), parent.qn())

            return clazz_hierarchy
        else:
            return None

    def _print_class_rst(self, clazz):
        class_docstring = get_class_docstring(clazz)

        # Title
        line = clazz.name

        self.ctx.writeln(line)
        self.ctx.writeln('=' * len(line))
        self.ctx.bline()
        # TOC Tree
        self._write_toctree(clazz)

        self.ctx.bline()

        self.ctx.writeln('.. py:currentmodule:: %s' %
                         (clazz.get_py_mod_name()))
        self.ctx.bline()

        # Class Header
        #self.rst_printer.writeln('.. _%s:' % (get_sphinx_ref_label(clazz)))
        self.ctx.writeln('.. py:class:: %s' % (clazz.qn()))
        self.ctx.bline()
        self.ctx.lvl_inc()

        # Bases
        bases = [':class:`object`']
        if clazz.extends:
            for item in clazz.extends:
                bases.append(':class:`%s`' % (item.name))
        self.ctx.writeln('Bases: %s' % (', '.join(bases)))
        self.ctx.bline()

        # Class Hierarchy
        if not clazz.is_identity() and not clazz.is_grouping():
            clazz_hierarchy = self._get_class_hierarchy(clazz)
            if clazz_hierarchy is not None:
                self.ctx.writeln(clazz_hierarchy)
                self.ctx.bline()

        # Presence Container
        self.ctx.bline()
        if clazz.stmt.search_one('presence') is not None:
            line = """This class is a :ref:`presence class<presence-class>`"""
            self.ctx.writeln(line)
            self.ctx.bline()

        # Doc String
        if len(class_docstring) > 0:
            for line in class_docstring.split('\n'):
                if line.strip() != '':
                    self.ctx.writeln(line)
                    self.ctx.bline()

        if not clazz.is_identity() and not clazz.is_grouping():
            # Config Method
            self.ctx.writeln('.. method:: is_config()\n')
            self.ctx.lvl_inc()
            self.ctx.writeln("Returns True if this instance \
                represents config data else returns False")
            self.ctx.lvl_dec()

            self.ctx.bline()
        self.ctx.lvl_dec()

    def _print_package_rst(self, package):
        # Header
        line = package.name
        if package.stmt.keyword == 'module':
            line = line + ' module'
        self.ctx.writeln(line)
        self.ctx.writeln('=' * len(line))
        self.ctx.bline()
        self._write_toctree(package)
        self.ctx.bline()
        self.ctx.writeln('.. py:module:: %s.%s' %
                         (package.get_py_mod_name(), package.name))
        self.ctx.bline()
        self.ctx.writeln('%s' % package.name)
        self.ctx.bline()

        if package.comment is not None:
            self.ctx.writeln(package.comment)

    def _print_enum_rst(self, enumz):

        # Title
        line = enumz.name
        self.ctx.writeln(line)
        self.ctx.writeln('=' * len(line))
        self.ctx.bline()

        self.ctx.writeln('.. py:currentmodule:: %s' %
                         (enumz.get_py_mod_name()))
        self.ctx.bline()

        self.ctx.writeln('.. py:class:: %s' % (enumz.qn()))
        self.ctx.bline()
        self.ctx.lvl_inc()

        # Bases
        bases = [':class:`enum.Enum`']
        self.ctx.writeln('Bases: %s' % (', '.join(bases)))
        self.ctx.bline()

        enumz_docstring = get_enum_class_docstring(enumz)

        if len(enumz_docstring):
            for line in enumz_docstring.split('\n'):
                if line.strip() != '':
                    self.ctx.writeln(line)
                    self.ctx.bline()


        self.ctx.lvl_dec()
