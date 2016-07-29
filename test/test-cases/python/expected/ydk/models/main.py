""" main 

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

from ydk.types import Empty, YList, YLeafList, DELETE, Decimal64, FixedBitsDict

from ydk.errors import YPYError, YPYModelError




class MainA(object):
    """
    
    
    .. attribute:: main_aug1_c
    
    	
    	**type**\:  :py:class:`MainAug1_C <ydk.models.main.MainA.MainAug1_C>`
    
    .. attribute:: main_aug2_c
    
    	
    	**type**\:  :py:class:`MainAug2_C <ydk.models.main.MainA.MainAug2_C>`
    
    .. attribute:: main_aug2_d
    
    	
    	**type**\:  :py:class:`MainAug2_D <ydk.models.main.MainA.MainAug2_D>`
    
    .. attribute:: main_aug3_c
    
    	
    	**type**\:  :py:class:`MainAug3_C <ydk.models.main.MainA.MainAug3_C>`
    
    .. attribute:: main_aug3_d
    
    	
    	**type**\:  :py:class:`MainAug3_D <ydk.models.main.MainA.MainAug3_D>`
    
    .. attribute:: one
    
    	blah
    	**type**\:  int
    
    	**range:** \-2147483648..2147483647
    
    

    """

    _prefix = 'main'
    _revision = '2015-11-17'

    def __init__(self):
        self.main_aug1_c = MainA.MainAug1_C()
        self.main_aug1_c.parent = self
        self.main_aug2_c = MainA.MainAug2_C()
        self.main_aug2_c.parent = self
        self.main_aug2_d = MainA.MainAug2_D()
        self.main_aug2_d.parent = self
        self.main_aug3_c = MainA.MainAug3_C()
        self.main_aug3_c.parent = self
        self.main_aug3_d = MainA.MainAug3_D()
        self.main_aug3_d.parent = self
        self.one = None


    class MainAug1_C(object):
        """
        
        
        .. attribute:: two
        
        	blah
        	**type**\:  str
        
        

        """

        _prefix = 'aug1'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.two = None

        @property
        def _common_path(self):

            return '/main:main-A/main-aug1:main-aug1_C'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.two is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models._meta import _main as meta
            return meta._meta_table['MainA.MainAug1_C']['meta_info']


    class MainAug2_C(object):
        """
        
        
        .. attribute:: three
        
        	blah
        	**type**\:  int
        
        	**range:** \-32768..32767
        
        

        """

        _prefix = 'aug2'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.three = None

        @property
        def _common_path(self):

            return '/main:main-A/main-aug2:main-aug2_C'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.three is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models._meta import _main as meta
            return meta._meta_table['MainA.MainAug2_C']['meta_info']


    class MainAug2_D(object):
        """
        
        
        .. attribute:: poo
        
        	blah
        	**type**\:  int
        
        	**range:** \-2147483648..2147483647
        
        

        """

        _prefix = 'aug2'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.poo = None

        @property
        def _common_path(self):

            return '/main:main-A/main-aug2:main-aug2_D'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.poo is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models._meta import _main as meta
            return meta._meta_table['MainA.MainAug2_D']['meta_info']


    class MainAug3_C(object):
        """
        
        
        .. attribute:: meh
        
        	blah
        	**type**\:  int
        
        	**range:** \-128..127
        
        

        """

        _prefix = 'aug3'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.meh = None

        @property
        def _common_path(self):

            return '/main:main-A/main-aug3:main-aug3_C'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.meh is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models._meta import _main as meta
            return meta._meta_table['MainA.MainAug3_C']['meta_info']


    class MainAug3_D(object):
        """
        
        
        .. attribute:: buh
        
        	blah
        	**type**\:  str
        
        

        """

        _prefix = 'aug3'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.buh = None

        @property
        def _common_path(self):

            return '/main:main-A/main-aug3:main-aug3_D'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.buh is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models._meta import _main as meta
            return meta._meta_table['MainA.MainAug3_D']['meta_info']

    @property
    def _common_path(self):

        return '/main:main-A'

    def is_config(self):
        ''' Returns True if this instance represents config data else returns False '''
        return True

    def _has_data(self):
        if not self.is_config():
            return False
        if self.main_aug1_c is not None and self.main_aug1_c._has_data():
            return True

        if self.main_aug2_c is not None and self.main_aug2_c._has_data():
            return True

        if self.main_aug2_d is not None and self.main_aug2_d._has_data():
            return True

        if self.main_aug3_c is not None and self.main_aug3_c._has_data():
            return True

        if self.main_aug3_d is not None and self.main_aug3_d._has_data():
            return True

        if self.one is not None:
            return True

        return False

    @staticmethod
    def _meta_info():
        from ydk.models._meta import _main as meta
        return meta._meta_table['MainA']['meta_info']


