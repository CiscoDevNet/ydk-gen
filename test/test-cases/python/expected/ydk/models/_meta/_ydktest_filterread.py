


import re
import collections

from enum import Enum

from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, YLeafList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST,     REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION

from ydk.errors import YPYError, YPYModelError
from ydk.providers._importer import _yang_ns

_meta_table = {
    'A.B.C' : {
        'meta_info' : _MetaInfoClass('A.B.C',
            False, 
            [
            ],
            'ydktest-filterread',
            'c',
            _yang_ns._namespaces['ydktest-filterread'],
        'ydk.models.ydktest_filterread'
        ),
    },
    'A.B.D.E' : {
        'meta_info' : _MetaInfoClass('A.B.D.E',
            False, 
            [
            _MetaInfoClassMember('e1', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'e1',
                'ydktest-filterread', False),
            _MetaInfoClassMember('e2', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'e2',
                'ydktest-filterread', False),
            ],
            'ydktest-filterread',
            'e',
            _yang_ns._namespaces['ydktest-filterread'],
        'ydk.models.ydktest_filterread'
        ),
    },
    'A.B.D' : {
        'meta_info' : _MetaInfoClass('A.B.D',
            False, 
            [
            _MetaInfoClassMember('d1', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'd1',
                'ydktest-filterread', False),
            _MetaInfoClassMember('d2', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'd2',
                'ydktest-filterread', False),
            _MetaInfoClassMember('d3', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'd3',
                'ydktest-filterread', False),
            _MetaInfoClassMember('e', REFERENCE_CLASS, 'E' , 'ydk.models.ydktest_filterread', 'A.B.D.E', 
                [], [], 
                '''                ''',
                'e',
                'ydktest-filterread', False),
            ],
            'ydktest-filterread',
            'd',
            _yang_ns._namespaces['ydktest-filterread'],
        'ydk.models.ydktest_filterread'
        ),
    },
    'A.B.F' : {
        'meta_info' : _MetaInfoClass('A.B.F',
            False, 
            [
            _MetaInfoClassMember('f1', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'f1',
                'ydktest-filterread', False),
            ],
            'ydktest-filterread',
            'f',
            _yang_ns._namespaces['ydktest-filterread'],
        'ydk.models.ydktest_filterread'
        ),
    },
    'A.B' : {
        'meta_info' : _MetaInfoClass('A.B',
            False, 
            [
            _MetaInfoClassMember('b1', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'b1',
                'ydktest-filterread', False),
            _MetaInfoClassMember('b2', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'b2',
                'ydktest-filterread', False),
            _MetaInfoClassMember('b3', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'b3',
                'ydktest-filterread', False),
            _MetaInfoClassMember('c', REFERENCE_CLASS, 'C' , 'ydk.models.ydktest_filterread', 'A.B.C', 
                [], [], 
                '''                ''',
                'c',
                'ydktest-filterread', False),
            _MetaInfoClassMember('d', REFERENCE_CLASS, 'D' , 'ydk.models.ydktest_filterread', 'A.B.D', 
                [], [], 
                '''                ''',
                'd',
                'ydktest-filterread', False),
            _MetaInfoClassMember('f', REFERENCE_CLASS, 'F' , 'ydk.models.ydktest_filterread', 'A.B.F', 
                [], [], 
                '''                ''',
                'f',
                'ydktest-filterread', False),
            ],
            'ydktest-filterread',
            'b',
            _yang_ns._namespaces['ydktest-filterread'],
        'ydk.models.ydktest_filterread'
        ),
    },
    'A.Lst' : {
        'meta_info' : _MetaInfoClass('A.Lst',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                ''',
                'number',
                'ydktest-filterread', True),
            _MetaInfoClassMember('value', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'value',
                'ydktest-filterread', False),
            ],
            'ydktest-filterread',
            'lst',
            _yang_ns._namespaces['ydktest-filterread'],
        'ydk.models.ydktest_filterread'
        ),
    },
    'A' : {
        'meta_info' : _MetaInfoClass('A',
            False, 
            [
            _MetaInfoClassMember('a1', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'a1',
                'ydktest-filterread', False),
            _MetaInfoClassMember('a2', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'a2',
                'ydktest-filterread', False),
            _MetaInfoClassMember('a3', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'a3',
                'ydktest-filterread', False),
            _MetaInfoClassMember('b', REFERENCE_CLASS, 'B' , 'ydk.models.ydktest_filterread', 'A.B', 
                [], [], 
                '''                ''',
                'b',
                'ydktest-filterread', False),
            _MetaInfoClassMember('lst', REFERENCE_LIST, 'Lst' , 'ydk.models.ydktest_filterread', 'A.Lst', 
                [], [], 
                '''                ''',
                'lst',
                'ydktest-filterread', False),
            ],
            'ydktest-filterread',
            'a',
            _yang_ns._namespaces['ydktest-filterread'],
        'ydk.models.ydktest_filterread'
        ),
    },
}
_meta_table['A.B.D.E']['meta_info'].parent =_meta_table['A.B.D']['meta_info']
_meta_table['A.B.C']['meta_info'].parent =_meta_table['A.B']['meta_info']
_meta_table['A.B.D']['meta_info'].parent =_meta_table['A.B']['meta_info']
_meta_table['A.B.F']['meta_info'].parent =_meta_table['A.B']['meta_info']
_meta_table['A.B']['meta_info'].parent =_meta_table['A']['meta_info']
_meta_table['A.Lst']['meta_info'].parent =_meta_table['A']['meta_info']
