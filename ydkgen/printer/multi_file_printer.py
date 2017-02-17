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

"""
 import_test_printer.py

 YANG model driven API, python emitter.

"""
import abc
import os
from ydkgen.printer import FilePrinter
from ydkgen.builder import MultiFile
import logging

logger = logging.getLogger('ydkgen')


class MultiFilePrinter(FilePrinter):
    def __init__(self, ctx):
        super(MultiFilePrinter, self).__init__(ctx)

    def print_output(self, package, multi_file, path_prefix):
        assert isinstance(multi_file, MultiFile)
        path = path_prefix
        if multi_file.fragmented:
            path = os.path.join(path, 'fragmented')
            if not os.path.isdir(path):
                os.mkdir(path)
        path = os.path.join(path, multi_file.file_name)
        logger.debug('Printing fragmented file {0}'.format(multi_file.file_name))
        with open(path, 'w+') as file_descriptor:
            self.ctx.fd = file_descriptor
            self._start_tab_leak_check()
            self.print_header(package, multi_file)
            self.print_body(multi_file)
            self.print_extra(package, multi_file)
            self.print_trailer(package, multi_file)
            self._check_tab_leak()

    @abc.abstractmethod
    def print_body(self, multi_file):
        pass

    def print_header(self, package, multi_file):
        pass

    def print_extra(self, package, multi_file):
        pass
