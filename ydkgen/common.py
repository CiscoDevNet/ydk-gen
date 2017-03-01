from __future__ import print_function
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
import logging
import hashlib
import keyword
from collections import OrderedDict
from pyang import types as ptypes
from ydkgen import api_model as atypes

"""
 common.py

 YANG model driven API, common definitions.
"""

#  ----------------------------------------------------------------
#  Generic lookups
# -----------------------------------------------------------------
yang_int = {
    'int8',
    'int16',
    'int32',
    'int64',
    'uint8',
    'uint16',
    'uint32',
    'uint64',
}

yang_int_ranges = {
    'int8': (-128, 127),
    'int16': (-32768, 32767),
    'int32': (-2147483648, 2147483647),
    'int64': (-9223372036854775808, 9223372036854775807),
    'uint8': (0, 255),
    'uint16': (0, 65535),
    'uint32': (0, 4294967295),
    'uint64': (0, 18446744073709551615),
}

yang_base_types = {
    'binary',
    'bits',
    'boolean',
    'decimal64',
    'empty',
    'identityref',
    'instance-identifier',
    'int8',
    'int16',
    'int32',
    'int64',
    'leafref',
    'string',
    'uint8',
    'uint16',
    'uint32',
    'uint64',
    # union, separate handling
    # enumeration, separate handling
}

container_nodes = {
    'module',
    'container',
    'choice',
    'case',
    'list',
    'augment',
    #    'grouping',
    'uses',
    'rpc',
    'input',
    'output',
}


class YdkGenException(Exception):

    """Exception raised when there is a problem in the generation.

        .. attribute:: msg
                      The message describing the error.

    """

    def __init__(self, msg):
        self.msg = msg
        logger = logging.getLogger('ydkgen')
        if len(logger.handlers) == 1:
            print(msg)


def yang_id(stmt):
    if hasattr(stmt, 'arg') and stmt.arg is not None:
        return stmt.arg.replace(':', '_')
    else:
        return None

def merge_file_path_segments(segs):
    '''Merge the segs to form a path '''
    return_seg = ''

    for seg in segs:
        if not seg.length() == 0 and not return_seg.endswith('/'):
            return_seg = '%s/' % (return_seg)
        return_seg = '%s%s' % (return_seg, seg)
    return return_seg


def ispythonkeyword(word):
    return keyword.iskeyword(word) or word in ('None', 'parent')


def iscppkeyword(word):
    return word in ('parent', 'operator', 'inline', 'default', 'virtual',
                    'children', 'value', 'auto', 'entity', 'int', 'static',
                    'final', 'template', 'index', 'protected', 'true', 'false',
                    'default' , 'auto', 'static', 'or', 'do', 'new', 'delete',
                    'private', 'public', 'export' , 'virtual', 'for', 'and',
                    'break', 'case', 'catch', 'float', 'long', 'return',
                    'explicit', 'class', 'if', 'try', 'while', 'and', 'or',
                    'const', 'continue', 'double', 'else', 'value', 'namespace',
                    'operation')


def get_sphinx_ref_label(named_element):
    return named_element.fqn().replace('.', '_')


def split_to_words(input_text):
    words = []
    ''' A word boundary starts if the current character is
    in Caps and the previous character is in lowercase
    for example NetworkElement , at Element the E is in Caps
    and the previoud character is k in lower

    or if the current character is in Caps and the next character
    in in lower case ApplicationCLIEvent for Event while reaching E
    the next chracter is v

    '''
    word = None
    previous_caps = False
    for index, ch in enumerate(input_text):
        if ch.isupper():
            if not previous_caps:
                # word boundary
                if word is not None:
                    words.append(word)
                word = ch
            else:
                # previous was caps
                if index != len(input_text) - 1:
                    if input_text[index + 1].islower():
                        # this is a word boundary
                        if word is not None:
                            words.append(word)
                        word = ch
                    else:
                        # add it to the current word
                        word = '%s%s' % (word, ch)
                else:
                    word = '%s%s' % (word, ch)

            previous_caps = True
        else:
            if word is None:
                word = ch
            else:
                word = '%s%s' % (word, ch)
            previous_caps = False

    words.append(word)
    return words


def convert_to_reStructuredText(yang_text):
    if isinstance(yang_text, bytes):
        yang_text = yang_text.decode('utf-8')
    reSt = yang_text
    if reSt is not None and len(reSt) > 0:
        reSt = yang_text.replace('\\', '\\\\')
        reSt = reSt.replace(':', '\:')
        reSt = reSt.replace('_', '\_')
        reSt = reSt.replace('-', '\-')
        reSt = reSt.replace('*', '\*')
        reSt = reSt.replace('|', '\|')
    return reSt


def is_config_stmt(stmt):

    if hasattr(stmt, 'i_config'):
        is_config = stmt.i_config
        if is_config is not None:
            return is_config
    parent = stmt.parent
    if parent is None:
        return True
    else:
        return is_config_stmt(parent)


def get_module_name(stmt):
    if stmt.keyword == 'module':
        return stmt.arg

    module_stmt = stmt.i_module
    if module_stmt.i_including_modulename is not None:
        return module_stmt.i_including_modulename
    else:
        return module_stmt.arg


def sort_classes_at_same_level(classes, sort_clazz):
    ''' Returns a list of the classes in the same order  '''
    if sort_clazz:
        classes = sorted(classes, key=lambda cls: cls.name)
    classes_processed = []
    classes_not_processed = OrderedDict()
    for clazz in classes:
        dependent_siblings = clazz.get_dependent_siblings()
        if len(dependent_siblings) == 0:
            classes_processed.append(clazz)
        else:
            classes_not_processed[clazz] = dependent_siblings
    classes_not_processed = OrderedDict(classes_not_processed.items())
    while len(classes_not_processed) > 0:
        for clazz in list(classes_not_processed.keys()):
            dependent_siblings = classes_not_processed[clazz]

            not_processed = False
            for sibling in dependent_siblings:
                if sibling not in classes_processed:
                    not_processed = True
                    break
            if not not_processed:
                # all dependents are processed so go ahead and add to processed
                classes_processed.append(clazz)
                del classes_not_processed[clazz]

    return classes_processed


def get_rst_file_name(named_element):
    if hasattr(named_element, 'get_package'):
        package = named_element.get_package()
    else:
        package = named_element
    filename = package.bundle_name + named_element.fqn()
    filename = filename.encode('utf-8')
    hex_name = 'gen_doc_%s' % hashlib.sha1(filename).hexdigest()
    return hex_name



def has_terminal_nodes(element):
    # has leaf or leaflist
    if isinstance(element, atypes.Property):
        ptype = element.property_type
    else:
        ptype = element
    for p in ptype.properties():
        if is_terminal_prop(p):
            return True
    return False


def is_config_prop(prop):
    is_config = True
    if hasattr(prop.stmt, 'i_config'):
        is_config = prop.stmt.i_config
    return is_config


def snake_case(input_text):
    snake_case = input_text.replace('-', '_')
    snake_case = snake_case.replace('.', '_')
    return snake_case.lower()


def get_include_guard_name(name, file_index=-1):
        if file_index > -1:
            return '_{0}_{1}_'.format(name.upper(), file_index)
        else:
            return '_{0}_'.format(name.upper())


def is_nonid_class_element(element):
    return isinstance(element, atypes.Class) and not element.is_identity()


def is_class_element(element):
    return isinstance(element, atypes.Class)


def is_identity_element(element):
    return isinstance(element, atypes.Class) and element.is_identity()


def is_list_element(element):
    return element.stmt.keyword == 'list'


def is_mandatory_element(element):
    mandatory = element.stmt.search_one('mandatory')
    return mandatory is not None and mandatory.arg == 'true'


def is_pkg_element(element):
    return isinstance(element, atypes.Package)


def is_presence_element(element):
    return element.stmt.search_one('presence') is not None


def is_prop_element(element):
    return isinstance(element, atypes.Property)


def is_class_prop(prop):
    return is_class_element(prop.property_type)


def is_decimal64_prop(prop):
    return isinstance(prop.property_type, ptypes.Decimal64TypeSpec)


def is_empty_prop(prop):
    return isinstance(prop.property_type, ptypes.EmptyTypeSpec)


def is_identity_prop(prop):
    return is_identity_element(prop.property_type)


def is_identityref_prop(prop):
    return (isinstance(prop.property_type, atypes.Class) and
            prop.property_type.is_identity() and
            prop.stmt.i_leafref_ptr is not None)


def is_leaflist_prop(prop):
    return prop.stmt.keyword == 'leaf-list'


def is_leafref_prop(prop):
    return (isinstance(prop.property_type, ptypes.PathTypeSpec) and
            prop.stmt.i_leafref_ptr is not None)


def is_path_prop(prop):
    return isinstance(prop.property_type, ptypes.PathTypeSpec)


def is_reference_prop(prop):
    return (is_leafref_prop(prop) or is_identityref_prop(prop))


def is_terminal_prop(prop):
    return prop.stmt.keyword in ('leaf', 'leaflist')


def is_union_prop(prop):
    return is_union_type_spec(prop.property_type)


def is_union_type_spec(type_spec):
    return isinstance(type_spec, ptypes.UnionTypeSpec)


def is_identityref_type_spec(type_spec):
    return isinstance(type_spec, ptypes.IdentityrefTypeSpec)


def is_match_all(pattern):
    return pattern in ('[^\*].*', '\*')


def get_typedef_stmt(type_stmt):
    while all([hasattr(type_stmt, 'i_typedef') and
               type_stmt.i_typedef is not None]):
        type_stmt = type_stmt.i_typedef.search_one('type')
    return type_stmt


def get_top_class(clazz):
    while not isinstance(clazz.owner, atypes.Package):
        clazz = clazz.owner
    return clazz


def get_obj_name(clazz):
    obj_names = []
    while not isinstance(clazz, atypes.Package):
        obj_name = clazz.name.lower()
        obj_names.append(obj_name)
        clazz = clazz.owner
    return '_'.join(reversed(obj_names))


def get_qn(lang, element):
    qn = ''
    if lang == 'py':
        qn = element.qn()
    elif lang == 'cpp':
        qn = element.fully_qualified_cpp_name()
    return qn


def get_element_path(lang, element, length=None):
    # path is consists of path segments(seg)
    path = []
    sep = get_path_sep(lang)
    while not is_pkg_element(element):
        seg = _get_element_seg(element)
        if all((is_list_element(element),
                not is_pkg_element(element.owner),
                path)):
            # list/leaf-list contains one element
            seg += '[0]'
        path.append(seg)
        element = element.owner

    if length is None:
        return sep.join(reversed(path))
    else:
        # ever used?
        path = list(reversed(path))[:length]
        return sep.join(path)


def _get_element_seg(element):
    seg = ''
    if any((is_pkg_element(element.owner),
            is_prop_element(element))):
        seg = element.name
    else:
        for prop in element.owner.properties():
            if prop.stmt == element.stmt:
                seg = prop.name
    return seg.lower()


def get_path_sep(lang):
    sep = ''
    if lang == 'py':
        sep = '.'
    elif lang == 'cpp':
        sep = '->'
    return sep
