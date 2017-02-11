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
Generate YDK core library, bundle packages.
"""
from __future__ import print_function
from distutils import dir_util
from subprocess import call

import os, sys, shutil
import json
import logging
import tempfile
import fileinput

from .common import YdkGenException, iscppkeyword, ispythonkeyword
from ydkgen.builder import (ApiModelBuilder, GroupingClassApiModelBuilder,
                            PyangModelBuilder, SubModuleBuilder)
from .resolver import bundle_resolver, bundle_translator
from ydkgen.printer import printer_factory


logger = logging.getLogger('ydkgen')
logger.addHandler(logging.NullHandler())


class YdkGenerator(object):
    """ YdkGenerator class, based in the output_directory using the supplied
        profile description file.

        Attributes:
            output_dir (str): The output directory for generated APIs.
            ydk_root   (str): The ydk root directory. Relative file names in
                              the profile file are resolved relative to this.
            groupings_as_class (bool): If set to true, YANG grouping is
                                       converted to a class.
            language (str): Language for generated APIs
            pkg_type (str): Package type for generated APIs.
                            Valid options for bundle approach are: 'core',
                                                                   'bundle'.
            sort_clazz (bool): Option to sort generated classes at same leve.

        Raises:
            YdkGenException: If an error has occurred
    """

    def __init__(self, output_dir, ydk_root, groupings_as_class, generate_tests, language, pkg_type, sort_clazz=False):

        _check_generator_args(output_dir, ydk_root, language, pkg_type)

        self.output_dir = output_dir
        self.ydk_root = ydk_root
        self.groupings_as_class = groupings_as_class
        self.language = language
        self.pkg_type = pkg_type
        self.generate_tests = generate_tests
        self.sort_clazz = sort_clazz
        if self.language == 'cpp':
            self.iskeyword = iscppkeyword
        else:
            self.iskeyword = ispythonkeyword

    def generate(self, description_file=None):
        """ Generate ydk bundle packages or ydk core library.

        Args:
            description_file (str): Path to description file.

        Returns:
            Generated APIs root directory for core and profile package.
            List of Generated APIs root directories for bundle packages.
        """
        if self.pkg_type == 'bundle':
            return self._generate_bundle(description_file)

        elif self.pkg_type == 'core':
            return self._generate_core()

        else:
            raise YdkGenException('Invalid package type specified: %s' % self.pkg_type)

    def _generate_bundle(self, profile_file):
        """ Generate ydk bundle package. First translate profile file to
        bundle syntax.

        Args:
            profile_file (str): Path to profile description file.

        Returns:
            gen_api_root (str): Root directory for generated APIs.
        """
        _check_description_file(profile_file)
        tmp_file = tempfile.mkstemp(suffix='.bundle')[-1]
        bundle_translator.translate(profile_file, tmp_file, self.ydk_root)

        resolver = bundle_resolver.Resolver(self.output_dir, self.ydk_root, self.iskeyword)
        curr_bundle, all_bundles = resolver.resolve(tmp_file)

        api_pkgs = self._get_api_pkgs(curr_bundle.resolved_models_dir)
        if len(api_pkgs) == 0:
            raise YdkGenException('No YANG models were found. Please check your JSON profile file to make sure it is valid')

        _set_api_pkg_sub_name(all_bundles, api_pkgs, curr_bundle)
        gen_api_root = self._init_dirs(api_pkgs=api_pkgs, bundle=curr_bundle)

        self._print_pkgs(api_pkgs, gen_api_root, curr_bundle.name)

        os.remove(tmp_file)

        return gen_api_root

    def _generate_core(self):
        """ Generate ydk core package.
            Copy core library sdk template to generated APIs root directory.

        Returns:
            gen_api_root (str): Root directory for generated APIs.
        """
        gen_api_root = self._init_dirs(pkg_name='ydk', pkg_type='core')


        return gen_api_root

    def _get_api_pkgs(self, resolved_model_dir):
        """ Return api packages for resolved YANG modules. Each module will be
            represented as an api package.

        Args:
            resolved_model_dir (str): Path to resolved YANG modules.

        Returns:
            api_pkgs (List[.api_model.Package]): List of api packages.
        """

        pyang_builder = PyangModelBuilder(resolved_model_dir)
        modules = pyang_builder.parse_and_return_modules()

        # build api model packages
        if not self.groupings_as_class:
            api_pkgs = ApiModelBuilder(self.iskeyword, self.language).generate(modules)
        else:
            api_pkgs = GroupingClassApiModelBuilder(self.iskeyword, self.language).generate(modules)
        api_pkgs.extend(
            SubModuleBuilder().generate(pyang_builder.get_submodules(), self.iskeyword))

        return api_pkgs

    def _print_pkgs(self, pkgs, output_dir, bundle_name=''):
        """ Emit generated APIs.

        Args:
            pkgs (List[.api_model.Package]): List of api packages to print.
            pkg_name (str): Package name for generated APIs.
                            For example 'ydk_bgp', 'ydk_ietf'.
        """
        factory = printer_factory.PrinterFactory()
        ydk_printer = factory.get_printer(self.language)(output_dir, bundle_name, self.generate_tests, self.sort_clazz)
        ydk_printer.emit(pkgs)

    # Initialize generated API directory ######################################
    def _init_dirs(self, api_pkgs=None, pkg_name=None, pkg_type=None, bundle=None):
        """ Initialize and return generated APIs root directory.

        Args:
            pkg_name (str): Package name for generated APIs.
            pkg_type (str): Sdk template type to be copied from.
            bundle (bundle_resolver.Bundle): Bundle instance.
        """
        if bundle:
            return self._init_bundle_dirs(api_pkgs, bundle)
        else:
            return self._init_gen_api_dirs(pkg_name, pkg_type)

    def _init_gen_api_dirs(self, pkg_name=None, pkg_type=None):
        """ Initialize and return generated APIs root directory.
            If pkg_name is specified, the gen_api_root is:
                <self.output_dir>/<self.language>/<pkg_name>
            otherwise, the gen_api_root is:
                <self.output_dir>/<self.language>

            Args:
                pkg_name (str): Package name for generated API, if specified.
                pkg_type (str): Sdk template type to copied from, if specified.
                                Valid options are: core, packages.
        """
        gen_api_root = os.path.join(self.output_dir, self.language)
        if pkg_name:
            gen_api_root = os.path.join(gen_api_root, pkg_name)

        # clean up gen_api_root
        if not os.path.isdir(gen_api_root):
            os.makedirs(gen_api_root)

        self._copy_sdk_template(gen_api_root, pkg_type)

        return gen_api_root

    def _init_bundle_dirs(self, api_pkgs, bundle):
        """ Initialize generated API directory for bundle approach.

        Args:
            bundle (bundle_resolver.Bundle): Bundle instance.

        Returns:
            gen_api_root (str): Root directory for generated APIs.
        """
        gen_api_root = self._init_gen_api_dirs(bundle.name + '-bundle', 'packages')

        if self.language == 'python':
            _modify_python_setup(gen_api_root,
                                 'ydk-models-%s' % bundle.name,
                                 bundle.str_version,
                                 bundle.dependencies)

        elif self.language == 'cpp':
            _modify_cpp_cmake(gen_api_root,
                              bundle.name,
                              api_pkgs,
                              bundle.models,
                              bundle.version)
        # write init file for bundle models directory.
        bundle_model_dir = os.path.join(gen_api_root, 'ydk')
        os.mkdir(bundle_model_dir)

        return gen_api_root

    def _copy_sdk_template(self, gen_api_root, pkg_type):
        """ Copy sdk template to gen_api_root directory.

        Args:
            gen_api_root (str): Root directory for generated APIs.
            pkg_type (str): Sdk template to copied from.
                            Valid options are: core, packages.
        """
        target_dir = os.path.join(self.ydk_root, 'sdk', self.language)
        if self.language == 'python':
            target_dir = os.path.join(target_dir, pkg_type)
        elif self.language == 'cpp':
            if pkg_type == 'packages':
                target_dir = os.path.join(target_dir, pkg_type)
            elif pkg_type == 'core':
                target_dir = os.path.join(target_dir, 'core')
        shutil.rmtree(gen_api_root)
        logger.debug('Copying %s to %s' % (target_dir, gen_api_root))
        dir_util.copy_tree(target_dir, gen_api_root)


def _set_api_pkg_sub_name(bundles, api_pkgs, curr_bundle):
    """ Set nmsp_pkg for api packages.

    Args:
        bundles (List[.resolver.bundle_resolver.Bundle]): Bundle instance.
        api_pkgs (List[.api_model.Package]): List of api packages.

    """
    # add API for model being augmented to bundle
    for pkg in api_pkgs:
        pkg.curr_bundle_name = curr_bundle.name
        for bundle in bundles:
            for module in bundle.models:
                if pkg.name == module.pkg_name:
                    pkg.bundle_name = bundle.name


def _modify_python_setup(gen_api_root, pkg_name, version, dependencies=None):
    """ Modify setup.py template for python packages. Replace package name
        and version number in setup.py located in <gen_api_root>/setup.py.
        If dependencies are specified, $DEPENDENCY$ in setup.py will be replaced.
        The ``fileinput`` module redirect stdout back to input file.

    Args:
        gen_api_root (str): Root directory for generated APIs.
        pkg_name (str): Package name for generated APIs.
        version (str): Package version for generated APIs.
    """
    setup_file = os.path.join(gen_api_root, 'setup.py')
    replaced_package = False
    replaced_version = False
    replaced_dependencies = False
    for line in fileinput.input(setup_file, inplace=True):
        if not replaced_package and "$PACKAGE$" in line:
            replaced_package = True
            print(line.replace("$PACKAGE$", pkg_name), end='')
        elif not replaced_version and "$VERSION$" in line:
            replaced_version = True
            print(line.replace("$VERSION$", version), end='')
        elif not replaced_dependencies and "$DEPENDENCY$" in line:
            replaced_dependencies = True
            if dependencies:
                additional_requires = ["'ydk-models-%s>=%s'" % (d.name, d.str_version)
                                       for d in dependencies]
                print(line.replace("'$DEPENDENCY$'", ", ".join(additional_requires)))
            else:
                print(line.replace("'$DEPENDENCY$'", ""))
        else:
            print(line, end='')


def _modify_cpp_cmake(gen_api_root, bundle_name, packages, models, version, descriptions=""):
    """ Modify CMakeLists.txt template for cpp libraries.

    Args:
        gen_api_root (str): Root directory for generated APIs.
        bundle_name (str): Package name for generated APIs.
        version (str): Package version for generated APIs.
    """
    cmake_file = os.path.join(gen_api_root, 'CMakeLists.txt')

    files = _get_cpp_files(packages, models, {'source':'cpp', 'header':'hpp'})
    for line in fileinput.input(cmake_file, inplace=True):
        if "@DESCRIPTIONS@" in line:
            print(line.replace("@DESCRIPTIONS@", descriptions), end='')
        elif "@BRIEF_NAME@" in line:
            line = line.replace("@BRIEF_NAME@", bundle_name)
            if "@SOURCE_FILES@" in line:
                line = line.replace("@SOURCE_FILES@", files['source'])
            elif "@HEADER_FILES@" in line:
                line = line.replace("@HEADER_FILES@", files['header'])
            print(line, end='')
        elif "@BRIEF_NAME_CAP@" in line:
            print(line.replace("@BRIEF_NAME_CAP@", bundle_name.upper()), end='')
        else:
            print(line, end='')


def _get_cpp_files(packages, models, extensions):
    files = {}
    for model in models:
        skip = False
        if 'deviation' in model.pkg_name:
            skip = True
        else:
            for package in packages:
                if (package.name == model.pkg_name) and (len(package.owned_elements) == 0):
                    skip = True
                    break
        if skip:
            continue

        for key, val in extensions.items():
            file_name = 'ydk/models/%s.%s' % (model.pkg_name, val)
            if key not in files:
                files[key] = [file_name]
            else:
                files[key].append(file_name)
    for key, val in files.items():
        files[key] = ' '.join(files[key])
    return files


# Generator checks #####################################################
def _check_generator_args(output_dir, ydk_root, language, pkg_type):
    """ Check generator arguments.

    Args:
        output_dir (str): The output directory for generated APIs.
        ydk_root (str): The ydk root directory.
        language (str): Language for generated APIs.
        pkg_type (str): Package type for generated APIs.
                        Valid options for bundle approach are: 'core',
                                                               'bundle'.
    Raises:
        YdkGenException: If invalid arguments are passed in.
    """
    if language != 'cpp' and language != 'python':
        raise YdkGenException('Language {0} not supported'.format(language))

#    if language != 'python' and pkg_type == 'bundle':
#        raise YdkGenException('{0} bundle not supported'.format(language))

    if output_dir is None or len(output_dir) == 0:
        logger.error('output_directory is None.')
        raise YdkGenException('output_dir cannot be None.')

    if ydk_root is None or len(ydk_root) == 0:
        logger.error('ydk_root is None.')
        raise YdkGenException('YDKGEN_HOME is not set.')


def _check_description_file(description_file):
    """ Check if description_file is valid path to file.

    Args:
        description_file (str): Path to description file.

    Raises:
        YdkGenException: If path to description file is not valid.
    """
    if not os.path.isfile(description_file):
        logger.error('Path to description file is not valid.')
        raise YdkGenException('Path to description file is not valid.')

