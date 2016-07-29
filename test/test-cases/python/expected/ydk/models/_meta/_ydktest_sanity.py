


import re
import collections

from enum import Enum

from ydk._core._dm_meta_info import _MetaInfoClassMember, _MetaInfoClass, _MetaInfoEnum
from ydk.types import Empty, YList, YLeafList, DELETE, Decimal64, FixedBitsDict
from ydk._core._dm_meta_info import ATTRIBUTE, REFERENCE_CLASS, REFERENCE_LIST, REFERENCE_LEAFLIST,     REFERENCE_IDENTITY_CLASS, REFERENCE_ENUM_CLASS, REFERENCE_BITS, REFERENCE_UNION

from ydk.errors import YPYError, YPYModelError
from ydk.providers._importer import _yang_ns

_meta_table = {
    'YdkEnumIntTestEnum' : _MetaInfoEnum('YdkEnumIntTestEnum', 'ydk.models.ydktest_sanity',
        {
            'any':'ANY',
        }, 'ydktest-sanity', _yang_ns._namespaces['ydktest-sanity']),
    'YdkEnumTestEnum' : _MetaInfoEnum('YdkEnumTestEnum', 'ydk.models.ydktest_sanity',
        {
            'not-set':'NOT_SET',
            'none':'NONE',
            'local':'LOCAL',
            'remote':'REMOTE',
        }, 'ydktest-sanity', _yang_ns._namespaces['ydktest-sanity']),
    'BaseIdentityIdentity' : {
        'meta_info' : _MetaInfoClass('BaseIdentityIdentity',
            False, 
            [
            ],
            'ydktest-sanity',
            'base-identity',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.InbtwList.Ldata.Subc.SubcSubl1' : {
        'meta_info' : _MetaInfoClass('Runner.InbtwList.Ldata.Subc.SubcSubl1',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'subc-subl1',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.InbtwList.Ldata.Subc' : {
        'meta_info' : _MetaInfoClass('Runner.InbtwList.Ldata.Subc',
            False, 
            [
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', False),
            _MetaInfoClassMember('subc-subl1', REFERENCE_LIST, 'SubcSubl1' , 'ydk.models.ydktest_sanity', 'Runner.InbtwList.Ldata.Subc.SubcSubl1', 
                [], [], 
                '''                one list data
                ''',
                'subc_subl1',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'subc',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.InbtwList.Ldata' : {
        'meta_info' : _MetaInfoClass('Runner.InbtwList.Ldata',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('subc', REFERENCE_CLASS, 'Subc' , 'ydk.models.ydktest_sanity', 'Runner.InbtwList.Ldata.Subc', 
                [], [], 
                '''                one list subcontainer data
                ''',
                'subc',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'ldata',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.InbtwList' : {
        'meta_info' : _MetaInfoClass('Runner.InbtwList',
            False, 
            [
            _MetaInfoClassMember('ldata', REFERENCE_LIST, 'Ldata' , 'ydk.models.ydktest_sanity', 'Runner.InbtwList.Ldata', 
                [], [], 
                '''                one list data
                ''',
                'ldata',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'inbtw-list',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.LeafRef.One.Two' : {
        'meta_info' : _MetaInfoClass('Runner.LeafRef.One.Two',
            False, 
            [
            _MetaInfoClassMember('self-ref-one-name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'self_ref_one_name',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'two',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.LeafRef.One' : {
        'meta_info' : _MetaInfoClass('Runner.LeafRef.One',
            False, 
            [
            _MetaInfoClassMember('name-of-one', ATTRIBUTE, 'str' , None, None, 
                [], ['(([0\\-9]\\|[1\\-9][0\\-9]\\|1[0\\-9][0\\-9]\\|2[0\\-4][0\\-9]\\|25[0\\-5])\\.){3}([0\\-9]\\|[1\\-9][0\\-9]\\|1[0\\-9][0\\-9]\\|2[0\\-4][0\\-9]\\|25[0\\-5])(%[\\p{N}\\p{L}]+)?'], 
                '''                ''',
                'name_of_one',
                'ydktest-sanity', False),
            _MetaInfoClassMember('two', REFERENCE_CLASS, 'Two' , 'ydk.models.ydktest_sanity', 'Runner.LeafRef.One.Two', 
                [], [], 
                '''                ''',
                'two',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'one',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.LeafRef' : {
        'meta_info' : _MetaInfoClass('Runner.LeafRef',
            False, 
            [
            _MetaInfoClassMember('one', REFERENCE_CLASS, 'One' , 'ydk.models.ydktest_sanity', 'Runner.LeafRef.One', 
                [], [], 
                '''                ''',
                'one',
                'ydktest-sanity', False),
            _MetaInfoClassMember('ref-inbtw', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'ref_inbtw',
                'ydktest-sanity', False),
            _MetaInfoClassMember('ref-one-name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'ref_one_name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('ref-three-sub1-sub2-number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                ''',
                'ref_three_sub1_sub2_number',
                'ydktest-sanity', False),
            _MetaInfoClassMember('ref-two-sub1-number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                ''',
                'ref_two_sub1_number',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'leaf-ref',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.NotSupported1.NotSupported12' : {
        'meta_info' : _MetaInfoClass('Runner.NotSupported1.NotSupported12',
            False, 
            [
            _MetaInfoClassMember('some-leaf', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'some_leaf',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'not-supported-1-2',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.NotSupported1' : {
        'meta_info' : _MetaInfoClass('Runner.NotSupported1',
            False, 
            [
            _MetaInfoClassMember('not-supported-1-2', REFERENCE_CLASS, 'NotSupported12' , 'ydk.models.ydktest_sanity', 'Runner.NotSupported1.NotSupported12', 
                [], [], 
                '''                ''',
                'not_supported_1_2',
                'ydktest-sanity', False),
            _MetaInfoClassMember('not-supported-leaf', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'not_supported_leaf',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'not-supported-1',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.NotSupported2' : {
        'meta_info' : _MetaInfoClass('Runner.NotSupported2',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                Integer key for not supported list
                ''',
                'number',
                'ydktest-sanity', True),
            ],
            'ydktest-sanity',
            'not-supported-2',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.One.OneAug' : {
        'meta_info' : _MetaInfoClass('Runner.One.OneAug',
            False, 
            [
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity-augm', False),
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity-augm', False),
            ],
            'ydktest-sanity-augm',
            'one-aug',
            _yang_ns._namespaces['ydktest-sanity-augm'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.One' : {
        'meta_info' : _MetaInfoClass('Runner.One',
            False, 
            [
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', False),
            _MetaInfoClassMember('one-aug', REFERENCE_CLASS, 'OneAug' , 'ydk.models.ydktest_sanity', 'Runner.One.OneAug', 
                [], [], 
                '''                config for one_level data
                ''',
                'one_aug',
                'ydktest-sanity-augm', False),
            ],
            'ydktest-sanity',
            'one',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.OneList.Ldata' : {
        'meta_info' : _MetaInfoClass('Runner.OneList.Ldata',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'ldata',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.OneList.OneAugList.Ldata' : {
        'meta_info' : _MetaInfoClass('Runner.OneList.OneAugList.Ldata',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity-augm', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity-augm', False),
            ],
            'ydktest-sanity-augm',
            'ldata',
            _yang_ns._namespaces['ydktest-sanity-augm'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.OneList.OneAugList' : {
        'meta_info' : _MetaInfoClass('Runner.OneList.OneAugList',
            False, 
            [
            _MetaInfoClassMember('enabled', ATTRIBUTE, 'bool' , None, None, 
                [], [], 
                '''                integer value type
                ''',
                'enabled',
                'ydktest-sanity-augm', False),
            _MetaInfoClassMember('ldata', REFERENCE_LIST, 'Ldata' , 'ydk.models.ydktest_sanity', 'Runner.OneList.OneAugList.Ldata', 
                [], [], 
                '''                one list data
                ''',
                'ldata',
                'ydktest-sanity-augm', False),
            ],
            'ydktest-sanity-augm',
            'one-aug-list',
            _yang_ns._namespaces['ydktest-sanity-augm'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.OneList' : {
        'meta_info' : _MetaInfoClass('Runner.OneList',
            False, 
            [
            _MetaInfoClassMember('ldata', REFERENCE_LIST, 'Ldata' , 'ydk.models.ydktest_sanity', 'Runner.OneList.Ldata', 
                [], [], 
                '''                one list data
                ''',
                'ldata',
                'ydktest-sanity', False, max_elements=5),
            _MetaInfoClassMember('one-aug-list', REFERENCE_CLASS, 'OneAugList' , 'ydk.models.ydktest_sanity', 'Runner.OneList.OneAugList', 
                [], [], 
                '''                config for one_level list data
                ''',
                'one_aug_list',
                'ydktest-sanity-augm', False),
            ],
            'ydktest-sanity',
            'one-list',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Runner2' : {
        'meta_info' : _MetaInfoClass('Runner.Runner2',
            False, 
            [
            _MetaInfoClassMember('some-leaf', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                ''',
                'some_leaf',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'runner-2',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Three.Sub1.Sub2' : {
        'meta_info' : _MetaInfoClass('Runner.Three.Sub1.Sub2',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'sub2',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Three.Sub1' : {
        'meta_info' : _MetaInfoClass('Runner.Three.Sub1',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', False),
            _MetaInfoClassMember('sub2', REFERENCE_CLASS, 'Sub2' , 'ydk.models.ydktest_sanity', 'Runner.Three.Sub1.Sub2', 
                [], [], 
                '''                subconfig2 for config container
                ''',
                'sub2',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'sub1',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Three' : {
        'meta_info' : _MetaInfoClass('Runner.Three',
            False, 
            [
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', False),
            _MetaInfoClassMember('sub1', REFERENCE_CLASS, 'Sub1' , 'ydk.models.ydktest_sanity', 'Runner.Three.Sub1', 
                [], [], 
                '''                subconfig1 for config container
                ''',
                'sub1',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'three',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.ThreeList.Ldata.Subl1.SubSubl1' : {
        'meta_info' : _MetaInfoClass('Runner.ThreeList.Ldata.Subl1.SubSubl1',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'sub-subl1',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.ThreeList.Ldata.Subl1' : {
        'meta_info' : _MetaInfoClass('Runner.ThreeList.Ldata.Subl1',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('sub-subl1', REFERENCE_LIST, 'SubSubl1' , 'ydk.models.ydktest_sanity', 'Runner.ThreeList.Ldata.Subl1.SubSubl1', 
                [], [], 
                '''                one list data
                ''',
                'sub_subl1',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'subl1',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.ThreeList.Ldata' : {
        'meta_info' : _MetaInfoClass('Runner.ThreeList.Ldata',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('subl1', REFERENCE_LIST, 'Subl1' , 'ydk.models.ydktest_sanity', 'Runner.ThreeList.Ldata.Subl1', 
                [], [], 
                '''                one list data
                ''',
                'subl1',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'ldata',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.ThreeList' : {
        'meta_info' : _MetaInfoClass('Runner.ThreeList',
            False, 
            [
            _MetaInfoClassMember('ldata', REFERENCE_LIST, 'Ldata' , 'ydk.models.ydktest_sanity', 'Runner.ThreeList.Ldata', 
                [], [], 
                '''                one list data
                ''',
                'ldata',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'three-list',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Two.Sub1' : {
        'meta_info' : _MetaInfoClass('Runner.Two.Sub1',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'sub1',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Two' : {
        'meta_info' : _MetaInfoClass('Runner.Two',
            False, 
            [
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', False),
            _MetaInfoClassMember('sub1', REFERENCE_CLASS, 'Sub1' , 'ydk.models.ydktest_sanity', 'Runner.Two.Sub1', 
                [], [], 
                '''                subconfig1 for config container
                ''',
                'sub1',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'two',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.TwoList.Ldata.Subl1' : {
        'meta_info' : _MetaInfoClass('Runner.TwoList.Ldata.Subl1',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'subl1',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.TwoList.Ldata' : {
        'meta_info' : _MetaInfoClass('Runner.TwoList.Ldata',
            False, 
            [
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', True),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('subl1', REFERENCE_LIST, 'Subl1' , 'ydk.models.ydktest_sanity', 'Runner.TwoList.Ldata.Subl1', 
                [], [], 
                '''                one list data
                ''',
                'subl1',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'ldata',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.TwoList' : {
        'meta_info' : _MetaInfoClass('Runner.TwoList',
            False, 
            [
            _MetaInfoClassMember('ldata', REFERENCE_LIST, 'Ldata' , 'ydk.models.ydktest_sanity', 'Runner.TwoList.Ldata', 
                [], [], 
                '''                one list data
                ''',
                'ldata',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'two-list',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Ytypes.BuiltInT.EmbededEnumEnum' : _MetaInfoEnum('EmbededEnumEnum', 'ydk.models.ydktest_sanity',
        {
            'zero':'ZERO',
            'two':'TWO',
            'seven':'SEVEN',
        }, 'ydktest-sanity', _yang_ns._namespaces['ydktest-sanity']),
    'Runner.Ytypes.BuiltInT' : {
        'meta_info' : _MetaInfoClass('Runner.Ytypes.BuiltInT',
            False, 
            [
            _MetaInfoClassMember('bincoded', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is binary value
                ''',
                'bincoded',
                'ydktest-sanity', False),
            _MetaInfoClassMember('bits-llist', REFERENCE_LEAFLIST, 'BitsLlist_Bits' , 'ydk.models.ydktest_sanity', 'Runner.Ytypes.BuiltInT.BitsLlist_Bits', 
                [], [], 
                '''                ''',
                'bits_llist',
                'ydktest-sanity', False),
            _MetaInfoClassMember('bits-value', REFERENCE_BITS, 'BitsValue_Bits' , 'ydk.models.ydktest_sanity', 'Runner.Ytypes.BuiltInT.BitsValue_Bits', 
                [], [], 
                '''                this is bits type value
                ''',
                'bits_value',
                'ydktest-sanity', False),
            _MetaInfoClassMember('bool-value', ATTRIBUTE, 'bool' , None, None, 
                [], [], 
                '''                this is boolean type value
                ''',
                'bool_value',
                'ydktest-sanity', False),
            _MetaInfoClassMember('deci64', ATTRIBUTE, 'Decimal64' , None, None, 
                [('1', '3.14'), ('10', 'None'), ('20', '92233720368547758.07')], [], 
                '''                this is decimal value
                ''',
                'deci64',
                'ydktest-sanity', False),
            _MetaInfoClassMember('embeded-enum', REFERENCE_ENUM_CLASS, 'EmbededEnumEnum' , 'ydk.models.ydktest_sanity', 'Runner.Ytypes.BuiltInT.EmbededEnumEnum', 
                [], [], 
                '''                enum embeded in leaf
                ''',
                'embeded_enum',
                'ydktest-sanity', False),
            _MetaInfoClassMember('emptee', ATTRIBUTE, 'Empty' , None, None, 
                [], [], 
                '''                this is empty value
                ''',
                'emptee',
                'ydktest-sanity', False),
            _MetaInfoClassMember('enum-int-value', REFERENCE_UNION, 'str' , None, None, 
                [], [], 
                '''                enum int type
                ''',
                'enum_int_value',
                'ydktest-sanity', False, [
                    _MetaInfoClassMember('enum-int-value', REFERENCE_ENUM_CLASS, 'YdkEnumIntTestEnum' , 'ydk.models.ydktest_sanity', 'YdkEnumIntTestEnum', 
                        [], [], 
                        '''                        enum int type
                        ''',
                        'enum_int_value',
                        'ydktest-sanity', False),
                    _MetaInfoClassMember('enum-int-value', ATTRIBUTE, 'int' , None, None, 
                        [(1, 4096)], [], 
                        '''                        enum int type
                        ''',
                        'enum_int_value',
                        'ydktest-sanity', False),
                ]),
            _MetaInfoClassMember('enum-llist', REFERENCE_LEAFLIST, 'YdkEnumTestEnum' , 'ydk.models.ydktest_sanity', 'YdkEnumTestEnum', 
                [], [], 
                '''                A leaf-list of enum
                ''',
                'enum_llist',
                'ydktest-sanity', False),
            _MetaInfoClassMember('enum-value', REFERENCE_ENUM_CLASS, 'YdkEnumTestEnum' , 'ydk.models.ydktest_sanity', 'YdkEnumTestEnum', 
                [], [], 
                '''                this is enum type value
                ''',
                'enum_value',
                'ydktest-sanity', False),
            _MetaInfoClassMember('identity-llist', REFERENCE_LEAFLIST, 'BaseIdentityIdentity' , 'ydk.models.ydktest_sanity', 'BaseIdentityIdentity', 
                [], [], 
                '''                A leaf-list of identityref
                ''',
                'identity_llist',
                'ydktest-sanity', False),
            _MetaInfoClassMember('identity-ref-value', REFERENCE_IDENTITY_CLASS, 'BaseIdentityIdentity' , 'ydk.models.ydktest_sanity', 'BaseIdentityIdentity', 
                [], [], 
                '''                ''',
                'identity_ref_value',
                'ydktest-sanity', False),
            _MetaInfoClassMember('leaf-ref', ATTRIBUTE, 'int' , None, None, 
                [(-128, 127)], [], 
                '''                leaf-ref
                ''',
                'leaf_ref',
                'ydktest-sanity', False),
            _MetaInfoClassMember('llstring', REFERENCE_LEAFLIST, 'str' , None, None, 
                [], [], 
                '''                A list of string
                ''',
                'llstring',
                'ydktest-sanity', False),
            _MetaInfoClassMember('llunion', REFERENCE_UNION, 'str' , None, None, 
                [], [], 
                '''                A list of union
                ''',
                'llunion',
                'ydktest-sanity', False, [
                    _MetaInfoClassMember('llunion', REFERENCE_LEAFLIST, 'int' , None, None, 
                        [(-32768, 32767)], [], 
                        '''                        A list of union
                        ''',
                        'llunion',
                        'ydktest-sanity', False),
                    _MetaInfoClassMember('llunion', REFERENCE_LEAFLIST, 'str' , None, None, 
                        [], [], 
                        '''                        A list of union
                        ''',
                        'llunion',
                        'ydktest-sanity', False),
                ]),
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number16', ATTRIBUTE, 'int' , None, None, 
                [(-32768, 32767)], [], 
                '''                16 bit integer value type
                ''',
                'number16',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number32', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number32',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number64', ATTRIBUTE, 'int' , None, None, 
                [(-9223372036854775808, 9223372036854775807)], [], 
                '''                integer value type
                ''',
                'number64',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number8', ATTRIBUTE, 'int' , None, None, 
                [(-128, 127)], [], 
                '''                 8 bit integer value type
                ''',
                'number8',
                'ydktest-sanity', False),
            _MetaInfoClassMember('u_number16', ATTRIBUTE, 'int' , None, None, 
                [(0, 65535)], [], 
                '''                16 bit uinteger value type
                ''',
                'u_number16',
                'ydktest-sanity', False),
            _MetaInfoClassMember('u_number32', ATTRIBUTE, 'int' , None, None, 
                [(0, 4294967295)], [], 
                '''                32 bit uinteger value type
                ''',
                'u_number32',
                'ydktest-sanity', False),
            _MetaInfoClassMember('u_number64', ATTRIBUTE, 'long' , None, None, 
                [(0, 18446744073709551615L)], [], 
                '''                64 bit uinteger value type
                ''',
                'u_number64',
                'ydktest-sanity', False),
            _MetaInfoClassMember('u_number8', ATTRIBUTE, 'int' , None, None, 
                [(0, 255)], [], 
                '''                 8 bit uinteger value type
                ''',
                'u_number8',
                'ydktest-sanity', False),
            _MetaInfoClassMember('younion', REFERENCE_UNION, 'str' , None, None, 
                [], [], 
                '''                union test value
                ''',
                'younion',
                'ydktest-sanity', False, [
                    _MetaInfoClassMember('younion', REFERENCE_ENUM_CLASS, 'YdkEnumTestEnum' , 'ydk.models.ydktest_sanity', 'YdkEnumTestEnum', 
                        [], [], 
                        '''                        union test value
                        ''',
                        'younion',
                        'ydktest-sanity', False),
                    _MetaInfoClassMember('younion', ATTRIBUTE, 'int' , None, None, 
                        [(0, 63)], [], 
                        '''                        union test value
                        ''',
                        'younion',
                        'ydktest-sanity', False),
                ]),
            _MetaInfoClassMember('younion-list', REFERENCE_UNION, 'str' , None, None, 
                [], [], 
                '''                members of the younion
                ''',
                'younion_list',
                'ydktest-sanity', False, [
                    _MetaInfoClassMember('younion-list', REFERENCE_UNION, 'str' , None, None, 
                        [], [], 
                        '''                        members of the younion
                        ''',
                        'younion_list',
                        'ydktest-sanity', False, [
                            _MetaInfoClassMember('younion-list', REFERENCE_LEAFLIST, 'int' , None, None, 
                                [(0, 4294967295)], [], 
                                '''                                members of the younion
                                ''',
                                'younion_list',
                                'ydktest-sanity', False),
                            _MetaInfoClassMember('younion-list', REFERENCE_LEAFLIST, 'str' , None, None, 
                                [], [], 
                                '''                                members of the younion
                                ''',
                                'younion_list',
                                'ydktest-sanity', False),
                        ]),
                    _MetaInfoClassMember('younion-list', REFERENCE_LEAFLIST, 'str' , None, None, 
                        [], [], 
                        '''                        members of the younion
                        ''',
                        'younion_list',
                        'ydktest-sanity', False),
                    _MetaInfoClassMember('younion-list', REFERENCE_LEAFLIST, 'str' , None, None, 
                        [], [], 
                        '''                        members of the younion
                        ''',
                        'younion_list',
                        'ydktest-sanity', False),
                ]),
            _MetaInfoClassMember('younion-recursive', REFERENCE_UNION, 'str' , None, None, 
                [], [], 
                '''                Recursive union leaf
                ''',
                'younion_recursive',
                'ydktest-sanity', False, [
                    _MetaInfoClassMember('younion-recursive', REFERENCE_UNION, 'str' , None, None, 
                        [], [], 
                        '''                        Recursive union leaf
                        ''',
                        'younion_recursive',
                        'ydktest-sanity', False, [
                            _MetaInfoClassMember('younion-recursive', ATTRIBUTE, 'int' , None, None, 
                                [(0, 4294967295)], [], 
                                '''                                Recursive union leaf
                                ''',
                                'younion_recursive',
                                'ydktest-sanity', False),
                            _MetaInfoClassMember('younion-recursive', ATTRIBUTE, 'str' , None, None, 
                                [], [], 
                                '''                                Recursive union leaf
                                ''',
                                'younion_recursive',
                                'ydktest-sanity', False),
                        ]),
                    _MetaInfoClassMember('younion-recursive', ATTRIBUTE, 'int' , None, None, 
                        [(-128, 127)], [], 
                        '''                        Recursive union leaf
                        ''',
                        'younion_recursive',
                        'ydktest-sanity', False),
                ]),
            ],
            'ydktest-sanity',
            'built-in-t',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Ytypes.DerivedT' : {
        'meta_info' : _MetaInfoClass('Runner.Ytypes.DerivedT',
            False, 
            [
            ],
            'ydktest-sanity',
            'derived-t',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner.Ytypes' : {
        'meta_info' : _MetaInfoClass('Runner.Ytypes',
            False, 
            [
            _MetaInfoClassMember('built-in-t', REFERENCE_CLASS, 'BuiltInT' , 'ydk.models.ydktest_sanity', 'Runner.Ytypes.BuiltInT', 
                [], [], 
                '''                config for built-in types
                ''',
                'built_in_t',
                'ydktest-sanity', False),
            _MetaInfoClassMember('derived-t', REFERENCE_CLASS, 'DerivedT' , 'ydk.models.ydktest_sanity', 'Runner.Ytypes.DerivedT', 
                [], [], 
                '''                config for one_level derived data types
                ''',
                'derived_t',
                'ydktest-sanity', False),
            _MetaInfoClassMember('enabled', ATTRIBUTE, 'Empty' , None, None, 
                [], [], 
                '''                ''',
                'enabled',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'ytypes',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'Runner' : {
        'meta_info' : _MetaInfoClass('Runner',
            False, 
            [
            _MetaInfoClassMember('inbtw-list', REFERENCE_CLASS, 'InbtwList' , 'ydk.models.ydktest_sanity', 'Runner.InbtwList', 
                [], [], 
                '''                config for one_level list data
                ''',
                'inbtw_list',
                'ydktest-sanity', False),
            _MetaInfoClassMember('leaf-ref', REFERENCE_CLASS, 'LeafRef' , 'ydk.models.ydktest_sanity', 'Runner.LeafRef', 
                [], [], 
                '''                ''',
                'leaf_ref',
                'ydktest-sanity', False),
            _MetaInfoClassMember('not-supported-1', REFERENCE_CLASS, 'NotSupported1' , 'ydk.models.ydktest_sanity', 'Runner.NotSupported1', 
                [], [], 
                '''                ''',
                'not_supported_1',
                'ydktest-sanity', False),
            _MetaInfoClassMember('not-supported-2', REFERENCE_LIST, 'NotSupported2' , 'ydk.models.ydktest_sanity', 'Runner.NotSupported2', 
                [], [], 
                '''                ''',
                'not_supported_2',
                'ydktest-sanity', False),
            _MetaInfoClassMember('one', REFERENCE_CLASS, 'One' , 'ydk.models.ydktest_sanity', 'Runner.One', 
                [], [], 
                '''                config for one_level data
                ''',
                'one',
                'ydktest-sanity', False),
            _MetaInfoClassMember('one-list', REFERENCE_CLASS, 'OneList' , 'ydk.models.ydktest_sanity', 'Runner.OneList', 
                [], [], 
                '''                config for one_level list data
                ''',
                'one_list',
                'ydktest-sanity', False),
            _MetaInfoClassMember('runner-2', REFERENCE_CLASS, 'Runner2' , 'ydk.models.ydktest_sanity', 'Runner.Runner2', 
                [], [], 
                '''                ''',
                'runner_2',
                'ydktest-sanity', False),
            _MetaInfoClassMember('three', REFERENCE_CLASS, 'Three' , 'ydk.models.ydktest_sanity', 'Runner.Three', 
                [], [], 
                '''                config for one_level data
                ''',
                'three',
                'ydktest-sanity', False),
            _MetaInfoClassMember('three-list', REFERENCE_CLASS, 'ThreeList' , 'ydk.models.ydktest_sanity', 'Runner.ThreeList', 
                [], [], 
                '''                config for one_level list data
                ''',
                'three_list',
                'ydktest-sanity', False),
            _MetaInfoClassMember('two', REFERENCE_CLASS, 'Two' , 'ydk.models.ydktest_sanity', 'Runner.Two', 
                [], [], 
                '''                config for one_level data
                ''',
                'two',
                'ydktest-sanity', False),
            _MetaInfoClassMember('two-list', REFERENCE_CLASS, 'TwoList' , 'ydk.models.ydktest_sanity', 'Runner.TwoList', 
                [], [], 
                '''                config for one_level list data
                ''',
                'two_list',
                'ydktest-sanity', False),
            _MetaInfoClassMember('ytypes', REFERENCE_CLASS, 'Ytypes' , 'ydk.models.ydktest_sanity', 'Runner.Ytypes', 
                [], [], 
                '''                config for one_level data types
                ''',
                'ytypes',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'runner',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'SubTest.OneAug' : {
        'meta_info' : _MetaInfoClass('SubTest.OneAug',
            False, 
            [
            _MetaInfoClassMember('name', ATTRIBUTE, 'str' , None, None, 
                [], [], 
                '''                this is string value
                ''',
                'name',
                'ydktest-sanity', False),
            _MetaInfoClassMember('number', ATTRIBUTE, 'int' , None, None, 
                [(-2147483648, 2147483647)], [], 
                '''                integer value type
                ''',
                'number',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'one-aug',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'SubTest' : {
        'meta_info' : _MetaInfoClass('SubTest',
            False, 
            [
            _MetaInfoClassMember('one-aug', REFERENCE_CLASS, 'OneAug' , 'ydk.models.ydktest_sanity', 'SubTest.OneAug', 
                [], [], 
                '''                config for one_level data
                ''',
                'one_aug',
                'ydktest-sanity', False),
            ],
            'ydktest-sanity',
            'sub-test',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'ChildIdentityIdentity' : {
        'meta_info' : _MetaInfoClass('ChildIdentityIdentity',
            False, 
            [
            ],
            'ydktest-sanity',
            'child-identity',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
    'ChildChildIdentityIdentity' : {
        'meta_info' : _MetaInfoClass('ChildChildIdentityIdentity',
            False, 
            [
            ],
            'ydktest-sanity',
            'child-child-identity',
            _yang_ns._namespaces['ydktest-sanity'],
        'ydk.models.ydktest_sanity'
        ),
    },
}
_meta_table['Runner.InbtwList.Ldata.Subc.SubcSubl1']['meta_info'].parent =_meta_table['Runner.InbtwList.Ldata.Subc']['meta_info']
_meta_table['Runner.InbtwList.Ldata.Subc']['meta_info'].parent =_meta_table['Runner.InbtwList.Ldata']['meta_info']
_meta_table['Runner.InbtwList.Ldata']['meta_info'].parent =_meta_table['Runner.InbtwList']['meta_info']
_meta_table['Runner.LeafRef.One.Two']['meta_info'].parent =_meta_table['Runner.LeafRef.One']['meta_info']
_meta_table['Runner.LeafRef.One']['meta_info'].parent =_meta_table['Runner.LeafRef']['meta_info']
_meta_table['Runner.NotSupported1.NotSupported12']['meta_info'].parent =_meta_table['Runner.NotSupported1']['meta_info']
_meta_table['Runner.One.OneAug']['meta_info'].parent =_meta_table['Runner.One']['meta_info']
_meta_table['Runner.OneList.OneAugList.Ldata']['meta_info'].parent =_meta_table['Runner.OneList.OneAugList']['meta_info']
_meta_table['Runner.OneList.Ldata']['meta_info'].parent =_meta_table['Runner.OneList']['meta_info']
_meta_table['Runner.OneList.OneAugList']['meta_info'].parent =_meta_table['Runner.OneList']['meta_info']
_meta_table['Runner.Three.Sub1.Sub2']['meta_info'].parent =_meta_table['Runner.Three.Sub1']['meta_info']
_meta_table['Runner.Three.Sub1']['meta_info'].parent =_meta_table['Runner.Three']['meta_info']
_meta_table['Runner.ThreeList.Ldata.Subl1.SubSubl1']['meta_info'].parent =_meta_table['Runner.ThreeList.Ldata.Subl1']['meta_info']
_meta_table['Runner.ThreeList.Ldata.Subl1']['meta_info'].parent =_meta_table['Runner.ThreeList.Ldata']['meta_info']
_meta_table['Runner.ThreeList.Ldata']['meta_info'].parent =_meta_table['Runner.ThreeList']['meta_info']
_meta_table['Runner.Two.Sub1']['meta_info'].parent =_meta_table['Runner.Two']['meta_info']
_meta_table['Runner.TwoList.Ldata.Subl1']['meta_info'].parent =_meta_table['Runner.TwoList.Ldata']['meta_info']
_meta_table['Runner.TwoList.Ldata']['meta_info'].parent =_meta_table['Runner.TwoList']['meta_info']
_meta_table['Runner.Ytypes.BuiltInT']['meta_info'].parent =_meta_table['Runner.Ytypes']['meta_info']
_meta_table['Runner.Ytypes.DerivedT']['meta_info'].parent =_meta_table['Runner.Ytypes']['meta_info']
_meta_table['Runner.InbtwList']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.LeafRef']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.NotSupported1']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.NotSupported2']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.One']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.OneList']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.Runner2']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.Three']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.ThreeList']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.Two']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.TwoList']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['Runner.Ytypes']['meta_info'].parent =_meta_table['Runner']['meta_info']
_meta_table['SubTest.OneAug']['meta_info'].parent =_meta_table['SubTest']['meta_info']
