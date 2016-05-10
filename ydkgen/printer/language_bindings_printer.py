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


class _EmitArgs:

    def __init__(self, ctx, package, extra_args=None):
        self.ctx = ctx
        self.package = package
        self.extra_args = extra_args


class LanguageBindingsPrinter(object):

    def __init__(self, ydk_root_dir):
        self.ydk_dir = ydk_root_dir + '/ydk/'
        self.ydk_doc_dir = ydk_root_dir + '/docsgen'

    def emit(self, packages):
        self.ypy_ctx = None
        self.packages = []
        self.models_dir = ''
        self.test_dir = ''

        self.packages = packages
        self.packages = sorted(self.packages, key=lambda package: package.name)
        self.deviation_packages = [p for p in self.packages if hasattr(p, 'is_deviation')]
        self.packages = [p for p in self.packages if not hasattr(p, 'is_deviation')]

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
