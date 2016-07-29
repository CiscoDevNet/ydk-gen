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

from ydk.types import Empty, YList, YLeafList, DELETE, Decimal64, FixedBitsDict

from ydk.errors import YPYError, YPYModelError


from ydk.models.ydktest_sanity import BaseIdentityIdentity


class YdktestTypeIdentity(BaseIdentityIdentity):
    """
    This identity is used as a base for all YDK types.
    
    

    """

    _prefix = 'ydkut-types'
    _revision = '2016-04-11'

    def __init__(self):
        BaseIdentityIdentity.__init__(self)

    @staticmethod
    def _meta_info():
        from ydk.models._meta import _ydktest_sanity_types as meta
        return meta._meta_table['YdktestTypeIdentity']['meta_info']


class AnotherOneIdentity(YdktestTypeIdentity):
    """
    
    
    

    """

    _prefix = 'ydkut-types'
    _revision = '2016-04-11'

    def __init__(self):
        YdktestTypeIdentity.__init__(self)

    @staticmethod
    def _meta_info():
        from ydk.models._meta import _ydktest_sanity_types as meta
        return meta._meta_table['AnotherOneIdentity']['meta_info']


class OtherIdentity(YdktestTypeIdentity):
    """
    
    
    

    """

    _prefix = 'ydkut-types'
    _revision = '2016-04-11'

    def __init__(self):
        YdktestTypeIdentity.__init__(self)

    @staticmethod
    def _meta_info():
        from ydk.models._meta import _ydktest_sanity_types as meta
        return meta._meta_table['OtherIdentity']['meta_info']


