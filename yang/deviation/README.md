# Models in this folder

This folder contains openconfig models and deviation models. Openconfig models below

* bgp.yang
* bgp-types.yangn
* bgp-policy.yang
* bgp-operational.yang
* bgp-multiprotocol.yang
* policy-types.yang
* routing-policy.yang

are fetched from [YnagModels/yang](https://github.com/YangModels/yang), with commit id f6b4e2d59d4eedf31ae8b2fa3119468e4c38259c.

Deviation models below:

* cisco-xr-bgp-deviations.yang
* cisco-xr-bgp-policy-deviations.yang
* cisco-xr-routing-policy-deviations.yang

are fetched from Cisco XR 600 models.

# Changes

The following require-instance statements in bgp-policy.yang and bgp.yang are removed:

* bgp-policy.yang, line: 132
* bgp-policy.yang, line: 155
* bgp-policy.yang, line: 176
* bgp-policy.yang, line: 426
* bgp-policy.yang, line: 475
* bgp.yang, line: 419