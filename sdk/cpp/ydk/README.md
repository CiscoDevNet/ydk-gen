<a href="https://github.com/CiscoDevNet/ydk-gen"><img src="https://cloud.githubusercontent.com/assets/17089095/14834057/2e1fe270-0bb7-11e6-9e94-73dd7d71e87d.png" height="240" width="240" ></a>

# YDK

YDK or YANG Development Kit is a Software Development Kit that provides API's that are modeled in YANG.

## Installation

Create a build directory to build the core library and install it:
```
    $ cd ydk
    $ mkdir build && cd build
    $ cmake .. && sudo make install
```

## Running samples

Create a build directory to build the samples:
```
    $ cd samples
    $ mkdir build && cd build
    $ cmake .. && make
```
Run the sample. For example, to run the `bgp_create.cpp` sample:
```
    $ ./bgp_create ssh://<username>:<password>@<host-address>:<port> [-v]
```
