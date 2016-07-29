


import re
import collections

from enum import Enum

from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, YLeafList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST,     REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION

from ydk.errors import YPYError, YPYModelError
from ydk.providers._importer import _yang_ns

_meta_table = {
    'YdktestTypeIdentity' : {
        'meta_info' : _MetaInfoClass('YdktestTypeIdentity',
            False, 
            [
            ],
            'ydktest-sanity-types',
            'ydktest-type',
            _yang_ns._namespaces['ydktest-sanity-types'],
        'ydk.models.ydktest_sanity_types'
        ),
    },
    'AnotherOneIdentity' : {
        'meta_info' : _MetaInfoClass('AnotherOneIdentity',
            False, 
            [
            ],
            'ydktest-sanity-types',
            'another-one',
            _yang_ns._namespaces['ydktest-sanity-types'],
        'ydk.models.ydktest_sanity_types'
        ),
    },
    'OtherIdentity' : {
        'meta_info' : _MetaInfoClass('OtherIdentity',
            False, 
            [
            ],
            'ydktest-sanity-types',
            'other',
            _yang_ns._namespaces['ydktest-sanity-types'],
        'ydk.models.ydktest_sanity_types'
        ),
    },
}
