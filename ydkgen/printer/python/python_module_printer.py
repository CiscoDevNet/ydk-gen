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
 python_module_printer.py 
 
 YANG model driven API, python emitter.
 
"""
from ydkgen.api_model import Class, Enum, Bits
from .bits_printer import BitsPrinter
from .class_printer import ClassPrinter
from .enum_printer import EnumPrinter
from ydkgen.common import get_module_name
from .import_test_printer import ImportTestPrinter
from .class_meta_printer import ClassMetaPrinter
from pyang.error import EmitError
from .test_case_printer import TestCasePrinter
from .python_rst_printer import PythonRstPrinter
from .deviation_printer import DeviationPrinter


class _Stack:

    def __init__(self):
        self.items = []

    def isEmpty(self):
        return self.items == []

    def push(self, item):
        self.items.append(item)

    def pop(self):
        return self.items.pop()

    def peek(self):
        return self.items[len(self.items) - 1]

    def size(self):
        return len(self.items)


class PythonModulePrinter(object):

    def __init__(self, ctx):
        self.ctx = ctx
        self._start_tab = _Stack()

    def _start_tab_leak_check(self):
        self._start_tab.push(self.ctx.lvl)

    def _check_tab_leak(self):
        end_tab = self.ctx.lvl
        if self._start_tab.pop() != end_tab:
            raise EmitError('Tab leak !!!')

    def header(self, mheader):
        self.ctx.str('''

import re
import collections

from enum import Enum

from ydk.types import Empty, YList, DELETE, Decimal64, FixedBitsDict

from ydk.errors import YPYError, YPYDataValidationError


''')

    def meta_header(self, anyxml_import):
        self.ctx.str("""


import re
import collections

from enum import Enum

from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST, \
    REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION{0}

from ydk.errors import YPYError, YPYDataValidationError
from ydk.models import _yang_ns

""".format(anyxml_import))

    def print_yang_ns_header(self):
        self.ctx.str('''
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

''')

    def comment(self, lines):
        for line in lines:
            self.ctx.writeln("# %s" % line)
        self.ctx.bline()

    def imports(self, p):

        imports_to_print = []

        for imported_type in p.imported_types():
            import_stmt = 'from %s import %s' % (
                imported_type.get_py_mod_name(), imported_type.qn().split('.')[0])
            if import_stmt in imports_to_print:
                continue
            else:
                imports_to_print.append(import_stmt)

        imports_to_print = sorted(imports_to_print)
        for import_to_print in imports_to_print:
            self.ctx.writeln('%s' % import_to_print)

        self.ctx.bline()

    def print_namespaces(self, ns):
        for n in ns:
            self.ctx.writeln("_global_%s_nsp = '%s'" % (n[0], n[1]))
        self.ctx.writeln("_namespaces = { \\")
        for n in ns:
            self.ctx.writeln("'%s' : '%s', " % (n[2], n[1]), 1)
        self.ctx.writeln("}")
        self.ctx.bline()

    def print_namespaces_map(self, namespace_map):
        self.ctx.writeln("_namespace_package_map = { \\")
        for namespace, python_import in namespace_map.iteritems():
            self.ctx.writeln("('%s', '%s') : 'from %s import %s', " % (namespace[0], namespace[1], python_import[0], python_import[1]))
        self.ctx.writeln("}")
        self.ctx.bline()

    def print_identity_map(self, packages):
        self.ctx.writeln("_identity_map = { \\")
        self.ctx.lvl_inc()
        for package in packages:
            identities = [idx for idx in package.owned_elements if isinstance(
                idx, Class) and idx.is_identity()]
            identities = sorted(identities, key=lambda c: c.name)
            for identity_clazz in identities:
                self.ctx.writeln("('%s', '%s'):('%s', '%s')," % (get_module_name(identity_clazz.stmt), identity_clazz.stmt.arg,
                                                                 identity_clazz.get_py_mod_name(), identity_clazz.qn()))
        self.ctx.lvl_dec()
        self.ctx.writeln("}")
        self.ctx.bline()

    def print_classes_at_same_level(self, unsorted_classes):
        ClassPrinter(self.ctx, self).print_classes(unsorted_classes)

    def print_classes_meta(self, unsorted_classes):
        ClassMetaPrinter(self.ctx, self).print_output(unsorted_classes)

    def print_classes_meta_parents(self, unsorted_classes):
        ClassMetaPrinter(self.ctx, self).print_parents(unsorted_classes)

    def print_child_enums(self, parent):
        enumz = []
        enumz.extend([nested_enum for nested_enum in parent.owned_elements if isinstance(
            nested_enum, Enum)])

        for nested_enumz in sorted(enumz, key=lambda e: e.name):
            self.print_enum(nested_enumz)

    def print_child_bits(self, parent):
        bits = []
        bits.extend(
            [nested_bit for nested_bit in parent.owned_elements if isinstance(nested_bit, Bits)])

        for bit in sorted(bits, key=lambda b: b.name):
            self.print_bits(bit)

    def print_child_classes(self, parent):
        self.print_classes_at_same_level(
            [nested_class for nested_class in parent.owned_elements if isinstance(nested_class, Class)])

    def print_bits(self, bits):
        BitsPrinter(self.ctx, self).print_bits(bits)

    def print_enum(self, enum_class, no_meta=False):
        EnumPrinter(self.ctx, self).print_enum(enum_class, no_meta)

    def print_enum_meta(self, enum_class):
        EnumPrinter(self.ctx, self).print_enum_meta(enum_class)

    def print_import_tests(self, packages):
        ImportTestPrinter(self.ctx, self).print_import_tests(packages)

    def print_testcases(self, package):
        TestCasePrinter(self.ctx, self).print_testcases(package)

    def print_python_rst(self, named_element):
        PythonRstPrinter(self.ctx, self).print_rst_file(named_element)

    def print_ydk_models_rst(self, packages):
        PythonRstPrinter(self.ctx, self).print_ydk_models_rst(packages)

    def print_deviation(self, package):
        DeviationPrinter(self.ctx, self).print_deviation(package)

    def print_meta_class_member(self, meta, ctx):
        ClassMetaPrinter(self.ctx, self).print_meta_class_member(meta, ctx)
