Name: ydk
Version: 0.5.4
Release: 1
Summary: YDK Core Library
Source: %{name}-%{version}.tar.gz

Vendor: Cisco
URL: https://github.com/CiscoDevNet/ydk-cpp
Group: Development/Libraries
License: Apache 2.0

PreReq: epel-release
Requires: python-pip, python-devel, libxml2-devel, libxslt-devel, libssh-devel, libcurl-devel, libtool, clang, cmake3, pcre-devel

%description
The YANG Development Kit (YDK) is a Software Development Kit 
that provides API's that are modeled in YANG. 
The main goal of YDK is to reduce the learning curve of 
YANG data models by expressing the model semantics 
in an API and abstracting protocol/encoding details. 
YDK is composed of a core package that defines services and providers, 
plus one or more bundles that are based on YANG models. 
Each bundle is generated using a bundle profile and the ydk-gen tool.
https://github.com/CiscoDevNet/ydk-cpp

%prep
%setup -q

%build
mkdir core/ydk/build && cd core/ydk/build
mkdir -p install/usr/local
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_INSTALL_PREFIX=./install/usr/local ..
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
cd core/ydk/build
make install
cp -r install/* $RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/local/include/libnetconf/
/usr/local/include/libyang/
/usr/local/lib/libydk.a
/usr/local/include/ydk/
/usr/local/include/spdlog/

%changelog
* Mon Apr 10 2017 Lily Li <lilyl2@cisco.com>
- Initial RPM release