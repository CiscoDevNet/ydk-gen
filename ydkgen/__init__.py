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
Generate YDK core library, profile package and bundle packages.
"""
import os
import json
import shutil
import logging
import tempfile
import fileinput

from .common import YdkGenException
from ydkgen.builder import (ApiModelBuilder, GroupingClassApiModelBuilder,
                            PyangModelBuilder, SubModuleBuilder)
from .resolver import resolve_profile, bundle_resolver, bundle_translator
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
                            Valid option for profile approach is: 'profile'.
                            Valid options for bundle approach are: 'core',
                                                                   'bundle'.
            sort_clazz (bool): Option to sort generated classes at same leve.

        Raises:
            YdkGenException: If an error has occurred
    """

    def __init__(self, output_dir, ydk_root, groupings_as_class, language, pkg_type, sort_clazz=False):

        _check_generator_args(output_dir, ydk_root, language, pkg_type)

        self.output_dir = output_dir
        self.ydk_root = ydk_root
        self.groupings_as_class = groupings_as_class
        self.language = language
        self.pkg_type = pkg_type
        self.sort_clazz = sort_clazz

    def generate(self, description_file=None):
        """ Generate ydk profile package, bundle packages or ydk core library.

        Args:
            description_file (str): Path to description file.

        Returns:
            Generated APIs root directory for core and profile package.
            List of Generated APIs root directories for bundle packages.
        """
        if self.pkg_type == 'profile':
            return self._generate_profile(description_file)

        elif self.pkg_type == 'bundle':
            return self._generate_bundle(description_file)

        elif self.pkg_type == 'core':
            return self._generate_core()

    def _generate_profile(self, profile_file):
        """ Generate ydk profile package.

        Args:
            profile_file (str): Path to profile description file.

        Returns:
            gen_api_root (str): Root directory for generated APIs.
        """
        _check_description_file(profile_file)

        gen_api_root = self._init_dirs(pkg_type='profile')
        resolved_models_dir = self._get_profile_resolved_model_dir(
                                        profile_file, gen_api_root)
        api_pkgs = self._get_api_pkgs(resolved_models_dir)
        self._print_pkgs(api_pkgs, gen_api_root)

        return gen_api_root

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

        bundle_translator.translate(profile_file, tmp_file)

        resolver = bundle_resolver.Resolver(self.output_dir)
        curr_bundle, all_bundles = resolver.resolve(tmp_file)

        api_pkgs = self._get_api_pkgs(curr_bundle.resolved_models_dir)

        _set_api_pkg_sub_name(all_bundles, api_pkgs)

        gen_api_root = self._init_dirs(bundle=curr_bundle)
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

    def _get_api_pkgs(self, bundle):
        """ Return api packages for resolved YANG modules. Each module will be
            represented as an api package.

        Args:
            resolved_model_dir (str): Path to resolved YANG modules.

        Returns:
            api_pkgs (List[.api_model.Package]): List of api packages.
        """

        pyang_builder = PyangModelBuilder(bundle)
        modules = pyang_builder.parse_and_return_modules()

        # build api model packages
        if not self.groupings_as_class:
            api_pkgs = ApiModelBuilder().generate(modules)
        else:
            api_pkgs = GroupingClassApiModelBuilder().generate(modules)
        api_pkgs.extend(
            SubModuleBuilder().generate(pyang_builder.get_submodules()))

        return api_pkgs

    def _print_pkgs(self, pkgs, output_dir, bundle_name=''):
        """ Emit generated APIs.

        Args:
            pkgs (List[.api_model.Package]): List of api packages to print.
            pkg_name (str): Package name for generated APIs.
                            For example 'ydk_bgp', 'ydk_ietf'.
        """
        factory = printer_factory.PrinterFactory()
        ydk_printer = factory.get_printer(self.language)(output_dir, bundle_name, self.sort_clazz)
        ydk_printer.emit(pkgs)

    def _get_profile_resolved_model_dir(self, profile_file, gen_api_root):
        """ Return resolve models directory for profile package.

        Args:
            profile_file (str): Path to a profile description file.
            gen_api_root (str): Path to generated APIs root directory.

        Returns:
            (str) Path to resolved yang modules.

        Raises:
            YdkGenException: If path to profile file is invalid.
        """
        try:
            with open(profile_file) as json_file:
                data = json.load(json_file)
                if self.language == 'python':
                    _modify_python_setup(gen_api_root, 'ydk', data['version'])

                return resolve_profile.resolve_profile(data, self.ydk_root)

        except IOError as err:
            err_msg = 'Cannot open profile file (%s)' % err.strerror
            logger.error(err_msg)
            raise YdkGenException(err_msg)
        except ValueError as err:
            err_msg = 'Cannot parse profile file (%s)' % err.message
            logger.error(err_msg)
            raise YdkGenException(err_msg)

    # Initialize generated API directory ######################################
    def _init_dirs(self, pkg_name=None, pkg_type=None, bundle=None):
        """ Initialize and return generated APIs root directory.

        Args:
            pkg_name (str): Package name for generated APIs.
            pkg_type (str): Sdk template type to be copied from.
            bundle (bundle_resolver.Bundle): Bundle instance.
        """
        if bundle:
            return self._init_bundle_dirs(bundle)
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
        shutil.rmtree(gen_api_root)

        self._copy_sdk_template(gen_api_root, pkg_type)

        return gen_api_root

    def _init_bundle_dirs(self, bundle):
        """ Initialize generated API directory for bundle approach.

        Args:
            bundle (bundle_resolver.Bundle): Bundle instance.

        Returns:
            gen_api_root (str): Root directory for generated APIs.
        """
        gen_api_root = self._init_gen_api_dirs(bundle.name, 'packages')

        _modify_python_setup(gen_api_root,
                             'ydk-models-%s' % bundle.name,
                             bundle.str_version,
                             bundle.dependencies)
        # write init file for bundle models directory.
        bundle_model_dir = os.path.join(gen_api_root, 'ydk')

        os.mkdir(bundle_model_dir)
        with open(os.path.join(bundle_model_dir, '__init__.py'), 'w') as init_file:
            init_file.close()

        return gen_api_root

    def _copy_sdk_template(self, gen_api_root, pkg_type):
        """ Copy sdk template to gen_api_root directory.

        Args:
            gen_api_root (str): Root directory for generated APIs.
            pkg_type (str): Sdk template to copied from.
                            Valid options are: core, packages.
        """
        template_dir = os.path.join(self.ydk_root, 'sdk', self.language)
        if self.language == 'python':
            template_dir = os.path.join(template_dir, pkg_type)
        shutil.copytree(template_dir,
                        gen_api_root,
                        ignore=shutil.ignore_patterns('.gitignore',
                                                      'ncclient'))


def _set_api_pkg_sub_name(bundles, api_pkgs):
    """ Set nmsp_pkg for api packages.

    Args:
        bundles (List[.resolver.bundle_resolver.Bundle]): Bundle instance.
        api_pkgs (List[.api_model.Package]): List of api packages.

    """
    # add API for model being augmented to bundle
    for pkg in api_pkgs:
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
            print line.replace("$PACKAGE$", pkg_name),
        elif not replaced_version and "$VERSION$" in line:
            replaced_version = True
            print line.replace("$VERSION$", version),
        elif not replaced_dependencies and "$DEPENDENCY$" in line:
            replaced_dependencies = True
            if dependencies:
                additional_requires = ["'ydk-models-%s>=%s'" % (d.name, d.str_version)
                                       for d in dependencies]
                print line.replace("'$DEPENDENCY$'", ", ".join(additional_requires))
            else:
                print line.replace("'$DEPENDENCY$'", "")
        else:
            print line,


# Generator checks #####################################################
def _check_generator_args(output_dir, ydk_root, language, pkg_type):
    """ Check generator arguments.

    Args:
        output_dir (str): The output directory for generated APIs.
        ydk_root (str): The ydk root directory.
        language (str): Language for generated APIs.
        pkg_type (str): Package type for generated APIs.
                        Valid option for profile approach is: 'profile'.
                        Valid options for bundle approach are: 'core',
                                                               'bundle'.
    Raises:
        YdkGenException: If invalid arguments are passed in.
    """
    if language != 'cpp' and language != 'python':
        raise YdkGenException('Language {0} not supported'.format(language))

    if language != 'python' and pkg_type == 'bundle':
        raise YdkGenException('{0} bundle not supported'.format(language))

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
