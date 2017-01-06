
from ydkgen import api_model as atypes
from .test_value_builder import ValueBuilder, BitsValue, IdentityValue
from .. import utils

_DEFAULT_VALUES = {
    'isis.instances.instance[0].instance_name': 'DEFAULT',
    'isis.instances.instance[0].running': 'Empty()',
    'policy_name': 'PASS-ALL',
    'route_policy_name': 'PASS-ALL',
    'system.radius.server[0].name': 'udp',
}

_DEFAULT_LEAFREFS = set({
    'afi_safi[0].afi_safi_name',
    'vlan[0].vlan_id',
    'destination_group[0].group_id',
    'destination_group[0].destinations.destination[0].destination_address',
    'destination_group[0].destinations.destination[0].destination_port',
    'sensor_group[0].sensor_group_id',
    'sensor_path[0].path',
    'subscription[0].subscription_id',
    'sensor_profile[0]->sensor_group'
})

_RELATIVE_DEFAULTS = {
    'afi_safi': 'config/enabled'
}


class Statements(object):
    def __init__(self):
        self.append_stmts = {}
        self.reference_stmts = {}
        self.adjustment_stmts = {}
        self.assignment_stmts = {}
        self.declaration_stmts = {}
        self.leaflist_append_stmts = {}
        self.reference_adjustment_stmts = {}
        self.key_properties = set()
        self.adjusted_leaflist_appends = {}

    def add_append(self, path, val):
        self.append_stmts[path] = val

    def add_assignment(self, path, val):
        self.assignment_stmts[path] = val

    def add_declaration(self, path, val):
        self.declaration_stmts[path] = val

    def add_leaflist_append(self, path, val):
        self.leaflist_append_stmts[path] = val

    def add_adjustment(self, path, val):
        self.adjustment_stmts[path] = val

    def add_reference_adjustment(self, path, val):
        self.reference_adjustment_stmts[path] = val

    def add_key_prop(self, key_prop):
        self.key_properties.add(key_prop)

    def add_reference(self, path, reference_path):
        if reference_path in self.reference_stmts:
            self.reference_adjustment_stmts[path] = reference_path
        else:
            self.reference_stmts[path] = reference_path

    @property
    def unadjusted_leaflist_appends(self):
        for path in sorted(self.leaflist_append_stmts):
            val = self.leaflist_append_stmts[path]
            if val in self.assignment_stmts:
                self.adjusted_leaflist_appends[path] = val
            else:
                yield path, val


class TestCasesBuilder(ValueBuilder):
    def __init__(self, lang, identity_subclasses):
        super(TestCasesBuilder, self).__init__(lang, identity_subclasses)
        self.stmts = Statements()
        self.test_name = ''
        self.clazz = None
        self.ref_top_classes = {}
        self.derived_identities = set()

    def build_test_case(self, clazz):
        self.clazz = clazz
        self.test_name = clazz.qn().lower().replace('.', '_')
        top_class = utils.get_top_class(clazz)
        self._add_declaration_stmt(top_class)
        self._add_requisite_stmts(clazz)
        self._add_mandatory_stmts(top_class)
        self._add_list_stmts(clazz)
        for prop in clazz.properties():
            self._add_prop_stmts(prop)

    def _add_declaration_stmt(self, element):
        obj_name = utils.get_obj_name(element)

        if isinstance(element, (atypes.Bits, atypes.Class)):
            value = utils.get_qn(self.lang, element)
            self.stmts.add_declaration(obj_name, value)
        else:
            # add dec stmt for bits value
            value = self._get_value(element)
            self.stmts.add_assignment(obj_name, value)
            if isinstance(value, BitsValue):
                value = value.type_spec
                self.stmts.add_declaration(obj_name, value)

    def _add_requisite_stmts(self, clazz):
        while not utils.is_pkg_element(clazz):
            self._add_requisite_clazz_stmts(clazz)
            for prop in clazz.properties():
                self._add_requisite_prop_stmts(prop)

            clazz = clazz.owner

    def _add_requisite_clazz_stmts(self, clazz):
        if utils.is_presence_element(clazz):
            self._add_presence_clazz_stmts(clazz)
        if utils.is_list_element(clazz):
            self._add_list_stmts(clazz)

    def _add_requisite_prop_stmts(self, prop):
        self._add_default_stmts(prop)
        self._add_relative_default_stmts(prop)
        if utils.is_mandatory_element(prop):
            self._add_prop_stmts(prop)
        elif utils.is_presence_element(prop):
            self._add_presence_prop_stmts(prop)

    def _add_presence_clazz_stmts(self, clazz):
        self._add_declaration_stmt(clazz)
        self._add_assignment_stmt(clazz)

    def _add_presence_prop_stmts(self, prop):
        self._add_declaration_stmt(prop.property_type)
        self._add_assignment_stmt(prop)

    def _add_mandatory_stmts(self, clazz):
        for prop in clazz.properties():
            if utils.is_class_prop(prop):
                self._add_mandatory_stmts(prop.property_type)
            if utils.is_mandatory_element(prop):
                self._add_requisite_stmts(prop.owner)
                self._add_prop_stmts(prop)

    def _add_assignment_stmt(self, element):
        ptype = self._get_element_ptype(element)
        path = self._get_element_path(element)
        if path not in self.stmts.declaration_stmts:
            obj_name = utils.get_obj_name(ptype)
            if utils.is_class_element(ptype):
                obj_name = self.assignment_fmt.format(obj_name)
            self.stmts.add_assignment(path, obj_name)

    def _add_list_stmts(self, clazz):
        while not utils.is_pkg_element(clazz):
            if all((utils.is_list_element(clazz),
                    not utils.is_pkg_element(clazz.owner))):
                self._add_declaration_stmt(clazz)
                self._add_list_key_stmts(clazz)
                self._add_append_stmt(clazz)

            clazz = clazz.owner

    def _add_list_key_stmts(self, clazz):
        for key_prop in clazz.get_key_props():
            if key_prop not in self.stmts.key_properties:
                self.stmts.add_key_prop(key_prop)
                self._add_prop_stmts(key_prop)

    def _add_prop_stmts(self, prop):
        if utils.is_config_prop(prop):
            if utils.is_reference_prop(prop):
                self._add_reference_stmts(prop)
                self._add_requisite_prop_stmts(prop)
            elif utils.is_terminal_prop(prop):
                self._add_terminal_prop_stmts(prop)

    def _add_reference_stmts(self, prop):
        refprop, refclass = self._get_reference_prop(prop)
        top_class = utils.get_top_class(prop)
        top_refclass = utils.get_top_class(refprop)
        if top_class != top_refclass:
            top_refclass_name = utils.get_qn(self.lang, top_refclass)
            self.ref_top_classes[top_refclass_name] = top_refclass
            self._add_mandatory_stmts(top_refclass)
            self._add_declaration_stmt(top_refclass)
        # addjust the key value in leafref path
        self._add_leafref_path_key_stmts(prop)
        self._add_list_stmts(refclass)
        self._add_prop_stmts(refprop)
        self._add_reference_stmt(prop, refprop)

    def _add_reference_stmt(self, prop, refprop):
        path = self._get_element_path(prop)
        refpath = self._get_element_path(refprop)
        value = self._get_value(refprop)
        if utils.is_leaflist_prop(prop):
            self.stmts.add_leaflist_append(path, refpath)
        else:
            self.stmts.add_reference(path, refpath)
        if utils.is_reference_prop(refprop):
            prop = refprop
            refprop, _ = self._get_reference_prop(prop)
            self._add_reference_stmt(prop, refprop)
        else:
            self._add_requisite_stmts(refprop.owner)
            self.stmts.add_assignment(refpath, value)

    def _add_terminal_prop_stmts(self, prop):
        path = self._get_element_path(prop)
        if utils.is_leaflist_prop(prop):
            path = '{}[0]'.format(path)
            value = self._get_value(prop)
            if isinstance(value, BitsValue):
                return
                # # ConfD internal error for leaflist of bits
                # path, value = self._render_bits_value(path, value.val)
                # self.stmts.add_assignment(path, value)
            self._add_declaration_stmt(prop)
            self._add_leaflist_append_stmts(prop)
        else:
            value = self._get_value(prop)
            if isinstance(value, BitsValue):
                if self.lang == 'py' and utils.is_union_prop(prop):
                    # add additional dec, assignment stmt
                    # for UnionType contains Bits
                    dec_obj = self._get_element_path(value.type_spec)
                    dec = utils.get_qn(self.lang, value.type_spec)
                    self.stmts.add_assignment(path, dec_obj)
                    self.stmts.add_declaration(dec_obj, dec)

                path, value = self._render_bits_value(path, value.val)

            self.stmts.add_assignment(path, value)

    def _add_leaflist_append_stmts(self, element):
        path = self._get_element_path(element)
        obj_name = utils.get_obj_name(element)
        self.stmts.add_leaflist_append(path, obj_name)

    def _add_append_stmt(self, element):
        path = self._get_element_path(element)
        obj_name = utils.get_obj_name(element)
        self.stmts.add_append(path, obj_name)

    def _add_default_stmts(self, prop):
        prop_path = self._get_element_path(prop)
        default_path = prop_path.replace('->', '.')
        for seg in _DEFAULT_LEAFREFS:
            if all((default_path.endswith(seg),
                    utils.is_reference_prop(prop))):
                self._add_default_reference_stmts(prop)

    def _add_default_reference_stmts(self, prop):
        refprop, refclass = self._get_reference_prop(prop)
        path = self._get_element_path(prop)
        refpath = self._get_element_path(refprop)
        self.stmts.add_adjustment(path, refpath)
        if utils.is_reference_prop(refprop):
            self._add_default_reference_stmts(refprop)

    def _add_relative_default_stmts(self, prop):
        path = _RELATIVE_DEFAULTS.get(prop.name, None)
        if path is None:
            return
        path, curr_prop = path.split('/'), prop
        for seg in path:
            for prop in curr_prop.property_type.owned_elements:
                if isinstance(prop, atypes.Property) and prop.name == seg:
                    curr_prop = prop
                    break

        self._add_terminal_prop_stmts(curr_prop)

    def _add_leafref_path_key_stmts(self, prop):
        orig_refstmt, _ = prop.stmt.i_leafref_ptr
        orig_refprop = orig_refstmt.i_property
        path_type_spec = prop.stmt.i_leafref
        if path_type_spec is None:
            return
        plist = path_type_spec.i_path_list
        _, pspec_list, _, _ = path_type_spec.path_spec
        if len(pspec_list) > len(plist):
            idx = 0
            for _, pstmt in plist:
                idx += 1
                pspec, pspec_list = pspec_list[0], pspec_list[1:]
                if pstmt.keyword == 'list':
                    if len(pspec) == 4 and pspec[0] == 'predicate':
                        pspec, pspec_list = pspec_list[0], pspec_list[1:]
                        _, identifier, up, dn = pspec
                        if up == 0:
                            # absolute path
                            continue

                        path_prop = self._get_path_predicate_prop(prop, up, dn)
                        # need to adjust value assigned according to predicate
                        if utils.is_reference_prop(path_prop):
                            path = self._get_element_path(orig_refprop,
                                                          length=idx)
                            if isinstance(identifier, tuple):
                                _, identifier = identifier
                            path = self.path_sep.join([path, identifier])
                            value = self._get_element_path(path_prop)
                            self.stmts.add_adjustment(path, value)
                        elif utils.is_terminal_prop(path_prop):
                            self._add_terminal_prop_stmts(path_prop)

    def _get_path_predicate_prop(self, prop, up, dn):
        stmt = prop.stmt
        while up:
            up -= 1
            stmt = stmt.parent

        for node in dn:
            for child in stmt.i_children:
                if child.arg == node:
                    stmt = child
                    break

        return stmt.i_property

    def _get_value(self, prop, default=None):
        prop_path = self._get_element_path(prop)
        default = None
        if prop_path in _DEFAULT_VALUES:
            default = _DEFAULT_VALUES[prop_path]
        elif prop.name in _DEFAULT_VALUES:
            default = _DEFAULT_VALUES[prop.name]
        value = self.get_prop_value(prop, default=default)
        if isinstance(value, IdentityValue):
            self.derived_identities.add(value.identity)
            value = value.val
        return value

    def _get_element_path(self, element, length=None):
        return utils.get_element_path(self.lang, element, length)

    def _get_element_ptype(self, element):
        ptype = element
        if isinstance(element, atypes.Property):
            ptype = element.property_type
        return ptype

    def _get_reference_prop(self, prop):
        ref, _ = prop.stmt.i_leafref_ptr
        refprop, refclass = ref.i_property, ref.parent.i_class
        return refprop, refclass

    def _render_bits_value(self, path, value):
        path = '{}["{}"]'.format(path, value)
        value = 'True'
        if self.lang == 'cpp':
            value = 'true'
        return path, value

    @property
    def assignment_fmt(self):
        fmt = '{}'
        if self.lang == 'cpp':
            fmt = 'std::move({})'
        return fmt
