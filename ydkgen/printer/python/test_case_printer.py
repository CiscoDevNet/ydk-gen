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
 test_case_printer.py 

 YANG model driven API, python emitter.
"""

from ydkgen.api_model import Class, Enum, Package
from pyang.types import IntTypeSpec, StringTypeSpec


class FieldAccess(object):

    def __init__(self):
        ''' Left if the parent of this field access'''
        self.left = None
        ''' field is an instanceof the property clazz'''
        self.field = None

    def is_config(self):
        # to determine if a field is a config
        # field or not we need to ask the leaf/container
        # if it has a i_config flag
        if self.field is not None:
            # get the stmt
            stmt = self.field.stmt
            if hasattr(stmt, 'i_config'):
                return stmt.i_config
            else:
                if self.left is not None:
                    return self.left.is_config()

        return False

    def test_case_name(self):
        name_list = []
        name_list.append(self.field.name)
        current = self
        while current.left is not None:
            name_list.append(current.left.field.name)
            current = current.left
        return '_'.join(reversed(name_list))

    def segments(self):
        segments = []
        current = self
        while current is not None:
            segments.append(current)
            current = current.left
        return reversed(segments)

    def get_expr(self):
        '''Returns the expression'''
        prev_seg = None
        expr = ''
        for seg in self.segments():
            if prev_seg is None:
                expr = seg.field.name
            else:
                expr = '%s.%s' % (expr, seg.field.name)
            prev_seg = seg
        return expr


class TestCasePrinter(object):

    def __init__(self, ctx, parent):
        self.ctx = ctx
        self.parent = parent

    def print_test_cases(self, clazz):
        ''''''

    def print_testcases(self, package):
        assert isinstance(package, Package)
        self.ctx.bline()
        self.ctx.writeln('import unittest')
        self.ctx.writeln('from compare import is_equal')
        self.ctx.writeln('from ydk.providers import NetconfServiceProvider')
        self.ctx.writeln('from ydk.services import CRUDService')
        self.ctx.bline()

        for top_element in [t for t in package.owned_elements if isinstance(t, Class) or isinstance(t, Enum)]:
            self.ctx.writeln('from %s import %s' %
                             (package.get_py_mod_name(), top_element.name))

        self.ctx.bline()
        self.ctx.writeln('class %sTest(unittest.TestCase):' % package.name)
        self.ctx.lvl_inc()
        self.ctx.bline()
        self.ctx.writeln('def setUp(self):')
        self.ctx.lvl_inc()
        self.ctx.writeln("""self.ncc = NetconfServiceProvider(address='localhost' , 
                            username='admin', password='admin', port=12022)""")
        self.ctx.writeln('self.crud = CRUDService()')
        self.ctx.bline()

        self.ctx.lvl_dec()

        # we construct field access expressions
        # A field access is of the form a.b.c.d where b is a property of a
        # c is a property of b and d is a property of c
        # Each field access expression is then converted to a test case

        field_accesses = []

        for clazz in [p for p in package.owned_elements if isinstance(p, Class)]:
            field_accesses.extend(
                self.create_field_access_for_clazz(None, clazz, False, True))

        # for each field access go create a test case
        for field_access in [f for f in field_accesses if f.is_config()]:
            self.create_test_case_for_field_access(field_access)
        self.ctx.lvl_dec()

    def create_field_access_for_clazz(self, parent_access, clazz, include_attributes, include_references):
        field_accesses = []

        for p in clazz.properties():
            if isinstance(p.property_type, Class) and not include_references:
                continue

            if not isinstance(p.property_type, Class) and not include_attributes:
                continue

            field_access = FieldAccess()
            field_access.left = parent_access
            field_access.field = p
            field_accesses.append(field_access)
            if isinstance(p.property_type, Class):
                # go recursive
                child_field_accesses = self.create_field_access_for_clazz(field_access,
                                                                          p.property_type,
                                                                          include_attributes,
                                                                          include_references)
                field_accesses.extend(child_field_accesses)

        return field_accesses

    def create_test_case_for_field_access(self, field_access):
        self.ctx.writeln('def test_%s(self):' %
                         (field_access.test_case_name()))
        self.ctx.lvl_inc()
        if not field_access.is_config():
            self.ctx.writeln("''' This is an oper field '''")
            self.ctx.writeln('pass')
        else:
            # start building the object tree
            prev_seg = None
            instance = None
            for seg in field_access.segments():
                if isinstance(seg.field.property_type, Class):
                    clazz = seg.field.property_type
                    if prev_seg is None:
                        # we have to instantiate
                        self.ctx.writeln(
                            '%s = %s()' % (seg.field.name, clazz.qn()))
                        instance = seg.field.name
                    else:
                        # skip if the property is not many
                        if seg.field.is_many:
                            # need to create
                            self.ctx.writeln('%s = %s.%s()' % (
                                seg.field.name, prev_seg.get_expr(), seg.field.property_type.name))
                            self.ctx.writeln(
                                '%s.parent = %s' % (seg.field.name, prev_seg.get_expr()))
                            self.ctx.writeln('%s.append(%s)' %
                                             (seg.get_expr(), seg.field.name))
                            instance = seg.field.name
                        else:
                            instance = seg.get_expr()
                prev_seg = seg

            attributes = self.create_field_access_for_clazz(
                field_access, field_access.field.property_type, True, False)
            for seg in attributes:

                if isinstance(seg.field.property_type, Enum):
                    self.ctx.writeln('%s.%s = %s.%s()' % (
                        instance, seg.field.name, prev_seg.get_expr(), seg.field.property_type.name))
                else:
                    type_spec = seg.field.property_type
                    if isinstance(type_spec, StringTypeSpec):
                        self.ctx.writeln("%s.%s = 'Hello" %
                                         (instance, seg.get_expr()))
                    elif isinstance(type_spec, IntTypeSpec):
                        self.ctx.writeln(
                            "%s.%s = 100" % (instance, seg.get_expr()))
                    else:
                        self.ctx.writeln(
                            '%s.%s = None' % (instance, seg.get_expr()))

                    # check to see if it multivalued or not
        self.ctx.lvl_dec()
        self.ctx.bline()
