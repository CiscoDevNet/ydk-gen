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
