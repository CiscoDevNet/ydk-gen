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

import json
import logging
import os
import shutil

from common import YdkGenException
from ydkgen.builder import ApiModelBuilder, GroupingClassApiModelBuilder, PyangModelBuilder
from . import resolve_profile
from .printer import YdkPrinter

logger = logging.getLogger('ydkgen')
logger.addHandler(logging.NullHandler())


class YdkGenerator(object):
    def generate(self, profile_file, output_directory, ydk_root, groupings_as_class, generate_python):
        """
            Generate ydk-py based in the output_directory using the supplied
            profile_file

            :param str profile_file: The profile file to use
            :param str output_directory: The output directory where the generated ydk python code will be created.
            :param str ydk_root: The ydk root directory. Relative file names in the profile file are resolved relative to this.
            :param bool groupings_as_class: If set to true YANG grouping is converted to a class.
            :raise YdkGenException: if an error has occurred
        """

        self._perform_argument_checks(profile_file, output_directory, ydk_root)
        (ydk_dir, ydk_doc_dir) = self._get_ydk_paths(output_directory, ydk_root)
        resolved_model_dir = self._get_resolved_model_path_and_version(profile_file, ydk_root, ydk_dir)

        pyang_builder = PyangModelBuilder(resolved_model_dir)
        modules = pyang_builder.parse_and_return_modules()

        # create the packages
        packages = self._build_api_model_packages(modules, groupings_as_class)

        # call the language emitter
        language = ''
        if generate_python:
            language = 'python'
        printer = YdkPrinter(ydk_dir, ydk_doc_dir, language)
        printer.emit(packages, pyang_builder.submodules)

    def _perform_argument_checks(self, profile_file, output_directory, ydk_root):
        if profile_file is None or len(profile_file) == 0:
            logger.error('profile_file is None.')
            raise YdkGenException('profile_file cannot be None.')

        if output_directory is None or len(output_directory) == 0:
            logger.error('output_directory is None.')
            raise YdkGenException('output_directory cannot be None.')

        if ydk_root is None or len(ydk_root) == 0:
            logger.error('ydk_root is None.')
            raise YdkGenException('YDKGEN_HOME is not set.')

    def _get_resolved_model_path_and_version(self, profile_file, ydk_root, ydk_dir):
        resolved_model_dir = None
        try:
            with open(profile_file) as json_file:
                profile_data = json.load(json_file)
                version = profile_data['version']
                # print version file
                version_file = os.path.join(ydk_dir, '_version.py')
                with open(version_file, 'w') as version_fd:
                    version_fd.write('__version__ = "{}"'.format(version))

                resolved_model_dir = resolve_profile.resolve_profile(profile_data, ydk_root)
        except IOError as e:
            err_msg = 'Cannot open profile file (%s)' % e.strerror
            logger.error(err_msg)
            raise YdkGenException(err_msg)
        except ValueError as e:
            err_msg = 'Cannot parse profile file (%s)' % e.message
            logger.error(err_msg)
            raise YdkGenException(err_msg)
        return resolved_model_dir

    def _get_ydk_paths(self, output_directory, ydk_root):
        py_sdk_root = output_directory + '/python/'
        py_api_doc_gen = py_sdk_root + '/docsgen'
        py_api_doc = output_directory + '/python/docs_expanded'

        if os.path.isdir(py_sdk_root):
            shutil.rmtree(py_sdk_root)
        if os.path.isdir(py_api_doc):
            shutil.rmtree(py_api_doc)
        if os.path.isfile(py_api_doc_gen + '/getting_started.rst'):
            os.remove(py_api_doc_gen + '/getting_started.rst')

        shutil.copytree(ydk_root + '/sdk/python', output_directory + '/python',
                        ignore=shutil.ignore_patterns('.gitignore', 'ncclient'))


        ydk_dir = '%sydk' % py_sdk_root
        ydk_doc_dir = '%sdocsgen' % py_sdk_root
        return (ydk_dir, ydk_doc_dir)

    def _build_api_model_packages(self, modules, groupings_as_class):
        packages = []

        if not groupings_as_class:
            packages = ApiModelBuilder().generate(modules)
        else:
            packages = GroupingClassApiModelBuilder().generate(
                    modules)
        return packages
