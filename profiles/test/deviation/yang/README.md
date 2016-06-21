### Models in this folder

This folder contains openconfig modules and deviation modules

#### Deviation modules

Deviation modules are fetched from github repo:

* [cisco-xr-bgp-deviations.yang](https://github.com/YangModels/yang/blob/master/vendor/cisco/xr/600/cisco-xr-bgp-deviations.yang)
* [cisco-xr-bgp-policy-deviations.yang](https://github.com/YangModels/yang/blob/master/vendor/cisco/xr/600/cisco-xr-bgp-policy-deviations.yang)
* [cisco-xr-routing-policy-deviations.yang](https://github.com/YangModels/yang/blob/master/vendor/cisco/xr/600/cisco-xr-routing-policy-deviations.yang)

#### Matching openconfig modules

* bgp.yang
* bgp-types.yangn
* bgp-policy.yang
* bgp-operational.yang
* bgp-multiprotocol.yang
* policy-types.yang
* routing-policy.yang

They are fetched from [YnagModels/yang](https://github.com/YangModels/yang), with commit id f6b4e2d59d4eedf31ae8b2fa3119468e4c38259c.

#### Changes

The following require-instance statements in bgp-policy.yang and bgp.yang are removed:

* bgp-policy.yang, line: 132
* bgp-policy.yang, line: 155
* bgp-policy.yang, line: 176
* bgp-policy.yang, line: 426
* bgp-policy.yang, line: 475
* bgp.yang, line: 419