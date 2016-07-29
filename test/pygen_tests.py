"""Python generation tests.

Copyright (c) 2015 by Cisco Systems, Inc.
All rights reserved.
"""

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

import os
import sys
import glob
import filecmp
import logging
import argparse
import subprocess
from difflib import context_diff
from itertools import ifilterfalse, tee
from unittest import TestCase, TestSuite, TextTestRunner

import pip
import ydkgen


logger = logging.getLogger('ydkgen')


def init_verbose_logger():
    """Initialize the logging infra and add a handler."""
    logger.setLevel(logging.DEBUG)
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    logger.addHandler(ch)


def get_ydk_root():
    """Return YDKGEN_HOME."""
    if "YDKGEN_HOME" not in os.environ:
        logger.debug("YDKGEN_HOME not set."
                     " Assuming current directory is working directory.")
        ydk_root = os.getcwd()
    else:
        ydk_root = os.environ['YDKGEN_HOME']
    return ydk_root


def get_test_suite_kwargs(args):
    """Set and return test suite kwargs based on options."""
    kwargs = vars(args)
    del kwargs['verbose']
    ydk_root = get_ydk_root()
    default_profile = ydk_root + '/profiles/test/ydktest.json'
    default_test_root = os.path.join(ydk_root, 'test', 'test-cases')
    kwargs.update({'ydk_root': ydk_root})
    if not kwargs['profile']:
        kwargs.update({'profile': default_profile})
    if not kwargs['test_cases_root']:
        kwargs.update({'test_cases_root': default_test_root})

    return kwargs


class PyGenTest(TestCase):
    """PyGenTest class, generate and compare new APIs with expected APIs."""

    def __init__(self, profile, ydk_root, test_cases_root, groupings_as_class,
                 language='python'):
        self.profile = profile
        self.ydk_root = ydk_root
        self.test_cases_root = test_cases_root
        self.groupings_as_class = groupings_as_class
        self.language = language
        setattr(self, self.language + " Gen", self.translate_and_check)
        self.actual_dir = self.test_cases_root + '/' + self.language + '/actual'
        self.expected_dir = self.test_cases_root + '/' + self.language + '/expected'
        TestCase.__init__(self, self.language + " Gen")


    def id(self):
        return 'Python Gen'

    def compare(self, src, dst):
        """ Compare src with dst."""
        if os.path.isfile(src):
            if src.split('/')[-1].startswith('.'):
                return
            same = filecmp.cmp(src, dst, False)
            if not same:
                self.throw_diff_error_msg(src, dst)
                return

    def throw_diff_error_msg(self, src, dst):
        """Throw diff error message, src and dst should be different."""
        dst_file = open(dst, 'r')
        src_file = open(src, 'r')
        diff = context_diff(dst_file.readlines(), src_file.readlines())
        delta = ''.join(diff)
        self.assertEquals(
            True, True,
            '\n'.join(["File comparison failed for test case "
                       "%s\n" % self._testMethodName,
                       "expected %s \n " % dst,
                       "generated %s \n " % src,
                       "if generated is correct use command \n ",
                       "cp %s %s \n " % (src, os.path.dirname(dst)),
                       "diff %s" % delta]))

    def equal_dirs(self, dir1, dir2, ignore=[]):
        """Checks equality between dir1 and dir2, ignores files in 'ignore' list."""
        dirs_cmp = filecmp.dircmp(dir1, dir2, ignore)
        if (len(dirs_cmp.left_only) > 0 or len(dirs_cmp.right_only) > 0 or
                len(dirs_cmp.funny_files) > 0):
            return False
        (_, mismatch, errors) = filecmp.cmpfiles(dir1, dir2, dirs_cmp.common_files, shallow=False)
        if len(mismatch) > 0 or len(errors) > 0:
            return False
        for common_dir in dirs_cmp.common_dirs:
            new_dir1 = os.path.join(dir1, common_dir)
            new_dir2 = os.path.join(dir2, common_dir)
            if not self.equal_dirs(new_dir1, new_dir2, ignore):
                return False
        return True

    def translate_and_check(self):
        """Generate and compare ydktest package with expected APIs."""
        ydkgen.YdkGenerator(self.actual_dir,
                            self.ydk_root,
                            self.groupings_as_class,
                            self.language,
                            'profile',
                            True).generate(self.profile)
        actual_dir = self.actual_dir + '/' + self.language + '/ydk/models'
        expected_dir = self.expected_dir + '/ydk/models'
        ignore = ['.gitignore']

        self.assertTrue(self.equal_dirs(actual_dir, expected_dir, ignore))


class PyBundlePatchTest(TestCase):
    """PyBundlePatchTest class, generate, patch and compare APIs."""

    def __init__(self, profile, ydk_root, groupings_as_class, test_cases_root,
                 aug_base, aug_contrib, aug_compare):
        self.profile = profile
        self.ydk_root = ydk_root
        self.groupings_as_class = groupings_as_class
        self.test_cases_root = test_cases_root
        self.aug_base = aug_base
        self.aug_contrib = aug_contrib
        self.aug_compare = aug_compare
        setattr(self, 'python' + " Patch", self.translate_and_check)
        TestCase.__init__(self, 'python' + " Patch")
        self.actual_dir = self.test_cases_root + '/python/actual/bundle_aug'
        self.expected_dir = self.test_cases_root + '/python/expected/bundle_aug'

    def generate_pkg(self, profile, bundle_name, target_dir, *args):
        """Generate YDK distribution tarball based on profile file."""
        py_sdk_root = target_dir + '/python/%s' % bundle_name
        ydkgen.YdkGenerator(target_dir, *args).generate(profile)
        cwd = os.getcwd()
        os.chdir(py_sdk_root)
        args = [sys.executable, 'setup.py', 'sdist']
        exit_code = subprocess.call(args, env=os.environ.copy())
        self.assertEquals(exit_code, 0, "Failed to generate bundle package %s" % bundle_name)
        os.chdir(cwd)

    def cmp_patched_modules(self, namespace_base_module, gen_aug_path):
        """Compare patched modules and generated modules."""
        __import__('ydk.models.%s' % namespace_base_module)
        __import__('ydk.models.%s._meta' % namespace_base_module)
        module = sys.modules['ydk.models.%s' % namespace_base_module]
        patched_dir = os.path.dirname(module.__file__)
        patched_module_dir = os.path.join(patched_dir, '_aug_patch')
        patched_meta_dir = os.path.join(patched_dir, '_meta', '_aug_patch')

        gen_module_dir = os.path.join(gen_aug_path)
        gen_meta_dir = os.path.join(gen_aug_path, '_meta')

        return (self.cmp_dirs(patched_module_dir, gen_module_dir, self.same_module) and
                self.cmp_dirs(patched_meta_dir, gen_meta_dir, self.same_meta))

    def cmp_dirs(self, patched_dir, gen_dir, cmp_func):
        """Compare files in patched_dir and gen_dir using compare function."""
        for module in os.listdir(patched_dir):
            if module.endswith('.py'):
                src = os.path.join(patched_dir, module)
                dst = os.path.join(gen_dir, module)
                if not cmp_func(src, dst):
                    return False
        return True

    def same_module(self, src, dst):
        """Return true if src and dst is same."""
        logger.debug("pygen_test: comparing \n\t%s with \n\t%s", src, dst)
        return filecmp.cmp(src, dst, False)

    def same_meta(self, src, dst):
        """Return true if content in src and dst is same."""
        # partition meta file to _meta_table and parent pointers.
        def predicate(line):
            if line.startswith('_meta_table['):
                return False
            return True
        def partition(predicate, lines):
            lines_1, lines_2 = tee(lines)
            table = filter(predicate, lines_1)
            meta = sorted(ifilterfalse(predicate, lines_2))
            return table, meta
        with open(src) as src_file:
            src_lines = src_file.readlines()
        with open(dst) as dst_file:
            dst_lines = dst_file.readlines()
        src_table, src_meta = partition(predicate, src_lines)
        dst_table, dst_meta = partition(predicate, dst_lines)
        logger.debug("pygen_test: comparing \n\t%s with \n\t%s", src, dst)
        return src_table == dst_table and src_meta == dst_meta

    def get_cmp_aug_path(self, compare, actual_dir):
        """Get path for generated augmentation modules."""
        return os.path.join(actual_dir, 'python', compare, 'ydk', 'models', compare, '_aug')

    def translate_and_check(self):
        """Generate YDK core library, augmentation bundles, import module from
        augmentation bundles at runtime to generate patched module, then compare
        patched moudle with generated module."""
        def get_bundle_name(profile):
            """Return bundle name from profile file name."""
            return os.path.basename(profile).split('.')[0].replace('-', '_')
        # TODO: add wrapper function to get arguments for YdkGenerator
        # ydk core
        self.generate_pkg(None, 'ydk', self.actual_dir, self.ydk_root,
                          self.groupings_as_class, 'python', 'core', True)
        # bundle packages, augmentation base bundle and augmentation contributors
        base = get_bundle_name(self.aug_base)
        patch = []
        for profile in self.aug_contrib + [self.aug_base]:
            bundle_name = get_bundle_name(profile)
            patch.append(bundle_name)
            self.generate_pkg(profile, bundle_name, self.actual_dir, self.ydk_root,
                              self.groupings_as_class, 'python', 'bundle', True)
        # bundle package for comparison
        compare = bundle_name = get_bundle_name(self.aug_compare)
        self.generate_pkg(self.aug_compare, bundle_name, self.actual_dir, self.ydk_root,
                          self.groupings_as_class, 'python', 'bundle', True)

        patch_pkgs = []
        for p in ['ydk'] + sorted(patch):
            # installation order should be: 0. ydk core library 1. ietf, 2. ydktest_aug_ietf_[0-9]
            patch_pkgs.extend(glob.glob(self.actual_dir + '/python/%s/dist/*.tar.gz' % p))

        for pkg in patch_pkgs:
            pip.main(['install', pkg])

        # import and generate aug_base modules
        cmp_aug_path = self.get_cmp_aug_path(compare, self.actual_dir)
        self.assertEquals(self.cmp_patched_modules(base, cmp_aug_path), True)

        # uninstall bundle packages
        for pkg in patch:
            pip.main(['uninstall', '-y', 'ydk-models-' + pkg.replace('_', '-')])

def suite(kwargs):
    suite = TestSuite()
    suite.addTest(PyBundlePatchTest(**kwargs))

    # Enabled class sorting for PyGenTest
    del kwargs['aug_base']
    del kwargs['aug_contrib']
    del kwargs['aug_compare']

    suite.addTest(PyGenTest(**kwargs))
    kwargs.update({'language': 'cpp'})
    suite.addTest(PyGenTest(**kwargs))

    return suite


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Compare generated YDK APIs.")

    parser.add_argument("--profile", type=str,
                        help="Take options from a profile file,"
                             " any CLI targets ignored.")

    parser.add_argument("--test-cases-root", type=str,
                        help="The root directory for the test cases.")

    parser.add_argument("-v", "--verbose", action="store_true",
                        help="Verbose mode.")

    parser.add_argument("--groupings-as-class", action="store_true",
                        help="Consider yang groupings as classes.")

    parser.add_argument("--aug-base", type=str,
                        help="Path to profile file for augmentation bundle "
                             "contains models being augmented.")

    parser.add_argument("--aug-contrib", type=str, nargs="+",
                        help="List of profile files for augmentation "
                             "contributor bundle.")

    parser.add_argument("--aug-compare", type=str,
                        help="The profile file combines YANG models in "
                             "augmentation base bundle and contributors.")

    args = parser.parse_args()

    if args.verbose:
        init_verbose_logger()

    kwargs = get_test_suite_kwargs(args)

    ret = not TextTestRunner(verbosity=2).run(suite(kwargs)).wasSuccessful()
    sys.exit(ret)
