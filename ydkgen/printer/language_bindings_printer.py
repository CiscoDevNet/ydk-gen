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

import abc
import os
import shutil

from ydkgen.printer import printer_context
from ydkgen.api_model import Class


class _EmitArgs:

    def __init__(self, ctx, package, extra_args=None):
        self.ctx = ctx
        self.package = package
        self.extra_args = extra_args


class LanguageBindingsPrinter(object):

    def __init__(self, ydk_root_dir, bundle, generate_tests, one_class_per_module, sort_clazz=False):
        self.ydk_dir = os.path.join(ydk_root_dir, 'ydk')
        self.ydk_doc_dir = os.path.join(ydk_root_dir, 'docsgen')
        self.bundle = bundle
        self.bundle_name = bundle.name
        self.bundle_version = bundle.str_version
        self.sort_clazz = sort_clazz
        self.generate_tests = generate_tests
        self.one_class_per_module = one_class_per_module

    def emit(self, packages, classes_per_source_file):
        self.ypy_ctx = None
        self.packages = []
        self.models_dir = ''
        self.test_dir = ''
        self.sub_dir = ''
        self.aug_dir = ''
        self.classes_per_source_file = classes_per_source_file

        self.packages = packages
        self.packages = sorted(self.packages, key=lambda package: package.name)
        self.deviation_packages = [p for p in self.packages if p.is_deviation is True]
        self.packages = [p for p in self.packages if p.is_deviation is not True]
        self.identity_subclasses = self._get_identity_subclasses_map()
        self.packages = self._filter_bundle_pkgs()
        self.initialize_print_environment()
        return self.print_files()

    def initialize_print_environment(self):
        self.initialize_top_level_directories()
        self.initialize_printer_context()

    def initialize_top_level_directories(self):
        self.models_dir = self.initialize_output_directory(
            os.path.join(self.ydk_dir, 'models'), True)
        self.deviation_dir = self.initialize_output_directory(
            self.models_dir + '/_deviate', True)
        if self.bundle:
            self.models_dir = self.initialize_output_directory(
                os.path.join(self.models_dir, self.bundle_name), True)
            self.sub_dir = self.models_dir
            self.aug_dir = self.initialize_output_directory(
                self.sub_dir + '/_aug', True)
            self.sub_dir = self.models_dir

        self.test_dir = self.initialize_output_directory(
            os.path.join(self.models_dir, 'test'), False)


    def initialize_printer_context(self):
        self.ypy_ctx = printer_context.PrinterContext()
        self.ypy_ctx.meta = True
        self.ypy_ctx.tab_size = 4

    @abc.abstractmethod
    def print_files(self):
        pass

    def print_file(self, path, emit_func=None, emit_args=None):
        with open(path, 'w+') as file_descriptor:
            if emit_func is not None and emit_args is not None:
                emit_args.ctx.fd = file_descriptor
                if emit_args.extra_args is None:
                    emit_func(emit_args.ctx, emit_args.package)
                else:
                    emit_func(emit_args.ctx, emit_args.package, emit_args.extra_args)

    def initialize_output_directory(self, path, delete_if_exists=False):
        if delete_if_exists:
            if os.path.exists(path):
                shutil.rmtree(path)
        if not os.path.isdir(path):
            os.mkdir(path)
        return path

    def is_empty_package(self, package):
        empty = True
        for element in package.owned_elements:
            if isinstance(element, Class) and not element.is_identity():
                empty = False
                break
        return empty

    def _get_identity_subclasses_map(self):
        identity_subclasses = {}
        identity_tuples = self._get_identity_tuples()
        for (child_identity, base_identities) in identity_tuples:
            for base_identity in base_identities:
                if id(base_identity) in identity_subclasses:
                    existing_child_identities = identity_subclasses[id(base_identity)]
                    existing_child_identities.append(child_identity)
                    identity_subclasses[id(base_identity)] = existing_child_identities
                else:
                    identity_subclasses[id(base_identity)] = [child_identity]
        return identity_subclasses

    def _get_identity_tuples(self):
        identity_subclasses = []
        for package in self.packages:
            identity_subclasses.extend(self._get_identity_subclasses_for_package(package))
        return identity_subclasses

    def _get_identity_subclasses_for_package(self, element):
        identity_subclasses = []
        for subelement in element.owned_elements:
            if isinstance(subelement, Class):
                if subelement.is_identity():
                    identity_subclasses.append((subelement, subelement.extends))
                else:
                    identity_subclasses.extend(self._get_identity_subclasses_for_package(subelement))
        return identity_subclasses

    def _filter_bundle_pkgs(self):
        bundle_pkgs = {}
        for pkg in self.packages:
            if pkg.bundle_name == self.bundle_name:
                # we have multiple models being augmented.
                if hasattr(pkg.stmt, 'i_aug_targets'):
                    for target in pkg.stmt.i_aug_targets:
                        aug_pkg = target.i_package
                        if aug_pkg.bundle_name != self.bundle_name:
                            # augmenting models in existing bundle.
                            aug_pkg.aug_bundle_name = aug_pkg.bundle_name
                            aug_pkg.bundle_name = self.bundle_name
                            bundle_pkgs[id(aug_pkg)] = aug_pkg

                bundle_pkgs[id(pkg)] = pkg

        return list(bundle_pkgs.values())

