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
 multi_file_builder.py


"""
from ydkgen.api_model import Class, Package
from ydkgen.common import sort_classes_at_same_level


class MultiFile(object):
    def __init__(self, fragmented):
        self.fragmented = fragmented
        self.file_name = ''
        self.class_list = []
        self.imports = set()


class MultiFileHeader(MultiFile):
    def __init__(self, package, file_index, fragmented):
        super(MultiFileHeader, self).__init__(fragmented)
        self.file_name = _get_header_name(package, file_index)
        self.include_guard = _get_include_guard_name(package, file_index)


class MultiFileSource(MultiFile):
    def __init__(self, package, file_index, fragmented):
        super(MultiFileSource, self).__init__(fragmented)
        self.file_name = _get_source_name(package, file_index)


class MultiFileData(object):
    def __init__(self):
        self._multi_file_list = []

    @property
    def multi_file_list(self):
        return self._multi_file_list

    @multi_file_list.setter
    def multi_file_list(self, multi_file_list):
        assert all(isinstance(x, MultiFile) for x in multi_file_list)
        self._multi_file_list = multi_file_list

    def append(self, multi_file):
        assert isinstance(multi_file, MultiFile)
        self._multi_file_list.append(multi_file)


class MultiFileBuilder(object):
    def __init__(self, package, classes_per_source_file, sort_clazz):
        self._multi_file_data = MultiFileData()
        self.sort_clazz = sort_clazz
        self.classes_per_source_file = classes_per_source_file
        self.is_all_identities = False
        self.class_list = []
        self.class_to_header_lookup = {}
        self.header_to_class_list_lookup = {}

        self._populate_class_list(package)
        self._populate_multi_file_data(package)
        self._populate_imports_for_fragmented_files()
#         for x in self._multi_file_data.multi_file_list:
#             print (x.file_name, x.imports)
#             for y in x.class_list:
#                 print y.fully_qualified_cpp_name()
#             print
#         print '---------------\n'

    @property
    def multi_file_data(self):
        return self._multi_file_data

    def _populate_class_list(self, element):
        clazzes = [n for n in element.owned_elements if isinstance(n, Class)]
        sorted_classes = sort_classes_at_same_level(clazzes, self.sort_clazz)
        for clazz in sorted_classes:
            if clazz.is_identity():
                self.is_all_identities = True
            else:
                self.is_all_identities = False
            self.class_list.append(clazz)
            self._populate_class_list(clazz)
            
    def _populate_multi_file_data(self, package):
        file_index = -1
        self._create_and_append_multi_file(MultiFileHeader, package, file_index, False, self.class_list)

        if not self.is_all_identities and len(self.class_list) > self.classes_per_source_file:
            index = 0
            file_index = 0

            fragmented_class_list = []
            current_index = 0
            while index < len(self.class_list):
                fragmented_class_list.append(self.class_list[index])
                index += 1
                current_index += 1
                if current_index >= self.classes_per_source_file or index >= len(self.class_list):
                    self._create_and_append_multi_file(MultiFileHeader, package, file_index, True, fragmented_class_list)
                    self._create_and_append_multi_file(MultiFileSource, package, file_index, True, fragmented_class_list)
                    file_index += 1

                    fragmented_class_list = []
                    current_index = 0
        else:
            self._create_and_append_multi_file(MultiFileSource, package, file_index, False, self.class_list)

    def _create_and_append_multi_file(self, class_type, package, file_index, fragmented, class_list):
        multi_file = class_type(package, file_index, fragmented)
        self._populate_multi_file(multi_file, class_list)
        self._multi_file_data.append(multi_file)
            
    def _populate_multi_file(self, multi_file, class_list):
        index = 0
        if isinstance(multi_file, MultiFileHeader):
            while index < len(class_list):
                clazz = class_list[index]
                self.class_to_header_lookup[clazz.fully_qualified_cpp_name()] = multi_file.file_name
                index += 1

        multi_file.class_list = class_list

    def _populate_imports_for_fragmented_files(self):
        for header in [x for x in self._multi_file_data.multi_file_list if x.fragmented and isinstance(x, MultiFileHeader)]:
            header.imports = self._get_imported_headers_for_parents(header.file_name, header.class_list)

        for source in [x for x in self._multi_file_data.multi_file_list if x.fragmented and isinstance(x, MultiFileSource)]:
            source.imports = self._get_imported_headers_for_children(source.file_name.replace('.cpp', '.hpp'), source.class_list)
                    
    def _get_imported_headers_for_parents(self, current_header, classes):
        parents = []
        for clazz in classes:
            owner = clazz.owner
            while owner is not None and not isinstance(owner, Package):
                parents.append(owner)
                owner = owner.owner
        return self._get_imported_headers(current_header, parents)

    def _get_imported_headers_for_children(self, current_header, classes):
        children = []
        for clazz in classes:
            child_classes = [nested_class for nested_class in clazz.owned_elements if isinstance(nested_class, Class)]
            for child in child_classes:
                children.append(child)
        return self._get_imported_headers(current_header, children)
    
    def _get_imported_headers(self, current_header, classes):
        imports_to_print = set()
        for clazz in classes:
            imported_header = self.class_to_header_lookup[clazz.fully_qualified_cpp_name()]
            if imported_header != current_header:
                import_stmt = '#include "{0}"'.format(imported_header)
                imports_to_print.add(import_stmt)
        return imports_to_print


def _get_header_name(package, file_index=-1):
    if file_index > -1:
        return '{0}_{1}.hpp'.format(package.name, file_index)
    return '{0}.hpp'.format(package.name)


def _get_source_name(package, file_index=-1):
    if file_index > -1:
        return '{0}_{1}.cpp'.format(package.name, file_index)
    return '{0}.cpp'.format(package.name)


def _get_include_guard_name(package, file_index=-1):
        if file_index > -1:
            return '_{0}_{1}_'.format(package.name.upper(), file_index)
        else:
            return '_{0}_'.format(package.name.upper())
