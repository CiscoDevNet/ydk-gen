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
from git import Repo
import os
import logging
import subprocess

logger = logging.getLogger('ydkgen')


def init_verbose_logger():
    """ Initialize the logging infra and add a handler """
    logger.setLevel(logging.DEBUG)

    # create a console logger
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    # add the handlers to the logger
    logger.addHandler(ch)


def print_about_ydk_page(ydk_root, py_api_doc_gen):
    repo = Repo(ydk_root)
    remote = repo.remote().name
    branch = repo.active_branch.name
    url = repo.remote().url.split('://')[-1].split('.git')[0]
    commit_id = repo.rev_parse(remote + '/' + branch).hexsha

    contents = ''

    with open(os.path.join(py_api_doc_gen, 'about_ydk.rst'), 'r+w') as about_file:
        contents = about_file.read()
        contents = contents.replace('git clone repo-url', 'git clone https://{0}.git'.format(url))
        contents = contents.replace('git checkout commit-id', 'git checkout {0}'.format(commit_id))

    with open(os.path.join(py_api_doc_gen, 'about_ydk.rst'), 'w') as about_file:
        about_file.write(contents)


def generate_documentation(output_directory, ydk_root):

    py_api_doc_gen = output_directory + '/python/docsgen'
    py_api_doc = output_directory + '/python/docs_expanded'
    execfile(os.path.join(output_directory, 'python', 'ydk', '_version.py'))

    os.mkdir(py_api_doc)
    
    # set documentation version and release from setup.py setting
    version_number = locals()['__version__']
    release = 'release={}'.format(version_number)
    version = 'version={}'.format(version_number[:version_number.rfind(".")])

    # print about YDK page
    print_about_ydk_page(ydk_root, py_api_doc_gen)

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


def create_pip_package(output_directory):

    py_sdk_root = output_directory + '/python/'
    os.chdir(py_sdk_root)
    args = [sys.executable, 'setup.py', 'sdist']
    exit_code = subprocess.call(args, env=os.environ.copy())

    if exit_code == 0:
        print('\nSuccessfully created source distribution at %sdist' %
              (py_sdk_root,))
    else:
        print('\nFailed to create source distribution')
        sys.exit(exit_code)
    print('=================================================')
    print('Successfully generated Python YDK at %s' % (py_sdk_root,))
    print('Please read %sREADME.rst for information on how to install the package in your environment' % (
        py_sdk_root,))


def create_shared_library(output_directory):

    cpp_sdk_root = output_directory + '/cpp/'
    os.chdir(cpp_sdk_root)
    args = ['make']
    exit_code = subprocess.call(args, env=os.environ.copy())

    if exit_code == 0 and os.path.isfile(cpp_sdk_root + 'ydk_cpp.so'):
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

    if not os.environ.has_key('YDKGEN_HOME'):
        logger.debug('YDKGEN_HOME not set. Assuming current directory is working directory.')
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

    YdkGenerator().generate(options.profile, output_directory, ydk_root,
                    options.groupings_as_class, language)

    if options.gendoc == True:
        generate_documentation(output_directory, ydk_root)

    if options.cpp:
        create_shared_library(output_directory)
    else:
        create_pip_package(output_directory)

    print 'Code generation completed successfully!'

