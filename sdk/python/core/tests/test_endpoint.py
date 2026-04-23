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

"""test_endpoint.py

Standalone unit tests for ``ydk.providers._endpoint``.

These tests deliberately avoid importing ``ydk.providers`` as a
package (which would trigger import of the compiled ``ydk_`` extension
via ``netconf_provider.py``). Instead, the ``_endpoint`` module is
loaded directly from its file path so the tests run on any machine
with a Python interpreter, with no C++ build or NETCONF server.
"""

from __future__ import absolute_import

import os
import sys
import unittest
import importlib.util

# Make the ydk source tree importable so `from ydk.errors import ...`
# inside _endpoint.py resolves against the source (not an installed
# package).
_HERE = os.path.dirname(os.path.abspath(__file__))
_CORE = os.path.dirname(_HERE)  # .../sdk/python/core
if _CORE not in sys.path:
    sys.path.insert(0, _CORE)

from ydk.errors import YInvalidArgumentError  # noqa: E402

# Load _endpoint.py directly, bypassing ydk/providers/__init__.py
# (which would pull in the ydk_ compiled extension).
_ENDPOINT_PATH = os.path.join(
    _CORE, "ydk", "providers", "_endpoint.py")
_spec = importlib.util.spec_from_file_location(
    "ydk_providers_endpoint_under_test", _ENDPOINT_PATH)
_endpoint = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_endpoint)

parse_endpoint = _endpoint.parse_endpoint
validate_endpoint = _endpoint.validate_endpoint


class TestParseEndpointValid(unittest.TestCase):
    """Happy-path inputs to ``parse_endpoint``."""

    def test_bare_hostname(self):
        self.assertEqual(
            parse_endpoint("router.example.com"),
            (None, "router.example.com", None))

    def test_bare_ipv4(self):
        self.assertEqual(
            parse_endpoint("192.0.2.1"),
            (None, "192.0.2.1", None))

    def test_missing_scheme_host_port(self):
        # Explicitly: no scheme, host:port form
        self.assertEqual(
            parse_endpoint("host.example.com:830"),
            (None, "host.example.com", 830))

    def test_scheme_host(self):
        self.assertEqual(
            parse_endpoint("ssh://router1"),
            ("ssh", "router1", None))

    def test_scheme_host_port(self):
        self.assertEqual(
            parse_endpoint("ssh://10.0.0.1:22"),
            ("ssh", "10.0.0.1", 22))

    def test_scheme_case_insensitive(self):
        self.assertEqual(
            parse_endpoint("SSH://host:830"),
            ("ssh", "host", 830))

    def test_tcp_scheme(self):
        self.assertEqual(
            parse_endpoint("tcp://host:4334"),
            ("tcp", "host", 4334))

    def test_ipv6_bracketed_no_port(self):
        self.assertEqual(
            parse_endpoint("[2001:db8::1]"),
            (None, "2001:db8::1", None))

    def test_ipv6_bracketed_with_port(self):
        # Explicit user-requested case: IPv6 bracket + port
        self.assertEqual(
            parse_endpoint("[2001:db8::1]:830"),
            (None, "2001:db8::1", 830))

    def test_ipv6_scheme_bracketed_port(self):
        self.assertEqual(
            parse_endpoint("ssh://[::1]:12022"),
            ("ssh", "::1", 12022))

    def test_ipv6_bare_no_brackets(self):
        # Bare IPv6 with >=2 colons is treated as host-only (no port).
        self.assertEqual(
            parse_endpoint("2001:db8::1"),
            (None, "2001:db8::1", None))

    def test_leading_trailing_whitespace_stripped(self):
        self.assertEqual(
            parse_endpoint("  ssh://host:830  "),
            ("ssh", "host", 830))

    def test_fqdn_trailing_dot(self):
        self.assertEqual(
            parse_endpoint("router.example.com."),
            (None, "router.example.com", None))

    def test_underscore_in_hostname_label(self):
        # Pragmatic allowance for lab/vendor hostnames.
        self.assertEqual(
            parse_endpoint("lab_router_1"),
            (None, "lab_router_1", None))

    def test_port_boundaries(self):
        self.assertEqual(parse_endpoint("h:1"), (None, "h", 1))
        self.assertEqual(parse_endpoint("h:65535"), (None, "h", 65535))


class TestParseEndpointInvalid(unittest.TestCase):
    """Malformed inputs must raise ``YInvalidArgumentError``
    with an actionable message."""

    def _assert_rejects(self, value, msg_substr):
        with self.assertRaises(YInvalidArgumentError) as cm:
            parse_endpoint(value)
        self.assertIn(msg_substr, str(cm.exception))

    def test_empty_string(self):
        self._assert_rejects("", "endpoint is empty")

    def test_whitespace_only(self):
        # Explicit user-requested case: whitespace host
        self._assert_rejects("   ", "endpoint is empty")

    def test_non_string(self):
        self._assert_rejects(None, "expected str")
        self._assert_rejects(12345, "expected str")

    def test_unsupported_scheme(self):
        # Explicit user-requested case
        self._assert_rejects("http://host:80", "unsupported scheme")
        self._assert_rejects("telnet://host", "unsupported scheme")

    def test_scheme_no_host(self):
        self._assert_rejects("ssh://", "no host component")

    def test_userinfo_rejected(self):
        # Explicit user-requested case: userinfo in URL
        self._assert_rejects(
            "ssh://admin:secret@host:830", "userinfo")
        self._assert_rejects(
            "user@host", "userinfo")

    def test_port_zero(self):
        # Explicit user-requested case: invalid port 0
        self._assert_rejects("host:0", "out of range")

    def test_port_65536(self):
        # Explicit user-requested case: invalid port 65536
        self._assert_rejects("host:65536", "out of range")

    def test_port_non_numeric(self):
        # Explicit user-requested case
        self._assert_rejects("host:abc", "not a positive integer")
        self._assert_rejects("host:8 30", "not a positive integer")
        self._assert_rejects("host:-1", "not a positive integer")

    def test_trailing_colon_no_port(self):
        self._assert_rejects("host:", "trailing ':' but no port")

    def test_ipv6_unclosed_bracket(self):
        self._assert_rejects("[2001:db8::1", "missing closing bracket")
        self._assert_rejects(
            "ssh://[2001:db8::1:830", "missing closing bracket")

    def test_ipv6_garbage_after_bracket(self):
        self._assert_rejects(
            "[2001:db8::1]garbage", "expected ':' after IPv6")

    def test_ipv6_empty_brackets(self):
        self._assert_rejects("[]", "IPv6 literal is empty")

    def test_embedded_whitespace(self):
        self._assert_rejects("ho st", "whitespace")

    def test_path_separator(self):
        self._assert_rejects("host/path", "contains '/'")
        self._assert_rejects("ssh://host/path", "contains '/'")

    def test_invalid_hostname_label(self):
        self._assert_rejects("host..example.com", "invalid host label")
        self._assert_rejects("-host", "invalid host label")
        self._assert_rejects("host-", "invalid host label")
        self._assert_rejects("a" * 64, "invalid host label")


class TestValidateEndpoint(unittest.TestCase):
    """Tests for ``validate_endpoint`` (constructor-arg validation)."""

    def _assert_rejects(self, address, port, protocol, msg_substr):
        with self.assertRaises(YInvalidArgumentError) as cm:
            validate_endpoint(address, port, protocol)
        self.assertIn(msg_substr, str(cm.exception))

    # --- happy path / normalisation ---------------------------------

    def test_representative_scenario(self):
        # Matches the default test harness: ssh://admin:admin@127.0.0.1
        # -> address="127.0.0.1", port=12022, protocol="ssh"
        self.assertEqual(
            validate_endpoint("127.0.0.1", 12022, "ssh"),
            ("127.0.0.1", 12022))

    def test_port_none_defaults_to_830(self):
        # Must preserve existing wrapper behaviour.
        self.assertEqual(
            validate_endpoint("host", None, "ssh"),
            ("host", 830))

    def test_string_port_coerced(self):
        self.assertEqual(
            validate_endpoint("host", "22", "ssh"),
            ("host", 22))

    def test_ipv6_brackets_stripped(self):
        self.assertEqual(
            validate_endpoint("[::1]", 830, "ssh"),
            ("::1", 830))

    def test_whitespace_stripped(self):
        self.assertEqual(
            validate_endpoint("  host  ", 830, "ssh"),
            ("host", 830))

    def test_protocol_case_insensitive_validation(self):
        # Validated case-insensitively; value is NOT transformed
        # (wrapper passes its original protocol string through).
        host, port = validate_endpoint("host", 830, "SSH")
        self.assertEqual((host, port), ("host", 830))

    def test_custom_default_port(self):
        self.assertEqual(
            validate_endpoint("host", None, "tcp", default_port=4334),
            ("host", 4334))

    # --- rejection cases --------------------------------------------

    def test_empty_address(self):
        self._assert_rejects("", 830, "ssh", "host is empty")

    def test_none_address(self):
        self._assert_rejects(None, 830, "ssh", "expected str")

    def test_uri_passed_as_address(self):
        # Common user mistake: passing a full URI as `address`.
        self._assert_rejects(
            "ssh://host:830", 830, "ssh", "contains '/'")

    def test_port_zero_rejected(self):
        self._assert_rejects("host", 0, "ssh", "out of range")

    def test_port_negative_rejected(self):
        self._assert_rejects("host", -1, "ssh", "out of range")

    def test_port_too_large_rejected(self):
        self._assert_rejects("host", 99999, "ssh", "out of range")

    def test_port_bool_rejected(self):
        self._assert_rejects("host", True, "ssh", "got bool")

    def test_port_float_rejected(self):
        self._assert_rejects("host", 830.0, "ssh", "got float")

    def test_port_non_numeric_str_rejected(self):
        self._assert_rejects("host", "abc", "ssh",
                             "not a positive integer")

    def test_unsupported_protocol(self):
        self._assert_rejects("host", 830, "https", "not supported")

    def test_non_string_protocol(self):
        self._assert_rejects("host", 830, 123, "expected str")


if __name__ == '__main__':
    unittest.main(verbosity=2)
