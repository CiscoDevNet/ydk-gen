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
class_path_printer.py

Prints Go class methods

"""
from ydkgen.api_model import Bits, Enum, Package
from ydkgen.common import is_empty_prop
from .function_printer import FunctionPrinter

class GetSegmentPathPrinter(FunctionPrinter):
    def __init__(self, ctx, clazz):
        super(GetSegmentPathPrinter, self).__init__(ctx, clazz)

    def print_function_header(self):
        self.print_function_header_helper('GetSegmentPath', return_type='string')

    def print_function_body(self):
        path = ['"']
        prefix = ''
        if self.clazz.owner is not None:
            if isinstance(self.clazz.owner, Package):
                prefix = '%s:' % self.clazz.owner.stmt.arg
            elif self.clazz.owner.stmt.i_module.arg != self.clazz.stmt.i_module.arg:
                prefix = '%s:' % self.clazz.stmt.i_module.arg
        path.append('%s%s"' % (prefix, self.clazz.stmt.arg))

        key_props = self.clazz.get_key_props()
        for key_prop in key_props:
            prefix = ''
            if key_prop.stmt.i_module.arg != self.clazz.stmt.i_module.arg:
                prefix = '%s:' % (key_prop.stmt.i_module.arg)

            predicate = '{0}"[%s%s=\\""{0}fmt.Sprintf("%%v", %s.%s){0}"\\"]"'
            predicate = predicate.format(' + ') % (
                prefix, key_prop.stmt.arg, self.class_alias, key_prop.go_name())
            path.append(predicate)

        self.ctx.writeln('return %s' % (''.join(path)))

class GetEntityPathPrinter(FunctionPrinter):
    def __init__(self, ctx, clazz, leafs):
        super(GetEntityPathPrinter, self).__init__(ctx, clazz, leafs)

    def print_function_header(self):
        self.print_function_header_helper(
            'GetEntityPath', 'entity types.Entity', 'types.EntityPath')

    def print_function_body(self):
        self.ctx.writeln(
            'entityPath := types.EntityPath{Path: %s.GetSegmentPath()}' % self.class_alias)

        # todo: check if ancestor is irrelevant (see cpp)
        if self.clazz.owner is not None and isinstance(self.clazz.owner, Package):
            pass
        else:
            pass

        if self.leafs != []:
            self.ctx.writeln('var leafData types.LeafData')

        for leaf in self.leafs:
            leaf_var = "%s.%s" % (self.class_alias, leaf.go_name())
            if leaf.is_many:
                self._print_check_leaf_many(leaf, leaf_var)
            else:
                self._print_check_leaf(leaf, leaf_var)

        self.ctx.writeln('return entityPath')

    def _print_check_leaf_many(self, leaf, leaf_var):
        self.ctx.writeln('for _, llv := range %s {' % leaf_var)
        self.ctx.lvl_inc()
        self._print_check_leaf(leaf, "llv")
        self.ctx.lvl_dec()
        self.ctx.writeln('}')

    def _print_check_leaf(self, leaf, leaf_var):
        self._print_check_leaf_switch_header(leaf_var)
        self._print_check_leaf_switch_case_block(leaf_var)
        self._print_check_leaf_switch_default_block(leaf, leaf_var)
        self._print_check_leaf_switch_trailer(leaf)

    def _print_check_leaf_switch_header(self, leaf_var):
        self.ctx.writeln('if %s != nil {' % leaf_var)
        self.ctx.lvl_inc()

    def _print_check_leaf_switch_case_block(self, leaf_var):
        self.ctx.writeln('switch %s.(type) {' % leaf_var)
        self.ctx.writeln('case yfilter.YFilter:')
        self.ctx.lvl_inc()
        fvalue = 'Filter: %s.(yfilter.YFilter)' % leaf_var
        self.ctx.writeln("leafData = types.LeafData{IsSet: true, %s}" % fvalue)
        self.ctx.lvl_dec()

    def _print_check_leaf_switch_default_block(self, leaf, leaf_var):
        self.ctx.writeln('default:')
        self.ctx.lvl_inc()
        self.ctx.writeln('var v string')

        if isinstance(leaf.property_type, Enum):
            self._print_check_leaf_enum(leaf, leaf_var)
        elif isinstance(leaf.property_type, Bits):
            self._print_check_leaf_bits(leaf, leaf_var)
        elif not is_empty_prop(leaf):
            self.ctx.writeln('v = fmt.Sprintf("%%v", %s)' % leaf_var)
        self.ctx.writeln("leafData = types.LeafData{IsSet: true, Value: v}")
        self.ctx.lvl_dec()

    def _print_check_leaf_switch_trailer(self, leaf):
        self.ctx.writeln('}')
        self.ctx.writeln("entityPath.ValuePaths = "
                         "append(entityPath.ValuePaths, "
                         "types.NameLeafData{Name: \"%s\", Data: leafData})" %
                         leaf.stmt.arg)
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_check_leaf_enum(self, leaf, leaf_var):
        valid_enum_values = [e.stmt.arg for e in leaf.property_type.literals]
        venums = '","'.join(valid_enum_values)
        self.ctx.writeln('valid_enum_values := []string{"%s"}' % (venums))
        self.ctx.writeln("found := false")
        self.ctx.writeln('for _, e := range valid_enum_values {')
        self.ctx.lvl_inc()
        self.ctx.writeln('if e == fmt.Sprintf("%%v", %s) {' % leaf_var)
        self.ctx.lvl_inc()
        self.ctx.writeln('v = e')
        self.ctx.writeln('found = true')
        self.ctx.writeln('break')
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.lvl_dec()
        self.ctx.writeln('}')

        self.ctx.writeln('if !found {')
        self.ctx.lvl_inc()
        self.ctx.writeln("panic(fmt.Sprintf(\""
                         "Wrong enum value '%%v'\", %s))" % leaf_var)
        self.ctx.lvl_dec()
        self.ctx.writeln('}')

    def _print_check_leaf_bits(self, leaf, leaf_var):
        valid_bits = list(leaf.property_type._dictionary.keys())
        self.ctx.writeln('valid_bits := []string{"%s"}' % '", "'.join(valid_bits))
        self.ctx.writeln('var used_bits []string')
        self.ctx.writeln('m := %s.(map[string]bool)' % leaf_var)
        self.ctx.writeln('for _, vb := range valid_bits {')
        self.ctx.lvl_inc()
        self.ctx.writeln('enabled, ok := m[vb]')

        self.ctx.writeln('if ok && enabled {')
        self.ctx.lvl_inc()
        self.ctx.writeln('used_bits = append(used_bits, vb)')
        self.ctx.lvl_dec()
        self.ctx.writeln('}')

        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.writeln('v = strings.Join(used_bits, " ")')
