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

import logging
import os
import pyang
import re
import sys

from pyang import error, statements
from pyang.error import err_add
from ydkgen.common import YdkGenException


logger = logging.getLogger('ydkgen')
logger.addHandler(logging.NullHandler())


class PyangModelBuilder(object):
    def __init__(self, resolved_model_dir):
        self.repos = pyang.FileRepository(resolved_model_dir, False)
        self.ctx = pyang.Context(self.repos)
        self.resolved_model_dir = resolved_model_dir
        self.submodules = []
        try:
            reload(sys)
            sys.setdefaultencoding('utf8')
        except:
            pass

    def parse_and_return_modules(self):
        """ Use pyang to parse the files, validate them and get a list of modules.

            :param str resolved_model_dir The directory where all models to be compiled are found.
            :raise YdkGenException If there was a problem parsing the modules
        """
        statements.add_validation_fun('reference_3', ['deviation'], self._add_i_deviation)
        statements.add_validation_fun('reference_3', ['deviation'], self._add_d_info)
        statements.add_validation_fun('reference_3', ['deviate'], self._remove_d_info)

        # set marker for models being augmented
        statements.add_validation_fun('expand_2', ['augment'], self._set_i_aug)

        filenames = self._get_yang_file_names()
        modules = self._get_pyang_modules(filenames)
        self._validate_pyang_modules(filenames)

        self.submodules = [m for m in modules if  m.keyword == 'submodule']
        return [m for m in modules if m.keyword == 'module']

    def get_submodules(self):
        return self.submodules

    def _copy_substmts_ichildren(self, stmt):
        """ Return copy of current statement's substmts and i_children """
        chs = set(stmt.i_children) if hasattr(stmt, 'i_children') else set()
        non_chs = set()
        for s in stmt.substmts:
            if s not in chs:
                non_chs.add(s)
        chs = list(chs)
        non_chs = list(non_chs)
        return chs + non_chs, chs

    def _set_d_substmts_ichildren(self, stmt):
        """ Set d_substmts and d_children for further recovery """
        if not hasattr(stmt, 'd_children') and not hasattr(stmt, 'd_substmts'):
            d_substmts, d_children = self._copy_substmts_ichildren(stmt)
            stmt.d_children = d_children
            stmt.d_substmts = d_substmts

    def _add_d_info(self, ctx, stmt):
        """ Copy the i_children and substmts attribute for target statement or
         target statement's parent. """
        # stmt.keyword == 'deviation'
        t = stmt.i_target_node
        if t is None:
            return
        for d in stmt.search('deviate'):
            if d.arg == 'not-supported':
                t = t.parent
            self._set_d_substmts_ichildren(t)

    def _remove_d_info(self, ctx, stmt):
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

    def _add_deviation(self, target, dev_type, dev_module, dev_stmt):
        """ Add deviation information to target statement """
        if not hasattr(target, 'i_deviation'):
            target.i_deviation = {}
        if dev_type not in target.i_deviation:
            target.i_deviation[dev_type] = []
        target.i_deviation[dev_type].append((dev_module, dev_stmt))

    def _add_deviation_r(self, target, dev_type, dev_module, dev_stmt):
        """ Add deviation information to target node recursively """
        self._add_deviation(target, dev_type, dev_module, dev_stmt)
        sub = target.substmts
        if hasattr(target, 'i_children'):
            sub = sub + target.i_children
        for d in sub:
            self._add_deviation(d, dev_type, dev_module, dev_stmt)

    def _add_i_deviation(self, ctx, stmt):
        if not hasattr(stmt.i_module, 'is_deviation_module'):
            stmt.i_module.is_deviation_module = True
        t = stmt.i_target_node
        if t is None:
            return
        stmt = stmt.search_one('deviate')

        if stmt.arg == 'not-supported':
            if ((t.parent.keyword == 'list') and
                (t in t.parent.i_key)):
                err_add(self.ctx.errors, stmt.pos, 'BAD_DEVIATE_KEY',
                        (t.i_module.arg, t.arg))
                return
            if not hasattr(t.parent, 'i_not_supported'):
                t.parent.i_not_supported = []
            t.parent.i_not_supported.append(t)
            self._add_deviation(t, 'not_supported', stmt.i_module, stmt)
        elif stmt.arg == 'add':
            for c in stmt.substmts:
                if c.keyword in statements._singleton_keywords:
                    if t.search_one(c.keyword) != None:
                        err_add(self.ctx.errors, c.pos, 'BAD_DEVIATE_ADD',
                                (c.keyword, t.i_module.arg, t.arg))
                    elif t.keyword not in statements._valid_deviations[c.keyword]:
                        err_add(self.ctx.errors, c.pos, 'BAD_DEVIATE_TYPE',
                                c.keyword)
                    else:
                        self._add_deviation(t, 'add', stmt.i_module, c)
                else:
                    if t.keyword not in statements._valid_deviations[c.keyword]:
                        err_add(self.ctx.errors, c.pos, 'BAD_DEVIATE_TYPE',
                                c.keyword)
                    else:
                        self._add_deviation(t, 'add', stmt.i_module, c)
        else:  # delete or replace
            for c in stmt.substmts:
                if (c.keyword == 'config'
                    and stmt.arg == 'replace'
                    and hasattr(t, 'i_config')):
                    self._add_deviation_r(t, 'replace', stmt.i_module, c)
                if c.keyword in statements._singleton_keywords:
                    old = t.search_one(c.keyword)
                else:
                    old = t.search_one(c.keyword, c.arg)
                if old is None:
                    err_add(self.ctx.errors, c.pos, 'BAD_DEVIATE_DEL',
                            (c.keyword, t.i_module.arg, t.arg))
                else:
                    self._add_deviation(t, stmt.arg, stmt.i_module, c)

    def _set_i_aug(self, ctx, stmt):
        """ inject bool 'i_augment' to top statement for model being augmented"""
        i_target_node = None
        if hasattr(stmt, 'i_target_node'):
            i_target_node = stmt.i_target_node
        else:
            i_target_node = statements.find_target_node(ctx, stmt, is_augment=True)
        if i_target_node is not None:
            if hasattr(stmt.top , 'i_aug_targets'):
                stmt.top.i_aug_targets.add(i_target_node.top)
            else:
                stmt.top.i_aug_targets = set([i_target_node.top])
            i_target_node.top.is_augmented_module = True

    def _get_yang_file_names(self):
        filenames = []

        # (name, rev, handle)
        # where handle is (format, absfilename)
        for (_, _, (_, filename)) in self.repos.get_modules_and_revisions(self.ctx):
            filenames.append(filename)
        return filenames

    def _get_pyang_modules(self, filenames):
        modules = []
        regex_expression = re.compile(r"^(.*?)(\@(\d{4}-\d{2}-\d{2}))?\.(yang|yin)$")
        for filename in filenames:
            base_file_name = filename
            if filename.startswith('file://'):
                base_file_name = filename[len('file://') - 1:]
            try:
                fd = open(base_file_name)
                text = fd.read()
            except IOError as ex:
                err_msg = "error %s: %s\n" % (filename, str(ex))
                logger.error(err_msg)
                raise YdkGenException(err_msg)

            match = regex_expression.search(filename)
            if match is not None:
                (name, _dummy, rev, _) = match.groups()
                name = os.path.basename(name)
                logger.debug(
                    'Parsing file %s. Module name: %s. Revision: %s', filename, name, rev)
                module = self.ctx.add_module(filename, text, format, name, rev,
                                        expect_failure_error=False)
            else:
                module = self.ctx.add_module(filename, text)
            if module is None:
                raise YdkGenException('Could not add module "%s", (%s). Please remove any duplicate files and verify that all the models pass pyang. Run "pyang *" on all the models.'%(name, filename))
            else:
                modules.append(module)
        return modules

    def _validate_pyang_modules(self, filenames):
        # all the module have been added so get the context to validate
        # call prevalidate before this and post validate after
        self.ctx.validate()

        def keyfun(e):
            if e[0].ref == filenames[0]:
                return 0
            else:
                return 1

        self.ctx.errors.sort(key=lambda e: (e[0].ref, e[0].line))
        if len(filenames) > 0:
            # first print error for the first filename given
            self.ctx.errors.sort(key=keyfun)

        error_messages = []
        for (epos, etag, eargs) in self.ctx.errors:
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
            raise YdkGenException('Error occured: "%s". Verify that all the models pass pyang compilation. Run "pyang *" on all the models.'%err_msg)

