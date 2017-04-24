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
import pkgutil
import importlib

from ydk import models
from ydk.types import EncodingFormat
from ydk.errors import YPYServiceProviderError
from ydk.path import Capability as _Capability
from ydk.path import Repository as _Repository


_TRACE_LEVEL_NUM = 5
_USER_PROVIDED_REPO = "ydk-user-provider-repo"


class CodecServiceProvider(object):
    """Python CodecServiceProvider wrapper.

    Args:
        type (ydk.types.EncodingFormat or str): Codec encoding format.
            Currently support XML or JSON.
        repo (ydk.path.Repository, optional): A user provided repository.
    Attributes:
        logger (logging.Logger): CodecServiceProvider logger.
        encoding (ydk.types.EncodingFormat): Codec encoding format.
        _root_schema_table (dict(str, ydk.path.RootSchemaNode)): A dictionary
            of root schemas, key is bundle name or _USER_PROVIDED_REPO, value
            is corresponding repository.
    """

    def __init__(self, **kwargs):
        self.logger = logging.getLogger(__name__)
        self._root_schema_table = {}


        repo = kwargs.get('repo', None)
        if repo is None:
            self._user_provided_repo = False
            self._repo = None
        else:
            self._user_provided_repo = True
            self._repo = repo

        encoding = kwargs.get('type', EncodingFormat.XML)
        if isinstance(encoding, str) and encoding == 'xml':
            encoding = EncodingFormat.XML
        elif isinstance(encoding, str) and encoding == 'json':
            encoding = EncodingFormat.JSON
        encoding = kwargs.get('encoding', encoding)
        self.encoding = encoding

    def initialize(self, bundle_name, models_path):
        """Initialize provider with bundle name and path for local YANG models.

        Args:
            bundle_name (str): bundle name.
            models_path (str): location for local YANG models.
        """
        if self._user_provided_repo:
            self._initialize_root_schema(bundle_name, self._repo, True)

        if bundle_name in self._root_schema_table:
            return

        self.logger.log(_TRACE_LEVEL_NUM, "Creating repo in path {}".format(models_path))
        repo = _Repository(models_path)
        self._initialize_root_schema(bundle_name, repo)

    def get_root_schema(self, bundle_name):
        """Return root_schema for bundle_name.

        Args:
            bundle_name (str): bundle name.
        """
        if self._user_provided_repo:
            return self._root_schema_table[_USER_PROVIDED_REPO]

        if bundle_name not in self._root_schema_table:
            self.logger.error("Root schema not created")
            raise YPYServiceProviderError(error_msg="Root schema not created")

        return self._root_schema_table[bundle_name]

    def _initialize_root_schema(self, bundle_name, repo, user_provided_repo=False):
        """Update root schema table entry.

        Args:
            name (str): bundle name.
            repo (ydk.path.Repository): default repository or repository provided by the user.
            user_provided_repo (bool, optional): Defaults to False.

        """
        name = bundle_name if not user_provided_repo else _USER_PROVIDED_REPO
        self.logger.log(_TRACE_LEVEL_NUM, "Initializing root schema for {}".format(name))
        # TODO: turn on and off libyang logging
        capabilities = self._get_bundle_capabilities(bundle_name)
        self._root_schema_table[name] = repo.create_root_schema(capabilities)

    def _get_bundle_capabilities(self, bundle_name):
        """Search installed local ydk-models python packages, and return corresponding
        capabilities.

        Args:
            bundle_name (str): bundle name.

        Returns:
            capabilities (list): List of ydk.path.Capability available for this bundle.
        """
        capabilities = []
        capability_map = {}
        for (_, name, ispkg) in pkgutil.iter_modules(models.__path__):
            if ispkg and name == bundle_name:
                try:
                    mod_yang_ns = importlib.import_module('ydk.models.{}._yang_ns'.format(name))
                    capability_map = mod_yang_ns.__dict__['CAPABILITIES']
                except ImportError:
                    continue

        for name in capability_map:
            capabilities.append(_Capability(name, capability_map[name]))

        return capabilities