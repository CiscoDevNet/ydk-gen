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
import ydkgen
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


def generate_documentation(output_directory):

    py_api_doc_gen = output_directory + '/python/docsgen'
    py_api_doc = output_directory + '/python/docs_expanded'

    os.mkdir(py_api_doc)
    
    # set documentation version and release from setup.py setting
    release = ''
    version = ''
    with open(ydk_root + '/sdk/python/setup.py') as fd:
        for line in fd.readlines():
            if 'version=' in line or 'version =' in line:
                rv = line[line.find('=') + 1:line.rfind(",")]
                release = "release=" + rv
                version = "version=" + rv[:rv.rfind(".")] + "'"

    # build docs
    logger.debug('Building docs using sphinx-build...\n')
    # print('\nBuilding docs using sphinx-build...\n')

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
        print('Successfully created source distribution at %sdist' %
              (py_sdk_root,))
    else:
        print('Failed to create source distribution')
    print('=================================================')
    print('Successfully generated Python YDK at %s' % (py_sdk_root,))
    print('Please read %sREADME.rst for information on how to install the package in your environment' % (
        py_sdk_root,))


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
                      default=False,
                      help="Generate Python SDK")

    parser.add_option("-v", "--verbose",
                      action="store_true",
                      dest="verbose",
                      default=False,
                      help="Verbose mode")

    parser.add_option("--no-doc",
                      action="store_true",
                      dest="nodoc",
                      default=False,
                      help="Skip generation of documentation")

    parser.add_option("--groupings-as-class",
                      action="store_true",
                      dest="groupings_as_class",
                      default=False,
                      help="Consider yang groupings as classes.")

    (options, args) = parser.parse_args()

    if options.verbose:
        init_verbose_logger()

    if not os.environ.has_key('YDKGEN_HOME'):
        logger.error('YDKGEN_HOME not set')
        print >> sys.stderr, "Need to have YDKGEN_HOME set!"
        sys.exit(1)

    ydk_root = os.environ['YDKGEN_HOME']

    if options.output_directory is None:
        output_directory = '%s/gen-api' % ydk_root
    else:
        output_directory = options.output_directory

    try:
        ydkgen.generate(options.profile, output_directory, options.nodoc, ydk_root,
                        options.groupings_as_class)

        if options.nodoc == False:
            generate_documentation(output_directory)

        create_pip_package(output_directory)

        print 'Code generation completed successfully!'

    except ydkgen.YdkGenException as e:
        print >> sys.stderr, e.message
        sys.exit(1)
