#  ----------------------------------------------------------------
# Copyright 2016 Cisco Systems
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------
""" _importer.py

    Import _yang_ns for each installed bundle at runtime.
"""

import pip
import sys
import importlib


class YangNs(object):
    def __init__(self, d):
        self.__dict__ = d

# TODO: add wrapper for YangNs, override __getattribute__, catch Attribute error,
# install(or notify user to install) missing package.

_yang_ns_dict = {}
installed_packages = pip.get_installed_distributions()
exempt = set(['__doc__', '__name__', '__package__', '__file__', '__builtins__'])

for installed_package in installed_packages:
    project_name = installed_package.project_name
    if project_name.startswith('ydk-'):
        module_name = project_name.replace('-', '_')
        mod_yang_ns = importlib.import_module('%s.models._yang_ns' % module_name)
        keys = set(mod_yang_ns.__dict__) - exempt
        for key in keys:
            if key not in _yang_ns_dict:
                _yang_ns_dict[key] = mod_yang_ns.__dict__[key]
            else:
                if isinstance(_yang_ns_dict[key], dict):
                    _yang_ns_dict[key].update(mod_yang_ns.__dict__[key])
                else:
                    # shadow old entry
                    _yang_ns_dict[key] = mod_yang_ns.__dict__[key]


_yang_ns = YangNs(_yang_ns_dict)
