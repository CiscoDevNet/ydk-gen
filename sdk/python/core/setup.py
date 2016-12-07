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

"""Setup for YDK
"""
from __future__ import print_function
import os
import subprocess
import sys

from codecs import open as copen
from distutils import file_util
from setuptools.command.develop import develop
from setuptools.command.install import install
from setuptools.command.sdist import sdist
from setuptools import setup, find_packages


NMSP_PKG_NAME = "$PACKAGE$"
NMSP_PKG_VERSION = "$VERSION$"
NMSP_PKG_DEPENDENCIES = ["$DEPENDENCY$"]
path_module_name = 'path.so'

# Define and modify version number and package name here,
# Namespace packages are share same prefix: "ydk-models"
NAME = 'ydk'
VERSION = '0.5.2'
INSTALL_REQUIREMENTS = ['ecdsa==0.13',
                        'enum34==1.1.3',
                        'lxml==3.4.4',
                        'paramiko==1.15.2',
                        'pyang==1.6',
                        'pycrypto==2.6.1',
                        'Twisted>=16.0.0',
                        'protobuf==3.0.0b2.post2',
                        'ncclient>=0.4.7']

if NMSP_PKG_NAME != "$PACKAGE$":
    NAME = NMSP_PKG_NAME
if NMSP_PKG_VERSION != "$VERSION$":
    VERSION = NMSP_PKG_VERSION
if NMSP_PKG_DEPENDENCIES != ["$DEPENDENCY$"]:
    INSTALL_REQUIREMENTS.extend(NMSP_PKG_DEPENDENCIES)


here = os.path.abspath(os.path.dirname(__file__))

# Get the long description from the README file
with copen(os.path.join(here, 'README.md'), encoding='utf-8') as f:
    LONG_DESCRIPTION = f.read()


YDK_PACKAGES = find_packages(exclude=['contrib', 'docs*', 'tests*',
                                      'ncclient', 'samples'])


def create_shared_library():
    root_path = os.getcwd()
    cmake_build_dir = os.path.join(root_path, 'build_cpp')
    ydk_path = os.path.join(root_path, 'ydk')
    if not os.path.exists(cmake_build_dir):
        os.makedirs(cmake_build_dir)
    os.chdir(cmake_build_dir)
    try:
        subprocess.check_call(['cmake', '..'])
        subprocess.check_call(['make', '-j5'])
        file_util.copy_file(path_module_name, os.path.join(ydk_path, path_module_name))
        os.chdir(root_path)
    except subprocess.CalledProcessError as e:
        print('\nERROR: Failed to create shared library!\n')
        sys.exit(e.returncode)


class YdkInstall(install):
    def run(self):
        create_shared_library()
        install.run(self)


class YdkDevelop(develop):
    def run(self):
        create_shared_library()
        develop.run(self)


class YdkSourceDist(sdist):
    def run(self):
        create_shared_library()
        sdist.run(self)


setup(
    name=NAME,
    version=VERSION,
    description='YDK Python SDK',
    long_description=LONG_DESCRIPTION,
    url='https://github.com/CiscoDevNet/ydk-py',
    author='Cisco Systems',
    author_email='yang-dk@cisco.com',
    license='Apache 2.0',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: C++'
    ],
    keywords='yang',
    packages=YDK_PACKAGES,
    install_requires=INSTALL_REQUIREMENTS,
    cmdclass={
             'develop' :YdkDevelop,
             'install' : YdkInstall,
             'sdist' : YdkSourceDist,
             },
    include_package_data=True
)
