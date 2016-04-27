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


import os
import shutil

from . import printer_context
from ydkgen.common import yang_id
from ydkgen.common import convert_to_reStructuredText
from .language_factory import LanguageFactory
from ydkgen.common import get_rst_file_name
from ydkgen.api_model import Bits, Class, Enum, Package


class _EmitArgs:

    def __init__(self, ctx, package, extra_args=None):
        self.ctx = ctx
        self.package = package
        self.extra_args = extra_args


# ============================================================
# Plugin class definitions
# ============================================================
class YdkPrinter():

    def __init__(self, ydk_dir, ydk_doc_dir, language):
        self.ydk_dir = ydk_dir
        self.ydk_doc_dir = ydk_doc_dir
        self.language = language

    def emit(self, packages, submodules):
        self.ypy_ctx = None
        self.packages = []
        self.models_dir = ''
        self.test_dir = ''
        
        self.packages = packages
        self.packages = sorted(self.packages, key=lambda package: package.name)
        self.deviation_packages = [p for p in self.packages if hasattr(p, 'is_deviation')]
        self.packages = [p for p in self.packages if not hasattr(p, 'is_deviation')]

        self.submodules = []
        for sub in submodules:
            package = Package()
            sub.i_package = package
            package.stmt = sub
            self.submodules.append(package)
        
        self.initialize_print_environment()
        self.print_files()

    def initialize_print_environment(self):
        self.initialize_top_level_directories()
        self.initialize_printer_context()

    def initialize_top_level_directories(self):
        self.models_dir = self.initialize_output_directory(
            self.ydk_dir + '/models', True)
        self.test_dir = self.initialize_output_directory(
            self.ydk_dir + '/tests', True)
        self.deviation_dir = self.initialize_output_directory(
            self.models_dir + '/_deviate', True)

    def initialize_printer_context(self):
        self.ypy_ctx = printer_context.PrintCtx()
        self.ypy_ctx.meta = True
        self.ypy_ctx.tab_size = 4
        self.ypy_ctx.printer = LanguageFactory().get_printer(self.language)(self.ypy_ctx)

    def print_files(self):
        self.print_modules()
        self.print_init_file(self.models_dir)
        self.print_yang_ns_file()
        self.print_import_tests_file()
        self.print_deviate_file()

        # RST Documentation
        if self.ydk_doc_dir is not None:
            self.print_python_rst_ydk_models()

    def print_modules(self):
        only_modules = [package.stmt for package in self.packages]
        size = len(only_modules)

        for index, package in enumerate(self.packages):
            self.print_module(index, package, size)

    def print_module(self, index, package, size):
        print 'Processing %d of %d %s' % (index + 1, size, package.stmt.pos.ref)

        py_mod_name = package.get_py_mod_name()
        sub = py_mod_name[len('ydk.models.'): py_mod_name.rfind('.')]

        module_dir = self.initialize_output_directory(
            '%s/%s' % (self.models_dir, sub))
        meta_dir = self.initialize_output_directory(module_dir + '/_meta')
        test_output_dir = self.initialize_output_directory(
            '%s/%s' % (self.test_dir, sub))

        # RST Documentation
        self.print_python_module(package, index, module_dir, size, sub)
        self.print_meta_module(package, meta_dir)
        self.print_test_module(package, test_output_dir)
        if self.ydk_doc_dir is not None:
            self.print_python_rst_module(package)

    def print_python_rst_module(self, package):
        if self.ydk_doc_dir is None:
            return
        # Skip generating documentation for empty modules
        if len(package.owned_elements) == 0:
            return

        def _walk_n_print(named_element, p):
            self.print_file(get_python_rst_file_name(p, named_element),
                            emit_python_rst,
                            _EmitArgs(self.ypy_ctx, named_element))

            for owned_element in named_element.owned_elements:
                if isinstance(owned_element, Class) or isinstance(owned_element, Enum):
                    _walk_n_print(owned_element, p)

        _walk_n_print(package, self.ydk_doc_dir)

    def print_python_rst_ydk_models(self):
        if self.ydk_doc_dir is None:
            return
        packages = [p for p in self.packages if len(p.owned_elements) > 0]

        self.print_file(get_python_rst_ydk_models_file_name(self.ydk_doc_dir),
                        emit_ydk_models_rst,
                        _EmitArgs(self.ypy_ctx, packages))

    def print_python_module(self, package, index, path, size, sub):
        self.print_init_file(path)

        package.parent_pkg_name = sub
        self.print_file(get_python_module_file_name(path, package),
                        emit_module,
                        _EmitArgs(self.ypy_ctx, package, (sub, package.name)))

    def print_meta_module(self, package, path):
        self.print_init_file(path)
        self.print_file(get_meta_module_file_name(path, package),
                        emit_meta,
                        _EmitArgs(self.ypy_ctx, package))

    def print_test_module(self, package, path):
        self.print_file(get_test_module_file_name(path, package),
                        emit_test_module,
                        _EmitArgs(self.ypy_ctx, package))

    def print_yang_ns_file(self):
        packages = self.packages + self.deviation_packages + self.submodules
        self.print_file(get_yang_ns_file_name(self.models_dir),
                        emit_yang_ns,
                        _EmitArgs(self.ypy_ctx, packages))

    def print_deviate_file(self):
        self.print_init_file(self.deviation_dir)
        for package in self.deviation_packages:
            self.print_file(get_meta_module_file_name(self.deviation_dir, package),
                            emit_deviation,
                            _EmitArgs(self.ypy_ctx, package))

    def print_import_tests_file(self):
        self.print_file(get_import_test_file_name(self.test_dir),
                        emit_importests,
                        _EmitArgs(self.ypy_ctx, self.packages))

    def print_init_file(self, path):
        file_name = get_init_file_name(path)
        if not os.path.isfile(file_name):
            self.print_file(file_name)

    def print_file(self, path, emit_func=None, emit_args=None):
        with open(path, 'w+') as file_descriptor:
            if emit_func is not None and emit_args is not None:
                emit_args.ctx.fd = file_descriptor
                if emit_args.extra_args is None:
                    emit_func(emit_args.ctx, emit_args.package)
                else:
                    emit_func(
                        emit_args.ctx, emit_args.package, emit_args.extra_args)

    def initialize_output_directory(self, path, delete_if_exists=False):
        if delete_if_exists:
            if os.path.exists(path):
                shutil.rmtree(path)
        if not os.path.isdir(path):
            os.mkdir(path)
        return path


def get_init_file_name(path):
    return path + '/__init__.py'


def get_yang_ns_file_name(path):
    return path + '/_yang_ns.py'


def get_import_test_file_name(path):
    return path + '/import_tests.py'


def get_python_rst_file_name(path, named_element):
    return '%s/%s.rst' % (path, get_rst_file_name(named_element))


def get_python_rst_ydk_models_file_name(path):
    return '%s/ydk.models.rst' % path


def get_python_module_file_name(path, package):
    return '%s/%s.py' % (path, package.name)


def get_meta_module_file_name(path, package):
    return '%s/_%s.py' % (path, package.name)


def get_test_module_file_name(path, package):
    return '%s/%sTest.py' % (path, package.stmt.arg.replace('-', '_'))



def emit_comment(ctx, comment):
    ctx.printer.comment(lines=comment.split('\n'))


def emit_module_header(ctx, package, mheader=None, is_meta=False):
    # ::::::::::::::::::::::::::::::::::::::::
    # Print the header
    # ::::::::::::::::::::::::::::::::::::::::
    s = package.stmt
    if is_meta:
        rpcs = [idx for idx in package.owned_elements if isinstance(idx, Class) and idx.is_rpc()]
        anyxml_import = ''
        if len(rpcs) > 0:
            anyxml_import = ', ANYXML_CLASS'
        ctx.printer.meta_header(anyxml_import)
    else:
        comment = s.search_one('description')
        ctx.writeln('""" %s ' % package.name)
        ctx.bline()

        if comment is not None and not is_meta:
            ctx.comment = comment.arg
            for line in ctx.comment.split('\n'):
                ctx.writeln(convert_to_reStructuredText(line))
        ctx.bline()
        ctx.writeln('"""')
        ctx.printer.header(mheader)
        ctx.printer.imports(package)
    ctx.root = s
    ctx.augment_path = ''
    ctx.aug_stmt = None
    ctx.module_name = yang_id(s)
    ctx.module = s
    # get the yang meta information.
    prefix = s.search_one('prefix')
    if prefix is not None:
        ctx.prefix = yang_id(prefix)
    namespace = s.search_one('namespace')
    if namespace is not None:
        ctx.namespace = yang_id(namespace)
    org = s.search_one('organization')
    if org is not None:
        ctx.organization = yang_id(org)
    contact = s.search_one('contact')
    if contact is not None:
        ctx.contact = yang_id(contact)
    revision = s.search_one('revision')
    if revision is not None:
        ctx.revision = yang_id(revision)

    ctx.ns += [(yang_id(s), ctx.namespace)]


def emit_yang_ns(ctx, packages):

    ctx.printer.print_yang_ns_header()
    ns_list = []
    module_map = {}
    namespace_map = {}
    for m in [p.stmt for p in packages]:
        ns = m.search_one('namespace')        
        if ns is not None:
            ns_list.append((m.arg.replace('-', '_'), ns.arg, yang_id(m)))
            module_map[m.arg] = ns.arg

    for m in [p.stmt for p in packages]:
        if m.keyword == 'submodule':
            including_module = m.i_including_modulename
            if including_module is not None and including_module in module_map:
                main_ns = module_map[including_module]
                ns_list.append((m.arg.replace('-', '_'), main_ns, yang_id(m)))

    for package in packages:
        ns = package.stmt.search_one('namespace')
        for ele in package.owned_elements:
            if hasattr(ele, 'stmt') and ele.stmt is not None and (ele.stmt.keyword == 'container' or ele.stmt.keyword == 'list'):
                namespace_map[(ns.arg, ele.stmt.arg)] = (package.get_py_mod_name(), ele.name)


    ctx.printer.print_namespaces(ns_list)
    ctx.printer.print_identity_map(packages)
    ctx.printer.print_namespaces_map(namespace_map)


def emit_importests(ctx, packages):
    ctx.printer.print_import_tests(packages)


def emit_python_rst(ctx, named_element):
    ctx.printer.print_python_rst(named_element)


def emit_ydk_models_rst(ctx, packages):
    ctx.printer.print_ydk_models_rst(packages)


def emit_module(ctx, package, mheader):

    emit_module_header(ctx, package, mheader=mheader)

    if package is not None:
        emit_module_enums(ctx, package)
        emit_module_bits(ctx, package)
        emit_module_classes(ctx, package)
        ctx.bline()


def emit_module_enums(ctx, package):
    enumz = []
    enumz.extend(
        [element for element in package.owned_elements if isinstance(element, Enum)])
    for nested_enumz in sorted(enumz, key=lambda element: element.name):
        ctx.printer.print_enum(nested_enumz)


def emit_module_bits(ctx, package):
    bits = []
    bits.extend(
        [bit for bit in package.owned_elements if isinstance(bit, Bits)])
    for bit in sorted(bits, key=lambda bit: bit.name):
        ctx.printer.print_bits(bit)


def emit_module_classes(ctx, package):
    ctx.printer.print_classes_at_same_level(
        [clazz for clazz in package.owned_elements if isinstance(clazz, Class)])


def emit_test_module(ctx, package):
    ctx.printer.print_testcases(package)


def emit_meta(ctx, package):
    ctx.print_meta = True
    emit_module_header(ctx, package, is_meta=True)
    if package is not None:
        emit_meta_table_open(ctx)
        for nested_enumz in [e for e in package.owned_elements if isinstance(e, Enum)]:
            ctx.printer.print_enum_meta(nested_enumz)
        ctx.printer.print_classes_meta([c for c in package.owned_elements if isinstance(c, Class)])
        emit_meta_table_close(ctx)
        ctx.printer.print_classes_meta_parents(
            [c for c in package.owned_elements if isinstance(c, Class)])


def emit_meta_table_open(ctx):
    ctx.writeln('_meta_table = {')
    ctx.lvl_inc()


def emit_meta_table_close(ctx):
    ctx.lvl_dec()
    ctx.writeln('}')

def emit_deviation(ctx, package):
    ctx.printer.print_deviation(package)
