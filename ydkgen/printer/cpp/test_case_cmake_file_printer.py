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

'''
 test_case_printer.py

 YANG model driven API, cpp test case emitter.
'''
from itertools import chain

from ydkgen.api_model import Class

class CMakeListsPrinter(object):
    def __init__(self, ctx):
        self.ctx = ctx

    def print_cmakelists_file(self, packages, bundle_name):
        self.ctx.str("""
cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(ydk_{0}_test)

enable_testing()

set(CMAKE_MODULE_PATH ${{CMAKE_MODULE_PATH}} "${{CMAKE_SOURCE_DIR}}/CMakeModules/")
set(test_cases {4})


find_library(xml2_location xml2)
find_library(curl_location curl)
find_library(ssh_location ssh)
find_library(ssh_threads_location ssh_threads)
find_library(pcre_location pcre)
find_library(xslt_location xslt)
find_library(pthread_location pthread)
find_library(dl_location dl)
find_library(ydk_location ydk)
find_library(ydk_{1}_location ydk_{2})
find_package(Boost REQUIRED)
find_package(Boost COMPONENTS log_setup log thread date_time system filesystem unit_test_framework REQUIRED)

set(BOOST_INCLUDE_DIRS $boost_installation_prefix/include)

include_directories(SYSTEM ${{BOOST_INCLUDE_DIR}})

set(CMAKE_CXX_FLAGS         "${{CMAKE_CXX_FLAGS}} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")


# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()

foreach(test_name IN LISTS test_cases)
    add_executable(${{test_name}} ${{test_name}}.cpp)
    set_property(TARGET ${{test_name}} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${{test_name}} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_include_directories(${{test_name}} PRIVATE ${{BOOST_INCLUDE_DIRS}} /opt/local/include /usr/local/include)
    target_compile_definitions(${{test_name}} PRIVATE "BOOST_TEST_DYN_LINK=1")
    target_compile_definitions(${{test_name}} PRIVATE "BOOST_LOG_DYN_LINK=1")
    target_link_libraries(${{test_name}}
        ${{ydk_location}}
        ${{ydk_{3}_location}}
        ${{ydk_location}}
        ${{xml2_location}}
        ${{curl_location}}
        ${{ssh_location}}
        ${{ssh_threads_location}}
        ${{pcre_location}}
        ${{xslt_location}}
        ${{pthread_location}}
        ${{dl_location}}
        ${{Boost_UNIT_TEST_FRAMEWORK_LIBRARY}}
        ${{Boost_LOG_SETUP_LIBRARY}}
        ${{Boost_LOG_LIBRARY}}
        ${{BOOST_THREAD_LIBRARY}}
        ${{BOOST_DATE_TIME_LIBRARY}}
        ${{Boost_FILESYSTEM_LIBRARY}}
        ${{Boost_SYSTEM_LIBRARY}}
        -rdynamic
    )


    add_test(NAME ${{test_name}} COMMAND $<TARGET_FILE:${{test_name}}>)
endforeach(test_name)

""".format(*([bundle_name]*4 + self._get_test_file_names(packages))))

    def _get_test_file_names(self, packages):
        names = []
        for package in packages:
            if _has_tests(package):
                names.append('test_%s' % package.name)
        return [';'.join(names)]


def _has_tests(package):
    for element in package.owned_elements:
        if isinstance(element, Class) and not element.is_identity():
            return True
    return False
