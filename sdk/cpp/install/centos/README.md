# How to Create an RPM Package
## Set Up CentOS Build Environment
RPM creation requires the rpm and rpmbuild packages, so check that they're installed:
```
$ which rpmbuild
```

The RPM environment consists of 5 directories:
* BUILD
* RPMS
* **SOURCES**
* **SPECS**
* SRPMS

Initialize the environment in the home directory with:
```
$ mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
```
To build the RPM, all that's required is the **source** tarball and the **spec** file.

For more details on setup, see [Set Up CentOS Build Environment](https://wiki.centos.org/HowTos/SetupRpmBuildEnvironment) 

## The Source Tarball
For example, to build ydk-cpp 0.5.4, place the tarball in the SOURCES directory:
```
$ cd ~/rpmbuild/SOURCES
$ wget https://github.com/CiscoDevNet/ydk-cpp/archive/0.5.4.tar.gz
```

RPM package names follow a strict format: name-version-release.architecture.rpm
Since the tarball will be unzipped and placed into the BUILD directory, we must sanitize the tarball such that its name and contents are consistent with the format name-version:
```
$ tar -xvf 0.5.4.tar.gz
$ mv ydk-cpp-0.5.4 ydk.0.5.4    // note: 0.5.4.tar.gz unzips to ydk-cpp-0.5.4
$ tar -cvf ydk.0.5.4.tar.gz ydk.0.5.4
$ rm 0.5.4.tar.gz
$ rm -rf ydk.0.5.4
```

## The Spec File
The spec file is included. Simply move it to the SPEC directory:
```
$ mv /path/to/ydk.spec ~/rpmbuild/SPEC/ydk.spec
```
For more information on spec files, see [Fedora Project Docs: Spec Files](https://docs.fedoraproject.org/en-US/Fedora_Draft_Documentation/0.1/html/RPM_Guide/ch-specfiles.html)

## Build the RPM
To build the binary, run:
```
$ rpmbuild -bb ~/rpmbuild/SPEC/ydk.spec
```
See more build options at [Fedora Project Docs: rpmbuild Command](https://docs.fedoraproject.org/en-US/Fedora_Draft_Documentation/0.1/html/RPM_Guide/ch08s02s04.html)
