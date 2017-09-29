# How to Package a YDK Library for RPM/Debian/MacOS
We can use the CPack module in CMake to produce packages based off os.
```
$ cd project_dir/ydk-gen/sdk/cpp/core/ydk
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="/usr/bin/clang" -DCMAKE_CXX_COMPILER="usr/bin/clang++" ..
$ make package
```
That's all for RPM/Debian packages. Please see the following section for creating packages for Mac OS.

## Mac OS
The CPack generators for producing a .pkg file for mac os are unreliable. Instead, we can use the pkgbuild tool included on Mac OS. **Please note that Mac OS packages will not check for dependencies.**

Unzip the tarball produced by the previous steps and then execute the build command:
```
$ tar -xvf tarball_name.tar.gz
$ pkgbuild --root tarball_name --identifier com.cisco.project_name --version project_version --install-location install_location package_name.pkg
```

For example, the following was how libydk was packaged for 0.5.4 release: 
```
$ tar -xvf libydk-0.5.4-Darwin.tar.gz
$ pkgbuild --root libydk-0.5.4-Darwin --identifier com.cisco.libydk --version 0.5.4 --install-location /usr/local libydk_0.5.4_Darwin.pkg
```

# Note
The following variables are hardcoded in the CMakeLists.txt file and may need to be modified depending on the desired package/bundle:
* CPACK_PACKAGE_NAME
* CPACK_PACKAGE_VERSION
* CPACK_PACKAGE_DESCRIPTION_SUMMARY
* CPACK_PACKAGE_FILE_NAME

Also, Mac OS packages will not check for dependencies.
