""" inherit 

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




class InheritRunner(object):
    """
    
    
    .. attribute:: jumper
    
    	
    	**type**\:  int
    
    	**range:** \-2147483648..2147483647
    
    .. attribute:: one
    
    	config for one\_level data
    	**type**\:  :py:class:`One <ydk.models.inherit.InheritRunner.One>`
    
    

    """

    _prefix = 'in'
    _revision = '2015-11-17'

    def __init__(self):
        self.jumper = None
        self.one = InheritRunner.One()
        self.one.parent = self


    class One(object):
        """
        config for one\_level data
        
        .. attribute:: name
        
        	this is string value
        	**type**\:  str
        
        .. attribute:: number
        
        	integer value type
        	**type**\:  int
        
        	**range:** \-2147483648..2147483647
        
        

        """

        _prefix = 'in'
        _revision = '2015-11-17'

        def __init__(self):
            self.parent = None
            self.name = None
            self.number = None

        @property
        def _common_path(self):

            return '/inherit:inherit-runner/inherit:one'

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
            from ydk.models._meta import _inherit as meta
            return meta._meta_table['InheritRunner.One']['meta_info']

    @property
    def _common_path(self):

        return '/inherit:inherit-runner'

    def is_config(self):
        ''' Returns True if this instance represents config data else returns False '''
        return True

    def _has_data(self):
        if not self.is_config():
            return False
        if self.jumper is not None:
            return True

        if self.one is not None and self.one._has_data():
            return True

        return False

    @staticmethod
    def _meta_info():
        from ydk.models._meta import _inherit as meta
        return meta._meta_table['InheritRunner']['meta_info']


