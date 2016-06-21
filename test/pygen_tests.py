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
Python generation tests

Copyright (c) 2015 by Cisco Systems, Inc.
All rights reserved.

'''
import sys
import os
import unittest
import filecmp
import difflib
import ydkgen
from optparse import OptionParser
import logging

yang_mod_path = []

logger = logging.getLogger('ydkgen')

def init_verbose_logger():
    """ Initialize the logging infra and add a handler """
    logger.setLevel(logging.DEBUG)

    # create a console logger
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    # add the handlers to the logger
    logger.addHandler(ch)

def suite(profile, test_cases_root, groupings_as_class):
    
    class PyGenTest(unittest.TestCase):
        def __init__(self, profile, test_cases_root, language, groupings_as_class):
            self.profile = profile
            self.groupings_as_class = groupings_as_class
            self.language = language
            setattr(self, language + " Gen", self.translate_and_check)
            self.actual_directory = test_cases_root + '/' + language + '/actual'
            self.expected_directory = test_cases_root + '/' + language + '/expected'
            unittest.TestCase.__init__(self, language + " Gen")

        def id(self):
            return 'Python Gen'
    
        def compare(self, src, dest):
            if os.path.isfile(src):
                if src.split('/')[-1].startswith('.'):
                    return
                same = filecmp.cmp(src, dest, False)
                if not same:
                    dest_file = open(dest, 'r')
                    src_file = open(src, 'r')
                    diff = difflib.context_diff(dest_file.readlines(), src_file.readlines())
                    delta = ''.join(diff)
                    self.assertEquals(same, True, 'File comparison failed for test case %s\n\
                       expected %s \n \
                       generated %s \n \
                       if generated is correct use command \n \
                       cp %s %s \n \
                         diff %s'%(self.test_case_name, dest, src, src, os.path.dirname(dest), delta))
                    return
        
        def are_dir_trees_equal(self, dir1, dir2, ignore=[]):
           
            dirs_cmp = filecmp.dircmp(dir1, dir2, ignore)
            if len(dirs_cmp.left_only)>0 or len(dirs_cmp.right_only)>0 or \
                len(dirs_cmp.funny_files)>0:
                return False
            (_, mismatch, errors) =  filecmp.cmpfiles(
                dir1, dir2, dirs_cmp.common_files, shallow=False)
            if len(mismatch)>0 or len(errors)>0:
                return False
            for common_dir in dirs_cmp.common_dirs:
                new_dir1 = os.path.join(dir1, common_dir)
                new_dir2 = os.path.join(dir2, common_dir)
                if not self.are_dir_trees_equal(new_dir1, new_dir2, ignore):
                    return False
            return True
    
        def translate_and_check(self):

            ydkgen.YdkGenerator().generate(self.profile, self.actual_directory, ydk_root, self.groupings_as_class, self.language)

            def check_diff_files(dcmp, diff_files):

                for name in dcmp.diff_files:
                    diff_files.append('File %s/%s does not match'%(dcmp.left,name))
                   
                for sub_dcmp in dcmp.subdirs.values():
                    check_diff_files(sub_dcmp, diff_files)
            
            self.assertTrue(self.are_dir_trees_equal(self.actual_directory + '/' + self.language + '/ydk/models', self.expected_directory + '/ydk/models', ['.gitignore']))


        def setUp(self):
            pass

        def tearDown(self):
            pass


        def testName(self):
            self.yang_file
    
    suite = unittest.TestSuite()

    suite.addTest(PyGenTest(profile, test_cases_root, 'python', groupings_as_class))
    suite.addTest(PyGenTest(profile, test_cases_root, 'cpp', groupings_as_class))
    
    return suite


if __name__ == "__main__":
    
    parser = OptionParser(usage="usage: %prog [options]",
                          version="%prog 0.3.0")

    parser.add_option("--profile",
                      type=str,
                      dest="profile",
                      help="Take options from a profile file, any CLI targets ignored")

    parser.add_option("--test-cases-root",
                      type=str,
                      dest="test_cases_root",
                      help="The root directory for the test cases.")

    parser.add_option("-v", "--verbose",
                      action="store_true",
                      dest="verbose",
                      default=False,
                      help="Verbose mode")

   
    parser.add_option("--groupings-as-class",
                      action="store_true",
                      dest="groupings_as_class",
                      default=False,
                      help="Consider yang groupings as classes.")

    (options, args) = parser.parse_args()

    if options.verbose:
        init_verbose_logger()
    
    if not os.environ.has_key('YDKGEN_HOME'):
        logger.debug('YDKGEN_HOME not set. Assuming current directory is working directory.')
        ydk_root = os.getcwd()
    else:
        ydk_root = os.environ['YDKGEN_HOME']
    
    test_cases_root = ydk_root + '/test/test-cases/'
    if options.test_cases_root is not None:
        test_cases_root = options.test_cases_root

    profile = ydk_root + '/profiles/test/ydktest.json'
    if options.profile is not None:
        profile =options.profile
    
     
    
    ret = unittest.TextTestRunner(verbosity=2).run(suite(profile, test_cases_root, options.groupings_as_class)).wasSuccessful()
    if ret:
        sys.exit(0)
    else:
        sys.exit(1)

def load_tests(loader , tests, pattern):
    return suite()
