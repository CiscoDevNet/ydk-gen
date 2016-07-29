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

import sys
from optparse import OptionParser
from ydkgen import YdkGenerator
import os
import shutil
import logging
import fileinput
import subprocess
from git import Repo
from itertools import izip_longest

logger = logging.getLogger('ydkgen')


def init_verbose_logger():
    """ Initialize the logging infra and add a handler """
    logger.setLevel(logging.DEBUG)

    # create a console logger
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    # add the handlers to the logger
    logger.addHandler(ch)


def print_about_page(ydk_root, py_api_doc_gen):
    repo = Repo(ydk_root)
    remote = repo.remote().name
    branch = repo.active_branch.name
    url = repo.remote().url.split('://')[-1].split('.git')[0]
    commit_id = repo.rev_parse(remote + '/' + branch).hexsha

    # modify about_ydk.rst page
    for line in fileinput.input(os.path.join(py_api_doc_gen, 'about_ydk.rst'), 'r+w'):
       if 'git clone repo-url' in line:
           print line.replace('repo-url', 'https://{0}.git'.format(url)),
       elif 'git checkout commit-id' in line:
           print line.replace('commit-id', '{}'.format(commit_id))
       else:
           print line,


def print_bundle_landing_page(py_api_doc_gen, bundle):
    if bundle:
        bundle_name = os.path.basename(bundle).rstrip('.json')
        for line in fileinput.input(os.path.join(py_api_doc_gen, 'index.rst'), 'r+w'):
            if '<bundle_name>' in line:
                print line.replace('<bundle_name>', bundle_name),
                print len(line) * '='
            else:
                print line,


def get_release_version(output_directory):
    version = ''
    release = ''
    setup_file = os.path.join(output_directory, 'setup.py')
    with open(setup_file, 'r') as f:
        for line in f:
            if 'version=' in line or 'version =' in line:
                rv = line[line.find('=') + 1:line.rfind(",")]
                release = "release=" + rv
                version = "version=" + rv[:rv.rfind(".")] + "'"
    return release, version


def generate_documentations(output_directory, ydk_root, language, bundle):
    py_api_doc_gen = os.path.join(output_directory, 'docsgen')
    py_api_doc = os.path.join(output_directory, 'docs_expanded')
    # if it is package type
    release, version = get_release_version(output_directory)
    os.mkdir(py_api_doc)
    # print about YDK page
    print_about_page(ydk_root, py_api_doc_gen)
    print_bundle_landing_page(py_api_doc_gen, bundle)
    # build docs
    print('\nBuilding docs using sphinx-build...\n')
    p = subprocess.Popen(['sphinx-build',
                          '-D', version,
                          '-D', release,
                          py_api_doc_gen, py_api_doc],
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    logger.debug(stdout)
    logger.error(stderr)
    print >> sys.stderr, stderr
    print(stdout)
    print('*' * 28 + '\n' + 'DOCUMENTATION ERRORS/WARNINGS\n' +
          '*' * 28 + '\n' + stderr)


def create_pip_packages(output_directory):
    py_sdk_root = output_directory
    os.chdir(py_sdk_root)
    args = [sys.executable, 'setup.py', 'sdist']
    exit_code = subprocess.call(args, env=os.environ.copy())

    if exit_code == 0:
        print('\nSuccessfully created source distribution at %s/dist' %
              (py_sdk_root,))
    else:
        print('\nFailed to create source distribution')
        sys.exit(exit_code)
    print('=================================================')
    print('Successfully generated Python YDK at %s' % (py_sdk_root,))
    print('Please read %s/README.rst for information on how to install the package in your environment' % (
        py_sdk_root,))


def create_shared_libraries(output_directory):
    cpp_sdk_root = output_directory
    os.chdir(cpp_sdk_root)
    args = ['make']
    exit_code = subprocess.call(args, env=os.environ.copy())

    if exit_code == 0 and os.path.isfile(
                            os.path.join(cpp_sdk_root, 'ydk_cpp.so')):
        print('\nSuccessfully created shared library %s as ydk_cpp.so' %
              (cpp_sdk_root))
    else:
        print('\nERROR: Failed to create shared library!\n')
        sys.exit(exit_code)
    print('\n=================================================')
    print('Successfully generated C++ YDK at %s' % (cpp_sdk_root,))
    print('Please read %sREADME.rst for information on how to install the package in your environment\n' % (
        cpp_sdk_root,))

if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog [options]",
                          version="%prog 0.4.0")

    parser.add_option("--profile",
                      type=str,
                      dest="profile",
                      help="Take options from a profile file, any CLI targets ignored")

    parser.add_option("--bundle",
                      type=str,
                      dest="bundle",
                      help="Take options from a bundle file, any CLI targets ignored")

    parser.add_option("--core",
                      action='store_true',
                      dest="core",
                      help="Take options from a bundle file, any CLI targets ignored")

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

    parser.add_option("--groupings-as-class",
                      action="store_true",
                      dest="groupings_as_class",
                      default=False,
                      help="Consider yang groupings as classes.")

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


    if options.profile:
        output_directory = (YdkGenerator(
                           output_directory,
                           ydk_root,
                           options.groupings_as_class,
                           language,
                           'profile').generate(options.profile))

    elif options.bundle:
        output_directory = (YdkGenerator(
                           output_directory,
                           ydk_root,
                           options.groupings_as_class,
                           language,
                           'bundle').generate(options.bundle))

    elif options.core:
        output_directory = (YdkGenerator(
                           output_directory,
                           ydk_root,
                           options.groupings_as_class,
                           language,
                           'core').generate())


    if options.gendoc:
        generate_documentations(output_directory, ydk_root, language, options.bundle)

    if options.cpp:
        create_shared_libraries(output_directory)
    else:
        create_pip_packages(output_directory)

    print 'Code generation completed successfully!'
