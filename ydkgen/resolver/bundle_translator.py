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
import logging
import tempfile
from os import walk
from git import Repo
from shutil import rmtree
from optparse import OptionParser
from collections import namedtuple
from jinja2 import Environment

logger = logging.getLogger('ydkgen')

MODULE_STATEMENT = re.compile(r'''^[ \t]*(sub)?module +(["'])?([-A-Za-z0-9]*(@[0-9-]*)?)(["'])? *\{.*$''')
REVISION_STATEMENT = re.compile(r'''^[ \t]*revision[\s]*(['"])?([-0-9]*)?(['"])?[\s]*\{.*$''')

Local_URI = namedtuple('Local_URI', ['url'])
Remote = namedtuple('Remote', ['url', 'commitid'])
Remote_URI = namedtuple('RemoteURI', ['url', 'commitid', 'path'])

Bundle = namedtuple('Bundle', ['name', 'version', 'ydk_version'])

TEMPLATE = """{% set comma = joiner(",") %}
{
    "modules" : [{% for m in modules %}{{ comma() }}
        {
            "name" : "{{ m.name }}",
            "revision" : "{{ m.revision }}",
            "kind" : "{{ m.kind }}",
            "uri" : "{{ m.uri }}"
        }{% endfor %}
    ],

    "bundle" : {
        "name" : "{{ definition.name }}",
        "version" : "{{ definition.version}}",
        "ydk-version" : "{{ definition.ydk_version }}"{% if dependency is defined %},
        "dependency" : [{% for d in dependency %}{{ comma() }}
            {
                "name" : "{{ d.name }}",
                "version" : "{{ d.version }}",
                "ydk-version" : "{{ d.ydk_version }}",
                "uri" : "{{ d.uri }}"
            }{% endfor %}
        ]{% endif %}
    }
}
"""

class Module(object):
    def __init__(self, name, revision, kind, uri):
        self.name = name
        self.revision = revision
        self.kind = kind
        self.uri = convert_uri(uri)

def convert_uri(uri):
    """ Convert uri to bundle format, local files is represented as:

        For example:
            >>> convert_uri(Local_URI('relative/path/to/file'))
            'file://relative/path/to/file'
            >>> convert_uri(Remote_URI('repository', 'commitid', 'path'))
            'repository?commit-id=commitid&path=path'

    """
    if isinstance(uri, Local_URI):
        # path relative to $YDKGEN_HOME
        return "file://%s" % uri.url
    elif isinstance(uri, Remote_URI):
        return "%s?commit-id=%s&path=%s" % uri

def get_module_attrs(module_file, root, remote=None):
    """ Return name, latest revision, kind and uri attribute for module."""
    name, revision, kind, rpath = None, None, None, os.path.relpath(module_file, root)
    with open(module_file) as f:
        for line in f:
            match =  MODULE_STATEMENT.match(line)
            if match:
                name = match.groups()[2]
                if match.groups()[0] == 'sub':
                    kind = 'SUBMODULE'
                else:
                    kind = 'MODULE'
            match = REVISION_STATEMENT.match(line)
            if match:
                revision = match.groups()[1]
                break

    if remote is None:
        uri = Local_URI(rpath)
    else:
        uri = Remote_URI(remote.url, remote.commitid, rpath)
    return Module(name, revision, kind, uri)

def get_file_attrs(files, root, remote=None):
    for f in files:
        logger.debug('Getting attrs from file: %s' % f)
        yield get_module_attrs(os.path.join(root, f), root, remote)

def get_dir_attrs(dirs, root, remote=None):
    for d in dirs:
        for (d, _, files) in walk(os.path.join(root, d.lstrip('/'))):
            for res in  get_file_attrs((os.path.join(d, f) for f in files), root, remote):
                yield res

def get_git_attrs(repos, root, remote=None):
    for g in repos:
        url, tmp_dir = g['url'], tempfile.mkdtemp(suffix='.yang')
        logger.debug('Cloning from %s to %s' % (url, tmp_dir))
        repo = Repo.clone_from(url, tmp_dir)
        for c in g['commits']:
            commitid = c['commitid'] if 'commitid' in c else 'HEAD'
            repo.git.checkout(commitid)
            if 'file' in c:
                for fattr in get_file_attrs(c['file'], tmp_dir, Remote(url, commitid)):
                    yield fattr
            if 'dir' in c:
                for fattr in get_dir_attrs(c['dir'], tmp_dir, Remote(url, commitid)):
                    yield fattr
        logger.debug('Removing folder %s:' % tmp_dir)
        rmtree(tmp_dir)

def check_envs():
    if not os.environ.has_key('YDKGEN_HOME'):
        logger.error('YDKGEN_HOME not set.')
        print >> sys.stderr, "Need to have YDKGEN_HOME set!"
        sys.exit(1)

def populate_template(in_file, out_file):
    """ Generate bundle file using profile file. File is a relative path to a
    local profile file.
    """
    check_envs()
    ydk_root = os.path.expandvars('$YDKGEN_HOME')

    with open(in_file) as f:
        data = json.load(f)

    modules = []
    accepted_resources = ['file', 'dir', 'git']
    for source in accepted_resources:
        if source in data['models']:
            modules.extend(globals()['get_%s_attrs' % source](data['models'][source], ydk_root))

    version = data['version']
    definition = Bundle('bundle_name', version, version)

    output = Environment().from_string(TEMPLATE).render(
        modules=modules, definition=definition)

    with open(out_file, 'w') as f:
        f.write(output)


if __name__ == '__main__':

    import doctest
    doctest.testmod()

    parser = OptionParser(usage="usage: %prog [options]")

    parser.add_option("-v", "--verbose",
                  action="store_true",
                  dest="verbose",
                  default=False,
                  help="Verbose mode")

    (options, args) = parser.parse_args()

    if options.verbose:
        handler = logging.StreamHandler()
        logger.addHandler(handler)
        logger.setLevel(logging.DEBUG)

    populate_template('profiles/ydk/ydk_0_4_0.json', 'b.json')
