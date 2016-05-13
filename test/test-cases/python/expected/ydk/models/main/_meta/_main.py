


import re
import collections

from enum import Enum

from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, YLeafList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST,     REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION

from ydk.errors import YPYError, YPYDataValidationError
from ydk.models import _yang_ns

_meta_table = {
    'A.MainAug1_C' : {
        'meta_info' : _MetaInfoClass('A.MainAug1_C',
            False, 
            [
            _MetaInfoClassMember('two', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                blah
                ''',
                'two',
                'main-aug1', False),
            ],
            'main-aug1',
            'main-aug1_C',
            _yang_ns._namespaces['main-aug1'],
        'ydk.models.main.main'
        ),
    },
    'A.MainAug2_C' : {
        'meta_info' : _MetaInfoClass('A.MainAug2_C',
            False, 
            [
            _MetaInfoClassMember('three', ATTRIBUTE, 'int' , None, None, 
                [(-32768, 32767)], [], 
                '''                blah
                ''',
                'three',
                'main-aug2', False),
            ],
            'main-aug2',
            'main-aug2_C',
            _yang_ns._namespaces['main-aug2'],
        'ydk.models.main.main'
        ),
    },
    'A.MainAug2_D' : {
        'meta_info' : _MetaInfoClass('A.MainAug2_D',
            False, 
            [
            _MetaInfoClassMember('poo', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                blah
                ''',
                'poo',
                'main-aug2', False),
            ],
            'main-aug2',
            'main-aug2_D',
            _yang_ns._namespaces['main-aug2'],
        'ydk.models.main.main'
        ),
    },
    'A.MainAug3_C' : {
        'meta_info' : _MetaInfoClass('A.MainAug3_C',
            False, 
            [
            _MetaInfoClassMember('meh', ATTRIBUTE, 'int' , None, None, 
                [(-128, 127)], [], 
                '''                blah
                ''',
                'meh',
                'main-aug3', False),
            ],
            'main-aug3',
            'main-aug3_C',
            _yang_ns._namespaces['main-aug3'],
        'ydk.models.main.main'
        ),
    },
    'A.MainAug3_D' : {
        'meta_info' : _MetaInfoClass('A.MainAug3_D',
            False, 
            [
            _MetaInfoClassMember('buh', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                blah
                ''',
                'buh',
                'main-aug3', False),
            ],
            'main-aug3',
            'main-aug3_D',
            _yang_ns._namespaces['main-aug3'],
        'ydk.models.main.main'
        ),
    },
    'A' : {
        'meta_info' : _MetaInfoClass('A',
            False, 
            [
            _MetaInfoClassMember('main-aug1_C', REFERENCE_CLASS, 'MainAug1_C' , 'ydk.models.main.main', 'A.MainAug1_C', 
                [], [], 
                '''                ''',
                'main_aug1_c',
                'main-aug1', False),
            _MetaInfoClassMember('main-aug2_C', REFERENCE_CLASS, 'MainAug2_C' , 'ydk.models.main.main', 'A.MainAug2_C', 
                [], [], 
                '''                ''',
                'main_aug2_c',
                'main-aug2', False),
            _MetaInfoClassMember('main-aug2_D', REFERENCE_CLASS, 'MainAug2_D' , 'ydk.models.main.main', 'A.MainAug2_D', 
                [], [], 
                '''                ''',
                'main_aug2_d',
                'main-aug2', False),
            _MetaInfoClassMember('main-aug3_C', REFERENCE_CLASS, 'MainAug3_C' , 'ydk.models.main.main', 'A.MainAug3_C', 
                [], [], 
                '''                ''',
                'main_aug3_c',
                'main-aug3', False),
            _MetaInfoClassMember('main-aug3_D', REFERENCE_CLASS, 'MainAug3_D' , 'ydk.models.main.main', 'A.MainAug3_D', 
                [], [], 
                '''                ''',
                'main_aug3_d',
                'main-aug3', False),
            _MetaInfoClassMember('one', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                blah
                ''',
                'one',
                'main', False),
            ],
            'main',
            'A',
            _yang_ns._namespaces['main'],
        'ydk.models.main.main'
        ),
    },
}
_meta_table['A.MainAug1_C']['meta_info'].parent =_meta_table['A']['meta_info']
_meta_table['A.MainAug2_C']['meta_info'].parent =_meta_table['A']['meta_info']
_meta_table['A.MainAug2_D']['meta_info'].parent =_meta_table['A']['meta_info']
_meta_table['A.MainAug3_C']['meta_info'].parent =_meta_table['A']['meta_info']
_meta_table['A.MainAug3_D']['meta_info'].parent =_meta_table['A']['meta_info']
