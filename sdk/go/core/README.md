### YDK GO Core and Bundle Installation

The YDK requires Go version 1.9 or higher. Make sure that corresponding software is installed and environment variables GOROOT and GOPATH are properly set before the YDK installation. Follow System Requirements [here](https://github.com/CiscoDevNet/ydk-gen/tree/master/sdk/go#system-requirements).

First, install [C++ core](https://github.com/CiscoDevNet/ydk-gen#second-step-generate--install-the-core). Then execute the below steps to install the ydk go core and bundle packages.

```
$ go get gopkg.in/stretchr/testify.v1
$ cd /your/path/to/ydk-gen
$ export GOPATH=/your/path/to/install/go/packages
$ mkdir -p $GOPATH/src/github.com/CiscoDevNet/ydk-go/ydk
$ cp -r sdk/go/core/ydk/* $GOPATH/src/github.com/CiscoDevNet/ydk-go/ydk
$ ./generate.py --bundle profiles/test/ydktest-cpp.json --go
$ cp -r gen-api/go/ydktest-bundle/ydk/models/*  $GOPATH/src/github.com/CiscoDevNet/ydk-go/ydk/models
```

Then execute the below to run sample tests
```
$ go run samples/cgo_path/cgo_path.go

$ go run samples/bgp_create/bgp_create.go

$ go run samples/bgp_read/bgp_read.go
```

To run tests:
```
$ cd /your/path/to/ydk-gen
$ cd sdk/cpp/core/tests/confd/ydktest
$ make clean
$ make all start
$ cd -
$ cd sdk/go/core/tests
$ go test
```
