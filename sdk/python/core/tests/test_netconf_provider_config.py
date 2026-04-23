#  ----------------------------------------------------------------
# Copyright 2016-2019 Cisco Systems
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

"""test_netconf_provider_config.py

Regression test: confirm that after wiring ``validate_endpoint`` into
the ``NetconfServiceProvider`` / ``NetconfSession`` wrappers, the
arguments forwarded to the underlying C++ base-class constructor are
**byte-for-byte identical** to the pre-change behaviour for valid
inputs.

The compiled ``ydk_`` extension is replaced with stub base classes
that simply record every positional-argument tuple passed to their
``__init__``. This lets the test run on any machine with no C++ build
and no NETCONF server.
"""

from __future__ import absolute_import

import os
import sys
import types
import unittest
from unittest.mock import MagicMock

# ---------------------------------------------------------------------
# 1. Make the ydk source tree importable.
# ---------------------------------------------------------------------
_HERE = os.path.dirname(os.path.abspath(__file__))
_CORE = os.path.dirname(_HERE)  # .../sdk/python/core
if _CORE not in sys.path:
    sys.path.insert(0, _CORE)

# ---------------------------------------------------------------------
# 2. Capture list for args passed to mock base __init__.
#    Each entry is (class_name, args_tuple).
# ---------------------------------------------------------------------
_CAPTURED = []


class _RecordingProviderBase(object):
    """Stand-in for the pybind11 ``_NetconfServiceProvider``."""
    def __init__(self, *args, **kwargs):
        # The real wrappers never pass kwargs to super().__init__,
        # but record them anyway so a surprise kwarg fails loudly.
        _CAPTURED.append(("NetconfServiceProvider", args, kwargs))


class _RecordingSessionBase(object):
    """Stand-in for the pybind11 ``_NetconfSession``."""
    def __init__(self, *args, **kwargs):
        _CAPTURED.append(("NetconfSession", args, kwargs))


# ---------------------------------------------------------------------
# 3. Inject stub modules *before* importing the wrappers.
#
#    (a) `ydk_` (the compiled extension) and its submodules are
#        replaced so `from ydk_.providers import NetconfServiceProvider`
#        and `from ydk_.path import NetconfSession` yield our recording
#        base classes.
#
#    (b) `ydk.providers`, `ydk.path`, `ydk.path.sessions` are replaced
#        with lightweight package stubs whose __path__ points at the
#        real source directory. This BYPASSES their heavy __init__.py
#        chains (which pull in codec_provider -> ydk.types ->
#        ydk.ext.types -> compiled symbols) while still letting Python
#        resolve submodule imports like `ydk.providers._endpoint` and
#        `ydk.providers.netconf_provider` directly from disk.
# ---------------------------------------------------------------------
_ydk_underscore = MagicMock(name="ydk_")
_ydk_underscore_providers = types.ModuleType("ydk_.providers")
_ydk_underscore_providers.NetconfServiceProvider = _RecordingProviderBase
_ydk_underscore_path = types.ModuleType("ydk_.path")
_ydk_underscore_path.NetconfSession = _RecordingSessionBase

sys.modules["ydk_"] = _ydk_underscore
sys.modules["ydk_.providers"] = _ydk_underscore_providers
sys.modules["ydk_.path"] = _ydk_underscore_path


def _pkg_stub(name, *path_parts):
    """Insert a minimal package stub into sys.modules.

    The stub has __path__ so Python can load submodules from disk
    without executing the real __init__.py.
    """
    m = types.ModuleType(name)
    m.__path__ = [os.path.join(_CORE, *path_parts)]
    sys.modules[name] = m
    return m


_pkg_stub("ydk.providers", "ydk", "providers")
_pkg_stub("ydk.path", "ydk", "path")
_pkg_stub("ydk.path.sessions", "ydk", "path", "sessions")

# ---------------------------------------------------------------------
# 4. Now import the wrappers; they subclass our recording bases.
# ---------------------------------------------------------------------
from ydk.providers.netconf_provider import NetconfServiceProvider  # noqa: E402
from ydk.path.sessions.netconf_session import NetconfSession       # noqa: E402
from ydk.errors import YInvalidArgumentError                        # noqa: E402


# ---------------------------------------------------------------------
# 5. Representative scenario (mirrors the test harness default:
#    ssh://admin:admin@127.0.0.1:12022 parsed by urllib).
# ---------------------------------------------------------------------
ADDRESS  = "127.0.0.1"
USERNAME = "admin"
PASSWORD = "admin"
PORT     = 12022
PROTOCOL = "ssh"
# Defaults, made explicit so the expected tuples below are obvious:
ON_DEMAND    = True
COMMON_CACHE = False
TIMEOUT      = -1    # wrapper normalises None -> -1
PRIV_KEY     = "/tmp/id_rsa"
PUB_KEY      = "/tmp/id_rsa.pub"
_REPO_SENTINEL = object()   # any non-None object triggers repo branch


class NetconfProviderConfigRegressionTest(unittest.TestCase):
    """Assert the args forwarded to ``super().__init__`` are unchanged.

    "Unchanged" means: for valid input, the positional tuple the
    wrapper passes to its C++ base class's ``__init__`` is the same
    tuple the pre-patch code would have passed. The pre-patch code
    did ``if port is None: port = 830`` and otherwise forwarded raw
    values; ``validate_endpoint`` does the same for valid inputs and
    only adds rejection of *invalid* inputs.
    """

    def setUp(self):
        _CAPTURED[:] = []

    def _last(self, expect_class):
        self.assertEqual(len(_CAPTURED), 1,
                         "expected exactly one base.__init__ call")
        cls, args, kwargs = _CAPTURED[0]
        self.assertEqual(cls, expect_class)
        self.assertEqual(kwargs, {})
        return args

    # --- NetconfServiceProvider ------------------------------------

    def test_provider_branch1_no_repo_password(self):
        """repo=None, password auth -> the representative scenario."""
        NetconfServiceProvider(
            ADDRESS, USERNAME, PASSWORD, PORT, PROTOCOL,
            ON_DEMAND, COMMON_CACHE, timeout=None)
        # Pre-patch code path (netconf_provider.py original lines 53-54):
        #   self.nsp.__init__(address, username, password, port,
        #                     protocol, on_demand, common_cache, timeout)
        self.assertEqual(
            self._last("NetconfServiceProvider"),
            (ADDRESS, USERNAME, PASSWORD, PORT,
             PROTOCOL, ON_DEMAND, COMMON_CACHE, TIMEOUT))

    def test_provider_branch2_no_repo_key_auth(self):
        NetconfServiceProvider(
            ADDRESS, USERNAME,
            private_key_path=PRIV_KEY, public_key_path=PUB_KEY,
            port=PORT)
        # Pre-patch (original lines 56-58):
        #   self.nsp.__init__(address, username,
        #                     private_key_path, public_key_path,
        #                     port, on_demand, common_cache, timeout)
        self.assertEqual(
            self._last("NetconfServiceProvider"),
            (ADDRESS, USERNAME, PRIV_KEY, PUB_KEY,
             PORT, ON_DEMAND, COMMON_CACHE, TIMEOUT))

    def test_provider_branch3_repo_password(self):
        NetconfServiceProvider(
            ADDRESS, USERNAME, PASSWORD, PORT, PROTOCOL,
            repo=_REPO_SENTINEL)
        # Pre-patch (original lines 61-62):
        #   self.nsp.__init__(repo, address, username, password,
        #                     port, protocol, on_demand, timeout)
        self.assertEqual(
            self._last("NetconfServiceProvider"),
            (_REPO_SENTINEL, ADDRESS, USERNAME, PASSWORD,
             PORT, PROTOCOL, ON_DEMAND, TIMEOUT))

    def test_provider_branch4_repo_key_auth(self):
        NetconfServiceProvider(
            ADDRESS, USERNAME,
            private_key_path=PRIV_KEY, public_key_path=PUB_KEY,
            port=PORT, repo=_REPO_SENTINEL)
        # Pre-patch (original lines 64-66):
        #   self.nsp.__init__(repo, address, username,
        #                     private_key_path, public_key_path,
        #                     port, on_demand, timeout)
        self.assertEqual(
            self._last("NetconfServiceProvider"),
            (_REPO_SENTINEL, ADDRESS, USERNAME,
             PRIV_KEY, PUB_KEY, PORT, ON_DEMAND, TIMEOUT))

    def test_provider_port_none_defaults_to_830(self):
        """Behavioural parity: port=None -> 830 (was a plain `if`)."""
        NetconfServiceProvider(ADDRESS, USERNAME, PASSWORD, port=None)
        args = self._last("NetconfServiceProvider")
        self.assertEqual(args[3], 830)

    def test_provider_invalid_address_raises_before_base_init(self):
        """New behaviour: validation error fires before C++ layer."""
        with self.assertRaises(YInvalidArgumentError) as cm:
            NetconfServiceProvider("", USERNAME, PASSWORD, PORT)
        self.assertIn("host is empty", str(cm.exception))
        self.assertEqual(_CAPTURED, [],
                         "base __init__ must not be reached")

    # --- NetconfSession --------------------------------------------

    def test_session_branch1_no_repo_password(self):
        NetconfSession(
            ADDRESS, USERNAME, PASSWORD, PORT, PROTOCOL,
            ON_DEMAND, COMMON_CACHE, timeout=None)
        # Pre-patch (netconf_session.py original lines 52-54):
        #   self.ns.__init__(address, username, password,
        #                    port, protocol, on_demand,
        #                    common_cache, timeout)
        self.assertEqual(
            self._last("NetconfSession"),
            (ADDRESS, USERNAME, PASSWORD, PORT,
             PROTOCOL, ON_DEMAND, COMMON_CACHE, TIMEOUT))

    def test_session_branch2_no_repo_key_auth(self):
        NetconfSession(
            ADDRESS, USERNAME,
            private_key_path=PRIV_KEY, public_key_path=PUB_KEY,
            port=PORT)
        # Pre-patch (original lines 56-59):
        #   self.ns.__init__(address, username,
        #                    private_key_path, public_key_path,
        #                    port, protocol, on_demand,
        #                    common_cache, timeout)
        self.assertEqual(
            self._last("NetconfSession"),
            (ADDRESS, USERNAME, PRIV_KEY, PUB_KEY,
             PORT, PROTOCOL, ON_DEMAND, COMMON_CACHE, TIMEOUT))

    def test_session_branch3_repo_password(self):
        NetconfSession(
            ADDRESS, USERNAME, PASSWORD, PORT, PROTOCOL,
            repo=_REPO_SENTINEL)
        # Pre-patch (original lines 62-64):
        #   self.ns.__init__(repo, address, username, password,
        #                    port, protocol,
        #                    on_demand, timeout)
        self.assertEqual(
            self._last("NetconfSession"),
            (_REPO_SENTINEL, ADDRESS, USERNAME, PASSWORD,
             PORT, PROTOCOL, ON_DEMAND, TIMEOUT))

    def test_session_branch4_repo_key_auth(self):
        NetconfSession(
            ADDRESS, USERNAME,
            private_key_path=PRIV_KEY, public_key_path=PUB_KEY,
            port=PORT, repo=_REPO_SENTINEL)
        # Pre-patch (original lines 66-69):
        #   self.ns.__init__(repo, address, username,
        #                    private_key_path, public_key_path,
        #                    port, protocol, on_demand,
        #                    common_cache, timeout)
        self.assertEqual(
            self._last("NetconfSession"),
            (_REPO_SENTINEL, ADDRESS, USERNAME,
             PRIV_KEY, PUB_KEY, PORT, PROTOCOL,
             ON_DEMAND, COMMON_CACHE, TIMEOUT))


if __name__ == '__main__':
    unittest.main(verbosity=2)
