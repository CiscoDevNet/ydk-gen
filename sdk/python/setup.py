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


# Always prefer setuptools over distutils
from setuptools import setup, find_packages, extension
# To use a consistent encoding
from codecs import open
from os import path
import platform
import shutil
import subprocess


__version__ = ''
here = path.abspath(path.dirname(__file__))

# Get version from version file
execfile(path.join(here, 'ydk', '_version.py'))

# Get the long description from the README file
with open(path.join(here, 'README.rst'), encoding='utf-8') as f:
    long_description = f.read()

ydk_packages = find_packages(exclude=['contrib', 'docs*', 'tests*', 'ncclient', 'samples'])
ext = []

'''lib_path = here + '/.libs'
libnetconf_include_path = here + '/.includes/'
lib_paths = [lib_path]
if platform.system() == 'Darwin' and subprocess.call(['brew info python &> /dev/null'], shell=True) == 0:
    python_homebrew_path='/usr/local/opt/python/Frameworks/Python.framework/Versions/2.7/lib'
    lib_paths = [lib_path, python_homebrew_path]


def _build_ydk_client_using_prebuilt_libnetconf():
    prebuilt_lib_path = lib_path + '/prebuilt/' + platform.system() + '/libnetconf.a'
    shutil.copy(prebuilt_lib_path, lib_path)
    return subprocess.call(['g++ -I/usr/include/python2.7 -I/usr/include/boost -I' + libnetconf_include_path +
                            ' -shared -fPIC ' + here + '/ydk/providers/_cpp_files/netconf_client.cpp'
                            ' -L/' + lib_path + ' -lnetconf -lpython2.7 -lboost_python -lxml2 -lcurl -lssh -lssh_threads -lxslt'], shell=True)


# Compile the YDK C++ code
exit_status = subprocess.call(['cd ' + here + '/.libs/libnetconf/ && ./configure > /dev/null && make > /dev/null && cp .libs/libnetconf.a .. '], shell=True)
if exit_status != 0:
    exit_status = _build_ydk_client_using_prebuilt_libnetconf()

if exit_status != 0:
    print('\nFailed to build libnetconf. Install all the dependencies mentioned in the README. No native code is being built.')
    ext = []
else:
    ext = [extension.Extension(
                              'ydk_client',
                              sources=[here + '/ydk/providers/_cpp_files/netconf_client.cpp'],
                              language='c++',
                              libraries=['netconf', 'python2.7', 'boost_python', 'xml2', 'curl', 'ssh', 'ssh_threads', 'xslt'],
                              extra_compile_args=['-Wall', '-std=c++0x'],
                              include_dirs=['/usr/include/python2.7', '/usr/include/boost', libnetconf_include_path],
                              library_dirs=lib_paths
                              )]'''

setup(
    name='ydk',

    # Versions should comply with PEP440.  For a discussion on single-sourcing
    # the version across setup.py and the project code, see
    # https://packaging.python.org/en/latest/single_source_version.html
    version=__version__,

    description='YDK Python SDK',
    long_description=long_description,

    # The project's main homepage.
    url='https://github.com/CiscoDevNet/ydk-py',

    # Author details
    author='Cisco Systems',
    author_email='yang-dk@cisco.com',

    # Choose your license
    license='Apache 2.0',

    # See https://pypi.python.org/pypi?%3Aaction=list_classifiers
    classifiers=[
        # How mature is this project? Common values are
        #   3 - Alpha
        #   4 - Beta
        #   5 - Production/Stable
        'Development Status :: 3 - Alpha',

        # Indicate who your project is intended for
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',

        # Pick your license as you wish (should match "license" above)
        'License :: OSI Approved :: Apache 2.0 License',

        # Specify the Python versions you support here. In particular, ensure
        # that you indicate whether you support Python 2, Python 3 or both.
        #'Programming Language :: Python :: 2',
        #'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        #'Programming Language :: Python :: 3',
        #'Programming Language :: Python :: 3.2',
        #'Programming Language :: Python :: 3.3',
        #'Programming Language :: Python :: 3.4',
    ],

    # What does your project relate to?
    keywords='yang',

    # You can just specify the packages manually here if your project is
    # simple. Or you can use find_packages().
    packages=ydk_packages,

    # List run-time dependencies here.  These will be installed by pip when
    # your project is installed. For an analysis of "install_requires" vs pip's
    # requirements files see:
    # https://packaging.python.org/en/latest/requirements.html
    install_requires=['ecdsa==0.13',
                    'enum34==1.1.3',
                    'lxml==3.4.4',
                    'paramiko==1.15.2',
                    'pyang==1.6',
                    'pycrypto==2.6.1',
                    'Twisted>=16.0.0',
                    'protobuf==3.0.0b2.post2',
                    'ncclient>=0.4.7'],


    ext_modules=ext,

    # List additional groups of dependencies here (e.g. development
    # dependencies). You can install these using the following syntax,
    # for example:
    # $ pip install -e .[dev,test]
    #extras_require={
    #    'dev': ['check-manifest'],
    #    'test': ['coverage'],
    #},

    # If there are data files included in your packages that need to be
    # installed, specify them here.  If using Python 2.6 or less, then these
    # have to be included in MANIFEST.in as well.
   #package_data={
   #     'sample': ['package_data.dat'],
   # },


    # Although 'package_data' is the preferred approach, in some case you may
    # need to place data files outside of your packages. See:
    # http://docs.python.org/3.4/distutils/setupscript.html#installing-additional-files # noqa
    # In this case, 'data_file' will be installed into '<sys.prefix>/my_data'
    #data_files=[('my_data', ['data/data_file'])],

    # To provide executable scripts, use entry points in preference to the
    # "scripts" keyword. Entry points provide cross-platform support and allow
    # pip to create the appropriate form of executable for the target platform.
    #entry_points={
    #    'console_scripts': [
    #        'sample=sample:main',
    #    ],
    #},
)



