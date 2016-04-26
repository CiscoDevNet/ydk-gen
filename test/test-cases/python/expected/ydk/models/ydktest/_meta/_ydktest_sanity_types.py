


import re
import collections

from enum import Enum

from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST,     REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION

from ydk.errors import YPYError, YPYDataValidationError
from ydk.models import _yang_ns

_meta_table = {
    'YdktestType_Identity' : {
        'meta_info' : _MetaInfoClass('YdktestType_Identity',
            False, 
            [
            ],
            'ydktest-sanity-types',
            'ydktest-type',
            _yang_ns._namespaces['ydktest-sanity-types'],
        'ydk.models.ydktest.ydktest_sanity_types'
        ),
    },
    'AnotherOne_Identity' : {
        'meta_info' : _MetaInfoClass('AnotherOne_Identity',
            False, 
            [
            ],
            'ydktest-sanity-types',
            'another-one',
            _yang_ns._namespaces['ydktest-sanity-types'],
        'ydk.models.ydktest.ydktest_sanity_types'
        ),
    },
    'Other_Identity' : {
        'meta_info' : _MetaInfoClass('Other_Identity',
            False, 
            [
            ],
            'ydktest-sanity-types',
            'other',
            _yang_ns._namespaces['ydktest-sanity-types'],
        'ydk.models.ydktest.ydktest_sanity_types'
        ),
    },
}
