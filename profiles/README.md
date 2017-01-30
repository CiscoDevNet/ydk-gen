#Bundle profiles

Contains bundle profiles to be used with the generate.py script as described in the [README](https://github.com/abhikeshav/ydk-gen/blob/gen_issues/README.md).

## An excerpt from the README on how to construct a bundle profile
A sample bundle profile file is described below. The file is in a JSON format. Specify the `name` of your bundle, the `version` of the bundle and the `ydk_version`, which refers to [the version](https://github.com/CiscoDevNet/ydk-gen/releases) of the ydk core package you want to use with this bundle. The `name` of the bundle here is especially important as this will form part of the installation path of the bundle.

```
{
    "name":"cisco-ios-xr",
    "version": "0.1.0",
    "ydk_version": "0.5.0",
    "Author": "Cisco",
    "Copyright": "Cisco",
    "Description": "Cisco IOS-XR Native Models From Git",
```

The "models" section of the file describes where to source models from. There are 3 sources:

- Directories
- Specific files
- Git, within which specific relative directories and files may be referenced

The sample below shows the use of git sources only.

```
    "models": {
        "git": [
```

We have a list of git sources. Each source must specify a URL. This URL should be one that allows the repository to be cloned without requiring user intervention, so please use a public URL such as the example below. There are three further options that can be specified:

- ```commitid``` - Optional specification of a commit in string form. The files identified will be copied from the context of this commit.
- ```dir``` - List of **relative** directory paths within git repository. All .yang files in this directory **and any sub-directories** will be pulled into the generated bundle.
- ```file```- List of **relative** file paths within the git repository.

Only directory examples are shown below.

```
            {
                "url": "https://github.com/YangModels/yang.git",
                "dir": [
                    "vendor/cisco/xr/532"
                ]
            },
            {
                "url": "https://github.com/YangModels/yang.git",
                "commitid": "f6b4e2d59d4eedf31ae8b2fa3119468e4c38259c",
                "dir": [
                    "experimental/openconfig/bgp",
                    "experimental/openconfig/policy"
                ]
            }
        ]
    },
```

