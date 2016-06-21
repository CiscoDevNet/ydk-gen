""" ydktest_types 

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

from ydk.errors import YPYError, YPYDataValidationError




class Ydk_Identity_Identity(object):
    """
    YDK identity
    
    

    """

    _prefix = 'types'
    _revision = '2016-05-23'

    def __init__(self):
        pass

    @staticmethod
    def _meta_info():
        from ydk.models.ydktest._meta import _ydktest_types as meta
        return meta._meta_table['Ydk_Identity_Identity']['meta_info']


