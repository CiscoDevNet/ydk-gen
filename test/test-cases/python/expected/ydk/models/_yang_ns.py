_global_ietf_inet_types_nsp = 'urn:ietf:params:xml:ns:yang:ietf-inet-types'
_global_ietf_netconf_nsp = 'urn:ietf:params:xml:ns:netconf:base:1.0'
_global_ietf_netconf_acm_nsp = 'urn:ietf:params:xml:ns:yang:ietf-netconf-acm'
_global_ietf_netconf_with_defaults_nsp = 'urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults'
_global_ietf_yang_types_nsp = 'urn:ietf:params:xml:ns:yang:ietf-yang-types'
_global_inherit_nsp = 'http://cisco.com/ns/yang/inherit'
_global_main_nsp = 'http://cisco.com/ns/yang/main'
_global_main_aug1_nsp = 'http://cisco.com/ns/yang/main-aug1'
_global_main_aug2_nsp = 'http://cisco.com/ns/yang/main-aug2'
_global_main_aug3_nsp = 'http://cisco.com/ns/yang/main-aug3'
_global_oc_pattern_nsp = 'http://cisco.com/ns/yang/oc-pattern'
_global_ydktest_filterread_nsp = 'http://cisco.com/ns/yang/ydk-filter'
_global_ydktest_sanity_nsp = 'http://cisco.com/ns/yang/ydktest-sanity'
_global_ydktest_sanity_augm_nsp = 'http://cisco.com/ns/yang/ydktest-sanity-augm'
_global_ydktest_sanity_types_nsp = 'http://cisco.com/ns/yang/ydktest-sanity-types'
_global_ydktest_sanity_deviations_another_nsp = 'http://cisco.com/ns/yang/ydktest-sanity-deviations-another'
_global_ydktest_sanity_deviations_types_nsp = 'http://cisco.com/ns/yang/ydktest-sanity-deviations-types'
_global_ydktest_sanity_submodule_nsp = 'http://cisco.com/ns/yang/ydktest-sanity'
_namespaces = { \
    'ietf-inet-types' : 'urn:ietf:params:xml:ns:yang:ietf-inet-types', 
    'ietf-netconf' : 'urn:ietf:params:xml:ns:netconf:base:1.0', 
    'ietf-netconf-acm' : 'urn:ietf:params:xml:ns:yang:ietf-netconf-acm', 
    'ietf-netconf-with-defaults' : 'urn:ietf:params:xml:ns:yang:ietf-netconf-with-defaults', 
    'ietf-yang-types' : 'urn:ietf:params:xml:ns:yang:ietf-yang-types', 
    'inherit' : 'http://cisco.com/ns/yang/inherit', 
    'main' : 'http://cisco.com/ns/yang/main', 
    'main-aug1' : 'http://cisco.com/ns/yang/main-aug1', 
    'main-aug2' : 'http://cisco.com/ns/yang/main-aug2', 
    'main-aug3' : 'http://cisco.com/ns/yang/main-aug3', 
    'oc-pattern' : 'http://cisco.com/ns/yang/oc-pattern', 
    'ydktest-filterread' : 'http://cisco.com/ns/yang/ydk-filter', 
    'ydktest-sanity' : 'http://cisco.com/ns/yang/ydktest-sanity', 
    'ydktest-sanity-augm' : 'http://cisco.com/ns/yang/ydktest-sanity-augm', 
    'ydktest-sanity-types' : 'http://cisco.com/ns/yang/ydktest-sanity-types', 
    'ydktest-sanity-deviations-another' : 'http://cisco.com/ns/yang/ydktest-sanity-deviations-another', 
    'ydktest-sanity-deviations-types' : 'http://cisco.com/ns/yang/ydktest-sanity-deviations-types', 
    'ydktest-sanity-submodule' : 'http://cisco.com/ns/yang/ydktest-sanity', 
}

_identity_map = { \
    ('ydktest-sanity', 'base-identity'):('ydk.models.ydktest.ydktest_sanity', 'BaseIdentity_Identity'),
    ('ydktest-sanity', 'child-child-identity'):('ydk.models.ydktest.ydktest_sanity', 'ChildChildIdentity_Identity'),
    ('ydktest-sanity', 'child-identity'):('ydk.models.ydktest.ydktest_sanity', 'ChildIdentity_Identity'),
    ('ydktest-sanity-types', 'another-one'):('ydk.models.ydktest.ydktest_sanity_types', 'AnotherOne_Identity'),
    ('ydktest-sanity-types', 'other'):('ydk.models.ydktest.ydktest_sanity_types', 'Other_Identity'),
    ('ydktest-sanity-types', 'ydktest-type'):('ydk.models.ydktest.ydktest_sanity_types', 'YdktestType_Identity'),
}

_namespace_package_map = { \
('http://cisco.com/ns/yang/inherit', 'runner') : 'from ydk.models.inherit.inherit import Runner', 
('http://cisco.com/ns/yang/oc-pattern', 'A') : 'from ydk.models.oc.oc_pattern import A', 
('urn:ietf:params:xml:ns:yang:ietf-netconf-acm', 'nacm') : 'from ydk.models.ietf.ietf_netconf_acm import Nacm', 
('http://cisco.com/ns/yang/ydktest-sanity', 'sub-test') : 'from ydk.models.ydktest.ydktest_sanity import SubTest', 
('http://cisco.com/ns/yang/ydk-filter', 'a') : 'from ydk.models.ydktest.ydktest_filterread import A', 
('http://cisco.com/ns/yang/ydktest-sanity', 'runner') : 'from ydk.models.ydktest.ydktest_sanity import Runner', 
('http://cisco.com/ns/yang/main', 'A') : 'from ydk.models.main.main import A', 
}

