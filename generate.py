#!/usr/bin/env python3
#
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

from __future__ import print_function
from distutils import dir_util, file_util
from argparse import ArgumentParser

import filecmp
import json
import logging
import os
import shutil
import subprocess
import sys
import time

from git import Repo
from ydkgen import YdkGenerator
from ydkgen.common import YdkGenException


logger = logging.getLogger('ydkgen')


def init_verbose_logger():
    """ Initialize the logging infra and add a handler """
    logger.setLevel(logging.DEBUG)

    # create a console logger
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    # add the handlers to the logger
    logger.addHandler(ch)


def print_about_page(ydk_root, docs_rst_directory, release):
    repo = Repo(ydk_root)
    url = repo.remote().url.split('://')[-1].split('.git')[0]
    commit_id = str(repo.head.commit)

    if language == 'python':
        lang = 'py'
        code_block_language = 'sh'
    elif language in ('cpp', 'go') :
        lang = language
        code_block_language = 'bash'
    else:
        raise Exception('Language {0} not yet supported'.format(language))

    # modify about_ydk.rst page
    with open(os.path.join(ydk_root, 'sdk/_docsgen_common/about_ydk.rst'), 'r+') as fd:
        lines = fd.read()
    if 'git clone repo-url' in lines:
        lines = lines.replace('repo-url', 'https://{0}.git'.format(url))
    if 'git checkout commit-id' in lines:
        lines = lines.replace('commit-id', '{}'.format(commit_id))
    if 'version-id' in lines:
        lines = lines.replace('version-id', '{}'.format(release.replace('release=', '')))
    if 'language-version' in lines:
        lines = lines.replace('language-version', lang)
    if 'code-block-language' in lines:
        lines = lines.replace('code-block-language', code_block_language)
    with open(os.path.join(docs_rst_directory, 'about_ydk.rst'), 'w+') as fd:
        fd.write(lines)


def get_release(ydk_root):
    with open(os.path.join(ydk_root, 'sdk', 'version.json')) as f:
        versions = json.load(f)
    release = versions['core']
    dev = '-dev' if release[-4:] == '-dev' else ''
    release = release.replace('-dev', '')
    return release, dev


def copy_docs_from_bundles(output_directory, destination_dir):
    output_root_dir = os.path.join(output_directory, '..')
    bundle_dirs = os.listdir(output_root_dir)
    index_file = os.path.join(destination_dir, 'index.rst')
    backup_index_file = os.path.join(destination_dir, 'index_bkp.rst')
    file_util.copy_file(index_file, backup_index_file)
    for bundle_dir in bundle_dirs:
        if '-bundle' in bundle_dir:
            bundle_dir_path = os.path.join(output_root_dir, bundle_dir)
            bundle_docsgen_dir = os.path.join(bundle_dir_path, 'docsgen')
            ydk_bundle_models_file_name = 'ydk.models.{0}.rst'.format(bundle_dir.replace('-bundle', ''))

            ydk_models_file = os.path.join(bundle_docsgen_dir, 'ydk.models.rst')
            ydk_bundle_models_file = os.path.join(bundle_docsgen_dir, ydk_bundle_models_file_name)
            logger.debug('Copying %s to %s' % (bundle_docsgen_dir, destination_dir))
            file_util.copy_file(ydk_models_file, ydk_bundle_models_file)

            dir_util.copy_tree(bundle_docsgen_dir, destination_dir)
            with open(backup_index_file, 'a') as myfile:
                myfile.write('   {0}\n'.format(ydk_bundle_models_file_name))

    file_util.copy_file(backup_index_file, index_file)
    os.remove(backup_index_file)


def copy_files_to_cache(output_root_directory, language, docs_rst_directory):
    original_cache_dir = os.path.join(output_root_directory, 'cache', language, 'ydk', 'docsgen')
    cache_dir = os.path.join(output_root_directory, 'cache-gen')
    shutil.rmtree(cache_dir, ignore_errors=True)
    logger.debug("Copying cache dir '%s' to '%s'" % (original_cache_dir, cache_dir))
    shutil.copytree(original_cache_dir, cache_dir)
    file_count = 0
    logger.debug("Looking at cache dir '%s'" % (cache_dir))
    for f in os.listdir(docs_rst_directory):
        if f.endswith('.rst'):
            basename = os.path.basename(f)
            cache_file = os.path.join(cache_dir, basename)
            fp = os.path.join(docs_rst_directory, f)
            logger.debug("Comparing files '%s', '%s'" % (fp, cache_file))
            if not os.path.isfile(fp):
                if os.path.isfile(cache_file):
                    os.remove(cache_file)
                    logger.debug("Deleting orphan %s from cache" % (cache_file))
            elif os.path.isfile(cache_file) == False or filecmp.cmp(fp, cache_file) == False:
                shutil.copy2(fp, cache_file)
                logger.debug("Copying non-existent or different file %s to %s" % (fp, cache_file))
                file_count += 1

    logger.debug("\n%s files copied\n" % file_count)
    docs_rst_directory = cache_dir
    docs_expanded_directory = os.path.join(output_root_directory, 'cache', language, 'ydk', 'docs_expanded')
    return docs_rst_directory, docs_expanded_directory


def generate_documentations(output_directory, ydk_root, language, is_bundle, is_core,
                            output_directory_contains_cache, output_root_directory):
    print('\nBuilding docs using sphinx-build...\n')
    docs_rst_directory = os.path.join(output_directory, 'docsgen')
    docs_expanded_directory = os.path.join(output_directory, 'docs_expanded')
    # if it is package type
    rv, _ = get_release(ydk_root)
    release = 'release=%s' % rv
    version = 'version=%s' % rv
    os.mkdir(docs_expanded_directory)

    if not is_bundle:
        print_about_page(ydk_root, docs_rst_directory, rv)

    if is_core:
        copy_docs_from_bundles(output_directory, docs_rst_directory)

    if is_core and output_directory_contains_cache:
        docs_rst_directory, docs_expanded_directory = copy_files_to_cache(output_root_directory, language, docs_rst_directory)

    # build docs
    if is_core:
        logger.debug("Invoking '%s'" %
                     (' '.join(['sphinx-build', '-D', release, '-D', version, '-vvv',
                                docs_rst_directory, docs_expanded_directory])))
        p = subprocess.Popen(['sphinx-build',
                              '-D', release,
                              '-D', version, '-vvv',
                              docs_rst_directory, docs_expanded_directory],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
    else:
        logger.debug("Invoking '%s'" % (['sphinx-build', '-vvv', docs_rst_directory, docs_expanded_directory]))
        p = subprocess.Popen(['sphinx-build',
                              '-vvv',
                              docs_rst_directory, docs_expanded_directory],
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    if stdout:
        msg = '%s\nDOCUMENTATION ERRORS/WARNINGS\n%s\n%s' % ('*' * 28, '*' * 28, stdout.decode('utf-8'))
        logger.debug(msg)
    if stderr:
        msg = '%s\nDOCUMENTATION ERRORS/WARNINGS\n%s\n%s' % ('*' * 28, '*' * 28, stderr.decode('utf-8'))
        print(msg)


def create_pip_packages(out_dir):
    py_sdk_root = out_dir
    os.chdir(py_sdk_root)
    shutil.rmtree('dist', ignore_errors=True)
    args = [sys.executable, 'setup.py', 'sdist']
    exit_code = subprocess.call(args, env=os.environ.copy())

    if exit_code == 0:
        print('\nSuccessfully created source distribution at %s/dist' %
              py_sdk_root)
    else:
        print('\nFailed to create source distribution')
        sys.exit(exit_code)

    package = generator.package_type
    if generator.package_name != '':
        package = '%s %s' % (generator.package_name, package)
    print('\n=================================================')
    print('Successfully generated Python YDK %s package at %s' % (package, py_sdk_root))
    print('Please refer to the README for information on how to install the package in your environment')


def install_go_package(gen_api_dir, generator):
    gopath = os.environ.get('GOPATH')
    if gopath is None:
        gopath = os.path.join(os.environ['HOME'], 'go')
        print('\nEnvironment variable GOPATH has not been set!!!')
        print('\nSetting GOPATH to %s' % gopath)
    source_dir = os.path.join(gen_api_dir, 'ydk')
    target_dir = os.path.join(gopath, 'src/github.com/CiscoDevNet/ydk-go/ydk')
    logger.debug('Copying %s to %s' % (source_dir, target_dir))
    dir_util.copy_tree(source_dir, target_dir)

    package = generator.package_type
    if generator.package_name != '':
        package = '%s %s' % (generator.package_name, package)
    print('\n=================================================')
    print('Successfully generated and installed Go YDK %s package at %s' % 
          (package, target_dir))


def generate_adhoc_bundle(adhoc_bundle_name, adhoc_bundle_files):
    adhoc_bundle = {
        "name": adhoc_bundle_name,
        "version": "0.1.0",
        "core_version": "0.8.5",
        "author": "Cisco",
        "copyright": "Cisco",
        "description": "Adhoc YDK bundle",
        "long_description": "Adhoc YDK bundle",
        "models": {
            "description": "User-specified list of files.",
            "file": [f for f in adhoc_bundle_files]
        },
        "dependency": [
            {
                "name": "ietf",
                "version": "0.1.5",
                "core_version": "0.8.5",
                "uri": "file://profiles/bundles/ietf_0_1_5.json"
            }
        ]
    }
    import tempfile
    import json
    adhoc_bundle_file = tempfile.NamedTemporaryFile(mode='w', delete=False)
    adhoc_bundle_file.write(json.dumps(adhoc_bundle, indent=2, sort_keys=True))
    adhoc_bundle_file.close()
    return adhoc_bundle_file.name


def preconfigure_generated_cpp_code(output_directory, generate_libydk):
    cmake_build_dir = os.path.join(output_directory, 'build')
    if os.path.exists(cmake_build_dir):
        shutil.rmtree(cmake_build_dir)
    os.makedirs(cmake_build_dir)
    os.chdir(cmake_build_dir)
    try:
        cmake3_installed = (0 == subprocess.call(['which', 'cmake3'], stdout=subprocess.PIPE, stderr=subprocess.PIPE))
        if cmake3_installed:
            cmake_executable = 'cmake3'
        else:
            cmake_executable = 'cmake'
        subprocess.check_call([cmake_executable, '-DCMAKE_BUILD_TYPE=Release', '..'])
    except subprocess.CalledProcessError as e:
        print('\nERROR: Failed to configure build!\n')
        sys.exit(e.returncode)


def _get_time_taken(start_time):
    end_time = time.time()
    uptime = end_time - start_time
    minutes = int(uptime / 60) if int(uptime) > 60 else 0
    seconds = int(uptime) % (60 * minutes) if int(uptime) > 60 else int(uptime)
    minutes_str = str(minutes) + ' minutes' if int(uptime) > 60 else ''
    seconds_str = str(seconds) + ' seconds'
    return minutes_str, seconds_str


def get_python_version():
    import sysconfig
    python_version = sysconfig.get_config_var('LDVERSION')
    if python_version is None or len(python_version) == 0:
        python_version = sysconfig.get_config_var('VERSION')
    return python_version


if __name__ == '__main__':
    start_time = time.time()

    parser = ArgumentParser(description='Generate YDK artifacts:')

    parser.add_argument(
        "-l", "--libydk",
        action="store_true",
        default=False,
        help="Generate libydk core package")

    parser.add_argument(
        "--core",
        action='store_true',
        help="Generate and/or install core library")

    parser.add_argument(
        "--service",
        type=str,
        help="Location of service profile JSON file")

    parser.add_argument(
        "--bundle",
        type=str,
        help="Location of bundle profile JSON file")

    parser.add_argument(
        "--adhoc-bundle-name",
        type=str,
        help="Name of the adhoc bundle")

    parser.add_argument(
        "--adhoc-bundle",
        type=str,
        nargs='+',
        help="Generate an SDK from a specified list of files")

    parser.add_argument(
        "--generate-meta",
        action="store_true",
        dest="genmeta",
        default=False,
        help="Generate meta-data for Python bundle")

    parser.add_argument(
        "--generate-doc",
        action="store_true",
        dest="gendoc",
        default=False,
        help="Generate documentation")

    parser.add_argument(
        "--generate-tests",
        action="store_true",
        dest="gentests",
        default=False,
        help="Generate tests")

    parser.add_argument(
        "--output-directory",
        type=str,
        help="The output directory where the sdk will get created.")

    parser.add_argument(
        "--cached-output-dir",
        action='store_true',
        help="The output directory specified with --output-directory includes a cache of previously generated \
        gen-api/<language> files under a directory called 'cache'. To be used to generate docs for --core")

    parser.add_argument(
        "-p", "--python",
        action="store_true",
        default=True,
        help="Generate Python SDK")

    parser.add_argument(
        "-c", "--cpp",
        action="store_true",
        default=False,
        help="Generate C++ SDK")

    parser.add_argument(
        "-g", "--go",
        action="store_true",
        # dest="go",
        default=False,
        help="Generate Go SDK")

    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        default=False,
        help="Verbose mode")

    parser.add_argument(
        "-o", "--one-class-per-module",
        action="store_true",
        default=False,
        help="Generate separate modules for each python class corresponding to yang containers or lists.")

    parser.add_argument(
        "-i", "--install",
        action="store_true",
        dest="install",
        default=False,
        help="Install generated component")

    parser.add_argument(
        "-s", "--sudo",
        action="store_true",
        dest="sudo",
        default=False,
        help="Install with sudo access")

    options = parser.parse_args()

    if options.verbose:
        init_verbose_logger()

    if 'YDKGEN_HOME' not in os.environ:
        logger.debug("YDKGEN_HOME not set."
                     " Assuming current directory is working directory.")
        ydk_root = os.getcwd()
    else:
        ydk_root = os.environ['YDKGEN_HOME']

    if options.cached_output_dir:
        if options.output_directory is None or not options.core:
            raise YdkGenException('--output-directory needs to be specified with --cached-output-dir and --core options')

    if options.output_directory is None:
        output_directory = '%s/gen-api' % ydk_root
    else:
        output_directory = options.output_directory

    language = ''
    if options.libydk:
        options.cpp = True
        options.core = True
        language = 'cpp'
    elif options.cpp:
        language = 'cpp'
    elif options.go:
        language = 'go'
    elif options.python:
        language = 'python'

    try:
        if options.adhoc_bundle_name:
            adhoc_bundle_file = generate_adhoc_bundle(
                options.adhoc_bundle_name,
                options.adhoc_bundle)
            init_verbose_logger()

            generator = YdkGenerator(
                output_directory,
                ydk_root,
                options.gentests,
                language,
                'bundle',
                options.one_class_per_module)
            generator.generate_meta = options.genmeta and language == 'python'

            output_directory = generator.generate(adhoc_bundle_file)
            os.remove(adhoc_bundle_file)

        if options.core:
            generator = YdkGenerator(
                output_directory,
                ydk_root,
                options.gentests,
                language,
                'core',
                options.one_class_per_module)

            output_directory = generator.generate(options.core)

        if options.bundle:
            generator = YdkGenerator(
                output_directory,
                ydk_root,
                options.gentests,
                language,
                'bundle',
                options.one_class_per_module)
            generator.generate_meta = options.genmeta and language == 'python'

            output_directory = (generator.generate(options.bundle))

        if options.service:
            generator = YdkGenerator(
                output_directory,
                ydk_root,
                options.gentests,
                language,
                'service',
                options.one_class_per_module)

            output_directory = (generator.generate(options.service))

    except YdkGenException as e:
        print('\nError(s) occurred in YdkGenerator()!\n')
        print(e.msg)
        sys.exit(1)

    if options.gendoc:
        generate_documentations(output_directory, ydk_root, language, options.bundle, options.core,
                                options.cached_output_dir, options.output_directory)
        minutes_str, seconds_str = _get_time_taken(start_time)
        print('\nTime taken for code/doc generation: {0} {1}\n'.format(minutes_str, seconds_str))

    success = True
    if options.cpp:
        preconfigure_generated_cpp_code(output_directory, options.libydk)
        cpp_package = os.path.basename(output_directory)
        print('\nSuccessfully generated {0} code for {1}\n'.format(language, cpp_package))
        cmake_build_dir = os.path.join(output_directory, 'build')
        if options.install:
            os.chdir(cmake_build_dir)
            print('\nCompiling {0} package ...\n'.format(language))
            if os.system('make') != 0:
                print('\nCompilation failed!!')
                success = False
            else:
                sudo = ''
                if options.sudo:
                    sudo = 'sudo '
                print('\nInstalling {0} package ...\n'.format(language, cpp_package))
                if os.system('%smake install' % sudo) != 0:
                    print('\nInstallation failed!!')
                    success = False
                os.chdir(ydk_root)
        else:
            make_command = '\nTo build and install, run "make && [sudo] make install" from {0}'.format(cmake_build_dir)
            print(make_command)

    elif options.go:
        if options.install:
            install_go_package(output_directory, generator)
    elif options.python:
        create_pip_packages(output_directory)
        if options.install:
            dist_dir = '%s/dist' % output_directory
            file_list=os.listdir(dist_dir)
            if len(file_list) == 1:
                dest = file_list[0]
                print('\nInstalling {0} package {1} ...\n'.format(language, dest))
                sudo = ''
                if options.sudo:
                    sudo = 'sudo '
                pip_command = 'pip'
                python_version = get_python_version()
                if python_version.startswith('3.'):
                    pip_command = 'pip3'
                # print('\nExecuting command: %s%s install %s/%s -U\n' % (sudo, pip_command, dist_dir, dest))
                os.system('%s%s install %s/%s -U' % (sudo, pip_command, dist_dir, dest))

            elif len(file_list) == 0:
                print('\nCannot find installation package in directory %s' % dist_dir)
                success = False
            else:
                print('\nThe directory %s contains multiple packages:\n  %s' % (dist_dir, file_list))
                print('Please manually complete the installation process')

    if success:
        if options.install:
            print('\nCode generation and installation completed successfully!')
        else:
            print('\nCode generation completed successfully!  Manual installation required!')

    minutes_str, seconds_str = _get_time_taken(start_time)
    print('\nTotal time taken: {0} {1}\n'.format(minutes_str, seconds_str))
