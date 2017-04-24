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

""" capabilities_printer.py

Print capabilities for bundle package.
"""
from ydkgen.printer.file_printer import FilePrinter


class NamespacePrinter(FilePrinter):
    def __init__(self, ctx):
        super(NamespacePrinter, self).__init__(ctx)
        self.bundle_name = ''
        self.packages = None

    def print_output(self, packages, bundle_name):
        self.packages = packages = [p for p in packages if p.bundle_name == bundle_name]
        self._print_bundle_name(bundle_name)
        self._print_capabilities(packages)
        self._print_entity_lookup(packages)

    def _get_imports(self, packages):
        imports = set()
        for p in packages:
            for e in p.owned_elements:
                if e.stmt.keyword in ('container', 'list'):
                    imports.add(e.get_py_mod_name())
        return imports

    def _print_bundle_name(self, bundle_name):
        self.ctx.writeln('BUNDLE_NAME = "{}"'.format(bundle_name))
        self.ctx.bline()

    def _print_capabilities(self, packages):
        self.ctx.writeln('CAPABILITIES = {')
        self.ctx.lvl_inc()
        for p in self.packages:
            revision = p.stmt.search_one('revision')
            revision = '' if revision is None else revision.arg
            name = p.stmt.arg
            self.ctx.writeln('"{}": "{}",'.format(name, revision))
        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()

    def _print_entity_lookup(self, packages):
        self.ctx.writeln('ENTITY_LOOKUP = {')
        self.ctx.lvl_inc()
        for p in packages:
            ns = p.stmt.search_one('namespace')
            for e in p.owned_elements:
                if all((hasattr(e, 'stmt'), e.stmt is not None,
                        e.stmt.keyword in ('container', 'list'))):
                    self.ctx.writeln('("{}", "{}"): "{}",'
                                     .format(ns.arg, e.stmt.arg, e.fqn()))
                    self.ctx.writeln('("{}", "{}"): "{}",'
                                     .format(p.stmt.arg, e.stmt.arg, e.fqn()))

        self.ctx.lvl_dec()
        self.ctx.writeln('}')
        self.ctx.bline()
