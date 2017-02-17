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
test_builder.py

Build information needed for a test program.
"""

from .test_cases_builder import TestCasesBuilder
from ydkgen.common import is_nonid_class_element, has_terminal_nodes, \
                         is_config_prop, is_class_prop


class TestBuilder(object):
    """Comprehends information needed for a single test program.

    Attributes:
        lang (str): output langauge.
        identity_subclasses (dict): dictionary holds derived identity map.
        test_cases (list of TestCasesBuilder): represents single test case.
    """

    def __init__(self, lang, identity_subclasses):
        self.lang = lang
        self.identity_subclasses = identity_subclasses
        self.test_cases = []

    def build_test(self, package):
        """Build test program."""
        for element in package.owned_elements:
            if is_nonid_class_element(element):
                self._traverse_and_build(element)

    def _traverse_and_build(self, clazz):
        """Traverse and build test cases."""
        if all((has_terminal_nodes(clazz),
                is_config_prop(clazz))):
            self._build_test_case(clazz)
        for prop in clazz.properties():
            if is_class_prop(prop):
                ptype = prop.property_type
                self._traverse_and_build(ptype)

    def _build_test_case(self, clazz):
        """Build single test case for target `clazz`."""
        builder = TestCasesBuilder(self.lang, self.identity_subclasses)
        builder.build_test_case(clazz)
        self.test_cases.append(builder)

    @property
    def derived_identities(self):
        """Get derived identities used in this test program."""
        for test_case in self.test_cases:
            for identity in test_case.derived_identities:
                yield identity
