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
 bits_printer.py

 Printer for Bits Classes
"""

from ydkgen.api_model import Bits
from ydkgen.common import convert_to_reStructuredText


class BitsPrinter(object):

    """
        Prints Bits classes

        :attribute ctx The printer context
        :attribute parent The parent printer object

    """

    def __init__(self, ctx):
        self.ctx = ctx

    def print_bits(self, bits):
        """
            print the bits class

            :param api_model.Bits bits The bits object.
        """
        assert isinstance(bits, Bits)
        self._print_bits_header(bits)
        self._print_bits_body(bits)
        self._print_bits_trailer(bits)

    def _print_bits_header(self, bits):
        self.ctx.writeln('class %s(FixedBitsDict):' % bits.name)

    def _print_bits_body(self, bits):
        self.ctx.lvl_inc()
        self._print_bits_docstring(bits)
        self._print_bits_init(bits)
        self.ctx.lvl_dec()

    def _print_bits_docstring(self, bits):
        self.ctx.writeln('"""')
        self.ctx.writeln('%s' % bits.name)
        self.ctx.bline()
        if bits.comment is not None:
            for line in bits.comment.split("\n"):
                self.ctx.writeln(convert_to_reStructuredText(line))
        self.ctx.writeln(
            'Keys are:- %s' % convert_to_reStructuredText(" , ".join(bits._dictionary)))
        self.ctx.bline()
        self.ctx.writeln('"""')
        self.ctx.bline()

    def _print_bits_init(self, bits):
        self.ctx.writeln('def __init__(self):')
        self._print_bits_dictionary(bits)
        self._print_bits_pos_map(bits)

    def _print_bits_dictionary(self, bits):
        self.ctx.lvl_inc()
        self.ctx.writeln('self._dictionary = { ')
        self.ctx.lvl_inc()
        for k in bits._dictionary:
            self.ctx.writeln("'%s':False," % k)

        self.ctx.lvl_dec()
        self.ctx.writeln('}')

    def _print_bits_pos_map(self, bits):
        self.ctx.writeln('self._pos_map = { ')
        self.ctx.lvl_inc()
        for k in bits._pos_map:
            self.ctx.writeln("'%s':%s," % (k, bits._pos_map[k]))
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.lvl_dec()

    def _print_bits_trailer(self, bits):
        self.ctx.bline()
