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
   YDK PY converter

'''
import sys
from ydkgen.common import yang_id

from ydkgen.api_model import Class
from ydkgen.common import get_module_name


class NamespacePrinter(object):
    def __init__(self, ctx, one_class_per_module):
        self.ctx = ctx
        self.namespace_list = []
        self.namespace_map = {}
        self.one_class_per_module = one_class_per_module

    def print_output(self, packages):
        self._init_namespace_info(packages)

        self._print_namespaces(self.namespace_list)
        self._print_identity_map(packages)
        self._print_namespaces_map(self.namespace_map)

    def _init_namespace_info(self, packages):
        module_map = {}

        for m in [p.stmt for p in packages]:
            ns = m.search_one('namespace')
            if ns is not None:
                self.namespace_list.append((m.arg.replace('-', '_'), ns.arg, yang_id(m)))
                module_map[m.arg] = ns.arg

        for m in [p.stmt for p in packages]:
            if m.keyword == 'submodule':
                including_module = m.i_including_modulename
                if including_module is not None and including_module in module_map:
                    main_ns = module_map[including_module]
                    self.namespace_list.append((m.arg.replace('-', '_'), main_ns, yang_id(m)))

        for package in packages:
            ns = package.stmt.search_one('namespace')
            for ele in package.owned_elements:
                if hasattr(ele, 'stmt') and ele.stmt is not None:
                    if (ele.stmt.keyword == 'container' or ele.stmt.keyword == 'list'):
                        self.namespace_map[(ns.arg, ele.stmt.arg)] = (ele.get_py_mod_name(self.one_class_per_module), ele.name)
                    elif ele.stmt.keyword == 'rpc':
                        for rpc_child in ele.owned_elements:
                            if isinstance(rpc_child, Class) and rpc_child.stmt.keyword == 'output':
                                self.namespace_map[(ns.arg, ele.stmt.arg)] = (ele.get_py_mod_name(self.one_class_per_module), ele.name)

    def _print_namespaces(self, ns):
        ns = sorted(ns)
        for n in ns:
            self.ctx.writeln("_global_%s_nsp = '%s'" % (n[0], n[1]))
        self.ctx.writeln("_namespaces = { \\")
        for n in ns:
            self.ctx.writeln("'%s' : '%s', " % (n[2], n[1]), 1)
        self.ctx.writeln("}")
        self.ctx.bline()

    def _print_namespaces_map(self, namespace_map):
        self.ctx.writeln("_namespace_package_map = { \\")
        items = namespace_map.items()
        if sys.version_info < (3,):
            items = namespace_map.iteritems()
        for namespace, python_import in sorted(items):
            self.ctx.writeln("('%s', '%s') : 'from %s import %s', " % (namespace[0], namespace[1], python_import[0], python_import[1]))
        self.ctx.writeln("}")
        self.ctx.bline()

    def _print_identity_map(self, packages):
        packages = sorted(packages, key=lambda p:p.name)
        self.ctx.writeln("_identity_map = { \\")
        self.ctx.lvl_inc()
        for package in packages:
            identities = [idx for idx in package.owned_elements if isinstance(
                idx, Class) and idx.is_identity()]
            identities = sorted(identities, key=lambda c: c.name)
            for identity_clazz in identities:
                self.ctx.writeln("('%s', '%s'):('%s', '%s')," % (get_module_name(identity_clazz.stmt), identity_clazz.stmt.arg,
                                                                 identity_clazz.get_py_mod_name(self.one_class_per_module), identity_clazz.qn()))
        self.ctx.lvl_dec()
        self.ctx.writeln("}")
        self.ctx.bline()
