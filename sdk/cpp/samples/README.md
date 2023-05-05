# YDK C++ Samples

### Prerequisite
First install the [system requirements](https://github.com/CiscoDevNet/ydk-cpp#system-requirements). Then follow the [ydk-cpp README](https://github.com/CiscoDevNet/ydk-cpp) and install the C++ [core](https://github.com/CiscoDevNet/ydk-cpp/blob/master/core), [ietf bundle](https://github.com/CiscoDevNet/ydk-cpp/blob/master/ietf), [openconfig bundle](https://github.com/CiscoDevNet/ydk-cpp/blob/master/openconfig) and [cisco-ios-xr bundle](https://github.com/CiscoDevNet/ydk-cpp/blob/master/ciscp-ios-xr).

### 1) Build the sample apps
```
$ mkdir build && cd build
$ cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..
$ make && sudo make install
$ cd ..
```

### 2) Run the apps

Note: `-v` will turn on verbose mode
```
$ cd build
$ ./bgp_xr_write ssh://<username>:<password>@<host address>:<port> [-v]
$ ./bgp_xr_read ssh://<username>:<password>@<host address>:<port> [-v]
```
