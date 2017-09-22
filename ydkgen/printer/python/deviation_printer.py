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
class_printer.py

 YANG model driven API, class emitter.

"""
# add inline enum to deviation module itself
from ydkgen.api_model import Bits
from ydkgen.api_model import Enum
from ydkgen.printer.meta_data_util import get_meta_info_data

from .bits_printer import BitsPrinter
from .class_meta_printer import ClassMetaPrinter
from .enum_printer import EnumPrinter


class DeviationPrinter(object):
    def __init__(self, ctx, sort_clazz):
        self.ctx = ctx
        self.collected_enum_meta = []
        self.sort_clazz = sort_clazz

    def print_deviation(self, package):
        self.print_deviation_header()
        deviations = package.owned_elements
        deviations = sorted(deviations, key= lambda d:d.qn())
        for deviation in deviations:
            self.print_deviation_inline_class(deviation)
        self.print_deviation_table_header()
        for deviation in deviations:
            self.print_deviation_entry(deviation)
        self.print_collected_enum_meta()
        self.print_deviation_table_trailer()

    def print_deviation_header(self):
        self.ctx.writeln("""
from enum import Enum
from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST, \
    REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION
from ydk.providers._importer import _yang_ns

""")

    def print_collected_enum_meta(self):
        for e in self.collected_enum_meta:
            self.ctx.lvl_inc()
            EnumPrinter(self.ctx).print_enum_meta(e)
            self.ctx.lvl_dec()

    def print_deviation_table_header(self):
        self.ctx.writeln("_deviation_table = {")

    def print_deviation_table_trailer(self):
        self.ctx.writeln("}")

    def print_deviation_inline_class(self, deviation):
        if deviation.owned_elements != []:
            bitz = [b for b in deviation.owned_elements if isinstance(b, Bits)]
            enumz = [e for e in deviation.owned_elements if isinstance(e, Enum)]
            for b in bitz:
                BitsPrinter(self.ctx).print_bits(b)
            for e in enumz:
                # collect inline enum meta
                EnumPrinter(self.ctx).print_enum(e, no_meta_assign=True)
                self.print_deviation_enum_meta_assignment(e)
                self.collected_enum_meta.append(e)

    def print_deviation_enum_meta_assignment(self, enum_class):
        self.ctx.lvl_inc()
        self.ctx.writeln('@staticmethod')
        self.ctx.writeln('def _meta_info():')
        self.ctx.lvl_inc()
        self.ctx.writeln("return _deviation_table['%s']" % (enum_class.qn()))
        self.ctx.lvl_dec(tab=2)
        self.ctx.write("\n\n")

    def print_deviation_entry(self, deviation):
        target = deviation.d_target
        stmts = list(deviation.d_stmts)
        stmts = sorted(stmts, key=lambda s:s.arg)
        typ = deviation.d_type
        qn = deviation.qn()
        self.ctx.lvl_inc()
        self.ctx.writeln("'%s' : {" % qn)
        self.ctx.lvl_inc()
        self.ctx.writeln("'deviation_typ' : '%s'," % typ)
        if typ != 'not_supported':
            self.ctx.writeln("'keyword_value' : [")
            self.ctx.lvl_inc()
            for stmt in stmts:
                key = stmt.keyword
                self.ctx.write("(%s, " %self.convert_key_val(key))
                if key == 'type':
                    prop = target.i_property
                    meta = get_meta_info_data(prop, prop.property_type, prop.stmt.search_one('type'), 'py', False)
                    self.ctx.bline()
                    self.ctx.lvl_inc()
                    ClassMetaPrinter(self.ctx, False, self.sort_clazz).print_meta_class_member(meta, self.ctx)
                    self.ctx.lvl_dec()
                    self.ctx.writeln("),")
                else:
                    val = self.convert_key_val(stmt.arg)
                    self.ctx.str("%s),\n" % val)
            self.ctx.lvl_dec()
            self.ctx.writeln("]")
        self.ctx.lvl_dec()
        self.ctx.writeln("},")
        self.ctx.lvl_dec()

    def convert_key_val(self, val):
        if val == 'false':
            return 'False'
        elif val.isdigit():
            return val
        else:
            return "'%s'" % val.replace('-', '_')
