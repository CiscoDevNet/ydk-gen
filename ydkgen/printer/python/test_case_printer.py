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

'''
 test_case_printer.py 

 YANG model driven API, python emitter.
'''

from ydkgen.api_model import Class, Enum, Package, Bits
from ydkgen.builder import TypesExtractor
from ydkgen.common import iskeyword, snake_case
from ydkgen.printer import meta_data_util
from pyang.types import IntTypeSpec, StringTypeSpec, UnionTypeSpec, PathTypeSpec, \
        RangeTypeSpec, BooleanTypeSpec, BinaryTypeSpec, EmptyTypeSpec, \
        LengthTypeSpec, Decimal64TypeSpec, PatternTypeSpec


class LeafInfo(object):
    def __init__(self, clazz):
        self.clazz = clazz
        self.props = []


class TestCasePrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx
        self.types_extractor = TypesExtractor()

    def print_testcases(self, package, identity_subclasses):
        assert isinstance(package, Package)
        self.identity_subclasses = identity_subclasses
        self._print_header(package)
        self._print_body([p for p in package.owned_elements if isinstance(p, Class) and not p.is_identity()])
        self._print_trailer()

    def _print_header(self, package):
        self._print_imports(package)
        self._print_class_header(package)

    def _print_body(self, clazzes):
        for clazz in clazzes:
            self._print_test_case(clazz)

    def _print_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.bline()
        self.ctx.writeln("if __name__ == '__main__':")
        self.ctx.lvl_inc()
        self.ctx.writeln("unittest.main()")
        self.ctx.lvl_dec()

    def _print_imports(self, package):
        self._print_common_imports()
        imports_to_print = self._get_imports(package)
        for import_to_print in imports_to_print:
            self.ctx.writeln('%s' % import_to_print)
        self.ctx.bline()

        for top_element in [t for t in package.owned_elements if isinstance(t, Class) or isinstance(t, Enum)]:
            self.ctx.writeln('from %s import %s' %
                             (package.get_py_mod_name(), top_element.name))

    def _get_imports(self, package):
        imports_to_print = []
        for imported_type in package.imported_types():
            import_stmt = 'from %s import %s' % (
                imported_type.get_py_mod_name(), imported_type.qn().split('.')[0])
            if import_stmt in imports_to_print:
                continue
            else:
                imports_to_print.append(import_stmt)
        imports_to_print.extend(x for x in self._get_properties_imports(package) if x not in imports_to_print)
        imports_to_print = sorted(imports_to_print)
        return imports_to_print

    def _get_properties_imports(self, element):
        leafref_imports = []
        if isinstance(element, Class):
            for prop in element.properties():
                if isinstance(prop.property_type, Class) and prop.property_type.is_identity():
                    ref_class = self.identity_subclasses[prop.property_type][0]
                    child_class = ref_class
                    if ref_class in self.identity_subclasses:
                        child_class = self.identity_subclasses[ref_class][0]
                    leafref_imports.append('from %s import %s' % (child_class.get_py_mod_name(), child_class.qn().split('.')[0]))
                elif isinstance(prop.property_type, PathTypeSpec):
                    if prop.stmt.i_leafref_ptr is not None:
                        ref_class = prop.stmt.i_leafref_ptr[0].parent.i_class
                        leafref_imports.append('from %s import %s' % (ref_class.get_py_mod_name(), ref_class.qn().split('.')[0]))
        for clazz in [t for t in element.owned_elements if isinstance(t, Class) and t.is_config() and not t.is_identity()]:
            leafref_imports.extend(self._get_properties_imports(clazz))
        return leafref_imports

    def _print_common_imports(self):
        self.ctx.bline()
        self.ctx.writeln('import unittest')
        self.ctx.writeln('from tests.compare import is_equal')
        self.ctx.writeln('from ydk.providers import NativeNetconfServiceProvider')
        self.ctx.writeln('from ydk.services import CRUDService')
        self.ctx.writeln('from ydk.types import Decimal64, Empty')
        self.ctx.bline()

    def _print_class_header(self, package):
        self.ctx.bline()
        self.ctx.bline()
        self.ctx.writeln('class %sTest(unittest.TestCase):' % package.name)
        self.ctx.lvl_inc()
        self._print_setup_class()
        self._print_teardown_class()
        self._print_setup(package)
        self._print_teardown(package)

    def _print_setup_class(self):
        self.ctx.bline()
        self.ctx.writeln('@classmethod')
        self.ctx.writeln('def setUpClass(self):')
        self.ctx.lvl_inc()
        self.ctx.writeln('''self.ncc = NativeNetconfServiceProvider(address='127.0.0.1' ,
                            username='admin', password='admin', port=12022)''')
        self.ctx.writeln('self.crud = CRUDService()')
        self.ctx.lvl_dec()

    def _print_setup(self, package):
        self.ctx.bline()
        self.ctx.writeln('def setUp(self):')
        self.ctx.lvl_inc()
        self.ctx.writeln("print '\\nIn method '+ self._testMethodName + ':'")
        for p in package.owned_elements:
            if isinstance(p, Class) and not p.is_identity() and p.is_config():
                self._print_mandatory_leafs(p)
        self.ctx.lvl_dec()

    def _print_teardown(self, package):
        self.ctx.bline()
        self.ctx.writeln('def tearDown(self):')
        self.ctx.lvl_inc()
        top_elements = [p for p in package.owned_elements if isinstance(p, Class) and not p.is_identity() and p.is_config()]
        if len(top_elements) > 0:
            self.ctx.writeln('#TEARDOWN TOP ELEMENTS')
        else:
            self.ctx.writeln('pass')
            self.ctx.bline()
        for clazz in top_elements:
            self._print_class_instance(clazz, clazz.get_key_props(), suffix='_teardown')
            self.ctx.writeln('print "Teardown %s"' % get_obj_name(clazz))
            self.ctx.writeln('try:')
            self.ctx.lvl_inc()
            self.ctx.writeln('self.crud.delete(self.ncc, %s_teardown)' % get_obj_name(clazz))
            self.ctx.lvl_dec()
            self.ctx.writeln('except Exception as e:')
            self.ctx.lvl_inc()
            self.ctx.writeln('pass')
            self.ctx.lvl_dec()
            self.ctx.bline()
        self.ctx.lvl_dec()

    def _print_teardown_class(self):
        self.ctx.bline()
        self.ctx.writeln('@classmethod')
        self.ctx.writeln('def tearDownClass(self):')
        self.ctx.lvl_inc()
        self.ctx.writeln('''self.ncc.close()''')
        self.ctx.lvl_dec()

    def _print_test_case(self, clazz):
        for prop in clazz.properties():
            if isinstance(prop.property_type, Class) and not prop.property_type.is_identity():
                self._print_test_case(prop.property_type)
        self._print_test_case_header(clazz)
        self._print_test_case_body(clazz)
        self._print_test_case_trailer()

    def _print_test_case_header(self, clazz):
        self.ctx.writeln('def test_%s(self):' % ('_'.join([snake_case(f) for f in clazz.qn().split('.')])))
        self.ctx.lvl_inc()

    def _print_test_case_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.bline()

    def _print_test_case_body(self, clazz):
        if not clazz.is_config():
            self.ctx.writeln("'''This is an oper field'''")
            self.ctx.writeln('pass')
        else:
            # start building the object tree
            self._print_leafref_references(clazz)
            self._print_parents_mandatory_leafs(clazz.owner)
            self.ctx.writeln('# CREATE')
            self._print_class_instance(clazz, clazz.properties(), suffix='')
            self._print_crud_operations(get_obj_name(clazz), clazz)

    def _print_class_instance(self, clazz, props, suffix):
        if clazz is None:
            return
        owner_obj_name = ''
        obj_name = get_obj_name_with_suffix(clazz, suffix)
        if clazz.owner is not None and isinstance(clazz.owner, Class):
            owner_obj_name = get_obj_name_with_suffix(clazz.owner, suffix)
        if isinstance(clazz, Class):
            self.ctx.writeln('%s = %s()' % (obj_name, clazz.qn()))            
            self._print_properties(obj_name, props)
            if len(clazz.get_key_props()) > 0:
                self._print_list_obj_append(clazz, clazz.owner, suffix)
        if isinstance(clazz.owner, Class):
            self._print_class_instance(clazz.owner, clazz.owner.get_key_props(), suffix)
        if clazz.owner is not None and isinstance(clazz.owner, Class):
            self.ctx.writeln('%s.parent = %s' % (obj_name, owner_obj_name))

    def _print_properties(self, obj_name, properties):
        for prop in properties:
            self._print_property(obj_name, prop)

    def _print_mandatory_leafs(self, element):
        mandatory_leafs = self._get_mandatory_leafs(element)
        if len(mandatory_leafs) > 0:
            self.ctx.writeln('#MANDATORY LEAFS CREATE')
        for mandatory_leaf in mandatory_leafs:
            leafs_to_set = mandatory_leaf.clazz.get_key_props()
            leafs_to_set.extend(x for x in mandatory_leaf.props if x not in leafs_to_set)
            self._print_class_instance(mandatory_leaf.clazz, leafs_to_set, suffix='')
            self.ctx.writeln('print "Creating mandatory %s"' % snake_case(mandatory_leaf.clazz.qn() + '.' + mandatory_leaf.props[0].name))
            self.ctx.writeln('self.crud.create(self.ncc, %s)' % get_obj_name(mandatory_leaf.clazz))
            self.ctx.bline()

    def _get_mandatory_leafs(self, element):
        mandatory_leafs = []
        if hasattr(element, 'properties'):
            mandatory_info = None
            for prop in element.properties():
                mandatory = prop.stmt.search_one('mandatory')
                if mandatory is not None and mandatory.arg == 'true':
                    if mandatory_info is None:
                        mandatory_info = LeafInfo(element)
                    mandatory_info.props.append(prop)
            if mandatory_info is not None:
                mandatory_leafs.append(mandatory_info)

        for p in element.owned_elements:
            if isinstance(p, Class) and (not p.is_identity()) and p.is_config() and (p.stmt.search_one('presence') is None) and len(p.get_key_props()) == 0:
                mandatory_leafs.extend(self._get_mandatory_leafs(p))

        return mandatory_leafs

    def _print_parents_mandatory_leafs(self,clazz):
        if isinstance(clazz, Class) and not clazz.is_identity() and clazz.is_config():
            if clazz.owner is not None:
                self._print_parents_mandatory_leafs(clazz.owner)
            self._print_mandatory_leafs(clazz)

    def _print_leafref_references(self, clazz):
        leaf_ref_infos = list(reversed(self._get_leafref_references(clazz)))
        if len(leaf_ref_infos) > 0:
            self.ctx.writeln('#LEAFREF REFERENCES CREATE')
        for leaf_info in leaf_ref_infos:
            reference_clazz = leaf_info.clazz
            leafs_to_set = leaf_info.props
            leafs_to_set.extend(x for x in reference_clazz.get_key_props() if x not in leafs_to_set)
            leafs_to_set.extend(x for x in reference_clazz.properties() if x.stmt.search_one('mandatory') is not None)
            self._print_class_instance(reference_clazz, leafs_to_set, '_reference')
            self.ctx.writeln('print "Creating reference %s"' % get_obj_name(reference_clazz))
            self.ctx.writeln('self.crud.create(self.ncc, %s_reference)' % get_obj_name(reference_clazz))
            self.ctx.bline()

    def _get_leafref_references(self, clazz):
        leaf_ref_infos = []
        info = None
        for prop in clazz.properties():
            if isinstance(prop.property_type, PathTypeSpec):
                if prop.stmt.i_leafref_ptr is not None:
                    if info is None:
                        info = LeafInfo(prop.stmt.i_leafref_ptr[0].parent.i_class)
                    info.props.append(prop.stmt.i_leafref_ptr[0].i_property)
        if info is not None and info not in leaf_ref_infos:
            leaf_ref_infos.append(info)

        if clazz.owner is not None and isinstance(clazz.owner, Class):
            leaf_ref_infos.extend([x for x in self._get_leafref_references(clazz.owner) if x not in leaf_ref_infos])

        return leaf_ref_infos

    def _print_property(self, obj_name, prop):
        format_string = '%s.%s = %s'
        if prop.stmt.keyword == 'leaf-list':
            format_string = '%s.%s.append(%s)'

        type_spec = prop.property_type
        type_stmt = prop.stmt.search_one('type')

        if isinstance(type_spec, UnionTypeSpec):
            if len(type_spec.types) > 0:
                type_spec, type_stmt = self._get_union_type_spec(type_spec.types[0])

        if type_stmt is not None:
            if 'prefix' in type_stmt.arg and 'ip' in type_stmt.arg:
                self.ctx.writeln(format_string % (obj_name, prop.name, "'1.2.3.4/32'"))
                return
            elif 'address' in type_stmt.arg and 'ipv6'  in type_stmt.arg:
                self.ctx.writeln(format_string % (obj_name, prop.name, "'123::123'"))
                return
            elif 'address' in type_stmt.arg and 'ip' in type_stmt.arg:
                self.ctx.writeln(format_string % (obj_name, prop.name, "'1.2.3.4'"))
                return
            elif 'host' in type_stmt.arg:
                self.ctx.writeln(format_string % (obj_name, prop.name, "'1.2.3.4'"))
                return
            elif 'mac-address' in type_stmt.arg:
                self.ctx.writeln(format_string % (obj_name, prop.name, "'00:0a:95:9d:68:16'"))
                return

        if isinstance(type_spec, Class):
            if type_spec.is_identity():
                ref_class = self.identity_subclasses[type_spec][0]
                child_class = ref_class
                if ref_class in self.identity_subclasses:
                    child_class = self.identity_subclasses[ref_class][0]
                self.ctx.writeln(format_string % (obj_name, prop.name, child_class.qn() + '()'))
        elif isinstance(type_spec, Enum):
            self.ctx.writeln(format_string % (obj_name, prop.name, type_spec.qn() + '.' + type_spec.literals[0].name))
        elif isinstance(type_spec, Bits):
            self.ctx.writeln("%s.%s['%s'] = True" % (obj_name, prop.name, list(type_spec._dictionary.keys())[0]))
        else:
            target_type_stmt = None
            while isinstance(type_spec, PathTypeSpec):
                if not hasattr(type_spec, 'i_target_node'):
                    return
                target_type_stmt = type_spec.i_target_node.search_one('type')
                type_spec = target_type_stmt.i_type_spec

            if target_type_stmt is not None:
                if 'prefix' in target_type_stmt.arg and 'ip' in target_type_stmt.arg:
                    self.ctx.writeln(format_string % (obj_name, prop.name, "'1.2.3.4/32'"))
                    return
                elif 'address' in target_type_stmt.arg and 'ip' in target_type_stmt.arg:
                    self.ctx.writeln(format_string % (obj_name, prop.name, "'1.2.3.4'"))
                    return
                elif 'host' in target_type_stmt.arg:
                    self.ctx.writeln(format_string % (obj_name, prop.name, "'1.2.3.4'"))
                    return

            if isinstance(type_spec, StringTypeSpec):
                self.ctx.writeln(format_string % (obj_name, prop.name, "'Hello'"))
            elif isinstance(type_spec, LengthTypeSpec):
                ranges = meta_data_util.get_length_limits(type_spec)
                for min_limit, max_limit in ranges:
                    if isinstance(min_limit, int) and isinstance(max_limit, int):
                        size = (min_limit + max_limit) / 2
                    else:
                        size = min_limit + 1
                    test_string = generate_string_of_size(size)
                    self.ctx.writeln(format_string % (obj_name, prop.name, "'" + test_string + "'"))
                    break
            elif isinstance(type_spec, IntTypeSpec):
                self.ctx.writeln(format_string % (obj_name, prop.name, '100'))
            elif isinstance(type_spec, RangeTypeSpec):
                ranges = meta_data_util.get_range_limits(type_spec)
                for min_limit, max_limit in ranges:
                    if isinstance(min_limit, int) and isinstance(max_limit, int):
                        size = (min_limit + max_limit) / 2
                    elif isinstance(min_limit, int):
                        size = min_limit + 1
                    else:
                        size = int(min_limit) + 1
                    self.ctx.writeln(format_string % (obj_name, prop.name, str(size)))
                    break
            elif isinstance(type_spec, Decimal64TypeSpec):
                self.ctx.writeln(format_string % (obj_name, prop.name, 'Decimal64("1.0")'))
            elif isinstance(type_spec, BooleanTypeSpec):
                self.ctx.writeln(format_string % (obj_name, prop.name, 'False'))
            elif isinstance(type_spec, BinaryTypeSpec):
                self.ctx.writeln(format_string % (obj_name, prop.name, "'0x00'"))
            elif  isinstance(type_spec, EmptyTypeSpec):
                self.ctx.writeln(format_string % (obj_name, prop.name, 'Empty()'))
            elif isinstance(type_spec, PatternTypeSpec):
                target_type_stmt = prop.stmt.search_one('type')
                while hasattr(target_type_stmt, 'i_typedef') and target_type_stmt.i_typedef is not None:
                    target_type_stmt = target_type_stmt.i_typedef.search_one('type')
                pattern = target_type_stmt.search_one('pattern')
                if pattern is not None:
                    if pattern.arg == '[\w\-\.:,_@#%$\+=\|;]+':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'abc'"))
                        return
                    elif pattern.arg == '(([a-zA-Z0-9_]*\d+/){3}\d+)|(([a-zA-Z0-9_]*\d+/){4}\d+)|(([a-zA-Z0-9_]*\d+/){3}\d+\.\d+)|(([a-zA-Z0-9_]*\d+/){2}([a-zA-Z0-9_]*\d+))|(([a-zA-Z0-9_]*\d+/){2}([a-zA-Z0-9_]+))|([a-zA-Z0-9_-]*\d+)|([a-zA-Z0-9_-]*\d+\.\d+)|(mpls)|(dwdm)':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'test1'"))
                        return
                    elif pattern.arg == '(!.+)|([^!].+)':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'!Hello'"))
                        return
                    elif pattern.arg == '[a-fA-F0-9]{2}(\.[a-fA-F0-9]{4}){3,9}\.[a-fA-F0-9]{2}':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'aa.aaaa.aaaa.aaaa.aa'"))
                        return
                    elif pattern.arg == '(act)|(pre)':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'act'"))
                        return
                    elif pattern.arg == '((([a-zA-Z0-9_]*\d+)|(\*))/){2}(([a-zA-Z0-9_]*\d+)|(\*))':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'test1/test1/test1'"))
                        return
                    elif pattern.arg == '[0-9a-fA-F]{1,8}':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'aaa'"))
                        return
                    elif pattern.arg == '[a-zA-Z0-9][a-zA-Z0-9\._@$%+#:=<>\-]{0,62}':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'Hello'"))
                        return
                    elif pattern.arg == '([0-9]|[1-5][0-9]|6[0-3])|(([0-9]|[1-5][0-9]|6[0-3])-([0-9]|[1-5][0-9]|6[0-3]))|(af11)|(af12)|(af13)|(af21)|(af22)|(af23)|(af31)|(af32)|(af33)|(af41)|(af42)|(af43)|(ef)|(default)|(cs1)|(cs2)|(cs3)|(cs4)|(cs5)|(cs6)|(cs7)':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'cs123'"))
                        return
                    elif pattern.arg == '(\d+)|(\d+\-\d+)':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'123'"))
                        return
                    elif pattern.arg == '([a-zA-Z0-9_]*\d+/){1,2}([a-zA-Z0-9_]*\d+)':
                        self.ctx.writeln(format_string % (obj_name, prop.name, "'a1/a2'"))
                        return
                if prop.name == 'masklength_range':
                    self.ctx.writeln(format_string % (obj_name, prop.name, "'1..2'"))
                elif 'address' in prop.stmt.search_one('type').arg and 'ip' in prop.stmt.search_one('type').arg:
                    self.ctx.writeln(format_string % (obj_name, prop.name, "'1.2.3.4'"))
                elif 'domain' in prop.stmt.search_one('type').arg and 'domain' in prop.stmt.search_one('type').arg:
                    self.ctx.writeln(format_string % (obj_name, prop.name, "'example.com'"))
                else:
#print prop.stmt.search_one('type').arg
#                    if pattern is not None:
#                        print pattern.arg, prop.name
                    self.ctx.writeln("#%s.%s = %s" % (obj_name, prop.name, "'Hello'"))
            else:
#                print prop.name, type_spec.arg
                self.ctx.writeln('#%s.%s = None' % (obj_name, prop.name))

    def _get_union_type_spec(self, type_stmt):
        contained_property_type = self.types_extractor.get_property_type(type_stmt)

        if isinstance(contained_property_type, UnionTypeSpec):
            return self._get_union_type_spec(contained_property_type.types[0])
        else:
            return contained_property_type, type_stmt

    def _print_list_obj_append(self, clazz, owner, suffix):
        parent_obj_name = get_obj_name_with_suffix(owner, suffix)
        prop_name = ''
        if isinstance(owner, Class):
            for prop in owner.properties():
                if isinstance(prop.property_type, Class) and prop.property_type.name == clazz.name:
                    prop_name = prop.name
                    break
        else:
            return
        self.ctx.writeln('%s = %s()' % (parent_obj_name, owner.qn()))
        self.ctx.writeln('%s.%s.append(%s)' % (parent_obj_name, prop_name, get_obj_name_with_suffix(clazz, suffix)))

    def _print_crud_operations(self, obj_name, clazz):
        self.ctx.writeln('print "Creating test %s"' % obj_name)
        self.ctx.writeln('self.crud.create(self.ncc, %s)' % obj_name)
        self._print_crud_read_test_operation(obj_name, clazz)
        self._print_crud_delete_operation(obj_name, clazz)

    def _print_crud_read_test_operation(self, obj_name, clazz):
        self.ctx.bline()
        self.ctx.writeln('# READ')
        self._print_class_instance(clazz, clazz.get_key_props(), suffix='_read')
        self.ctx.writeln('print "Reading test %s_read"' % obj_name)
        self.ctx.writeln('%s_read_output = self.crud.read(self.ncc, %s_read)' % (obj_name, obj_name))
        self.ctx.writeln('#self.assertEqual(is_equal(%s, %s_read_output), True)' % (obj_name, obj_name))

    def _print_crud_delete_operation(self, obj_name, clazz):
        self.ctx.bline()
        self.ctx.writeln('# DELETE')
        self._print_class_instance(clazz, clazz.get_key_props(), suffix='_delete')
        self.ctx.writeln('print "Deleting test %s_delete"' % obj_name)
        self.ctx.writeln('self.crud.delete(self.ncc, %s_delete)' % (obj_name))


def get_obj_name(clazz):
    obj_name = snake_case(clazz.qn())
    if iskeyword(obj_name):
        obj_name = '%s_' % obj_name
    return obj_name


def get_obj_name_with_suffix(clazz, suffix):
    return get_obj_name(clazz) + suffix


def generate_string_of_size(size):
    if size == 0:
        return ''
    if len('Hello') == size:
        return 'Hello'
    elif len('Hell') == size:
        return 'Hell'
    elif len('Hel') == size:
        return 'Hel'
    elif len('He') == size:
        return 'He'
    elif len('H') == size:
        return 'H'
    else:
        return 'Hello' + ''.join([str(x % 10) for x in range(size - len('Hello'))])
