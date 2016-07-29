""" ydktest_filterread 

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




class A(object):
    """
    
    
    .. attribute:: a1
    
    	
    	**type**\:  str
    
    .. attribute:: a2
    
    	
    	**type**\:  str
    
    .. attribute:: a3
    
    	
    	**type**\:  str
    
    .. attribute:: b
    
    	
    	**type**\:  :py:class:`B <ydk.models.ydktest_filterread.A.B>`
    
    .. attribute:: lst
    
    	
    	**type**\: list of  :py:class:`Lst <ydk.models.ydktest_filterread.A.Lst>`
    
    

    """

    _prefix = 'ydkflt'
    _revision = '2015-11-17'

    def __init__(self):
        self.a1 = None
        self.a2 = None
        self.a3 = None
        self.b = A.B()
        self.b.parent = self
        self.lst = YList()
        self.lst.parent = self
        self.lst.name = 'lst'


    class B(object):
        """
        
        
        .. attribute:: b1
        
        	
        	**type**\:  str
        
        .. attribute:: b2
        
        	
        	**type**\:  str
        
        .. attribute:: b3
        
        	
        	**type**\:  str
        
        .. attribute:: c
        
        	
        	**type**\:  :py:class:`C <ydk.models.ydktest_filterread.A.B.C>`
        
        .. attribute:: d
        
        	
        	**type**\:  :py:class:`D <ydk.models.ydktest_filterread.A.B.D>`
        
        .. attribute:: f
        
        	
        	**type**\:  :py:class:`F <ydk.models.ydktest_filterread.A.B.F>`
        
        

        """

        _prefix = 'ydkflt'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.b1 = None
            self.b2 = None
            self.b3 = None
            self.c = None
            self.d = A.B.D()
            self.d.parent = self
            self.f = None


        class C(object):
            """
            
            
            .. attribute:: _is_presence
            
            	Is present if this instance represents presence container else not
            	**type**\: bool
            
            

            This class is a :ref:`presence class<presence-class>`

            """

            _prefix = 'ydkflt'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self._is_presence = True

            @property
            def _common_path(self):

                return '/ydktest-filterread:a/ydktest-filterread:b/ydktest-filterread:c'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self._is_presence:
                    return True
                return False

            @staticmethod
            def _meta_info():
                from ydk.models._meta import _ydktest_filterread as meta
                return meta._meta_table['A.B.C']['meta_info']


        class D(object):
            """
            
            
            .. attribute:: d1
            
            	
            	**type**\:  str
            
            .. attribute:: d2
            
            	
            	**type**\:  str
            
            .. attribute:: d3
            
            	
            	**type**\:  str
            
            .. attribute:: e
            
            	
            	**type**\:  :py:class:`E <ydk.models.ydktest_filterread.A.B.D.E>`
            
            

            """

            _prefix = 'ydkflt'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self.d1 = None
                self.d2 = None
                self.d3 = None
                self.e = A.B.D.E()
                self.e.parent = self


            class E(object):
                """
                
                
                .. attribute:: e1
                
                	
                	**type**\:  str
                
                .. attribute:: e2
                
                	
                	**type**\:  str
                
                

                """

                _prefix = 'ydkflt'
                _revision = '2015-11-17'

                def __init__(self):
                    self.parent = None
                    self.e1 = None
                    self.e2 = None

                @property
                def _common_path(self):

                    return '/ydktest-filterread:a/ydktest-filterread:b/ydktest-filterread:d/ydktest-filterread:e'

                def is_config(self):
                    ''' Returns True if this instance represents config data else returns False '''
                    return True

                def _has_data(self):
                    if not self.is_config():
                        return False
                    if self.e1 is not None:
                        return True

                    if self.e2 is not None:
                        return True

                    return False

                @staticmethod
                def _meta_info():
                    from ydk.models._meta import _ydktest_filterread as meta
                    return meta._meta_table['A.B.D.E']['meta_info']

            @property
            def _common_path(self):

                return '/ydktest-filterread:a/ydktest-filterread:b/ydktest-filterread:d'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self.d1 is not None:
                    return True

                if self.d2 is not None:
                    return True

                if self.d3 is not None:
                    return True

                if self.e is not None and self.e._has_data():
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models._meta import _ydktest_filterread as meta
                return meta._meta_table['A.B.D']['meta_info']


        class F(object):
            """
            
            
            .. attribute:: f1
            
            	
            	**type**\:  str
            
            .. attribute:: _is_presence
            
            	Is present if this instance represents presence container else not
            	**type**\: bool
            
            

            This class is a :ref:`presence class<presence-class>`

            """

            _prefix = 'ydkflt'
            _revision = '2015-11-17'

            def __init__(self):
                self.parent = None
                self._is_presence = True
                self.f1 = None

            @property
            def _common_path(self):

                return '/ydktest-filterread:a/ydktest-filterread:b/ydktest-filterread:f'

            def is_config(self):
                ''' Returns True if this instance represents config data else returns False '''
                return True

            def _has_data(self):
                if not self.is_config():
                    return False
                if self._is_presence:
                    return True
                if self.f1 is not None:
                    return True

                return False

            @staticmethod
            def _meta_info():
                from ydk.models._meta import _ydktest_filterread as meta
                return meta._meta_table['A.B.F']['meta_info']

        @property
        def _common_path(self):

            return '/ydktest-filterread:a/ydktest-filterread:b'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.b1 is not None:
                return True

            if self.b2 is not None:
                return True

            if self.b3 is not None:
                return True

            if self.c is not None and self.c._has_data():
                return True

            if self.d is not None and self.d._has_data():
                return True

            if self.f is not None and self.f._has_data():
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models._meta import _ydktest_filterread as meta
            return meta._meta_table['A.B']['meta_info']


    class Lst(object):
        """
        
        
        .. attribute:: number  <key>
        
        	
        	**type**\:  int
        
        	**range:** \-2147483648..2147483647
        
        .. attribute:: value
        
        	
        	**type**\:  str
        
        

        """

        _prefix = 'ydkflt'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.number = None
            self.value = None

        @property
        def _common_path(self):
            if self.number is None:
                raise YPYModelError('Key property number is None')

            return '/ydktest-filterread:a/ydktest-filterread:lst[ydktest-filterread:number = ' + str(self.number) + ']'

        def is_config(self):
            ''' Returns True if this instance represents config data else returns False '''
            return True

        def _has_data(self):
            if not self.is_config():
                return False
            if self.number is not None:
                return True

            if self.value is not None:
                return True

            return False

        @staticmethod
        def _meta_info():
            from ydk.models._meta import _ydktest_filterread as meta
            return meta._meta_table['A.Lst']['meta_info']

    @property
    def _common_path(self):

        return '/ydktest-filterread:a'

    def is_config(self):
        ''' Returns True if this instance represents config data else returns False '''
        return True

    def _has_data(self):
        if not self.is_config():
            return False
        if self.a1 is not None:
            return True

        if self.a2 is not None:
            return True

        if self.a3 is not None:
            return True

        if self.b is not None and self.b._has_data():
            return True

        if self.lst is not None:
            for child_ref in self.lst:
                if child_ref._has_data():
                    return True

        return False

    @staticmethod
    def _meta_info():
        from ydk.models._meta import _ydktest_filterread as meta
        return meta._meta_table['A']['meta_info']


