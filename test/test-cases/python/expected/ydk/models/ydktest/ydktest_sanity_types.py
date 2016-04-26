""" ydktest_sanity_types 

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


from ydk.models.ydktest.ydktest_sanity import BaseIdentity_Identity


class YdktestType_Identity(BaseIdentity_Identity):
    """
    This identity is used as a base for all YDK types.
    
    

    """

    _prefix = 'ydkut-types'
    _revision = '2016-04-11'

    def __init__(self):
        BaseIdentity_Identity.__init__(self)

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity_types as meta
        return meta._meta_table['YdktestType_Identity']['meta_info']


class AnotherOne_Identity(YdktestType_Identity):
    """
    
    
    

    """

    _prefix = 'ydkut-types'
    _revision = '2016-04-11'

    def __init__(self):
        YdktestType_Identity.__init__(self)

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity_types as meta
        return meta._meta_table['AnotherOne_Identity']['meta_info']


class Other_Identity(YdktestType_Identity):
    """
    
    
    

    """

    _prefix = 'ydkut-types'
    _revision = '2016-04-11'

    def __init__(self):
        YdktestType_Identity.__init__(self)

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_sanity_types as meta
        return meta._meta_table['Other_Identity']['meta_info']


