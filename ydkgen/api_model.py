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
  api_model.py

 The meta-model of the generated API.
 Translation process converts the YANG model to classes defined in this module.
"""
from __future__ import absolute_import


from pyang.types import UnionTypeSpec


class Element(object):

    """
        The Element class.

        This is the super class of all modelled elements in the API.

        :attribute:: owned_elements
                    list of `Element` owned by this element

        :attribute:: owner
                    The owner of this `Element`.

        :attribute:: comment
                    The comments associated with this element.
    """

    def __init__(self):
        self.owned_elements = []
        self._owner = None
        self.comment = None
        self.revision = None

    @property
    def owner(self):
        return self._owner

    @owner.setter
    def owner(self, owner):
        self._owner = owner


class Deviation(Element):
    def __init__(self, iskeyword):
        Element.__init__(self)
        self._stmt = None
        self.d_type = None
        self.d_target = None
        self.iskeyword = iskeyword

    @property
    def stmt(self):
        return self._stmt

    @stmt.setter
    def stmt(self, stmt):
        self._stmt = stmt

    def qn(self):
        names = []
        stmt = self.d_target
        while stmt.parent:
            if stmt.keyword not in ('container', 'list', 'rpc'):
                names.append(self.convert_prop_name(stmt))
            else:
                names.append(self.convert_owner_name(stmt))
            stmt = stmt.parent

        return '.'.join(reversed(names))

    def convert_prop_name(self, stmt):
        name = snake_case(stmt.arg)
        if self.iskeyword(name) or self.iskeyword(name.lower()):
            name = '%s_' % name

        if name.startswith('_'):
            name = '%s%s' % ('y', name)
        return name

    def convert_owner_name(self, stmt):
        name = escape_name(stmt.arg)
        if stmt.keyword == 'grouping':
            name = '%sGrouping' % camel_case(name)
        elif stmt.keyword == 'identity':
            name = '%sIdentity' % camel_case(name)
        elif stmt.keyword == 'rpc':
            name = camel_case(name) + 'Rpc'
        else:
            name = camel_case(name)
        if self.iskeyword(name) or self.iskeyword(name.lower()):
            name = '%s_' % name

        if name.startswith('_'):
            name = '%s%s' % ('Y', name)
        return name

    def get_package(self):
        if self.owner is None:
            return None
        if isinstance(self.owner, Package):
            return self.owner
        else:
            if hasattr(self.owner, 'get_package'):
                return self.owner.get_package()


class NamedElement(Element):

    '''

        An abstract element that may have a name
        The name is used for identification of the named element
        within the namespace that is defined or accessible

       :attribute:: name
                   The name of the Element

    '''

    def __init__(self):
        ''' The name of the named element'''
        super(NamedElement, self).__init__()
        self.name = None

    def get_py_mod_name(self):
        """
            Get the python module name that contains this
            NamedElement.
        """
        pkg = get_top_pkg(self)
        if not pkg.bundle_name:
            py_mod_name = 'ydk.models.%s' % pkg.name
        elif pkg.aug_bundle_name:
            py_mod_name = 'ydk.models.%s.%s' % (pkg.aug_bundle_name, pkg.name)
        else:
            py_mod_name = 'ydk.models.%s.%s' % (pkg.bundle_name, pkg.name)
        return py_mod_name

    def get_cpp_header_name(self):
        """
            Get the c++ header that contains this
            NamedElement.
        """
        pkg = get_top_pkg(self)
        if pkg.curr_bundle_name == pkg.bundle_name:
            cpp_header_name = '%s.hpp' % pkg.name
        else:
            cpp_header_name = 'ydk_%s/%s.hpp' % (pkg.bundle_name, pkg.name)
        return cpp_header_name

    def get_meta_py_mod_name(self):
        """
            Get the python meta module that contains the meta model
            information about this NamedElement.

        """
        pkg = get_top_pkg(self)
        if not pkg.bundle_name:
            meta_py_mod_name = 'ydk.models._meta'
        elif pkg.aug_bundle_name:
            meta_py_mod_name = 'ydk.models.%s._meta' % pkg.aug_bundle_name
        else:
            meta_py_mod_name = 'ydk.models.%s._meta' % pkg.bundle_name
        return meta_py_mod_name

    def fqn(self):
        ''' get the Fully Qualified Name '''
        names = []
        element = self
        while element is not None:
            if isinstance(element, Deviation):
                element = element.owner
            names.append(element.name)
            element = element.owner
        return '.'.join(reversed(names))

    def qn(self):
        ''' get the qualified name , name sans
        package name '''
        names = []
        element = self
        while element is not None and not isinstance(element, Package):
            if isinstance(element, Deviation):
                element = element.owner
            names.append(element.name)
            element = element.owner
        return '.'.join(reversed(names))

    def qualified_cpp_name(self):
        ''' get the C++ qualified name , name sans
        package name '''
        names = []
        element = self
        while element is not None and not isinstance(element, Package):
            if isinstance(element, Deviation):
                element = element.owner
            names.append(element.name)
            element = element.owner
        return '::'.join(reversed(names))

    def fully_qualified_cpp_name(self):
        ''' get the C++ qualified name '''
        names = []
        element = self
        while element is not None:
            if isinstance(element, Deviation):
                element = element.owner
            names.append(element.name)
            element = element.owner
        return '::'.join(reversed(names))


class Package(NamedElement):

    """
        Represents a Package in the API
    """

    def __init__(self, iskeyword):
        super(Package, self).__init__()
        self._stmt = None
        self._sub_name = ''
        self._bundle_name = ''
        self._aug_bundle_name = ''
        self._curr_bundle_name = ''
        self._augments_other = False
        self.identity_subclasses = {}
        self.iskeyword = iskeyword

    def qn(self):
        """ Return the qualified name """
        return self.name

    @property
    def is_deviation(self):
        return hasattr(self.stmt, 'is_deviation_module')

    @property
    def is_augment(self):
        return hasattr(self.stmt, 'is_augmented_module')

    @property
    def augments_other(self):
        return self._augments_other
    @augments_other.setter
    def augments_other(self, augments_other):
        self._augments_other = augments_other

    @property
    def bundle_name(self):
        return self._bundle_name

    @bundle_name.setter
    def bundle_name(self, bundle_name):
        self._bundle_name = bundle_name

    @property
    def aug_bundle_name(self):
        return self._aug_bundle_name

    @aug_bundle_name.setter
    def aug_bundle_name(self, aug_bundle_name):
        self._aug_bundle_name = aug_bundle_name

    @property
    def curr_bundle_name(self):
        return self._curr_bundle_name

    @curr_bundle_name.setter
    def curr_bundle_name(self, curr_bundle_name):
        self._curr_bundle_name = curr_bundle_name

    @property
    def sub_name(self):
        if self.bundle_name != '':
            sub = self.bundle_name
        else:
            py_mod_name = self.get_py_mod_name()
            sub = py_mod_name[len('ydk.models.'): py_mod_name.rfind('.')]

        return sub

    @property
    def stmt(self):
        """ Return the `pyang.statements.Statement` associated
            with this package. This is usually a module statement.
        """
        return self._stmt

    @stmt.setter
    def stmt(self, stmt):
        name = stmt.arg.replace('-', '_')
        if self.iskeyword(name) or self.iskeyword(name.lower()):
            name = '%s_' % name
        if name[0] == '_':
            name = 'y%s' % name

        self.name = name
        revision = stmt.search_one('revision')
        if revision is not None:
            self.revision = revision.arg
        self._stmt = stmt
        desc = stmt.search_one('description')
        if desc is not None:
            self.comment = desc.arg

    def imported_types(self):
        """
            Returns a list of all types imported by elements in
            this package.
        """
        imported_types = []
        for clazz in [c for c in self.owned_elements if isinstance(c, Class)]:
            imported_types.extend(clazz.imported_types())
        return imported_types

    def __eq__(self, other):
        if isinstance(other, self.__class__):
            return self.name == other.name
        else:
            return False

    __hash__ = NamedElement.__hash__


class DataType(NamedElement):

    """
        Represents a DataType
    """

    def __init__(self):
        super(DataType, self).__init__()


class Class(NamedElement):

    """
       Represents a Class in the api.
    """

    def __init__(self, iskeyword):
        super(Class, self).__init__()
        self._stmt = None
        self._extends = []
        self._module = None
        self.iskeyword = iskeyword

    @property
    def extends(self):
        """ Returns the immediate super classes of this class. """
        if self.is_identity():
            base = []
            base_stmt = self.stmt.search_one('base')
            if base_stmt is not None and hasattr(base_stmt, 'i_identity'):
                base_identity = base_stmt.i_identity
                if hasattr(base_identity, 'i_class'):
                    base.append(base_identity.i_class)
            return base
        else:
            return self._extends

    def is_identity(self):
        """ Returns True if this is a class for a YANG identity. """
        return self._stmt.keyword == 'identity'

    def is_grouping(self):
        """ Returns True if this is a class for a YANG grouping. """
        return self._stmt.keyword == 'grouping'

    def is_rpc(self):
        return self._stmt.keyword == 'rpc'

    def all_owned_elements(self):
        """ Returns all the owned_element of this class and its super classes."""
        all_owned_elements = []
        for super_class in self.extends:
            all_owned_elements.extend(super_class.all_owned_elements())
        all_owned_elements.extend(self.owned_elements)
        return all_owned_elements

    def properties(self):
        """ Returns the properties defined by this class. """
        return get_properties(self.owned_elements)

    def get_package(self):
        """ Returns the Package that contains this Class. """
        if self.owner is None:
            return None
        if isinstance(self.owner, Package):
            return self.owner
        else:
            if hasattr(self.owner, 'get_package'):
                return self.owner.get_package()

    def imported_types(self):
        """ Returns all types that are referenced in this Class that are not
        from the same package as this Class."""

        imported_types = []
        package = self.get_package()

        # look at the super classes
        for super_class in self.extends:
            if super_class.get_package() != package:
                if super_class not in imported_types:
                    imported_types.append(super_class)

        for p in self.properties():
            prop_types = [p.property_type]
            if isinstance(p.property_type, UnionTypeSpec):
                for child_type_stmt in p.property_type.types:
                    prop_types.extend(self._get_union_types(child_type_stmt, p))
            for prop_type in prop_types:
                if isinstance(prop_type, Class) or isinstance(prop_type, Enum) or isinstance(prop_type, Bits):
                    if prop_type.get_package() != package:
                        if prop_type not in imported_types:
                            imported_types.append(prop_type)
        # do this for nested classes too
        for nested_class in [clazz for clazz in self.owned_elements if isinstance(clazz, Class)]:
            imported_types.extend(
                [c for c in nested_class.imported_types() if not c in imported_types])

        return imported_types

    def _get_union_types(self, type_stmt, p):
        from .builder import TypesExtractor
        prop_type = TypesExtractor().get_property_type(type_stmt)
        prop_types = [prop_type]
        if isinstance(prop_type, UnionTypeSpec):
            for child_type_stmt in prop_type.types:
                prop_types.extend(self._get_union_types(child_type_stmt, p))
        return prop_types

    def get_dependent_siblings(self):
        ''' This will return all types that are referenced by this Class
        or nested Classes that are at the same level as this type within the package and are
        used as super types .

        This is useful to determine which type needs to be printed
        before declaring this type in languages that do not support
        forward referencing like Python '''
        classes_at_same_level = []
        classes_at_same_level.extend(
            [c for c in self.owner.owned_elements if isinstance(c, Class) and c is not self])
        dependent_siblings = []
        package = self.get_package()

        def _walk_supers(clazz):
            for super_class in clazz.extends:
                if super_class.get_package() == package and super_class in classes_at_same_level:
                    if super_class not in dependent_siblings:
                        dependent_siblings.append(super_class)
                _walk_supers(super_class)

        def _walk_nested_classes(clazz):
            for nested_class in [c for c in clazz.owned_elements if isinstance(c, Class)]:
                _walk_supers(nested_class)
                _walk_nested_classes(nested_class)

        _walk_supers(self)
        _walk_nested_classes(self)

        return dependent_siblings

    def is_config(self):
        """
            Returns True if an instance of this Class represents config data.
        """
        if hasattr(self.stmt, 'i_config'):
            return self.stmt.i_config
        elif isinstance(self.owner, Class):
            return self.owner.is_config
        else:
            return True

    @property
    def stmt(self):
        """
            Returns the `pyang.statements.Statement` instance associated with this Class.
        """
        return self._stmt

    @property
    def module(self):
        """
            Returns the module `pyang.statements.Statement` that this Class was derived from.
        """
        return self._module

    @stmt.setter
    def stmt(self, stmt):
        name = escape_name(stmt.arg)
        if stmt.keyword == 'grouping':
            name = '%sGrouping' % camel_case(name)
        elif stmt.keyword == 'identity':
            name = '%sIdentity' % camel_case(name)
        elif stmt.keyword == 'rpc':
            name = camel_case(name) + 'Rpc'
        else:
            name = camel_case(name)
        if self.iskeyword(name) or self.iskeyword(name.lower()):
            name = '%s_' % name
        self.name = name

        if self.name.startswith('_'):
            self.name = '%s%s' % ('Y', name)

        self._stmt = stmt
        desc = stmt.search_one('description')
        if desc is not None:
            self.comment = desc.arg

        if stmt.keyword == 'module':
            self._module = stmt
        else:
            self._module = stmt.i_module

    def get_key_props(self):
        """ Returns a list of the properties of this class that are keys
        of a YANG list. """

        key_props = []

        if self.stmt.keyword == 'list':
            if hasattr(self.stmt, 'i_key'):
                key_stmts = self.stmt.i_key
                # do not use #properties here
                for prop in [p for p in self.owned_elements if isinstance(p, Property)]:
                    if prop.stmt in key_stmts:
                        key_props.append(prop)
        return sorted(key_props, key=lambda k: k.name)

    def __eq__(self, other):
        if isinstance(other, self.__class__):
            return self.fqn() == other.fqn()
        else:
            return False

    @property
    def owner(self):
        return self._owner

    @owner.setter
    def owner(self, owner):
        self._owner = owner
        self.name = _modify_nested_container_with_same_name(self)

    __hash__ = NamedElement.__hash__


class AnyXml(NamedElement):

    """
        Represents an AnyXml element.
    """

    def __init__(self):
        super(AnyXml, self).__init__()
        self._stmt = None

    @property
    def stmt(self):
        """ Returns the `pyang.statements.Statement` instance associated with this AnyXml instance."""
        return self._stmt

    @stmt.setter
    def stmt(self, stmt):
        self._stmt = stmt
        desc = stmt.search_one('description')
        if desc is not None:
            self.comment = desc.arg

    def properties(self):
        return get_properties(self.owned_elements)


class Bits(DataType):

    """
        A DataType representing the bits type in YANG.
    """

    def __init__(self, iskeyword):
        super(DataType, self).__init__()
        self._stmt = None
        self._dictionary = None
        self._pos_map = None
        self.iskeyword = iskeyword

    def __eq__(self, other):
        if isinstance(other, self.__class__):
            return self.fqn() == other.fqn()
        else:
            return False

    def get_package(self):
        """ Returns the Package for this DataType. """
        if self.owner is None:
            return None
        if isinstance(self.owner, Package):
            return self.owner
        else:
            return self.owner.get_package()

    @property
    def stmt(self):
        return self._stmt

    @stmt.setter
    def stmt(self, stmt):
        self._stmt = stmt
        self._dictionary = {}
        self._pos_map = {}
        # the name of the enumeration is derived either from the typedef
        # or the leaf under which it is defined
        leaf_or_typedef = stmt
        while leaf_or_typedef.parent is not None and not leaf_or_typedef.keyword in ('leaf', 'leaf-list', 'typedef'):
            leaf_or_typedef = leaf_or_typedef.parent

        name = '%s' % camel_case(leaf_or_typedef.arg)
        if self.iskeyword(name) or self.iskeyword(name.lower()):
            name = '%s' % name
        self.name = name

        desc = stmt.search_one('description')
        if desc is not None:
            self.comment = desc.arg
        else:
            desc = leaf_or_typedef.search_one('description')
            if desc is not None:
                self.comment = desc.arg
        for bit_stmt in stmt.search('bit'):
            self._dictionary[bit_stmt.arg] = False
            pos_stmt = bit_stmt.search_one('position')
            if pos_stmt is not None:
                self._pos_map[bit_stmt.arg] = pos_stmt.arg

    __hash__ = DataType.__hash__


class Property(NamedElement):

    """ Represents an attribute or reference of a Class.
    """

    def __init__(self, iskeyword):
        super(Property, self).__init__()
        self._stmt = None
        self.is_static = False
        self.featuring_classifiers = []
        self.read_only = False
        self.is_many = False
        # self.property_type = None
        self.default_value = None
        self.visibility = '+'
        self.id = False
        self.ordered = False
        self.unique = False
        self._property_type = None
        self.max_elements = None
        self.min_elements = None
        self.iskeyword = iskeyword

    def is_key(self):
        """ Returns True if this property represents a key of a YANG list."""
        if isinstance(self.owner, Class):
            return self in self.owner.get_key_props()
        return False

    @property
    def stmt(self):
        return self._stmt

    @stmt.setter
    def stmt(self, stmt):
        self._stmt = stmt
        name = snake_case(stmt.arg)
        if self.iskeyword(name) or self.iskeyword(name.lower()):
            name = '%s_' % name
        self.name = name

        if self.name.startswith('_'):
            self.name = '%s%s' % ('y', name)
        if stmt.keyword in ['leaf-list', 'list']:
            self.is_many = True
        desc = stmt.search_one('description')
        if desc is not None:
            self.comment = desc.arg

        max_elem_stmt = stmt.search_one('max-elements')
        min_elem_stmt = stmt.search_one('min-elements')
        if max_elem_stmt:
            self.max_elements = max_elem_stmt.arg
        if min_elem_stmt:
            self.min_elements = min_elem_stmt.arg

    @property
    def property_type(self):
        """ Returns the type of this property. """
        if self._property_type is not None:
            return self._property_type

        if self._stmt is None:
            return None

        if self._stmt.keyword in ['leaf', 'leaf-list']:
            type_stmt = self._stmt.search_one('type')
            return type_stmt.i_type_spec
        else:
            return None

    @property_type.setter
    def property_type(self, property_type):
        self._property_type = property_type


class Enum(DataType):

    """ Represents an enumeration. """

    def __init__(self, iskeyword):
        super(Enum, self).__init__()
        self._stmt = None
        self.literals = []
        self.iskeyword = iskeyword

    def get_package(self):
        """ Returns the Package that this enum is found in. """
        if self.owner is None:
            return None
        if isinstance(self.owner, Package):
            return self.owner
        else:
            return self.owner.get_package()

    @property
    def stmt(self):
        return self._stmt

    @stmt.setter
    def stmt(self, stmt):
        self._stmt = stmt
        # the name of the numeration is derived either from the typedef
        # or the leaf under which it is defined
        leaf_or_typedef = stmt
        while leaf_or_typedef.parent is not None and not leaf_or_typedef.keyword in ('leaf', 'leaf-list', 'typedef'):
            leaf_or_typedef = leaf_or_typedef.parent

        name = '%sEnum' % camel_case(escape_name(leaf_or_typedef.arg))
        if self.iskeyword(name) or self.iskeyword(name.lower()):
            name = '%s_' % name

        if name[0] == '_':
            name = 'Y%s' % name

        self.name = name

        desc = stmt.search_one('description')
        if desc is not None:
            self.comment = desc.arg
        else:
            desc = leaf_or_typedef.search_one('description')
            if desc is not None:
                self.comment = desc.arg
        for enum_stmt in stmt.search('enum'):
            literal = EnumLiteral(self.iskeyword)
            literal.stmt = enum_stmt
            self.literals.append(literal)


class EnumLiteral(NamedElement):

    """ Represents an enumeration literal. """

    def __init__(self, iskeyword):
        super(EnumLiteral, self).__init__()
        self._stmt = None
        self.value = None
        self.iskeyword = iskeyword

    @property
    def stmt(self):
        return self._stmt

    @stmt.setter
    def stmt(self, stmt):
        self._stmt = stmt
        self.name = stmt.arg.replace('-', '_')
        self.name = self.name.replace('+', '__PLUS__')
        self.name = self.name.replace('/', '__FWD_SLASH__')
        self.name = self.name.replace('\\', '__BACK_SLASH__')
        self.name = self.name.replace('.', '__DOT__')
        self.name = self.name.replace('*', '__STAR__')
        self.name = self.name.replace('$', '__DOLLAR__')
        self.name = self.name.replace('@', '__AT__')
        self.name = self.name.replace('#', '__POUND__')
        self.name = self.name.replace('^', '__CARET__')
        self.name = self.name.replace('&', '__AMPERSAND__')
        self.name = self.name.replace('(', '__LPAREN__')
        self.name = self.name.replace(')', '__RPAREN__')
        self.name = self.name.replace('=', '__EQUALS__')
        self.name = self.name.replace('{', '__LCURLY__')
        self.name = self.name.replace('}', '__RCURLY__')
        self.name = self.name.replace("'", '__SQUOTE__')
        self.name = self.name.replace('"', '__DQUOTE__')
        self.name = self.name.replace('<', '__GREATER_THAN__')
        self.name = self.name.replace('>', '__LESS_THAN__')
        self.name = self.name.replace(',', '__COMMA__')
        self.name = self.name.replace(':', '__COLON__')
        self.name = self.name.replace('?', '__QUESTION__')
        self.name = self.name.replace('!', '__BANG__')
        self.name = self.name.replace(';', '__SEMICOLON__')
        self.name = self.name.replace(' ', '_')

        if self.iskeyword(self.name):
            self.name += '_'

        if self.name[0:1].isdigit():
            self.name = 'Y_%s' % self.name

        if self.name[0] == '_':
            self.name = 'Y%s' % self.name

        self.value = stmt.i_value
        desc = stmt.search_one('description')
        if desc is not None:
            self.comment = desc.arg


def get_top_pkg(pkg):
    """
    Get top level Package instance of current NamedElement instance.
    """
    while pkg is not None and not isinstance(pkg, Package):
        pkg = pkg.owner

    return pkg


def get_properties(owned_elements):
    """ get all properties from the owned_elements. """
    props = []
    all_props = []
    all_props.extend([p for p in owned_elements if isinstance(p, Property)])

    # first get the key properties
    key_props = [p for p in all_props if p.is_key()]
    props.extend(sorted(key_props, key=lambda p:p. name))

    non_key_props = [p for p in all_props if not p.is_key()]
    props.extend(sorted(non_key_props, key=lambda p: p.name))

    return props


def _modify_nested_container_with_same_name(named_element):
    if named_element.owner.name.rstrip('_') == named_element.name:
        return '%s_' % named_element.owner.name
    else:
        return named_element.name



def snake_case(input_text):
    snake_case = input_text.replace('-', '_')
    snake_case = snake_case.replace('.', '_')
    return snake_case.lower()


def camel_case(input_text):
    return ''.join([word.title() for word in input_text.split('-')])


def escape_name(name):
    name = name.replace('+', '__PLUS__')
    name = name.replace('/', '__FWD_SLASH__')
    name = name.replace('\\', '__BACK_SLASH__')
    name = name.replace('.', '__DOT__')
    name = name.replace('*', '__STAR__')
    name = name.replace('$', '__DOLLAR__')
    name = name.replace('@', '__AT__')
    name = name.replace('#', '__POUND__')
    name = name.replace('^', '__CARET__')
    name = name.replace('&', '__AMPERSAND__')
    name = name.replace('(', '__LPAREN__')
    name = name.replace(')', '__RPAREN__')
    name = name.replace('=', '__EQUALS__')
    name = name.replace('{', '__LCURLY__')
    name = name.replace('}', '__RCURLY__')
    name = name.replace("'", '__SQUOTE__')
    name = name.replace('"', '__DQUOTE__')
    name = name.replace('<', '__GREATER_THAN__')
    name = name.replace('>', '__LESS_THAN__')
    name = name.replace(',', '__COMMA__')
    name = name.replace(':', '__COLON__')
    name = name.replace('?', '__QUESTION__')
    name = name.replace('!', '__BANG__')
    name = name.replace(';', '__SEMICOLON__')

    return name
