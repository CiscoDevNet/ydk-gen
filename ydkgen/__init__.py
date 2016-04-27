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

from . import resolve_profile
import logging
import json
import pyang
from pyang import plugin, Repository
from pyang import error
from pyang.error import err_add
from pyang import util
from pyang import statements
import re
from . import common
from .common import get_rst_file_name
from ydkgen.api_model_builder import ApiModelBuilder, GroupingClassApiModelBuilder
import optparse
import os
import shutil
from .printer import YdkPrinter
from optparse import OptionParser
import sys
from common import YdkGenException

logger = logging.getLogger('ydkgen')
logger.addHandler(logging.NullHandler())

def _copy_substmts_ichildren(stmt):
    """ Return copy of current statement's substmts and i_children """
    chs = set(stmt.i_children) if hasattr(stmt, 'i_children') else set()
    non_chs = set()
    for s in stmt.substmts:
        if s not in chs:
            non_chs.add(s)
    chs = list(chs)
    non_chs = list(non_chs)
    return chs + non_chs, chs

def _set_d_substmts_ichildren(stmt):
    """ Set d_substmts and d_children for further recovery """
    if not hasattr(stmt, 'd_children') and not hasattr(stmt, 'd_substmts'):
        d_substmts, d_children = _copy_substmts_ichildren(stmt)
        stmt.d_children = d_children
        stmt.d_substmts = d_substmts

def _add_d_info(ctx, stmt):
    """ Copy the i_children and substmts attribute for target statement or
     target statement's parent. """
    # stmt.keyword == 'deviation'
    t = stmt.i_target_node
    if t is None:
        return
    for d in stmt.search('deviate'):
        if d.arg == 'not-supported':
            t = t.parent
        _set_d_substmts_ichildren(t)

def _remove_d_info(ctx, stmt):
    """ Use the copied d_children and d_substmts to revcover i_childre and substmts
     stmt.keyword == 'deviate' """
    t = stmt.parent.i_target_node
    if t is None:
        return
    if stmt.arg == 'not-supported':
        t = t.parent
    if hasattr(t, 'd_children') and hasattr(t, 'd_substmts'):
        if hasattr(t, 'i_children'):
            t.i_children = t.d_children
        t.substmts = t.d_substmts
        del t.d_children
        del t.d_substmts

def _add_deviation(target, dev_type, dev_module, dev_stmt):
    """ Add deviation information to target statement """
    if not hasattr(target, 'i_deviation'):
        target.i_deviation = {}
    if dev_type not in target.i_deviation:
        target.i_deviation[dev_type] = []
    target.i_deviation[dev_type].append((dev_module, dev_stmt))

def _add_deviation_r(target, dev_type, dev_module, dev_stmt):
    """ Add deviation information to target node recursively """
    _add_deviation(target, dev_type, dev_module, dev_stmt)
    sub = target.substmts
    if hasattr(target, 'i_children'):
        sub = sub + target.i_children
    for d in sub:
        _add_deviation(d, dev_type, dev_module, dev_stmt)

def _add_i_deviation(ctx, stmt):
    if not hasattr(stmt.i_module, 'is_deviation_module'):
        stmt.i_module.is_deviation_module = True
    t = stmt.i_target_node
    if t is None:
        return
    stmt = stmt.search_one('deviate')

    if stmt.arg == 'not-supported':
        if ((t.parent.keyword == 'list') and
            (t in t.parent.i_key)):
            err_add(ctx.errors, stmt.pos, 'BAD_DEVIATE_KEY',
                    (t.i_module.arg, t.arg))
            return
        if not hasattr(t.parent, 'i_not_supported'):
            t.parent.i_not_supported = []
        t.parent.i_not_supported.append(t)
        _add_deviation(t, 'not_supported', stmt.i_module, stmt)
    elif stmt.arg == 'add':
        for c in stmt.substmts:
            if c.keyword in statements._singleton_keywords:
                if t.search_one(c.keyword) != None:
                    err_add(ctx.errors, c.pos, 'BAD_DEVIATE_ADD',
                            (c.keyword, t.i_module.arg, t.arg))
                elif t.keyword not in statements._valid_deviations[c.keyword]:
                    err_add(ctx.errors, c.pos, 'BAD_DEVIATE_TYPE',
                            c.keyword)
                else:
                    _add_deviation(t, 'add', stmt.i_module, c)
            else:
                if t.keyword not in statements._valid_deviations[c.keyword]:
                    err_add(ctx.errors, c.pos, 'BAD_DEVIATE_TYPE',
                            c.keyword)
                else:
                    _add_deviation(t, 'add', stmt.i_module, c)
    else: # delete or replace
        for c in stmt.substmts:
            if (c.keyword == 'config'
                and stmt.arg == 'replace'
                and hasattr(t, 'i_config')):
                _add_deviation_r(t, 'replace', stmt.i_module, c)
            if c.keyword in statements._singleton_keywords:
                old = t.search_one(c.keyword)
            else:
                old = t.search_one(c.keyword, c.arg)
            if old is None:
                err_add(ctx.errors, c.pos, 'BAD_DEVIATE_DEL',
                        (c.keyword, t.i_module.arg, t.arg))
            else:
                _add_deviation(t, stmt.arg, stmt.i_module, c)

def _parse_and_return_modules(resolved_model_dir):
    """ Use pyang to parse the files and get a list of modules.

        :param str resolved_model_dir The directory where all models to be compiled are found.
        :raise YdkGenException If there was a problem parsing the modules
    """
    repos = pyang.FileRepository(resolved_model_dir, False)
    ctx = pyang.Context(repos)

    statements.add_validation_fun(
        'reference_3', ['deviation'],
        _add_i_deviation)
    statements.add_validation_fun(
        'reference_3', ['deviation'],
        _add_d_info)
    statements.add_validation_fun(
        'reference_3', ['deviate'],
        _remove_d_info)


    filenames = []

    #(name, rev, handle)
    # where handle is (format, absfilename)
    for (_, _, (_, filename)) in repos.get_modules_and_revisions(ctx):
        filenames.append(filename)

    modules = []

    r = re.compile(r"^(.*?)(\@(\d{4}-\d{2}-\d{2}))?\.(yang|yin)$")
    for filename in filenames:
        f = filename
        if filename.startswith('file://'):
            f = filename[len('file://') - 1:]
        try:
            fd = open(f)
            text = fd.read()
        except IOError as ex:
            err_msg = "error %s: %s\n" % (filename, str(ex))
            logger.error(err_msg)
            raise YdkGenException(err_msg)

        m = r.search(filename)
        ctx.yin_module_map = {}
        if m is not None:
            (name, _dummy, rev, _) = m.groups()
            name = os.path.basename(name)
            logger.debug(
                'Parsing file %s format %s name %s revision %s', filename, format, name, rev)
            module = ctx.add_module(filename, text, format, name, rev,
                                    expect_failure_error=False)
        else:
            module = ctx.add_module(filename, text)
        if module is None:
            raise YdkGenException('Could not add module ')
        else:
            modules.append(module)

    # all the module have been added so get the context to validate
    # call prevalidate before this and post validate after
    ctx.validate()

    def keyfun(e):
        if e[0].ref == filenames[0]:
            return 0
        else:
            return 1

    ctx.errors.sort(key=lambda e: (e[0].ref, e[0].line))
    if len(filenames) > 0:
        # first print error for the first filename given
        ctx.errors.sort(key=keyfun)

    error_messages = []
    for (epos, etag, eargs) in ctx.errors:

        elevel = error.err_level(etag)
        if error.is_warning(elevel):
            logger.warning('%s: %s\n' %
                           (str(epos), error.err_to_str(etag, eargs)))
        else:
            err_msg = '%s: %s\n' % (str(epos), error.err_to_str(etag, eargs))
            logger.error(err_msg)
            error_messages.append(err_msg)

    if len(error_messages) > 0:
        err_msg = '\n'.join(error_messages)
        raise YdkGenException(err_msg)

    return [m for m in modules if m.keyword == 'module' or m.keyword == 'submodule']


def generate(profile_file, output_directory, nodoc, ydk_root, groupings_as_class, generate_python):
    """
        Generate ydk-py based in the output_directory using the supplied 
        profile_file

        :param str profile_file: The profile file to use
        :param str output_directory: The output directory where the generated ydk python code will be created.
        :param bool nodoc If set to true the documentation is not generated.
        :param str ydk_root: The ydk root directory. Relative file names in the profile file are resolved relative to this.
        :param bool groupings_as_class: If set to true YANG grouping is converted to a class.
        :raise YdkGenException: if an error has occurred
    """

    resolved_model_dir = None

    if profile_file is None or len(profile_file) == 0:
        logger.error('profile_file is None.')
        raise YdkGenException('profile_file cannot be None.')

    if output_directory is None or len(output_directory) == 0:
        logger.error('output_directory is None.')
        raise YdkGenException('output_directory cannot be None.')

    if ydk_root is None or len(ydk_root) == 0:
        logger.error('ydk_root is None.')
        raise YdkGenException('YDKGEN_HOME is not set.')

    try:
        with open(profile_file) as json_file:
            profile_data = json.load(json_file)
            version = profile_data['version']
            resolved_model_dir = resolve_profile.resolve_profile(
                profile_data, ydk_root)
    except IOError as e:
        err_msg = 'Cannot open profile file (%s)' % e.strerror
        logger.error(err_msg)
        raise YdkGenException(err_msg)
    except ValueError as e:
        err_msg = 'Cannot parse profile file (%s)' % e.message
        logger.error(err_msg)
        raise YdkGenException(err_msg)

    modules = _parse_and_return_modules(resolved_model_dir)

    py_sdk_root = output_directory + '/python/'
    py_api_doc_gen = py_sdk_root + '/docsgen'
    py_api_doc = output_directory + '/python/docs_expanded'

    if os.path.isdir(py_sdk_root):
        shutil.rmtree(py_sdk_root)
    if os.path.isdir(py_api_doc):
        shutil.rmtree(py_api_doc)
    if os.path.isfile(py_api_doc_gen + '/getting_started.rst'):
        os.remove(py_api_doc_gen + '/getting_started.rst')

    shutil.copytree(ydk_root + '/sdk/python', output_directory + '/python',
                    ignore=shutil.ignore_patterns('.gitignore', 'ncclient'))


    # no errors found now we can begin the transformation

    # begin generation
    ydk_dir = '%sydk' % py_sdk_root
    ydk_doc_dir = '%sdocsgen' % py_sdk_root

    # print version file
    version_file = os.path.join(ydk_dir, '_version.py')
    with open(version_file, 'w') as version_fd:
        version_fd.write('__version__ = "{}"'.format(version))

    #create the packages
    packages = []
    
    if not groupings_as_class:
        (packages, subm) = ApiModelBuilder().generate(modules)
    else:
        packages = GroupingClassApiModelBuilder().generate(
                modules)
    
    # call the language emitter
    language = ''
    if generate_python:
        language = 'python'
    printer = YdkPrinter(ydk_dir, ydk_doc_dir, language)
    printer.emit(packages, subm)
