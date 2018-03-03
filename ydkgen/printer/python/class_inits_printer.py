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
class_inits_printer.py

 __init__ printer

"""
from pyang.types import PathTypeSpec
from ydkgen.api_model import Bits, Class, Package, DataType, Enum, snake_case
from ydkgen.common import get_module_name, has_list_ancestor, is_top_level_class, get_qualified_yang_name, get_unclashed_name
from .class_get_entity_path_printer import GetAbsolutePathPrinter, GetSegmentPathPrinter


def get_leafs(clazz):
    leafs = []
    for child in clazz.owned_elements:
        if child.stmt.keyword in ('leaf', 'anyxml'):
            leafs.append(child)
    return leafs


def get_leaf_lists(clazz):
    leaf_lists = []
    for child in clazz.owned_elements:
        if child.stmt.keyword == 'leaf-list':
            leaf_lists.append(child)
    return leaf_lists


def get_lists(clazz):
    lists = []
    for child in clazz.owned_elements:
        if child.stmt.keyword == 'list':
            lists.append(child)
    return lists


def get_child_container_classes(clazz, one_class_per_module):
    m = []
    for prop in clazz.properties():
        if prop.stmt.keyword == 'container':
            if one_class_per_module:
                m.append('"%s" : ("%s", %s.%s)'%(get_qualified_yang_name(prop.property_type), prop.name, clazz.name, prop.property_type.name))
            else:
                m.append('"%s" : ("%s", %s)'%(get_qualified_yang_name(prop.property_type), prop.name, prop.property_type.qn()))
    return '%s' % (', '.join(m))


def get_child_list_classes(clazz, one_class_per_module):
    m = []
    for prop in clazz.properties():
        if prop.stmt.keyword == 'list':
            if one_class_per_module:
                m.append('"%s" : ("%s", %s.%s)' % (get_qualified_yang_name(prop.property_type), prop.name, clazz.name, prop.property_type.name))
            else:
                m.append('"%s" : ("%s", %s)' % (get_qualified_yang_name(prop.property_type), prop.name, prop.property_type.qn()))
    return '%s' % (', '.join(m))


class ClassInitsPrinter(object):

    def __init__(self, ctx, module_namespace_lookup, one_class_per_module):
        self.ctx = ctx
        self.module_namespace_lookup = module_namespace_lookup
        self.one_class_per_module = one_class_per_module

    def print_output(self, clazz, leafs, children):
        self._print_class_inits_header(clazz)
        self._print_class_inits_body(clazz, leafs, children)
        self._print_class_inits_trailer(clazz)

    def _print_class_inits_header(self, clazz):
        self.ctx.writeln('def __init__(self):')
        self.ctx.lvl_inc()

    def _print_class_inits_body(self, clazz, leafs, children):
        if clazz.is_identity():
            module_name = get_module_name(clazz.stmt)
            namespace = self.module_namespace_lookup[module_name]
            line = 'super(%s, self).__init__("%s", "%s", "%s:%s")' % (clazz.name, namespace, module_name, module_name, clazz.stmt.arg)
            self.ctx.writeln(line)
        else:
            if self.one_class_per_module:
                self.ctx.writeln('super(%s, self).__init__()' % clazz.name)
            else:
                self.ctx.writeln('super(%s, self).__init__()' % clazz.qn())
            if clazz.owner is not None and isinstance(clazz.owner, Package):
                self.ctx.writeln('self._top_entity = None')
            self.ctx.bline()
            if self.one_class_per_module:
                self._print_children_imports(clazz, children)
            self.ctx.writeln('self.yang_name = "%s"' % clazz.stmt.arg)
            self.ctx.writeln('self.yang_parent_name = "%s"' % clazz.owner.stmt.arg)
            self.ctx.writeln('self.is_top_level_class = %s' % ('True' if is_top_level_class(clazz) else 'False'))
            self.ctx.writeln('self.has_list_ancestor = %s' % ('True' if has_list_ancestor(clazz) else 'False'))
            self.ctx.writeln(
                'self.ylist_key_names = [%s]' % (','.join(["'%s'" % key.name for key in clazz.get_key_props()])))
            self.ctx.writeln('self._child_container_classes = {%s}' % (get_child_container_classes(clazz, self.one_class_per_module)))
            self.ctx.writeln('self._child_list_classes = {%s}' % (get_child_list_classes(clazz, self.one_class_per_module)))
            if clazz.stmt.search_one('presence') is not None:
                self.ctx.writeln('self.is_presence_container = True')
            self._print_init_leafs_and_leaflists(clazz, leafs)
            self._print_init_children(children)
            self._print_init_lists(clazz)
            self._print_class_segment_path(clazz)
            self._print_class_absolute_path(clazz, leafs)

    def _print_init_leafs_and_leaflists(self, clazz, leafs):
        if len(leafs) == 0:
            self.ctx.writeln('self._leafs = {}')
            return

        self.ctx.writeln('self._leafs = {')
        self.ctx.lvl_inc()
        declarations = []

        for prop in leafs:
            leaf_name = prop.name
            ytype = self._get_type_name(prop.property_type)

            leaf_type = 'YLeaf'
            declaration_stmt =      'self.%s = None' % leaf_name
            if prop.is_many:
                leaf_type = 'YLeafList'
                declaration_stmt =  'self.%s = []' % leaf_name
            elif isinstance(prop.property_type, Bits):
                declaration_stmt =  'self.%s = Bits()' % leaf_name

            yname = prop.stmt.arg
            if all((prop.stmt.top.arg != clazz.stmt.top.arg,
                    hasattr(prop.stmt.top, 'i_aug_targets') and
                    clazz.stmt.top in prop.stmt.top.i_aug_targets)):
                yname = ':'.join([prop.stmt.top.arg, prop.stmt.arg])

            self.ctx.writeln("'%s' : %s(YType.%s, '%s')," % (leaf_name, leaf_type, ytype, yname))
            declarations.append(declaration_stmt)

        self.ctx.lvl_dec()
        self.ctx.writeln('}')

        for line in declarations:
            self.ctx.writeln(line)

    def _print_children_imports(self, clazz, children):
        for child in children:
            self.ctx.writeln('from .%s import %s' % (get_unclashed_name(child.property_type, child.property_type.iskeyword), get_unclashed_name(child.property_type, child.property_type.iskeyword)))
            self.ctx.writeln('self.__class__.%s = %s.%s' % (child.property_type.name, get_unclashed_name(child.property_type, child.property_type.iskeyword), child.property_type.name))
        self.ctx.bline()

    def _print_init_children(self, children):
        for child in children:
            if not child.is_many:
                self.ctx.bline()
                if (child.stmt.search_one('presence') is None):
                    if self.one_class_per_module:
                        self.ctx.writeln('self.%s = %s.%s()' % (child.name, get_unclashed_name(child.property_type, child.property_type.iskeyword), child.property_type.name))
                    else:
                        self.ctx.writeln('self.%s = %s()' % (child.name, child.property_type.qn()))
                    self.ctx.writeln('self.%s.parent = self' % child.name)
                else:
                    self.ctx.writeln('self.%s = None' % (child.name))
                self.ctx.writeln('self._children_name_map["%s"] = "%s"' % (child.name, get_qualified_yang_name(child)))
                self.ctx.writeln('self._children_yang_names.add("%s")' % (get_qualified_yang_name(child)))

    def _print_init_lists(self, clazz):
        if clazz.is_identity() and len(clazz.extends) == 0:
            return

        output = []
        for prop in clazz.properties():
            if (prop.is_many and
                isinstance(prop.property_type, Class) and
                    not prop.property_type.is_identity()):
                output.append('self.%s = YList(self)' % prop.name)
        if len(output) > 0:
            self.ctx.bline()
            self.ctx.writelns(output)
            self.ctx.bline()

    def _print_class_segment_path(self, clazz):
        GetSegmentPathPrinter(self.ctx).print_output(clazz)

    def _print_class_absolute_path(self, clazz, leafs):
        GetAbsolutePathPrinter(self.ctx).print_output(clazz, leafs)

    def _print_class_inits_trailer(self, clazz):
        self.ctx.lvl_dec()
        self.ctx.bline()

    def _get_type_name(self, prop_type):
        if prop_type.name == 'string':
            return 'str'
        elif prop_type.name == 'leafref':
            return 'str'
        elif prop_type.name == 'decimal64':
            return 'str'
        elif prop_type.name == 'union':
            return 'str'
        elif prop_type.name == 'binary':
            return 'str'
        elif prop_type.name == 'instance-identifier':
            return 'str'
        elif isinstance(prop_type, Bits):
            return 'bits'
        elif isinstance(prop_type, Class) and prop_type.is_identity():
            return 'identityref'
        elif isinstance(prop_type, Enum):
            return 'enumeration'
        elif isinstance(prop_type, DataType):
            return 'str'
        return prop_type.name


class ClassSetAttrPrinter(object):

    def __init__(self, ctx, one_class_per_module):
        self.ctx = ctx
        self.one_class_per_module = one_class_per_module

    def print_setattr(self, clazz, leafs):
        yleafs = get_leafs(clazz)
        yleaf_lists = get_leaf_lists(clazz)
        ylists = get_lists(clazz)

        if len(yleafs) + len(yleaf_lists) + len(ylists)> 0:
            self._print_class_setattr_header()
            self._print_class_setattr_body(clazz, leafs)
            self._print_class_setattr_trailer()

    def _print_class_setattr_header(self):
        self.ctx.writeln('def __setattr__(self, name, value):')
        self.ctx.lvl_inc()

    def _print_class_setattr_body(self, clazz, leafs):
        leaf_names = ['%s' % (leaf.name) for leaf in leafs]
        if self.one_class_per_module:
            self.ctx.writeln('self._perform_setattr(%s, %s, name, value)'%(clazz.name, leaf_names))
        else:
            self.ctx.writeln('self._perform_setattr(%s, %s, name, value)'%(clazz.qn(), leaf_names))

    def _print_class_setattr_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.bline()

