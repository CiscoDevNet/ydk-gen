from ydk_ import is_set, get_relative_entity_path
from ydk_.types import (Enum, Entity, EntityPath, EditOperation, Identity,
                        YType, YLeaf)



class YdkEnumIntTestEnum(Enum):

    any = 4096

class YdkEnumTestEnum(Enum):

    not_set = 0

    none = 1

    local = 2

    remote = 3



class BaseIdentityIdentity(Identity):
    pass


class SubTest(Entity):

    __slots__ = ["one_aug"]

    def __init__(self):
        super(SubTest, self).__init__()
        self.parent = None
        self.operation = EditOperation.not_set
        self.one_aug = SubTest.OneAug()
        self.one_aug.__dict__["parent"] = self
        self.yang_name = "sub-test"
        self.yang_parent_name = "ydktest-sanity"


    class OneAug(Entity):


        def __init__(self):
            super(SubTest.OneAug, self).__init__()
            self.name = YLeaf(YType.str, "name")
            self.number = YLeaf(YType.int32, "number")
            self.yang_name = "one-aug"
            self.yang_parent_name = "sub-test"
            self.operation = EditOperation.not_set

        def __setattr__(self, name, value):
            if name in ("name", "number") and name in self.__dict__:
                self.__dict__[name].set(value)
            else:
                self.__dict__[name] = value

        def has_data(self):
            return any((self.name.is_set,
                        self.number.is_set))

        def has_operation(self):
            return any((is_set(self.operation),
                        is_set(self.name.operation),
                        is_set(self.number.operation)))

        def get_segment_path(self):
            return "one-aug"

        def get_entity_path(self, ancestor):
            if ancestor is None:
                pass
            ## get_relative_entity_path??

            leaf_name_data = []
            if self.name.is_set or is_set(self.name.operation):
                leaf_name_data.append(self.name.get_name_leafdata())
            if self.number.is_set or is_set(self.number.operation):
                leaf_name_data.append(self.number.get_name_leafdata())
            return EntityPath(self.get_segment_path() + '/', leaf_name_data)

        def get_children(self):
            if "one-aug" not in self.children:
                self.set_child["one-aug"] = self.one_aug
            return self.children

        def get_child_by_name(self, child_yang_name, segment_path):
            return None

        def set_value(self, value_path, value):
            pass

    def has_data(self):
        return all((self.one_aug is not None,
                    self.one_aug.has_data()))

    def has_operation(self):
        return all((is_set(self.operation),
                    self.one_aug is not None,
                    self.one_aug.has_operation()))

    def get_segment_path(self):
        return "ydktest-sanity:sub-test"

    def get_entity_path(self, ancestor):
        if ancestor is not None:
            raise Exception("ancestor has to be None for top-level node")
        leaf_name_data = []
        return EntityPath(self.get_segment_path(), leaf_name_data)

    def get_children(self):
        self.set_child("one-aug", self.one_aug)
        return self.children

    def get_child_by_name(self, child_yang_name, segment_path):
        if child_yang_name in self.children:
            return self.get_child(child_yang_name)
        elif segment_path in self.children:
            return self.get_child[segment_path]

        if child_yang_name == "one-aug":
            if self.one_aug is not None:
                self.set_child("one-aug", self.one_aug)
            else:
                self.one_aug = SubTest.OneAug()
                self.one_aug.parent = self
                self.set_child("one-aug", self.one_aug)
            return self.get_child("one-aug")
        return None

    def set_value(self, value_path, value):
        pass

