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
"""compare.py
return True if attributes in entity(lhs) = entity(rhs)
"""

import logging
from enum import Enum
from functools import reduce
from ydk.types import (Empty, Decimal64, FixedBitsDict,
                       YList, YListItem, YLeafList)

LOGGER = logging.getLogger('ydk.tests.unittest')
LOGGER.setLevel(logging.DEBUG)

def is_builtin_type(attr):
    # all the deridved types should have __cmp__ implemented
    if (isinstance(attr, (int, bool, dict, str, long, float)) or
            isinstance(attr, (Enum, Empty, Decimal64, FixedBitsDict)) or
            isinstance(attr, (YLeafList, YListItem))):
        return True
    else:
        return False

class ErrNo(Enum):
    WRONG_VALUE = 0
    WRONG_TYPES = 1
    POPULATION_FAILED = 2
    WRONG_DICT = 3
    WRONG_DICT_VALUE = 4
    WRONG_CLASS = 5

class ErrorMsg(object):
    def __init__(self, lhs, rhs, errno):
        self.lhs = lhs
        self.rhs = rhs
        self.errno = errno

    def __str__(self):
        rhs, lhs, errno = self.rhs, self.lhs, self.errno
        errlhs = "\tlhs = %s, type: %s;\n" % (str(lhs), type(lhs))
        errrhs = "\trhs = %s, type: %s;\n" % (str(rhs), type(rhs))
        if errno == ErrNo.WRONG_VALUE:
            errtyp = "Wrong value:\n"
        elif errno == ErrNo.WRONG_TYPES:
            errtyp = "Wrong types: not comparable\n"
        elif errno == ErrNo.WRONG_CLASS:
            errtyp = "Wrong types:\n"
        elif errno == ErrNo.POPULATION_FAILED:
            errtyp = "Failed population:\n"
        elif errno == ErrNo.WRONG_DICT:
            errtyp = "Wrong dict: different dictionary key\n"
        return ''.join([errtyp, errlhs, errrhs])

    def print_err(self):
        error_str = str(self)
        LOGGER.debug(error_str)


def is_equal(lhs, rhs):
    ret, errtyp = True, None
    if lhs is None and rhs is None or \
            lhs == [] and rhs == []:
        pass
    elif is_builtin_type(lhs) or is_builtin_type(rhs):
        try:
            if lhs != rhs:
                errtyp, ret = ErrNo.WRONG_VALUE, False
        except Exception:
            errtyp, ret = ErrNo.WRONG_TYPES, False
    elif lhs is None or rhs is None:
        errtyp, ret = ErrNo.POPULATION_FAILED, False
    elif isinstance(lhs, YList) and isinstance(rhs, YList) or \
            isinstance(lhs, list) and isinstance(rhs, list):
        if len(lhs) != len(rhs):
            errtyp, ret = ErrNo.WRONG_VALUE, False
        else:
            cmp_lst = zip(lhs, rhs)
            ret = reduce(lambda init, (l, r): init and is_equal(l,r), cmp_lst, True)
    elif lhs.__class__ != rhs.__class__:
        errtyp, ret = ErrNo.WRONG_CLASS, False
    else:
        dict_lhs, dict_rhs = lhs.__dict__, rhs.__dict__
        len_lhs = len(dict_lhs)
        len_rhs = len(dict_rhs)
        if 'i_meta' in dict_lhs:
            len_lhs -= 1
        if 'i_meta' in dict_rhs:
            len_rhs -= 1
        if len_lhs != len_rhs:
            errtyp, ret = ErrNo.WRONG_DICT, False
        for k in dict_lhs:
            if k == 'parent' or k == 'i_meta':
                continue
            elif is_builtin_type(dict_lhs[k]) or is_builtin_type(dict_rhs[k]):
                try:
                    if dict_lhs[k] != dict_rhs[k]:
                        lhs = dict_lhs[k]
                        rhs = dict_rhs[k]
                        errtyp, ret = ErrNo.WRONG_VALUE, False
                except Exception:
                    errtyp, ret = ErrNo.WRONG_TYPES, False
            elif k not in dict_rhs:
                errtyp, ret = ErrNo.WRONG_DICT, False
            elif not is_equal(dict_lhs[k], dict_rhs[k]):
                ret = False

    if ret is False and errtyp is not None:
        err_msg = ErrorMsg(rhs, lhs, errtyp)
        err_msg.print_err()

    return ret
