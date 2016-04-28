""" ydktest_sanity 

This module contains a collection of YANG definitions
for sanity package.

This module contains definitions
for the following management objects\:
    

Copyright (c) 2013\-2014 by Cisco Systems, Inc.
All rights reserved.

"""


import re
import collections

from enum import Enum

from ydk.types import Empty, YList, DELETE, Decimal64, FixedBitsDict

from ydk.errors import YPYError, YPYDataValidationError



class YdkEnumIntTestEnum(Enum):
    """
    YdkEnumIntTestEnum

    Int or any

    .. data:: ANY = 4096

    	Any value

    """

    ANY = 4096


    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity as meta
        return meta._meta_table['YdkEnumIntTestEnum']


class YdkEnumTestEnum(Enum):
    """
    YdkEnumTestEnum

    YDK Enum test

    .. data:: NOT_SET = 0

    	Not Set

    .. data:: NONE = 1

    	None

    .. data:: LOCAL = 2

    	Local

    .. data:: REMOTE = 3

    	Remote

    """

    NOT_SET = 0

    NONE = 1

    LOCAL = 2

    REMOTE = 3


    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity as meta
        return meta._meta_table['YdkEnumTestEnum']



class BaseIdentity_Identity(object):
    """
    
    
    

    """

    _prefix = 'ydkut'
    _revision = '2015-11-17'

    def __init__(self):
        pass

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity as meta
        return meta._meta_table['BaseIdentity_Identity']['meta_info']


class Runner(object):
    """
    
    
    .. attribute:: inbtw_list
    
    	config for one\_level list data
    	**type**\: :py:class:`InbtwList <ydk.models.ydktest.ydktest_sanity.Runner.InbtwList>`
    
    .. attribute:: leaf_ref
    
    	
    	**type**\: :py:class:`LeafRef <ydk.models.ydktest.ydktest_sanity.Runner.LeafRef>`
    
    .. attribute:: not_supported_1
    
    	
    	**type**\: :py:class:`NotSupported1 <ydk.models.ydktest.ydktest_sanity.Runner.NotSupported1>`
    
    .. attribute:: not_supported_2
    
    	
    	**type**\: list of :py:class:`NotSupported2 <ydk.models.ydktest.ydktest_sanity.Runner.NotSupported2>`
    
    .. attribute:: one
    
    	config for one\_level data
    	**type**\: :py:class:`One <ydk.models.ydktest.ydktest_sanity.Runner.One>`
    
    .. attribute:: one_list
    
    	config for one\_level list data
    	**type**\: :py:class:`OneList <ydk.models.ydktest.ydktest_sanity.Runner.OneList>`
    
    .. attribute:: runner_2
    
    	
    	**type**\: :py:class:`Runner2 <ydk.models.ydktest.ydktest_sanity.Runner.Runner2>`
    
    .. attribute:: three
    
    	config for one\_level data
    	**type**\: :py:class:`Three <ydk.models.ydktest.ydktest_sanity.Runner.Three>`
    
    .. attribute:: three_list
    
    	config for one\_level list data
    	**type**\: :py:class:`ThreeList <ydk.models.ydktest.ydktest_sanity.Runner.ThreeList>`
    
    .. attribute:: two
    
    	config for one\_level data
    	**type**\: :py:class:`Two <ydk.models.ydktest.ydktest_sanity.Runner.Two>`
    
    .. attribute:: two_list
    
    	config for one\_level list data
    	**type**\: :py:class:`TwoList <ydk.models.ydktest.ydktest_sanity.Runner.TwoList>`
    
    .. attribute:: ytypes
    
    	config for one\_level data types
    	**type**\: :py:class:`Ytypes <ydk.models.ydktest.ydktest_sanity.Runner.Ytypes>`
    
    

    """

    _prefix = 'ydkut'
    _revision = '2015-11-17'

    def __init__(self):
        self.inbtw_list = Runner.InbtwList()
        self.inbtw_list.parent = self
        self.leaf_ref = Runner.LeafRef()
        self.leaf_ref.parent = self
        self.not_supported_1 = Runner.NotSupported1()
        self.not_supported_1.parent = self
        self.not_supported_2 = YList()
        self.not_supported_2.parent = self
        self.not_supported_2.name = 'not_supported_2'
        self.one = Runner.One()
        self.one.parent = self
        self.one_list = Runner.OneList()
        self.one_list.parent = self
        self.runner_2 = None
        self.three = Runner.Three()
        self.three.parent = self
        self.three_list = Runner.ThreeList()
        self.three_list.parent = self
        self.two = Runner.Two()
        self.two.parent = self
        self.two_list = Runner.TwoList()
        self.two_list.parent = self
        self.ytypes = Runner.Ytypes()
        self.ytypes.parent = self


    class InbtwList(object):
        """
        config for one\_level list data
        
        .. attribute:: ldata
        
        	one list data
        	**type**\: list of :py:class:`Ldata <ydk.models.ydktest.ydktest_sanity.Runner.InbtwList.Ldata>`
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.ldata = YList()
            self.ldata.parent = self
            self.ldata.name = 'ldata'


        class Ldata(object):
            """
            one list data
            
            .. attribute:: number  <key>
            
            	integer value type
            	**type**\: int
            
            	**range:** \-2147483648..2147483647
            
            .. attribute:: name
            
            	this is string value
            	**type**\: str
            
            .. attribute:: subc
            
            	one list subcontainer data
            	**type**\: :py:class:`Subc <ydk.models.ydktest.ydktest_sanity.Runner.InbtwList.Ldata.Subc>`
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.number = None
                self.name = None
                self.subc = Runner.InbtwList.Ldata.Subc()
                self.subc.parent = self


            class Subc(object):
                """
                one list subcontainer data
                
                .. attribute:: name
                
                	this is string value
                	**type**\: str
                
                .. attribute:: number
                
                	integer value type
                	**type**\: int
                
                	**range:** \-2147483648..2147483647
                
                .. attribute:: subc_subl1
                
                	one list data
                	**type**\: list of :py:class:`SubcSubl1 <ydk.models.ydktest.ydktest_sanity.Runner.InbtwList.Ldata.Subc.SubcSubl1>`
                
                

                """

                _prefix = 'ydkut'
                _revision = '2015-11-17'

                def __init__(self):
                    self.parent = None
                    self.name = None
                    self.number = None
                    self.subc_subl1 = YList()
                    self.subc_subl1.parent = self
                    self.subc_subl1.name = 'subc_subl1'


                class SubcSubl1(object):
                    """
                    one list data
                    
                    .. attribute:: number  <key>
                    
                    	integer value type
                    	**type**\: int
                    
                    	**range:** \-2147483648..2147483647
                    
                    .. attribute:: name
                    
                    	this is string value
                    	**type**\: str
                    
                    

                    """

                    _prefix = 'ydkut'
                    _revision = '2015-11-17'

                    def __init__(self):
                        self.parent = None
                        self.number = None
                        self.name = None

                    @property
                    def _common_path(self):
                        if self.parent is None:
                            raise YPYDataValidationError('parent is not set . Cannot derive path.')
                        if self.number is None:
                            raise YPYDataValidationError('Key property number is None')

                        return self.parent._common_path +'/ydktest-sanity:subc-subl1[ydktest-sanity:number = ' + str(self.number) + ']'

                    def is_config(self):
                        ''' Returns True if this instance represents config data else returns False '''
                        return True

                    def _has_data(self):
                        if not self.is_config():
                            return False
                        if self.number is not None:
                            return True

                        if self.name is not None:
                            return True

                        return False

                    @staticmethod
                    def _meta_info():
                        from ydk.models.ydktest._meta import _ydktest_sanity as meta
                        return meta._meta_table['Runner.InbtwList.Ldata.Subc.SubcSubl1']['meta_info']

                @property
                def _common_path(self):
                    if self.parent is None:
                        raise YPYDataValidationError('parent is not set . Cannot derive path.')

                    return self.parent._common_path +'/ydktest-sanity:subc'

                def is_config(self):
                    ''' Returns True if this instance represents config data else returns False '''
                    return True

                def _has_data(self):
                    if not self.is_config():
                        return False
                    if self.name is not None:
                        return True

                    if self.number is not None:
                        return True

                    if self.subc_subl1 is not None:
                        for child_ref in self.subc_subl1:
                            if child_ref._has_data():
                                return True

                    return False

                @staticmethod
                def _meta_info():
                    from ydk.models.ydktest._meta import _ydktest_sanity as meta
                    return meta._meta_table['Runner.InbtwList.Ldata.Subc']['meta_info']

            @property
            def _common_path(self):
                if self.number is None:
                    raise YPYDataValidationError('Key property number is None')

                return '/ydktest-sanity:runner/ydktest-sanity:inbtw-list/ydktest-sanity:ldata[ydktest-sanity:number = ' + str(self.number) + ']'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.number is not None:
                    return True

                if self.name is not None:
                    return True

                if self.subc is not None and self.subc._has_data():
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.InbtwList.Ldata']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:inbtw-list'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.ldata is not None:
                for child_ref in self.ldata:
                    if child_ref._has_data():
                        return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.InbtwList']['meta_info']


    class LeafRef(object):
        """
        
        
        .. attribute:: one
        
        	
        	**type**\: :py:class:`One <ydk.models.ydktest.ydktest_sanity.Runner.LeafRef.One>`
        
        .. attribute:: ref_inbtw
        
        	
        	**type**\: str
        
        .. attribute:: ref_one_name
        
        	
        	**type**\: str
        
        .. attribute:: ref_three_sub1_sub2_number
        
        	
        	**type**\: int
        
        	**range:** \-2147483648..2147483647
        
        .. attribute:: ref_two_sub1_number
        
        	
        	**type**\: int
        
        	**range:** \-2147483648..2147483647
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.one = Runner.LeafRef.One()
            self.one.parent = self
            self.ref_inbtw = None
            self.ref_one_name = None
            self.ref_three_sub1_sub2_number = None
            self.ref_two_sub1_number = None


        class One(object):
            """
            
            
            .. attribute:: name_of_one
            
            	
            	**type**\: str
            
            	**pattern:** (([0\\\-9]\\\|[1\\\-9][0\\\-9]\\\|1[0\\\-9][0\\\-9]\\\|2[0\\\-4][0\\\-9]\\\|25[0\\\-5])\\.){3}([0\\\-9]\\\|[1\\\-9][0\\\-9]\\\|1[0\\\-9][0\\\-9]\\\|2[0\\\-4][0\\\-9]\\\|25[0\\\-5])(%[\\p{N}\\p{L}]+)?
            
            .. attribute:: two
            
            	
            	**type**\: :py:class:`Two <ydk.models.ydktest.ydktest_sanity.Runner.LeafRef.One.Two>`
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.name_of_one = None
                self.two = Runner.LeafRef.One.Two()
                self.two.parent = self


            class Two(object):
                """
                
                
                .. attribute:: self_ref_one_name
                
                	
                	**type**\: str
                
                

                """

                _prefix = 'ydkut'
                _revision = '2015-11-17'

                def __init__(self):
                    self.parent = None
                    self.self_ref_one_name = None

                @property
                def _common_path(self):

                    return '/ydktest-sanity:runner/ydktest-sanity:leaf-ref/ydktest-sanity:one/ydktest-sanity:two'

                def is_config(self):
                    ''' Returns True if this instance represents config data else returns False '''
                    return True

                def _has_data(self):
                    if not self.is_config():
                        return False
                    if self.self_ref_one_name is not None:
                        return True

                    return False

                @staticmethod
                def _meta_info():
                    from ydk.models.ydktest._meta import _ydktest_sanity as meta
                    return meta._meta_table['Runner.LeafRef.One.Two']['meta_info']

            @property
            def _common_path(self):

                return '/ydktest-sanity:runner/ydktest-sanity:leaf-ref/ydktest-sanity:one'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.name_of_one is not None:
                    return True

                if self.two is not None and self.two._has_data():
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.LeafRef.One']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:leaf-ref'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.one is not None and self.one._has_data():
                return True

            if self.ref_inbtw is not None:
                return True

            if self.ref_one_name is not None:
                return True

            if self.ref_three_sub1_sub2_number is not None:
                return True

            if self.ref_two_sub1_number is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.LeafRef']['meta_info']


    class NotSupported1(object):
        """
        
        
        .. attribute:: not_supported_1_2
        
        	
        	**type**\: :py:class:`NotSupported12 <ydk.models.ydktest.ydktest_sanity.Runner.NotSupported1.NotSupported12>`
        
        .. attribute:: not_supported_leaf
        
        	
        	**type**\: str
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.not_supported_1_2 = Runner.NotSupported1.NotSupported12()
            self.not_supported_1_2.parent = self
            self.not_supported_leaf = None


        class NotSupported12(object):
            """
            
            
            .. attribute:: some_leaf
            
            	
            	**type**\: str
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.some_leaf = None

            @property
            def _common_path(self):

                return '/ydktest-sanity:runner/ydktest-sanity:not-supported-1/ydktest-sanity:not-supported-1-2'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.some_leaf is not None:
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.NotSupported1.NotSupported12']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:not-supported-1'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.not_supported_1_2 is not None and self.not_supported_1_2._has_data():
                return True

            if self.not_supported_leaf is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.NotSupported1']['meta_info']


    class NotSupported2(object):
        """
        
        
        .. attribute:: number  <key>
        
        	Integer key for not supported list
        	**type**\: int
        
        	**range:** \-2147483648..2147483647
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.number = None

        @property
        def _common_path(self):
            if self.number is None:
                raise YPYDataValidationError('Key property number is None')

            return '/ydktest-sanity:runner/ydktest-sanity:not-supported-2[ydktest-sanity:number = ' + str(self.number) + ']'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.number is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.NotSupported2']['meta_info']


    class One(object):
        """
        config for one\_level data
        
        .. attribute:: name
        
        	this is string value
        	**type**\: str
        
        .. attribute:: number
        
        	integer value type
        	**type**\: int
        
        	**range:** \-2147483648..2147483647
        
        .. attribute:: one_aug
        
        	config for one\_level data
        	**type**\: :py:class:`OneAug <ydk.models.ydktest.ydktest_sanity.Runner.One.OneAug>`
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.name = None
            self.number = None
            self.one_aug = Runner.One.OneAug()
            self.one_aug.parent = self


        class OneAug(object):
            """
            config for one\_level data
            
            .. attribute:: name
            
            	this is string value
            	**type**\: str
            
            .. attribute:: number
            
            	integer value type
            	**type**\: int
            
            	**range:** \-2147483648..2147483647
            
            

            """

            _prefix = 'ysanity-augm'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.name = None
                self.number = None

            @property
            def _common_path(self):

                return '/ydktest-sanity:runner/ydktest-sanity:one/ydktest-sanity-augm:one-aug'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.name is not None:
                    return True

                if self.number is not None:
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.One.OneAug']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:one'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.name is not None:
                return True

            if self.number is not None:
                return True

            if self.one_aug is not None and self.one_aug._has_data():
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.One']['meta_info']


    class OneList(object):
        """
        config for one\_level list data
        
        .. attribute:: ldata
        
        	one list data
        	**type**\: list of :py:class:`Ldata <ydk.models.ydktest.ydktest_sanity.Runner.OneList.Ldata>`
        
        .. attribute:: one_aug_list
        
        	config for one\_level list data
        	**type**\: :py:class:`OneAugList <ydk.models.ydktest.ydktest_sanity.Runner.OneList.OneAugList>`
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.ldata = YList()
            self.ldata.parent = self
            self.ldata.name = 'ldata'
            self.one_aug_list = Runner.OneList.OneAugList()
            self.one_aug_list.parent = self


        class Ldata(object):
            """
            one list data
            
            .. attribute:: number  <key>
            
            	integer value type
            	**type**\: int
            
            	**range:** \-2147483648..2147483647
            
            .. attribute:: name
            
            	this is string value
            	**type**\: str
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.number = None
                self.name = None

            @property
            def _common_path(self):
                if self.number is None:
                    raise YPYDataValidationError('Key property number is None')

                return '/ydktest-sanity:runner/ydktest-sanity:one-list/ydktest-sanity:ldata[ydktest-sanity:number = ' + str(self.number) + ']'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.number is not None:
                    return True

                if self.name is not None:
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.OneList.Ldata']['meta_info']


        class OneAugList(object):
            """
            config for one\_level list data
            
            .. attribute:: enabled
            
            	integer value type
            	**type**\: bool
            
            .. attribute:: ldata
            
            	one list data
            	**type**\: list of :py:class:`Ldata <ydk.models.ydktest.ydktest_sanity.Runner.OneList.OneAugList.Ldata>`
            
            

            """

            _prefix = 'ysanity-augm'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.enabled = None
                self.ldata = YList()
                self.ldata.parent = self
                self.ldata.name = 'ldata'


            class Ldata(object):
                """
                one list data
                
                .. attribute:: number  <key>
                
                	integer value type
                	**type**\: int
                
                	**range:** \-2147483648..2147483647
                
                .. attribute:: name
                
                	this is string value
                	**type**\: str
                
                

                """

                _prefix = 'ysanity-augm'
                _revision = '2015-11-17'

                def __init__(self):
                    self.parent = None
                    self.number = None
                    self.name = None

                @property
                def _common_path(self):
                    if self.number is None:
                        raise YPYDataValidationError('Key property number is None')

                    return '/ydktest-sanity:runner/ydktest-sanity:one-list/ydktest-sanity-augm:one-aug-list/ydktest-sanity-augm:ldata[ydktest-sanity-augm:number = ' + str(self.number) + ']'

                def is_config(self):
                    ''' Returns True if this instance represents config data else returns False '''
                    return True

                def _has_data(self):
                    if not self.is_config():
                        return False
                    if self.number is not None:
                        return True

                    if self.name is not None:
                        return True

                    return False

                @staticmethod
                def _meta_info():
                    from ydk.models.ydktest._meta import _ydktest_sanity as meta
                    return meta._meta_table['Runner.OneList.OneAugList.Ldata']['meta_info']

            @property
            def _common_path(self):

                return '/ydktest-sanity:runner/ydktest-sanity:one-list/ydktest-sanity-augm:one-aug-list'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.enabled is not None:
                    return True

                if self.ldata is not None:
                    for child_ref in self.ldata:
                        if child_ref._has_data():
                            return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.OneList.OneAugList']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:one-list'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.ldata is not None:
                for child_ref in self.ldata:
                    if child_ref._has_data():
                        return True

            if self.one_aug_list is not None and self.one_aug_list._has_data():
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.OneList']['meta_info']


    class Runner2(object):
        """
        
        
        .. attribute:: some_leaf
        
        	
        	**type**\: str
        
        .. attribute:: _is_presence
        
        	Is present if this instance represents presence container else not
        	**type**\: bool
        
        

        This class is a :ref:`presence class<presence-class>`

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.some_leaf = None

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:runner-2'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.some_leaf is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.Runner2']['meta_info']


    class Three(object):
        """
        config for one\_level data
        
        .. attribute:: name
        
        	this is string value
        	**type**\: str
        
        .. attribute:: number
        
        	integer value type
        	**type**\: int
        
        	**range:** \-2147483648..2147483647
        
        .. attribute:: sub1
        
        	subconfig1 for config container
        	**type**\: :py:class:`Sub1 <ydk.models.ydktest.ydktest_sanity.Runner.Three.Sub1>`
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.name = None
            self.number = None
            self.sub1 = Runner.Three.Sub1()
            self.sub1.parent = self


        class Sub1(object):
            """
            subconfig1 for config container
            
            .. attribute:: number
            
            	integer value type
            	**type**\: int
            
            	**range:** \-2147483648..2147483647
            
            .. attribute:: sub2
            
            	subconfig2 for config container
            	**type**\: :py:class:`Sub2 <ydk.models.ydktest.ydktest_sanity.Runner.Three.Sub1.Sub2>`
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.number = None
                self.sub2 = Runner.Three.Sub1.Sub2()
                self.sub2.parent = self


            class Sub2(object):
                """
                subconfig2 for config container
                
                .. attribute:: number
                
                	integer value type
                	**type**\: int
                
                	**range:** \-2147483648..2147483647
                
                

                """

                _prefix = 'ydkut'
                _revision = '2015-11-17'

                def __init__(self):
                    self.parent = None
                    self.number = None

                @property
                def _common_path(self):

                    return '/ydktest-sanity:runner/ydktest-sanity:three/ydktest-sanity:sub1/ydktest-sanity:sub2'

                def is_config(self):
                    ''' Returns True if this instance represents config data else returns False '''
                    return True

                def _has_data(self):
                    if not self.is_config():
                        return False
                    if self.number is not None:
                        return True

                    return False

                @staticmethod
                def _meta_info():
                    from ydk.models.ydktest._meta import _ydktest_sanity as meta
                    return meta._meta_table['Runner.Three.Sub1.Sub2']['meta_info']

            @property
            def _common_path(self):

                return '/ydktest-sanity:runner/ydktest-sanity:three/ydktest-sanity:sub1'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.number is not None:
                    return True

                if self.sub2 is not None and self.sub2._has_data():
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.Three.Sub1']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:three'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.name is not None:
                return True

            if self.number is not None:
                return True

            if self.sub1 is not None and self.sub1._has_data():
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.Three']['meta_info']


    class ThreeList(object):
        """
        config for one\_level list data
        
        .. attribute:: ldata
        
        	one list data
        	**type**\: list of :py:class:`Ldata <ydk.models.ydktest.ydktest_sanity.Runner.ThreeList.Ldata>`
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.ldata = YList()
            self.ldata.parent = self
            self.ldata.name = 'ldata'


        class Ldata(object):
            """
            one list data
            
            .. attribute:: number  <key>
            
            	integer value type
            	**type**\: int
            
            	**range:** \-2147483648..2147483647
            
            .. attribute:: name
            
            	this is string value
            	**type**\: str
            
            .. attribute:: subl1
            
            	one list data
            	**type**\: list of :py:class:`Subl1 <ydk.models.ydktest.ydktest_sanity.Runner.ThreeList.Ldata.Subl1>`
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.number = None
                self.name = None
                self.subl1 = YList()
                self.subl1.parent = self
                self.subl1.name = 'subl1'


            class Subl1(object):
                """
                one list data
                
                .. attribute:: number  <key>
                
                	integer value type
                	**type**\: int
                
                	**range:** \-2147483648..2147483647
                
                .. attribute:: name
                
                	this is string value
                	**type**\: str
                
                .. attribute:: sub_subl1
                
                	one list data
                	**type**\: list of :py:class:`SubSubl1 <ydk.models.ydktest.ydktest_sanity.Runner.ThreeList.Ldata.Subl1.SubSubl1>`
                
                

                """

                _prefix = 'ydkut'
                _revision = '2015-11-17'

                def __init__(self):
                    self.parent = None
                    self.number = None
                    self.name = None
                    self.sub_subl1 = YList()
                    self.sub_subl1.parent = self
                    self.sub_subl1.name = 'sub_subl1'


                class SubSubl1(object):
                    """
                    one list data
                    
                    .. attribute:: number  <key>
                    
                    	integer value type
                    	**type**\: int
                    
                    	**range:** \-2147483648..2147483647
                    
                    .. attribute:: name
                    
                    	this is string value
                    	**type**\: str
                    
                    

                    """

                    _prefix = 'ydkut'
                    _revision = '2015-11-17'

                    def __init__(self):
                        self.parent = None
                        self.number = None
                        self.name = None

                    @property
                    def _common_path(self):
                        if self.parent is None:
                            raise YPYDataValidationError('parent is not set . Cannot derive path.')
                        if self.number is None:
                            raise YPYDataValidationError('Key property number is None')

                        return self.parent._common_path +'/ydktest-sanity:sub-subl1[ydktest-sanity:number = ' + str(self.number) + ']'

                    def is_config(self):
                        ''' Returns True if this instance represents config data else returns False '''
                        return True

                    def _has_data(self):
                        if not self.is_config():
                            return False
                        if self.number is not None:
                            return True

                        if self.name is not None:
                            return True

                        return False

                    @staticmethod
                    def _meta_info():
                        from ydk.models.ydktest._meta import _ydktest_sanity as meta
                        return meta._meta_table['Runner.ThreeList.Ldata.Subl1.SubSubl1']['meta_info']

                @property
                def _common_path(self):
                    if self.parent is None:
                        raise YPYDataValidationError('parent is not set . Cannot derive path.')
                    if self.number is None:
                        raise YPYDataValidationError('Key property number is None')

                    return self.parent._common_path +'/ydktest-sanity:subl1[ydktest-sanity:number = ' + str(self.number) + ']'

                def is_config(self):
                    ''' Returns True if this instance represents config data else returns False '''
                    return True

                def _has_data(self):
                    if not self.is_config():
                        return False
                    if self.number is not None:
                        return True

                    if self.name is not None:
                        return True

                    if self.sub_subl1 is not None:
                        for child_ref in self.sub_subl1:
                            if child_ref._has_data():
                                return True

                    return False

                @staticmethod
                def _meta_info():
                    from ydk.models.ydktest._meta import _ydktest_sanity as meta
                    return meta._meta_table['Runner.ThreeList.Ldata.Subl1']['meta_info']

            @property
            def _common_path(self):
                if self.number is None:
                    raise YPYDataValidationError('Key property number is None')

                return '/ydktest-sanity:runner/ydktest-sanity:three-list/ydktest-sanity:ldata[ydktest-sanity:number = ' + str(self.number) + ']'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.number is not None:
                    return True

                if self.name is not None:
                    return True

                if self.subl1 is not None:
                    for child_ref in self.subl1:
                        if child_ref._has_data():
                            return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.ThreeList.Ldata']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:three-list'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.ldata is not None:
                for child_ref in self.ldata:
                    if child_ref._has_data():
                        return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.ThreeList']['meta_info']


    class Two(object):
        """
        config for one\_level data
        
        .. attribute:: name
        
        	this is string value
        	**type**\: str
        
        .. attribute:: number
        
        	integer value type
        	**type**\: int
        
        	**range:** \-2147483648..2147483647
        
        .. attribute:: sub1
        
        	subconfig1 for config container
        	**type**\: :py:class:`Sub1 <ydk.models.ydktest.ydktest_sanity.Runner.Two.Sub1>`
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.name = None
            self.number = None
            self.sub1 = Runner.Two.Sub1()
            self.sub1.parent = self


        class Sub1(object):
            """
            subconfig1 for config container
            
            .. attribute:: number
            
            	integer value type
            	**type**\: int
            
            	**range:** \-2147483648..2147483647
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.number = None

            @property
            def _common_path(self):

                return '/ydktest-sanity:runner/ydktest-sanity:two/ydktest-sanity:sub1'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.number is not None:
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.Two.Sub1']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:two'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.name is not None:
                return True

            if self.number is not None:
                return True

            if self.sub1 is not None and self.sub1._has_data():
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.Two']['meta_info']


    class TwoList(object):
        """
        config for one\_level list data
        
        .. attribute:: ldata
        
        	one list data
        	**type**\: list of :py:class:`Ldata <ydk.models.ydktest.ydktest_sanity.Runner.TwoList.Ldata>`
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.ldata = YList()
            self.ldata.parent = self
            self.ldata.name = 'ldata'


        class Ldata(object):
            """
            one list data
            
            .. attribute:: number  <key>
            
            	integer value type
            	**type**\: int
            
            	**range:** \-2147483648..2147483647
            
            .. attribute:: name
            
            	this is string value
            	**type**\: str
            
            .. attribute:: subl1
            
            	one list data
            	**type**\: list of :py:class:`Subl1 <ydk.models.ydktest.ydktest_sanity.Runner.TwoList.Ldata.Subl1>`
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.number = None
                self.name = None
                self.subl1 = YList()
                self.subl1.parent = self
                self.subl1.name = 'subl1'


            class Subl1(object):
                """
                one list data
                
                .. attribute:: number  <key>
                
                	integer value type
                	**type**\: int
                
                	**range:** \-2147483648..2147483647
                
                .. attribute:: name
                
                	this is string value
                	**type**\: str
                
                

                """

                _prefix = 'ydkut'
                _revision = '2015-11-17'

                def __init__(self):
                    self.parent = None
                    self.number = None
                    self.name = None

                @property
                def _common_path(self):
                    if self.parent is None:
                        raise YPYDataValidationError('parent is not set . Cannot derive path.')
                    if self.number is None:
                        raise YPYDataValidationError('Key property number is None')

                    return self.parent._common_path +'/ydktest-sanity:subl1[ydktest-sanity:number = ' + str(self.number) + ']'

                def is_config(self):
                    ''' Returns True if this instance represents config data else returns False '''
                    return True

                def _has_data(self):
                    if not self.is_config():
                        return False
                    if self.number is not None:
                        return True

                    if self.name is not None:
                        return True

                    return False

                @staticmethod
                def _meta_info():
                    from ydk.models.ydktest._meta import _ydktest_sanity as meta
                    return meta._meta_table['Runner.TwoList.Ldata.Subl1']['meta_info']

            @property
            def _common_path(self):
                if self.number is None:
                    raise YPYDataValidationError('Key property number is None')

                return '/ydktest-sanity:runner/ydktest-sanity:two-list/ydktest-sanity:ldata[ydktest-sanity:number = ' + str(self.number) + ']'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.number is not None:
                    return True

                if self.name is not None:
                    return True

                if self.subl1 is not None:
                    for child_ref in self.subl1:
                        if child_ref._has_data():
                            return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.TwoList.Ldata']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:two-list'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.ldata is not None:
                for child_ref in self.ldata:
                    if child_ref._has_data():
                        return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.TwoList']['meta_info']


    class Ytypes(object):
        """
        config for one\_level data types
        
        .. attribute:: built_in_t
        
        	config for built\-in types
        	**type**\: :py:class:`BuiltInT <ydk.models.ydktest.ydktest_sanity.Runner.Ytypes.BuiltInT>`
        
        .. attribute:: derived_t
        
        	config for one\_level derived data types
        	**type**\: :py:class:`DerivedT <ydk.models.ydktest.ydktest_sanity.Runner.Ytypes.DerivedT>`
        
        

        """

        _prefix = 'ydkut'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.built_in_t = Runner.Ytypes.BuiltInT()
            self.built_in_t.parent = self
            self.derived_t = Runner.Ytypes.DerivedT()
            self.derived_t.parent = self


        class BuiltInT(object):
            """
            config for built\-in types
            
            .. attribute:: bincoded
            
            	this is binary value
            	**type**\: str
            
            .. attribute:: bits_value
            
            	this is bits type value
            	**type**\: :py:class:`BitsValue_Bits <ydk.models.ydktest.ydktest_sanity.Runner.Ytypes.BuiltInT.BitsValue_Bits>`
            
            .. attribute:: bool_value
            
            	this is boolean type value
            	**type**\: bool
            
            .. attribute:: deci64
            
            	this is decimal value
            	**type**\: Decimal64
            
            	**range:** 1..3.14 \| 10..None \| 20..92233720368547758.07
            
            .. attribute:: embeded_enum
            
            	enum embeded in leaf
            	**type**\: :py:class:`EmbededEnumEnum <ydk.models.ydktest.ydktest_sanity.Runner.Ytypes.BuiltInT.EmbededEnumEnum>`
            
            .. attribute:: emptee
            
            	this is empty value
            	**type**\: :py:class:`Empty <ydk.types.Empty>`
            
            .. attribute:: enum_int_value
            
            	enum int type
            	**type**\: one of { :py:class:`YdkEnumIntTestEnum <ydk.models.ydktest.ydktest_sanity.YdkEnumIntTestEnum>` | int }
            
            .. attribute:: enum_value
            
            	this is enum type value
            	**type**\: :py:class:`YdkEnumTestEnum <ydk.models.ydktest.ydktest_sanity.YdkEnumTestEnum>`
            
            .. attribute:: identity_ref_value
            
            	
            	**type**\: :py:class:`BaseIdentity_Identity <ydk.models.ydktest.ydktest_sanity.BaseIdentity_Identity>`
            
            .. attribute:: leaf_ref
            
            	leaf\-ref
            	**type**\: int
            
            	**range:** \-128..127
            
            .. attribute:: llstring
            
            	A list of string
            	**type**\: list of str
            
            .. attribute:: name
            
            	this is string value
            	**type**\: str
            
            .. attribute:: number16
            
            	16 bit integer value type
            	**type**\: int
            
            	**range:** \-32768..32767
            
            .. attribute:: number32
            
            	integer value type
            	**type**\: int
            
            	**range:** \-2147483648..2147483647
            
            .. attribute:: number64
            
            	integer value type
            	**type**\: int
            
            	**range:** \-9223372036854775808..9223372036854775807
            
            .. attribute:: number8
            
            	 8 bit integer value type
            	**type**\: int
            
            	**range:** \-128..127
            
            .. attribute:: u_number16
            
            	16 bit uinteger value type
            	**type**\: int
            
            	**range:** 0..65535
            
            .. attribute:: u_number32
            
            	32 bit uinteger value type
            	**type**\: int
            
            	**range:** 0..4294967295
            
            .. attribute:: u_number64
            
            	64 bit uinteger value type
            	**type**\: int
            
            	**range:** 0..18446744073709551615
            
            .. attribute:: u_number8
            
            	 8 bit uinteger value type
            	**type**\: int
            
            	**range:** 0..255
            
            .. attribute:: younion
            
            	union test value
            	**type**\: one of { :py:class:`YdkEnumTestEnum <ydk.models.ydktest.ydktest_sanity.YdkEnumTestEnum>` | int }
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.bincoded = None
                self.bits_value = Runner.Ytypes.BuiltInT.BitsValue_Bits()
                self.bool_value = None
                self.deci64 = None
                self.embeded_enum = None
                self.emptee = None
                self.enum_int_value = None
                self.enum_value = None
                self.identity_ref_value = None
                self.leaf_ref = None
                self.llstring = []
                self.name = None
                self.number16 = None
                self.number32 = None
                self.number64 = None
                self.number8 = None
                self.u_number16 = None
                self.u_number32 = None
                self.u_number64 = None
                self.u_number8 = None
                self.younion = None

            class EmbededEnumEnum(Enum):
                """
                EmbededEnumEnum

                enum embeded in leaf

                .. data:: ZERO = 0

                .. data:: TWO = 1

                .. data:: SEVEN = 7

                """

                ZERO = 0

                TWO = 1

                SEVEN = 7


                @staticmethod
                def _meta_info():
                    from ydk.models.ydktest._meta import _ydktest_sanity as meta
                    return meta._meta_table['Runner.Ytypes.BuiltInT.EmbededEnumEnum']


            class BitsValue_Bits(FixedBitsDict):
                """
                BitsValue_Bits

                this is bits type value
                Keys are:- disable\-nagle , auto\-sense\-speed

                """

                def __init__(self):
                    self._dictionary = { 
                        'disable-nagle':False,
                        'auto-sense-speed':False,
                    }
                    self._pos_map = { 
                        'disable-nagle':0,
                        'auto-sense-speed':1,
                    }

            @property
            def _common_path(self):

                return '/ydktest-sanity:runner/ydktest-sanity:ytypes/ydktest-sanity:built-in-t'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.bincoded is not None:
                    return True

                if self.bits_value is not None:
                    if self.bits_value._has_data():
                        return True

                if self.bool_value is not None:
                    return True

                if self.deci64 is not None:
                    return True

                if self.embeded_enum is not None:
                    return True

                if self.emptee is not None:
                    return True

                if self.enum_int_value is not None:
                    return True

                if self.enum_value is not None:
                    return True

                if self.identity_ref_value is not None:
                    return True

                if self.leaf_ref is not None:
                    return True

                if self.llstring is not None:
                    for child in self.llstring:
                        if child is not None:
                            return True

                if self.name is not None:
                    return True

                if self.number16 is not None:
                    return True

                if self.number32 is not None:
                    return True

                if self.number64 is not None:
                    return True

                if self.number8 is not None:
                    return True

                if self.u_number16 is not None:
                    return True

                if self.u_number32 is not None:
                    return True

                if self.u_number64 is not None:
                    return True

                if self.u_number8 is not None:
                    return True

                if self.younion is not None:
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.Ytypes.BuiltInT']['meta_info']


        class DerivedT(object):
            """
            config for one\_level derived data types
            
            

            """

            _prefix = 'ydkut'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None

            @property
            def _common_path(self):

                return '/ydktest-sanity:runner/ydktest-sanity:ytypes/ydktest-sanity:derived-t'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                return False

            @staticmethod
            def _meta_info():
                from ydk.models.ydktest._meta import _ydktest_sanity as meta
                return meta._meta_table['Runner.Ytypes.DerivedT']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-sanity:runner/ydktest-sanity:ytypes'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.built_in_t is not None and self.built_in_t._has_data():
                return True

            if self.derived_t is not None and self.derived_t._has_data():
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['Runner.Ytypes']['meta_info']

    @property
    def _common_path(self):

        return '/ydktest-sanity:runner'

    def is_config(self):
        ''' Returns True if this instance represents config data else returns False '''
        return True

    def _has_data(self):
        if not self.is_config():
            return False
        if self.inbtw_list is not None and self.inbtw_list._has_data():
            return True

        if self.leaf_ref is not None and self.leaf_ref._has_data():
            return True

        if self.not_supported_1 is not None and self.not_supported_1._has_data():
            return True

        if self.not_supported_2 is not None:
            for child_ref in self.not_supported_2:
                if child_ref._has_data():
                    return True

        if self.one is not None and self.one._has_data():
            return True

        if self.one_list is not None and self.one_list._has_data():
            return True

        if self.runner_2 is not None and self.runner_2._has_data():
            return True

        if self.three is not None and self.three._has_data():
            return True

        if self.three_list is not None and self.three_list._has_data():
            return True

        if self.two is not None and self.two._has_data():
            return True

        if self.two_list is not None and self.two_list._has_data():
            return True

        if self.ytypes is not None and self.ytypes._has_data():
            return True

        return False

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity as meta
        return meta._meta_table['Runner']['meta_info']


class SubTest(object):
    """
    
    
    .. attribute:: one_aug
    
    	config for one\_level data
    	**type**\: :py:class:`OneAug <ydk.models.ydktest.ydktest_sanity.SubTest.OneAug>`
    
    

    """

    _prefix = 'ydkut'
    _revision = '2016-04-25'

    def __init__(self):
        self.one_aug = SubTest.OneAug()
        self.one_aug.parent = self


    class OneAug(object):
        """
        config for one\_level data
        
        .. attribute:: name
        
        	this is string value
        	**type**\: str
        
        .. attribute:: number
        
        	integer value type
        	**type**\: int
        
        	**range:** \-2147483648..2147483647
        
        

        """

        _prefix = 'ydkut'
        _revision = '2016-04-25'

        def __init__(self):
            self.parent = None
            self.name = None
            self.number = None

        @property
        def _common_path(self):

            return '/ydktest-sanity-submodule:sub-test/ydktest-sanity-submodule:one-aug'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.name is not None:
                return True

            if self.number is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models.ydktest._meta import _ydktest_sanity as meta
            return meta._meta_table['SubTest.OneAug']['meta_info']

    @property
    def _common_path(self):

        return '/ydktest-sanity-submodule:sub-test'

    def is_config(self):
        ''' Returns True if this instance represents config data else returns False '''
        return True

    def _has_data(self):
        if not self.is_config():
            return False
        if self.one_aug is not None and self.one_aug._has_data():
            return True

        return False

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity as meta
        return meta._meta_table['SubTest']['meta_info']


class ChildIdentity_Identity(BaseIdentity_Identity):
    """
    
    
    

    """

    _prefix = 'ydkut'
    _revision = '2015-11-17'

    def __init__(self):
        BaseIdentity_Identity.__init__(self)

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity as meta
        return meta._meta_table['ChildIdentity_Identity']['meta_info']


class ChildChildIdentity_Identity(ChildIdentity_Identity):
    """
    
    
    

    """

    _prefix = 'ydkut'
    _revision = '2015-11-17'

    def __init__(self):
        ChildIdentity_Identity.__init__(self)

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity as meta
        return meta._meta_table['ChildChildIdentity_Identity']['meta_info']


