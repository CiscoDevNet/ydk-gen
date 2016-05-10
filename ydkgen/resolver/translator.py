#!/usr/bin/python
#  ----------------------------------------------------------------
# Copyright 2016 Cisco Systems
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------

"""
Translate profile file to profile file to bundle file.

Regular expression taken from https://github.com/xym-tool/symd/blob/master/symd.py.
"""
import os
import re
import sys
import json
import shutil
import tempfile
from os import walk
from git import Repo
from optparse import OptionParser
from collections import defaultdict
from jinja2 import Environment, FileSystemLoader

dd = lambda: defaultdict(dd)
CWD = os.path.dirname(os.path.abspath(__file__))

MODULE_STATEMENT = re.compile('''^[ \t]*(sub)?module +(["'])?([-A-Za-z0-9]*(@[0-9-]*)?)(["'])? *\{.*$''')
REVISION_STATEMENT = re.compile('''^[ \t]*revision[\s]*(['"])?([-0-9]*)?(['"])?[\s]*\{.*$''')

def convert_uri(file=None, url=None, commitid=None, path=None):
    """ Convert uri to bundle format, local files is represented as:
            file://$YDKGEN_HOME/[relative path]
        remote file is represented as:
            https://[url]?commit-id=[commitid]&path=[path]

    """
    if file:
        return "file://{}".format(file)
    else:
        return "{}?commit-id={}&path={}".format(url, commitid, path)

def parse_file(lines):
    name = None
    revision = None
    kind = None

    for line in lines:
        match = MODULE_STATEMENT.match(line)
        if match:
            name = match.groups()[2]
            if match.groups()[0] == 'sub':
                kind = 'SUBMODULE'
            else:
                kind = 'MODULE'
        match = REVISION_STATEMENT.match(line)
        if match:
            revision = match.groups()[1]

    return name, revision, kind

def get_attr(tmp_dir, file):
    file = os.path.join(tmp_dir, file)
    with open(file) as fd:
        name, revision, kind = parse_file(fd.readlines())

    return name, revision, kind

def get_local_file_attr(tmp_dir, file):
    name, revision, kind = get_attr(tmp_dir, file)
    uri = convert_uri(file=os.path.join(tmp_dir, file))
    return {
            'name' : name,
            'revision' : revision,
            'kind' : kind,
            'uri' : uri
            }

def get_local_files_attr(tmp_dir, d):
    files_attr = []
    target_dir = tmp_dir + d
    for (dirpath, dirnames, filenames) in walk(target_dir):
        for filename in filenames:
            path = os.path.join(target_dir, filename)
            attr = get_local_file_attr(target_dir, filename)
            files_attr.append(attr)
    return files_attr


def get_remote_file_attr(url, commitid, path, tmp_dir):
    file_attr = {}
    file = os.path.join(tmp_dir, path)

    name, revision, kind = get_attr(tmp_dir, file)
    uri = convert_uri(url=url, commitid=commitid, path=path)
    return {
            'name' : name,
            'revision' : revision,
            'kind' : kind,
            'uri' : uri
            }

def get_remote_files_attr(url, commitid, d, tmp_dir):
    files_attr = []
    target_dir = os.path.join(tmp_dir, d)
    for (dirpath, dirnames, filenames) in walk(target_dir):
        for filename in filenames:
            path = os.path.join(d, filename)
            attr = get_remote_file_attr(url, commitid, path, tmp_dir)
            files_attr.append(attr)
    return files_attr

def get_bundle_definition(data, file):
    major, minor, patch = data['version'].split('.')
    name = os.path.basename(file).split('.json')[0]
    return {
            "name"  : name,
            "major" : major,
            "minor" : minor,
            "patch" : patch
            }


def get_ydk_version(data):
    major, minor, patch = data['version'].split('.')
    return {
            "major" : major,
            "minor" : minor,
            "patch" : patch
            }

def print_json_output(file, output_dir):

    ydk_root = os.path.expandvars('$YDKGEN_HOME')
    env = Environment(loader=FileSystemLoader(CWD),
                    trim_blocks=True)
    with open(os.path.join(ydk_root, file)) as json_file:
        data = json.load(json_file)

    bundle = get_bundle_definition(data, file)

    ydk_version = get_ydk_version(data)

    # populate local files
    local_files = []
    tmp_dir = ydk_root
    if 'dir' in data['models']:
        for d in data['models']['dir']:
            attrs = get_local_files_attr(tmp_dir, d)
            local_files.extend(attrs)

    if 'file' in data['models']:
        for f in data['models']['file']:
            attr = get_local_file_attr(tmp_dir, f)
            local_files.append(attr)

    # populate remote files
    remote_files = []
    remote_dirs = []
    if 'git' in data['models']:
        for g in data['models']['git']:
            tmp_dir = tempfile.mkdtemp(suffix='.yang')
            url = g['url']
            repo = Repo.clone_from(url, tmp_dir)
            for commit in g['commits']:
                if 'commitid' in commit:
                    commitid = commit['commitid']
                else:
                    commitid = 'HEAD'

                if 'file' in commit:
                    for path in commit['file']:
                        attr = get_remote_file_attr(url, commitid, path, tmp_dir)
                        remote_files.append(attr)

                if 'dir' in commit:
                    for d in commit['dir']:
                        attrs = get_remote_files_attr(url, commitid, d, tmp_dir)
                        remote_files.extend(attrs)
            shutil.rmtree(tmp_dir)

    output = env.get_template('template.json').render(
        local_files=local_files, remote_files=remote_files,
        bundle=bundle, ydk_version=ydk_version
        )

    with open(os.path.join(output_dir, bundle['name'] + '.json'), 'w') as fd:
        fd.write(output)

if __name__ == '__main__':

    parser = OptionParser(usage="usage: %prog [options]",
                          version="%prog 0.0.1",
                          description="Translate profile file to bundle file.")

    parser.add_option("--profile",
                      type=str,
                      dest="profile",
                      help="Take options from a profile file, any CLI targets ignored")

    parser.add_option("--output-directory",
                      type=str,
                      dest="output",
                      help="Output directory for bundle file.")

    try:
        arg = sys.argv[1]
    except IndexError:
        parser.print_help()
        sys.exit(1)

    (options, args) = parser.parse_args()

    if not os.environ.has_key('YDKGEN_HOME'):
        logger.error('YDKGEN_HOME not set')
        print >> sys.stderr, "Need to have YDKGEN_HOME set!"
        sys.exit(1)

    if options.profile:
        print_json_output(options.profile, options.output)
    else:
        print >> sys.stderr, "Profile file is required, see help."
        sys.exit(1)
