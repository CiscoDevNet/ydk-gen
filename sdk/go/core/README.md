## YDK GO Core and Bundle Installation

The YDK requires Go version 1.9 or higher. Make sure that corresponding software is installed and environment variables GOROOT and GOPATH are properly set before the YDK installation. Follow System Requirements [here](https://github.com/CiscoDevNet/ydk-gen/tree/master/sdk/go#system-requirements).

### YDK core and model bundle installation

First, install [C++ core](https://github.com/CiscoDevNet/ydk-gen#second-step-generate--install-the-core). Then execute the below steps to install the ydk go core and bundle packages.

```
  go get gopkg.in/stretchr/testify.v1
  cd /your-path-to-ydk-gen
  export GOPATH=/your-path-to-go-packages-installation-directory
  ./generate.py -i --core --go
  ./generate.py -i --bundle profiles/test/ydktest-cpp.json --go
```

For security reasons starting from Go version 1.10 only a limited set of flags is allowed in the CGO code, notably -D, -I, and -l.
Current ydk-go code includes few additional CGO LDFLAGS flags in order to allow coverage testing; they are: "-fprofile-arcs -ftest-coverage --coverage". In order to allow these additional flags to be used, it is necessary to set environment variable CGO_LDFLAGS_ALLOW before running ydk-go based application. Here is how the sample applications could be executed with Go 1.10.x:

```
  export CGO_LDFLAGS_ALLOW="-fprofile-arcs|-ftest-coverage|--coverage"
  go run samples/cgo_path/cgo_path.go
  go run samples/bgp_create/bgp_create.go
  go run samples/bgp_read/bgp_read.go
```

To run YDK sanity tests execute:

```
  cd /your-path-to-ydk-gen
  cd sdk/go/core/tests
  go test
```
