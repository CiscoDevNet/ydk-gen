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

from ydkgen.api_model import Bits, Class, Enum, Package, Property, get_properties
from ydkgen.common import get_rst_file_name, is_config_stmt
from ydkgen.printer.meta_data_util import (
    get_bits_class_docstring,
    get_class_bases,
    get_class_crossref_tag,
    get_class_docstring,
    get_class_tag,
    get_enum_class_docstring,
    get_langage_spec_tags
)


class DocPrinter(object):
    def __init__(self, ctx, language, bundle_name=None, bundle_version=None):
        self.ctx = ctx
        self.lang = language
        self.bundle_name = bundle_name
        self.bundle_version = bundle_version

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

    def print_table_of_contents(self, packages):
        self.lines = []
        title = '{0} bundle API'.format(self.bundle_name)
        description = '\nModel API documentation for the {0} bundle.\n Version: **{1}**.\n'.format(
            self.bundle_name, self.bundle_version)
        self._print_title(title)
        self._append(description)
        self._print_toctree(sorted(packages, key=attrgetter('name')), None, is_package=True)

        self.ctx.writelns(self.lines)
        del self.lines

    def _collect_all_augments(self, stmt, augments):
        for substmt in stmt.substmts:
            if substmt.keyword == 'augment':
                if hasattr(substmt.i_target_node, 'i_class') \
                   and substmt.i_target_node.i_class is not None:
                    augments.add(substmt.i_target_node.i_class)
            if len(substmt.substmts) > 0:
                self._collect_all_augments(substmt, augments)

    def _print_package_rst(self, package):
        self._print_header(package)
        # Body / Package Comment
        self._append('%s\n' % package.name)
        if package.revision is not None:
            self._append('Revision: {0}\n'.format(package.revision))
        if package.comment is not None:
            self._append(package.comment)

        # Adding augment references
        augments = set()
        self._collect_all_augments(package.stmt, augments)
        if len(augments) > 0:
            self.ctx.bline()
            if len(augments) == 1:
                for aug_class in augments:
                    line = '\nThis module contains augment: ' + get_class_crossref_tag(aug_class.qn(), aug_class, self.lang) + '\n'
                    self._append(line)
            else:
                line = '\nThis module contains the following augments:\n'
                self._append(line)
                for aug_class in augments:
                    line = '\t - ' + get_class_crossref_tag(aug_class.qn(), aug_class, self.lang) + '\n'
                    self._append(line)

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
        if self.lang != 'go':
            self._print_bases(clazz)
            self._print_class_hierarchy(clazz)
        if clazz.stmt.search_one('presence') is not None:
            self._append('This class is a :ref:`presence class<presence-class>`\n')
        if clazz.stmt.keyword != 'rpc':
            if is_config_stmt(clazz.stmt):
                self._append('This class represents configuration data.\n')
            else:
                self._append('This class represents state data.\n')
        else:
            self._append('This class defines parameters to the RPC operation\n')

        docstring = get_class_docstring(
            clazz, self.lang, identity_subclasses=self.identity_subclasses)
        self._print_docstring(clazz, docstring)
        self.ctx.lvl_dec()

    def _print_enum_rst(self, enumz):
        self._print_namespace(enumz)
        self._print_header(enumz)
        # Body
        self.ctx.lvl_inc()
        docstring = get_enum_class_docstring(enumz)
        self._print_docstring(enumz, docstring)
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
        import_stmt = None
        if isinstance(named_element, Package) and named_element.stmt.keyword == 'module':
            template = '%s module'
            if self.lang == 'go':
                template = 'package %s'
                import_stmt = 'import "github.com/CiscoDevNet/ydk-go/ydk/models/%s/%s"\n' % (
                    self.bundle_name, named_element.name)
            title = template % title
        self._print_title(title)

        if import_stmt is not None and self.lang == 'go':
            self._append('\n')
            self._append('.. code-block:: sh\n')
            self.ctx.lvl_inc()
            self._append(import_stmt)
            self.ctx.lvl_dec()

        # TOC Tree
        if not isinstance(named_element, Enum):
            if self.lang == 'py':
                if hasattr(named_element, 'properties'):
                    self._print_attributes(named_element.properties())
                    self._append('\n')
            self._print_toctree(named_element.owned_elements, named_element)

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
        if len(title) > 0 and self.lang != 'go':
            self._append('**{}**\n'.format(title))
        self._append('.. toctree::')
        self.ctx.lvl_inc()
        self._append(':maxdepth: 1\n')
        for elem in elements:
            self._append('%s <%s>' % (elem.name, get_rst_file_name(elem)))
        self._append('')
        self.ctx.lvl_dec()

    def _print_attribute_list(self, props):
        for prop in props:
            self._append('* :py:attr:`%s<%s.%s.%s>`' % (
                prop.name, prop.owner.get_py_mod_name(), prop.owner.qn(), prop.name))

    def _print_attributes(self, props):
        if len(props) > 0:
            keys = []
            leafs = []
            children = []
            for prop in props:
                if prop.stmt.keyword in ('leaf', 'anyxml'):
                    if prop.is_key():
                        keys.append(prop)
                    else:
                        leafs.append(prop)
                else:
                    children.append(prop)

            keys = sorted(keys, key=attrgetter('name'))
            leafs = sorted(leafs, key=attrgetter('name'))
            children = sorted(children, key=attrgetter('name'))

            if len(keys)>0:
                self._append('**{}**\n'.format('Keys'))
                self._print_attribute_list(keys)
            if len(leafs)>0:
                self._append('**{}**\n'.format('Leafs'))
                self._print_attribute_list(leafs)
            if len(children)>0:
                self._append('**{}**\n'.format('Children'))
                self._print_attribute_list(children)

    def _print_toctree(self, elements, owner, is_package=False):
        if not is_package:
            # Data Classes
            elements = sorted(elements, key=attrgetter('name'))

            data_classes = []
            rpc_classes = []
            bits_classes = []
            enum_classes = []
            idty_classes = []

            for elem in elements:
                if isinstance(elem, Enum):
                    enum_classes.append(elem)

                if self.lang == 'py' and isinstance(elem, Bits):
                    bits_classes.append(elem)

                if (isinstance(elem, Class)):
                    if elem.is_identity():
                        idty_classes.append(elem)
                    elif elem.is_rpc():
                        rpc_classes.append(elem)
                    else:
                        data_classes.append(elem)

            if self.lang != 'py' or isinstance(owner, Package):
                self._print_toctree_section(data_classes, 'Data Classes')
            self._print_toctree_section(rpc_classes, 'RPC Classes')
            self._print_toctree_section(bits_classes, 'Bits Classes')
            self._print_toctree_section(enum_classes, 'Enum Classes')
            self._print_toctree_section(idty_classes, 'Identity Classes')

        else:
            self._print_toctree_section(elements, '')

    def _print_namespace(self, clazz):
        if self.lang == 'cpp':
            self._append('\n.. cpp:namespace:: {0}\n'.format(clazz.get_package().name))

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
