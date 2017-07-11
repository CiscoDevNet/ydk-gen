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
entity_lookup_printer.py

 Prints top entity lookup map

"""
from ydkgen.api_model import Class
from ydkgen.common import snake_case, get_include_guard_name, get_module_name
from ydkgen.printer.file_printer import FilePrinter


class EntityLookUpPrinter(FilePrinter):
    def __init__(self, ctx, module_namespace_lookup):
        super(EntityLookUpPrinter, self).__init__(ctx)
        self.headers = None
        self.entity_lookup = None
        self.capability_lookup = None
        self.module_namespace_lookup = module_namespace_lookup

    def print_header(self, bundle_name):
        self.bundle_name = bundle_name
        self._print_include_guard_header(get_include_guard_name('entity_lookup'))
        self.ctx.writeln('#include <map>')
        self.ctx.writeln('#include <string>')
        self.ctx.bline()
        self.ctx.writeln('namespace %s' % bundle_name)
        self.ctx.writeln('{')
        self.ctx.bline()
        self.ctx.writeln("void {}_augment_lookup_tables();".format(snake_case(self.bundle_name)))
        self.ctx.writeln("extern std::map<std::pair<std::string, std::string>, std::string> {0}_namespace_identity_lookup;".format(snake_case(self.bundle_name)))
        self.ctx.bline()
        self.ctx.writeln('}')
        self._print_include_guard_trailer(get_include_guard_name('entity_lookup'))

    def print_source(self, packages, bundle_name):
        self.bundle_name = bundle_name
        packages = sorted(packages, key=lambda p: p.name)

        self._init_headers(packages)
        self._init_insert_stmts(packages)
        self._print_headers()
        self._print_capabilities_lookup_func()
        self._print_namespace_identity_lookup(packages)
        self.ctx.writelns('}\n')

    def _init_headers(self, packages):
        unique_headers = set()
        self._add_common_headers(unique_headers)
        self.headers = list(sorted(unique_headers))

    def _add_common_headers(self, unique_headers):
        unique_headers.add('#include <ydk/entity_lookup.hpp>')
        unique_headers.add('#include <ydk/path_api.hpp>')
        unique_headers.add('#include "generated_entity_lookup.hpp"')

    def _init_insert_stmts(self, packages):
        capability_lookup = set()
        for package in packages:
            mod_rev_tuple = self._get_module_revision(package)
            capability_lookup.add(mod_rev_tuple)
        self.capability_lookup = capability_lookup

    def _get_module_revision(self, package):
        revision = ""
        revision_stmt = package.stmt.search_one('revision')
        if revision_stmt:
            revision = revision_stmt.arg
        module_name = package.stmt.arg

        return (module_name, revision)

    def _print_headers(self):
        for header in self.headers:
            self.ctx.writeln(header)

    def _print_capabilities_lookup_func(self):
        self._print_capabilities_lookup_func_header()
        self._print_capabilities_lookup_func_body()
        self._print_capabilities_lookup_func_trailer()

    def _print_capabilities_lookup_func_header(self):
        self.ctx.bline()
        self.ctx.writeln('namespace %s' % self.bundle_name)
        self.ctx.writeln('{')
        self.ctx.bline()
        self.ctx.writelns(["void {}_augment_lookup_tables()".format(snake_case(self.bundle_name)),
                           "{"])

        self.ctx.bline()
        self.ctx.lvl_inc()

    def _print_capabilities_lookup_func_body(self):
        self.ctx.bline()
        for (module_name, revision) in self.capability_lookup:
            self._print_push_back_statement(module_name, revision)
        self.ctx.bline()

    def _print_push_back_statement(self, module_name, revision):
        self.ctx.writeln("ydk::ydk_global_capabilities.push_back("
                         "ydk::path::Capability{std::string{\"%s\"},"
                         "\"%s\", {}, {}});"
                         % (module_name, revision))

    def _print_capabilities_lookup_func_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.writelns('}\n')

    def _print_namespace_identity_lookup(self, packages):
        self.ctx.bline()
        self.ctx.writeln(
            "std::map<std::pair<std::string, std::string>, std::string> %s_namespace_identity_lookup {" % (
                snake_case(self.bundle_name)))
        self.ctx.lvl_inc()
        for package in packages:
            identities = self._get_identities(package)
            for identity in identities:
                self._print_namespace_identity_lookup_statement(identity)
        self.ctx.lvl_dec()
        self.ctx.writeln("};")
        self.ctx.bline()

    def _print_namespace_identity_lookup_statement(self, identity):
        module_name = get_module_name(identity.stmt)
        namespace = self.module_namespace_lookup[module_name]
        self.ctx.writeln('{ {"%s", "%s"},  "%s"},' % (identity.stmt.arg, namespace, module_name))

    def _get_identities(self, element):
        identities = set()
        for child in element.owned_elements:
            if isinstance(child, Class) and child.is_identity():
                identities.add(child)
            identities = identities.union(self._get_identities(child))
        return identities

