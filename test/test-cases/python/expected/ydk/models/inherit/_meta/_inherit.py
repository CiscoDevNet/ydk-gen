


import re
import collections

from enum import Enum

from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, YLeafList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST,     REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION

from ydk.errors import YPYError, YPYDataValidationError
from ydk.models import _yang_ns

_meta_table = {
    'Runner.One' : {
        'meta_info' : _MetaInfoClass('Runner.One',
            False, 
            [
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'inherit', False),
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'inherit', False),
            ],
            'inherit',
            'one',
            _yang_ns._namespaces['inherit'],
        'ydk.models.inherit.inherit'
        ),
    },
    'Runner' : {
        'meta_info' : _MetaInfoClass('Runner',
            False, 
            [
            _MetaInfoClassMember('jumper', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                ''',
                'jumper',
                'inherit', False),
            _MetaInfoClassMember('one', REFERENCE_CLASS, 'One' , 'ydk.models.inherit.inherit', 'Runner.One', 
                [], [], 
                '''                config for one_level data
                ''',
                'one',
                'inherit', False),
            ],
            'inherit',
            'runner',
            _yang_ns._namespaces['inherit'],
        'ydk.models.inherit.inherit'
        ),
    },
}
_meta_table['Runner.One']['meta_info'].parent =_meta_table['Runner']['meta_info']
