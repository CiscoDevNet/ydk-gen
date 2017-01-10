from pyang import types as ptypes
from ydkgen import api_model as atypes


def has_terminal_nodes(element):
    # has leaf or leaflist
    if isinstance(element, atypes.Property):
        ptype = element.property_type
    else:
        ptype = element
    for p in ptype.properties():
        if is_terminal_prop(p):
            return True
    return False


def is_config_prop(prop):
    is_config = True
    if hasattr(prop.stmt, 'i_config'):
        is_config = prop.stmt.i_config
    return is_config


def is_nonid_class_element(element):
    return isinstance(element, atypes.Class) and not element.is_identity()


def is_class_element(element):
    return isinstance(element, atypes.Class)


def is_identity_element(element):
    return isinstance(element, atypes.Class) and element.is_identity()


def is_list_element(element):
    return element.stmt.keyword == 'list'


def is_mandatory_element(element):
    mandatory = element.stmt.search_one('mandatory')
    return mandatory is not None and mandatory.arg == 'true'


def is_pkg_element(element):
    return isinstance(element, atypes.Package)


def is_presence_element(element):
    return element.stmt.search_one('presence') is not None


def is_prop_element(element):
    return isinstance(element, atypes.Property)


def is_class_prop(prop):
    return is_class_element(prop.property_type)


def is_decimal64_prop(prop):
    return isinstance(prop.property_type, ptypes.Decimal64TypeSpec)


def is_empty_prop(prop):
    return isinstance(prop.property_type, ptypes.EmptyTypeSpec)


def is_identity_prop(prop):
    return is_identity_element(prop.property_type)


def is_identityref_prop(prop):
    return (isinstance(prop.property_type, atypes.Class) and
            prop.property_type.is_identity() and
            prop.stmt.i_leafref_ptr is not None)


def is_leaflist_prop(prop):
    return prop.stmt.keyword == 'leaf-list'


def is_leafref_prop(prop):
    return (isinstance(prop.property_type, ptypes.PathTypeSpec) and
            prop.stmt.i_leafref_ptr is not None)


def is_path_prop(prop):
    return isinstance(prop.property_type, ptypes.PathTypeSpec)


def is_reference_prop(prop):
    return (is_leafref_prop(prop) or is_identityref_prop(prop))


def is_terminal_prop(prop):
    return prop.stmt.keyword in ('leaf', 'leaflist')


def is_union_prop(prop):
    return is_union_type_spec(prop.property_type)


def is_union_type_spec(type_spec):
    return isinstance(type_spec, ptypes.UnionTypeSpec)


def is_identityref_type_spec(type_spec):
    return isinstance(type_spec, ptypes.IdentityrefTypeSpec)


def is_match_all(pattern):
    return pattern in ('[^\*].*', '\*')


def get_typedef_stmt(type_stmt):
    while all([hasattr(type_stmt, 'i_typedef') and
               type_stmt.i_typedef is not None]):
        type_stmt = type_stmt.i_typedef.search_one('type')
    return type_stmt


def get_top_class(clazz):
    while not isinstance(clazz.owner, atypes.Package):
        clazz = clazz.owner
    return clazz


def get_obj_name(clazz):
    obj_names = []
    while not isinstance(clazz, atypes.Package):
        obj_name = clazz.name.lower()
        obj_names.append(obj_name)
        clazz = clazz.owner
    return '_'.join(reversed(obj_names))


def get_qn(lang, element):
    qn = ''
    if lang == 'py':
        qn = element.qn()
    elif lang == 'cpp':
        qn = element.fully_qualified_cpp_name()
    return qn


def get_element_path(lang, element, length=None):
    # path is consists of path segments(seg)
    path = []
    sep = get_path_sep(lang)
    while not is_pkg_element(element):
        seg = _get_element_seg(element)
        if all((is_list_element(element),
                not is_pkg_element(element.owner),
                path)):
            # list/leaf-list contains one element
            seg += '[0]'
        path.append(seg)
        element = element.owner

    if length is None:
        return sep.join(reversed(path))
    else:
        # ever used?
        path = list(reversed(path))[:length]
        return sep.join(path)


def _get_element_seg(element):
    seg = ''
    if any((is_pkg_element(element.owner),
            is_prop_element(element))):
        seg = element.name
    else:
        for prop in element.owner.properties():
            if prop.stmt == element.stmt:
                seg = prop.name
    return seg.lower()


def get_path_sep(lang):
    sep = ''
    if lang == 'py':
        sep = '.'
    elif lang == 'cpp':
        sep = '->'
    return sep
