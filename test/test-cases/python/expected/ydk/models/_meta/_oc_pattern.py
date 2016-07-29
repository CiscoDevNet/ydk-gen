


import re
import collections

from enum import Enum

from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, YLeafList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST,     REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION

from ydk.errors import YPYError, YPYModelError
from ydk.providers._importer import _yang_ns

_meta_table = {
    'OcA.B' : {
        'meta_info' : _MetaInfoClass('OcA.B',
            False, 
            [
            _MetaInfoClassMember('b', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'b',
                'oc-pattern', False),
            ],
            'oc-pattern',
            'B',
            _yang_ns._namespaces['oc-pattern'],
        'ydk.models.oc_pattern'
        ),
    },
    'OcA' : {
        'meta_info' : _MetaInfoClass('OcA',
            False, 
            [
            _MetaInfoClassMember('a', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                blah
                ''',
                'a',
                'oc-pattern', True),
            _MetaInfoClassMember('B', REFERENCE_CLASS, 'B' , 'ydk.models.oc_pattern', 'OcA.B', 
                [], [], 
                '''                ''',
                'b',
                'oc-pattern', False),
            ],
            'oc-pattern',
            'oc-A',
            _yang_ns._namespaces['oc-pattern'],
        'ydk.models.oc_pattern'
        ),
    },
}
_meta_table['OcA.B']['meta_info'].parent =_meta_table['OcA']['meta_info']
