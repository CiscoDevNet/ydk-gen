

from ydkgen import api_model as atypes

from .test_cases_builder import TestCasesBuilder
from .. import utils


class TestBuilder(object):
    def __init__(self, lang, identity_subclasses):
        self.lang = lang
        self.identity_subclasses = identity_subclasses
        self.test_cases = []

    def build_test(self, package):
        for element in package.owned_elements:
            if utils.is_nonid_class_element(element):
                self._traverse_and_build(element)

    def _traverse_and_build(self, clazz):
        for prop in clazz.properties():
            if utils.is_class_prop(prop):
                ptype = prop.property_type
                self._traverse_and_build(ptype)
                if all((utils.has_terminal_nodes(prop),
                        utils.is_config_prop(prop))):
                    self._build_test_case(ptype)

    def _build_test_case(self, clazz):
        builder = TestCasesBuilder(self.lang, self.identity_subclasses)
        builder.build_test_case(clazz)
        self.test_cases.append(builder)

    @property
    def derived_identities(self):
        for test_case in self.test_cases:
            for identity in test_case.derived_identities:
                yield identity

