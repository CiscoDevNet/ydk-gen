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
import json
import sys
import tempfile
import shutil
from argparse import ArgumentParser
from os import listdir
from os import walk
from ..common import YdkGenException


from git import Repo

logger = logging.getLogger('ydkgen')


def resolve_dir(p, models_dir, ydk_root=None):
    '''Resolve a list of directories. 

    For a directory we traverse the
    entire hierarchy (may add a depth otion later). Only .yang files
    are picked up and copied into the models directory. The directory
    names are currently absolute only.

    :param list p
    :param str models_dir
    :param str ydk_root The root directory which is used to resolve relative file paths.

    '''

    logger.debug('Resolving a dir tag with params: ' +
                 json.dumps(p, indent=2, sort_keys=True))
    filelist = []
    for d in p:
        d = d if not ydk_root else ydk_root + d
        for (dirpath, dirnames, filenames) in walk(d):
            filelist.extend(
                [dirpath + '/' + f for f in filenames if f.endswith('.yang')])
    for f in filelist:
        logger.debug('Copying {} to {}'.format(f, models_dir))
        shutil.copy(f, models_dir)


def resolve_file(p, models_dir, ydk_root=None):
    ''' Resolve the file.

    The simplest resolver. Just copy the list of files passed in. Files
    are currently absolute paths. This probably won't be used all that
    often.

    :param list p
    :param str models_dir
    :param str ydk_root The root directory which is used to resolve relative file paths.

    '''
    logger.debug('Resolving a file tag with params: ' +
                 json.dumps(p, indent=2, sort_keys=True))
    # iterate over the files given
    for f in p:
        logger.debug('Copying {} to {}'.format(f, models_dir))
        shutil.copy(f, models_dir)


def resolve_git(p, models_dir, ydk_root=None):
    """ Resolves the git structure

       -- git
          -- commit*
              -- commitid
              -- dir
              -- file

        :param p
        :param str models_dir
        :param ydk_root The root directory which is used to resolve relative file paths.
    """
    logger.debug('Resolving a git tag with params: ' +
                 json.dumps(p, indent=2, sort_keys=True))
    for g in p:
        tmp_dir = tempfile.mkdtemp(suffix='.yang')
        repo_url = g['url']
        repo_path = tmp_dir + '/foo'
        logger.debug('Cloning from ' + repo_url + ' to ' + repo_path)
        repo = Repo.clone_from(repo_url, repo_path)
        for commit in g['commits']:
            repo.git.checkout(commit['commitid'])
            if 'dir' in commit:
                dir_list = [repo_path + '/' + d for d in commit['dir']]
                logger.debug('Created expanded directory list:')
                for d in dir_list:
                    logger.debug(d)
                resolve_dir(dir_list, models_dir)

            if 'file' in commit:
                file_list = [repo_path + '/' + f for f in commit['file']]
                logger.debug('Created expanded file list:')
                for f in file_list:
                    logger.debug(f)
                resolve_file(file_list, models_dir)
        shutil.rmtree(tmp_dir)


def resolve_profile(p, ydk_root=None):
    '''Resolve a profile data object. 

    Goal is to pull together all the
    models specified to a temporary directory that will then be
    returned to the caller.

    :param p json profile data object
    :param ydk_root The root directory which is used to resolve relative file paths.
    :raise YdkGenException when an error occurs trying to resolve the profile.
    '''
    # check for basic validity
    if 'models' not in p:
        return None
    
    # we think we have models, so let's go...firt order of business is
    # to create a temporary directory name to store all the models in
    # as we will be pulling them all together for a brief period of
    # time.
    tmp_dir = tempfile.mkdtemp(suffix='.models')

    # Now let's get down to business...
    accepted_sources = ['file', 'dir', 'git']
    for source in accepted_sources:
        if source in p['models']:
            # invoke the named resolver
            try:
                globals()[
                    'resolve_' + source](p['models'][source], tmp_dir, ydk_root)
            except shutil.Error as e:
                err_msg = '%s' % e.message
                logger.error(err_msg)
                shutil.rmtree(tmp_dir)
                raise YdkGenException(err_msg)
            except Exception as inst:
                err_msg = 'Got exception of type %s and args %s' % \
                             (str(type(inst)), inst.args)
                logger.error(err_msg)
                shutil.rmtree(tmp_dir)
                raise YdkGenException(err_msg)

    # if we get here, we assume that we just return the tmp_dir as a
    # sign of success!
    return tmp_dir


# logger file is not set up when import from resolve_profile directly
if __name__ == '__main__':

    parser = ArgumentParser(description='Profile resolver testing:')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help="Do I really need to explain?")
    parser.add_argument('-p', '--profile', type=str, required=True,
                        help="The profile file to test")
    args = parser.parse_args()

    #
    # We will be enabling logging to the debug level
    #
    logging.basicConfig(format='%(asctime)s:%(levelname)s:%(message)s')
    if args.verbose:
        for l in ['ydk.generate']:
            logger = logging.getLogger(l)
            logger.setLevel(logging.DEBUG)

    # simple testing
    profile_data = None
    try:
        with open(args.profile) as json_file:
            profile_data = json.load(json_file)
    except IOError as e:
        err_msg = 'Cannot open profile file (%s)' % e.strerror
        logger.error(err_msg)
        raise YdkGenException(err_msg)
        sys.exit(1)
    except ValueError as e:
        err_msg = 'Cannot parse profile file (%s)' % e.message
        logger.error(err_msg)
        raise YdkGenException(err_msg)
        sys.exit(1)

    model_dir = resolve_profile(profile_data)
    if not model_dir:
        err_msg = 'No resolution!'
        logger.error(err_msg)
        raise YdkGenException(err_msg)
    else:
        print('Resolved models to ' + model_dir)
        print('Models resolved:')
        for f in listdir(model_dir):
            print('  ' + f)

        # now tidy up the temp dir
        shutil.rmtree(model_dir)
