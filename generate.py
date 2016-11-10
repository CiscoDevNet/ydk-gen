#!/usr/bin/env python
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
from optparse import OptionParser

import fileinput
import logging
import os
import shutil
import subprocess
import sys
import time
import re

from git import Repo
from ydkgen import YdkGenerator


logger = logging.getLogger('ydkgen')


def init_verbose_logger():
    """ Initialize the logging infra and add a handler """
    logger.setLevel(logging.DEBUG)

    # create a console logger
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    # add the handlers to the logger
    logger.addHandler(ch)


def print_about_page(ydk_root, py_api_doc_gen, release, is_bundle):
    if is_bundle:
        return
    repo = Repo(ydk_root)
    url = repo.remote().url.split('://')[-1].split('.git')[0]
    commit_id = str(repo.head.commit)
    # modify about_ydk.rst page
    for line in fileinput.input(os.path.join(py_api_doc_gen, 'about_ydk.rst'), 'r+w'):
        if 'git clone repo-url' in line:
            print(line.replace('repo-url', 'https://{0}.git'.format(url)), end='')
        elif 'git checkout commit-id' in line:
            print(line.replace('commit-id', '{}'.format(commit_id)), end='')
        elif 'version-id' in line:
            print(line.replace('version-id', '{}'.format(release.replace('release=', ''))), end='')
        else:
            print(line, end='')


def get_release_version(output_directory, language):
    if language == 'python':
        return get_py_release_version(output_directory)
    elif language == 'cpp':
        return get_cpp_release_version(output_directory)


def get_py_release_version(output_directory):
    setup_file = os.path.join(output_directory, 'setup.py')
    with open(setup_file, 'r') as f:
        for line in f:
            if ('version=' in line or 'version =' in line or
                'NMSP_PKG_VERSION' in line and '$VERSION$' not in line or
                line.startswith('VERSION =')):
                rv = line[line.find('=')+1:].strip(' \'"\n')
                release = "release=" + rv
                version = "version=" + rv
                break
    return (release, version)


def get_cpp_release_version(output_directory):
    MAJOR_VERSION = re.compile(r"set\(YDK_[A-Z]*[_]*MAJOR_VERSION (?P<num>\d+)\)")
    MINOR_VERSION = re.compile(r"set\(YDK_[A-Z]*[_]*MINOR_VERSION (?P<num>\d+)\)")
    SERVICE_VERSION = re.compile(r"set\(YDK_[A-Z]*[_]*SERVICE_VERSION (?P<num>\d+)\)")
    major_version, minor_version, service_version = 0, 0, 0
    cmake_file = os.path.join(output_directory, 'CMakeLists.txt')
    with open(cmake_file) as f:
        for line in f:
            major_match = MAJOR_VERSION.match(line)
            minor_match = MINOR_VERSION.match(line)
            service_match = SERVICE_VERSION.match(line)
            if major_match:
                major_version = major_match.group('num')
            if minor_match:
                minor_version = minor_match.group('num')
            if service_match:
                service_version = service_match.group('num')
    version = "%s.%s.%s" % (major_version, minor_version, service_version)
    release = "release=%s" % version
    version = "version=%s" % version
    return (release, version)


def copy_docs_from_bundles(ydk_root, language, destination_dir):
    output_root_dir = os.path.join(ydk_root, 'gen-api')
    output_root_dir = os.path.join(output_root_dir, language)
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
            file_util.copy_file(ydk_models_file, ydk_bundle_models_file)

            dir_util.copy_tree(bundle_docsgen_dir, destination_dir)
            with open(backup_index_file, 'a') as myfile:
                myfile.write('   {0}\n'.format(ydk_bundle_models_file_name))

    file_util.copy_file(backup_index_file, index_file)
    os.remove(backup_index_file)


def generate_documentations(output_directory, ydk_root, language, is_bundle, is_core):
    py_api_doc_gen = os.path.join(output_directory, 'docsgen')
    py_api_doc = os.path.join(output_directory, 'docs_expanded')
    # if it is package type
    release, version = get_release_version(output_directory, language)
    os.mkdir(py_api_doc)
    # print about YDK page
    print_about_page(ydk_root, py_api_doc_gen, release, is_bundle)
    if is_core:
        copy_docs_from_bundles(ydk_root, language, py_api_doc_gen)
    # build docs
    print('\nBuilding docs using sphinx-build...\n')
    p = subprocess.Popen(['sphinx-build',
                          '-D', release,
                          '-D', version,
                          py_api_doc_gen, py_api_doc],
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    logger.debug(stdout)
    logger.error(stderr)
    print(stderr, file=sys.stderr)
    print(stdout)
    msg = '%s\nDOCUMENTATION ERRORS/WARNINGS\n%s\n%s' % ('*' * 28, '*' * 28, stderr.decode('utf-8'))
    print(msg)


def create_pip_packages(output_directory):
    py_sdk_root = output_directory
    os.chdir(py_sdk_root)
    args = [sys.executable, 'setup.py', 'sdist']
    exit_code = subprocess.call(args, env=os.environ.copy())

    if exit_code == 0:
        print('\nSuccessfully created source distribution at %s/dist' %
              py_sdk_root)
    else:
        print('\nFailed to create source distribution')
        sys.exit(exit_code)
    print('=================================================')
    print('Successfully generated Python YDK at %s' % (py_sdk_root,))
    print('Please read %s/README.md for information on how to install the package in your environment' % (
        py_sdk_root,))


def create_shared_libraries(output_directory, sudo):
    cpp_sdk_root = os.path.join(output_directory)
    cmake_build_dir = os.path.join(output_directory, 'build')
    if os.path.exists(cmake_build_dir):
        shutil.rmtree(cmake_build_dir)
    os.makedirs(cmake_build_dir)
    os.chdir(cmake_build_dir)
    sudo_cmd = 'sudo' if sudo else ''
    try:
        FNULL = open(os.devnull, 'w')
        subprocess.check_call(['cmake', '..'], stdout=FNULL, stderr=FNULL)
        subprocess.check_call(['make', '-j5'], stdout=FNULL, stderr=FNULL)
        subprocess.check_call(['%s' % sudo_cmd, 'make', 'install'])
    except subprocess.CalledProcessError as e:
        print('\nERROR: Failed to create shared library!\n')
        sys.exit(e.returncode)
    print('\nSuccessfully created and installed shared libraries')
    print('\n=================================================')
    print('Successfully generated C++ YDK at %s' % (cpp_sdk_root,))
    print('Please read %s/README.md for information on how to install the package in your environment\n' % (
        cpp_sdk_root,))


def _get_time_taken(start_time):
    end_time = time.time()
    uptime = end_time - start_time
    minutes = int(uptime / 60) if int(uptime) > 60 else 0
    seconds = int(uptime) % (60 * minutes) if int(uptime) > 60 else int(uptime)
    minutes_str = str(minutes) + ' minutes' if int(uptime) > 60 else ''
    seconds_str = str(seconds) + ' seconds'
    return minutes_str, seconds_str


if __name__ == '__main__':
    start_time = time.time()

    parser = OptionParser(usage="usage: %prog [options]",
                          version="%prog 0.4.0")

    parser.add_option("--bundle",
                      type=str,
                      dest="bundle",
                      help="Specify a bundle profile file to generate a bundle from")

    parser.add_option("--core",
                      action='store_true',
                      dest="core",
                      help="Generate and/or install core library")

    parser.add_option("--output-directory",
                      type=str,
                      dest="output_directory",
                      help="The output directory where the sdk will get created.")

    parser.add_option("-p", "--python",
                      action="store_true",
                      dest="python",
                      default=True,
                      help="Generate Python SDK")

    parser.add_option("-c", "--cpp",
                      action="store_true",
                      dest="cpp",
                      default=False,
                      help="Generate C++ SDK")

    parser.add_option("-v", "--verbose",
                      action="store_true",
                      dest="verbose",
                      default=False,
                      help="Verbose mode")

    parser.add_option("--generate-doc",
                      action="store_true",
                      dest="gendoc",
                      default=False,
                      help="Generate documentation")

    parser.add_option("--generate-tests",
                      action="store_true",
                      dest="gentests",
                      default=False,
                      help="Generate tests")

    parser.add_option("--groupings-as-class",
                      action="store_true",
                      dest="groupings_as_class",
                      default=False,
                      help="Consider yang groupings as classes.")

    parser.add_option("--sudo",
                      action="store_true",
                      dest="sudo",
                      default=False,
                      help="Use sudo for C++ core library installation.")

    try:
        arg = sys.argv[1]
    except IndexError:
        parser.print_help()
        sys.exit(1)

    (options, args) = parser.parse_args()

    if options.verbose:
        init_verbose_logger()

    if 'YDKGEN_HOME' not in os.environ:
        logger.debug("YDKGEN_HOME not set."
                     " Assuming current directory is working directory.")
        ydk_root = os.getcwd()
    else:
        ydk_root = os.environ['YDKGEN_HOME']

    if options.output_directory is None:
        output_directory = '%s/gen-api' % ydk_root
    else:
        output_directory = options.output_directory

    language = ''
    if options.cpp:
        language = 'cpp'
    elif options.python:
        language = 'python'

    if options.bundle:
        output_directory = (YdkGenerator(
                            output_directory,
                            ydk_root,
                            options.groupings_as_class,
                            options.gentests,
                            language,
                            'bundle').generate(options.bundle))

    if options.core:
        output_directory = (YdkGenerator(
                            output_directory,
                            ydk_root,
                            options.groupings_as_class,
                            options.gentests,
                            language,
                            'core').generate())

    if options.gendoc:
        generate_documentations(output_directory, ydk_root, language, options.bundle, options.core)

    minutes_str, seconds_str = _get_time_taken(start_time)
    print('\nTime taken for code/doc generation: {0} {1}\n'.format(minutes_str, seconds_str))
    print('\nPerforming compilation and/or installation...\n')

    if options.cpp:
        create_shared_libraries(output_directory, options.sudo)
    else:
        create_pip_packages(output_directory)

    minutes_str, seconds_str = _get_time_taken(start_time)
    print('Code generation and installation completed successfully!')
    print('Total time taken: {0} {1}\n'.format(minutes_str, seconds_str))
