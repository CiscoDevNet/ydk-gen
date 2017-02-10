#Getting Set Up
<a href="http://packaging.ubuntu.com/html/getting-set-up.html">http://packaging.ubuntu.com/html/getting-set-up.html</a>

Install basic packaging software
```
$ sudo apt-get install gnupg pbuilder ubuntu-dev-tools bzr-builddeb apt-file
$ sudo apt-get install packaging-dev
```

Create your GPG key
```
$ gpg --gen-key
```
The default key type (RSA and DSA) is fine.

The default key size (currently 2048) is fine.

Setting the key expiration to 0 (never) is fine.

Enter your name.

Enter your email address.

Set a passphrase.
```
$ gpg --send-keys --keyserver keyserver.ubuntu.com <KEY ID>
```
pub   aaaaa/xxxxxxxx yyyy-mm-dd

Key ID is xxxxxxxx

Create your SSH key
```
$ ssh-keygen -t rsa
```

Upload your SSH key to Launchpad

Follow instructions at <a href="https://launchpad.net/~/+editsshkeys">https://launchpad.net/~/+editsshkeys</a>

Set up pbuilder
```
$ pbuilder-dist xenial create
```

Upload your GPG key to Launchpad
```
gpg --fingerprint <EMAIL>
```
Copy the Key Fingerprint (xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx) into the text box at <a href="https://launchpad.net/~/+editpgpkeys">https://launchpad.net/~/+editpgpkeys</a>

Check your email account and read the email that Launchpad sent you and decrypt it using GPG. to decrypt on ubuntu, copy the email message beginning with `-----BEGIN PGP MESSAGE-----` and ending with `-----END PGP MESSAGE-----` to a file, e.g., test.txt and create and empty file out.txt and run the below command.
```
gpg --output out.txt --decrypt test.txt
```

## Configure Bazaar
```
$ bzr whoami "<NAME> <<EMAIL>>"
$ bzr launchpad-login <LAUNCHPAD ACCOUNT>
```
eg.
```
$ bzr whoami "Bob Bacon (BB) <BBacon@cisco.com>"
$ bzr launchpad-login ydk
```

Configure your shell. Make sure the email and name exactly matches the one on your GPG key (check using gpg --fingerprint)
```
export DEBFULLNAME="<NAME>"
export DEBEMAIL="<EMAIL>"
```
eg.
```
export DEBFULLNAME="Bob Bacon (BB)"
export DEBEMAIL="BBacon@cisco.com"
```

#Packaging
<a href="http://packaging.ubuntu.com/html/packaging-new-software.html">http://packaging.ubuntu.com/html/packaging-new-software.html</a>

Install build tools
```
$ sudo apt-get install build-essential
$ sudo apt-get install dh-make bzr-builddeb
```

Download main package
```
$ wget -O ydk-0.5.2.tar.gz "https://github.com/CiscoDevNet/ydk-cpp/archive/0.5.2.tar.gz"
```

Start Package. Substitute ydk for bundle package names (ydk-ietf etc)
```
$ bzr dh-make ydk 0.5.2 ydk-0.5.2.tar.gz
```
Package type is l for library. Verify information is correct. This will create a directory `ydk`. In this, a few files under `ydk/debian` need to be customized.

##Customize Debian Files

Remove the optional example files
```
$ cd ydk/debian
$ rm *ex *EX
```
See the example debian files for core and bundles under [ydk](ydk/debian), [ietf](ietf/debian) etc. For each new release, mostly only the control and changelog files need to be modified.

***Debian/changelog***

This has to be updated to the current date and version.

Ubuntu version is 0.5.2-0ubuntu1 (upstream version 0.5.2, Debian version 0, Ubuntu version 1)

Change unstable to current development release (xenial)

***Debian/control***

first paragraph describes the source package

second and following paragraphs describe the packages to be built and dependencies. For the ydk-ietf and other bundles, make sure to add ydk (core) plus any other interdependencies.
```
Build-Depends: debhelper (>=9), libcurl4-openssl-dev, libpcre3-dev, libssh-dev, libxml2-dev, libxslt1-dev, libtool-bin, cmake (>=3), git, pkg-config
```
fill in a description of the program

***Debian/copyright***

fill in to follow the licence of the upstream source

***Debian/docs***

any upstream documentation files you think should be included in the final package

***README.source*** and ***README.Debian***

only needed if your package has any non-standard features

***Debian/source/format***
```
3.0 (quilt)
```

***Debian/rules***

a Makefile which compiles the code using debhelper 7 (mostly automated)

Commit the code to your packaging branch
```
$ bzr add debian/source/format
$ bzr commit -m "Initial commit of Debian packaging."
```
##Build Package
Build Package
```
$ bzr builddeb -- -us -uc
```
Optionally, use the -d flag
```
$ bzr builddeb -- -us -uc -d
```
Generate source.changes File
```
$ bzr builddeb -S
```
If the above fails with any 'gpg' related error, the below should work
```
$ debuild -S -rfakeroot -k<key-id>
```
Packages and source.changes are output to one directory above and thus outside the project. 

##Confirm source (optional)

View Contents
```
$ cd ../..
$ lesspipe ydk_0.5.2-0ubuntu1_amd64.deb
```

Install
```
$ sudo dpkg --install ydk_0.5.2-0ubuntu1_amd64.deb
```

Uninstall
```
$ sudo apt-get remove ydk
```

Build on a clean system
```
$ cd build-area
$ pbuilder-dist <UBUNTU RELEASE> build ydk_0.5.2-0ubuntu1.dsc
```
eg.
```
$ pbuilder-dist xenial build ydk_0.5.2-0ubuntu1.dsc
```

Upload branch to launchpad
```
$ bzr push lp:~<LP USERNAME>/+junk/ydk-package
```
Set up a PPA in Launchpad
```
https://launchpad.net/~<LP USERNAME>
```

##Upload Package

Upload PPA with dput
```
$ dput ppa:<LP USERNAME>/<PPA NAME> <SOURCE.CHANGES>
```
use -f or remove .upload file to push updates to this package.

*Note:* In case the build has some issues and you have to run 'dput' again and the package is rejected, make sure you edit the debian/changelog (see above). Increment the `*ubuntu<number>` number by 1. For example, if on the first upload, it was `0.5.2-0ubuntu1`, change it to `0.5.2-0ubuntu2`
