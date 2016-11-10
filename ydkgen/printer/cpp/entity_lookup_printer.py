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
from ydkgen.printer.file_printer import FilePrinter


class EntityLookUpPrinter(FilePrinter):
    def __init__(self, ctx):
        super(EntityLookUpPrinter, self).__init__(ctx)
        self.headers = None
        self.entity_lookup = None
        self.capability_lookup = None

    def print_source(self, packages, bundle_name):
        self.bundle_name = bundle_name
        packages = sorted(packages, key=lambda p: p.name)

        self._init_headers(packages)
        self._init_insert_stmts(packages)
        self._print_headers()
        self._print_get_entity_lookup_func()

    def print_header(self, packages, bundle_name):
        pass

    def _init_headers(self, packages):
        unique_headers = set()
        self._add_common_headers(unique_headers)
        for package in packages:
            self._add_package_headers(unique_headers, package)
        self.headers = list(sorted(unique_headers))

    def _add_common_headers(self, unique_headers):
        unique_headers.add('#include "ydk/core.hpp"')
        unique_headers.add('#include "ydk/types.hpp"')
        unique_headers.add('#include "ydk/codec_provider.hpp"')

    def _add_package_headers(self, unique_headers, package):
        self._add_import_statement(unique_headers, package)
        for imported_type in package.imported_types():
            self._add_import_statement(unique_headers, imported_type)

    def _add_import_statement(self, unique_headers, named_element):
        header_name = named_element.get_cpp_header_name()
        unique_headers.add('#include "%s"' % header_name)

    def _init_insert_stmts(self, packages):
        entity_lookup = {}
        capability_lookup = set()
        for package in packages:
            top_level_entities = self._get_top_level_entities(package)
            self._add_top_level_entities(top_level_entities, entity_lookup)
            mod_rev_tuple = self._get_module_revision(package)
            capability_lookup.add(mod_rev_tuple)
        self.entity_lookup = entity_lookup
        self.capability_lookup = capability_lookup

    def _get_module_revision(self, package):
        module_name, revision = None, ""
        revision_stmt = package.stmt.search_one('revision')
        if revision_stmt:
            revision = revision_stmt.arg
        module_name = package.stmt.arg

        return (module_name, revision)

    def _get_top_level_entities(self, package):
        return [entity
                for entity in package.owned_elements
                if hasattr(entity, 'stmt') and
                entity.stmt.keyword in ('container', 'list')]

    def _add_top_level_entities(self, top_level_entities, entity_lookup):
        for top_entity in top_level_entities:
            path = '/%s:%s' % (top_entity.module.arg, top_entity.stmt.arg)
            entity_lookup[path] = top_entity.fully_qualified_cpp_name()

            ns_stmt = top_entity.module.search_one('namespace')
            if ns_stmt:
                ns = '%s:%s' % (ns_stmt.arg, top_entity.stmt.arg)
                entity_lookup[ns] = top_entity.fully_qualified_cpp_name()

    def _print_headers(self):
        for header in self.headers:
            self.ctx.writeln(header)

    def _print_get_entity_lookup_func(self):
        self._print_get_entity_lookup_func_header()
        self._print_get_entity_lookup_func_body()
        self._print_get_entity_lookup_func_trailer()

    def _print_get_entity_lookup_func_header(self):
        self.ctx.bline()
        self.ctx.writelns(["namespace ydk {\n",
                           "void",
                           "augment_lookup_tables()",
                           "{"])

        self.ctx.bline()
        self.ctx.lvl_inc()

    def _print_get_entity_lookup_func_body(self):
        for path in self.entity_lookup:
            self._print_insert_statement(path)

        for (module_name, revision) in self.capability_lookup:
            self._print_emplace_statement(module_name, revision)

    def _print_insert_statement(self, path):
        qualified_name = self.entity_lookup[path]
        self.ctx.writeln("ydk_global_entities.insert(std::string{\"%s\"},"
                         "std::make_unique<%s>());"
                         % (path, qualified_name))

    def _print_emplace_statement(self, module_name, revision):
        self.ctx.writeln("ydk_global_caps.emplace_back("
                         "path::Capability{std::string{\"%s\"},"
                         "\"%s\", {}, {}});"
                         % (module_name, revision))

    def _print_get_entity_lookup_func_trailer(self):
        self.ctx.lvl_dec()
        self.ctx.writelns(['}\n'] * 2)
