## Running Go Samples

Please set `$GOPATH` and have the ydk go core and [`cisco-ios-xr`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/cisco-ios-xr_6_3_1.json) & [`ietf`](https://github.com/CiscoDevNet/ydk-gen/blob/master/profiles/bundles/ietf_0_1_4.json) bundles installed as specified [here](https://github.com/CiscoDevNet/ydk-gen/blob/master/sdk/go/core/README.md).

To run these samples, execute the below.

Please refer to the [YDK API documentation](http://ydk.cisco.com) for an explanation of the different services.

For example, to run samples on a device supporting netconf and the `cisco-ios-xr` models, see the below.

To run a CRUD service sample, execute:

```
go run crud/cisco_ios_xr/shellutil_oper/nc_read_20/nc_read_20.go -device ssh://<username>:<password>@<host>:<port> [-v]
```

To run a Netconf service sample, execute:
```
go run netconf/cisco_ios_xr/infra_infra_locale_cfg/nc_edit_26/nc_edit_26.go -device ssh://<username>:<password>@<host>:<port> [-v]
```

To run an Executor service sample, execute:
```
go run executor/cisco_ios_xr/syslog_act/nc_execute_20/nc_execute_20.go -device ssh://<username>:<password>@<host>:<port> [-v]
```

To run a Codec service sample, execute:

```
go run codec/cisco_ios_xr/clns_isis_cfg/cd_encode_20/cd_encode_20.go [-v]
```

To run the `openconfig` `bgp_create` sample on a [confd](https://github.com/CiscoDevNet/ydk-gen/tree/master/sdk/cpp/core/tests) simulator, execute:
```
go run bgp_create/bgp_create.go [-v]
```
