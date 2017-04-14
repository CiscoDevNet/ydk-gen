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
doc_printer.py

Print rst documents for the generated Python api
"""
from operator import attrgetter

from ydkgen.api_model import Bits, Class, Enum, Package
from ydkgen.common import get_rst_file_name, is_config_stmt
from ydkgen.printer.meta_data_util import (get_bits_class_docstring,
                                           get_class_bases,
                                           get_class_crossref_tag,
                                           get_class_docstring,
                                           get_class_tag,
                                           get_enum_class_docstring,
                                           get_langage_spec_tags
                                           )


class DocPrinter(object):
    def __init__(self, ctx, language):
        self.ctx = ctx
        self.lang = language

    def print_module_documentation(self, named_element, identity_subclasses):
        self.identity_subclasses = identity_subclasses
        self.lines = []

        if isinstance(named_element, Bits):
            self._print_bits_rst(named_element)
        elif isinstance(named_element, Enum):
            self._print_enum_rst(named_element)
        elif isinstance(named_element, Class):
            self._print_class_rst(named_element)
        elif isinstance(named_element, Package):
            self._print_package_rst(named_element)
        else:
            raise EmitError('Unrecognized named_element')

        self.ctx.writelns(self.lines)
        del self.lines

    def print_table_of_contents(self, packages, bundle_name, bundle_version):
        self.lines = []
        title = '{0} bundle API'.format(bundle_name)
        description = '\nModel API documentation for the {0} bundle.\n Version: **{1}**.\n'.format(bundle_name, bundle_version)
        self._print_title(title)
        self._append(description)
        self._print_toctree(sorted(packages, key=attrgetter('name')), is_package=True)

        self.ctx.writelns(self.lines)
        del self.lines

    def _print_package_rst(self, package):
        self._print_header(package)
        # Body / Package Comment
        self._append('%s\n' % package.name)
        if package.revision is not None:
            self._append('Revision: {0}\n'.format(package.revision))
        if package.comment is not None:
            self._append(package.comment)

    def _print_bits_rst(self, bitz):
        if self.lang != 'py':
            return
        self._print_header(bitz)
        self.ctx.lvl_inc()
        self._print_docstring(bitz, get_bits_class_docstring(bitz))
        self.ctx.lvl_dec()

    def _print_class_rst(self, clazz):
        self._print_namespace(clazz)
        self._print_header(clazz)
        # Body
        self.ctx.lvl_inc()
        self._print_bases(clazz)
        self._print_class_hierarchy(clazz)
        if clazz.stmt.search_one('presence') is not None:
            self._append('This class is a :ref:`presence class<presence-class>`\n')
        if clazz.stmt.arg != 'rpc':
            if is_config_stmt(clazz.stmt):
                self._append('This class represents configuration data.\n')
            else:
                self._append('This class represents state data.\n')
        self._print_docstring(clazz, get_class_docstring(
            clazz, self.lang, identity_subclasses=self.identity_subclasses))
        self.ctx.lvl_dec()

    def _print_enum_rst(self, enumz):
        self._print_namespace(enumz)
        self._print_header(enumz)
        # Body
        self.ctx.lvl_inc()
        self._print_bases(enumz)
        self._print_docstring(enumz, get_enum_class_docstring(enumz))
        self.ctx.lvl_dec()

    def _append(self, line):
        _line = '%s%s' % (self.ctx.get_indent(), line)
        self.lines.append(_line)

    def _extend(self, lines):
        for line in lines:
            self._append(line)

    def _print_header(self, named_element):
        # Title
        title = named_element.name
        if isinstance(named_element, Package) and named_element.stmt.keyword == 'module':
            title = '%s module' % title
        self._print_title(title)

        # TOC Tree
        if not isinstance(named_element, Enum):
            self._print_toctree(named_element.owned_elements)

        # Tagging
        if not isinstance(named_element, Package):
            tags = get_langage_spec_tags(named_element, self.lang)
            tags.append(get_class_tag(named_element, self.lang))
            self._extend(tags)

    def _print_title(self, title):
        self._append(title)
        self._append('=' * len(title))
        self._append('\n')

    def _print_toctree_section(self, elements, title):
        if len(elements) == 0:
            return
        if len(title) > 0:
            self._append('**{}**\n'.format(title))
        self._append('.. toctree::')
        self.ctx.lvl_inc()
        self._append(':maxdepth: 1\n')
        for elem in elements:
            self._append('%s <%s>' % (elem.name, get_rst_file_name(elem)))
        self._append('')
        self.ctx.lvl_dec()

    def _print_toctree(self, elements, is_package=False):
        if not is_package:
            elements.reverse()
            data_classes = [elem for elem in elements if (isinstance(elem, Class) and not elem.is_identity())]
            self._print_toctree_section(data_classes, 'Data Classes')

            type_classes = [elem for elem in elements if any((isinstance(elem, Class) and elem.is_identity(),
                                                    isinstance(elem, Enum),
                                                    isinstance(elem, Bits) and self.lang == 'py'))]
            self._print_toctree_section(type_classes, 'Type Classes')
        else:
            self._print_toctree_section(elements, '')

    def _print_namespace(self, clazz):
        if self.lang == 'cpp':
            self._append('\n.. cpp:namespace:: ydk::{0}\n'.format(clazz.get_package().name))

    def _print_bases(self, clazz):
        bases = get_class_bases(clazz, self.lang)
        if bases:
            self._append('Bases: %s\n' % (', '.join(bases)))

    def _print_class_hierarchy(self, clazz):
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

                tag = get_class_crossref_tag(parent.name, parent, self.lang)
                clazz_hierarchy.append(tag)

            return ''.join(clazz_hierarchy)
        else:
            return None

    def _print_docstring(self, named_element, docstring):
        if(len(docstring) > 0):
            for line in docstring.split('\n'):
                if line.strip() != '':
                    self._append(line)
                    self._append('\n')
