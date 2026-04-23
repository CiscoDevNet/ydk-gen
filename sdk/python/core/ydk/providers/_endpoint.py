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

"""
_endpoint.py

Internal helpers for parsing and validating connection endpoints
(scheme / host / port) used by service providers and sessions.

This module deliberately has **no** dependency on the compiled
``ydk_`` extension so it can be unit-tested standalone.
"""

from ydk.errors import YInvalidArgumentError


_NETCONF_DEFAULT_PORT = 830
_SUPPORTED_SCHEMES = frozenset(("ssh", "tcp"))

# Hostname label characters. RFC 1123 permits letters, digits and
# hyphens; underscore is added here pragmatically because many lab
# and vendor devices use it in hostnames.
_LABEL_CHARS = frozenset(
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789-_"
)


def _is_valid_label(label):
    if not 1 <= len(label) <= 63:
        return False
    if label[0] == "-" or label[-1] == "-":
        return False
    return all(ch in _LABEL_CHARS for ch in label)


def _looks_like_ipv6(host):
    # IPv6 literals (after bracket stripping) contain at least one ':'.
    return ":" in host


def _coerce_port(raw, context):
    """Convert *raw* to a valid int port or raise.

    Accepts ``int`` (excluding ``bool``) and all-digit ``str``.
    Rejects out-of-range (<1 or >65535), floats and other types.
    *context* is prepended to error messages so callers can tell
    which input was at fault (e.g. ``"port"`` vs ``"parse_endpoint"``).
    """
    if isinstance(raw, bool):
        raise YInvalidArgumentError(
            "%s: expected int port, got bool" % context)
    if isinstance(raw, int):
        port = raw
    elif isinstance(raw, str):
        stripped = raw.strip()
        if not stripped or not stripped.isdigit():
            raise YInvalidArgumentError(
                "%s: port %r is not a positive integer" % (context, raw))
        port = int(stripped)
    else:
        raise YInvalidArgumentError(
            "%s: expected int or numeric str port, got %s"
            % (context, type(raw).__name__))
    if not 1 <= port <= 65535:
        raise YInvalidArgumentError(
            "%s: port %d out of range 1..65535" % (context, port))
    return port


def _coerce_host(raw, context):
    """Normalise a host string (hostname / IPv4 / IPv6 literal).

    Strips surrounding whitespace, IPv6 brackets, and a trailing FQDN
    dot. Rejects empty strings, embedded whitespace, path separators
    and malformed hostname labels. Returns the normalised host.
    """
    if not isinstance(raw, str):
        raise YInvalidArgumentError(
            "%s: expected str host, got %s"
            % (context, type(raw).__name__))
    host = raw.strip()
    if not host:
        raise YInvalidArgumentError("%s: host is empty" % context)
    if any(ch.isspace() for ch in host):
        raise YInvalidArgumentError(
            "%s: host %r contains whitespace" % (context, raw))
    if host.startswith("["):
        if not host.endswith("]"):
            raise YInvalidArgumentError(
                "%s: IPv6 literal %r missing closing bracket"
                % (context, raw))
        host = host[1:-1]
        if not host:
            raise YInvalidArgumentError(
                "%s: IPv6 literal is empty" % context)
    if "/" in host:
        raise YInvalidArgumentError(
            "%s: host %r contains '/'; pass a bare host, not a URI"
            % (context, raw))
    if _looks_like_ipv6(host):
        if ":::" in host or not host.replace(":", ""):
            raise YInvalidArgumentError(
                "%s: malformed IPv6 literal %r" % (context, raw))
        return host
    if host.endswith("."):
        host = host[:-1]
        if not host:
            raise YInvalidArgumentError("%s: host is empty" % context)
    for label in host.split("."):
        if not _is_valid_label(label):
            raise YInvalidArgumentError(
                "%s: invalid host label %r in %r"
                % (context, label, raw))
    return host


def parse_endpoint(endpoint):
    """Parse a scheme://host[:port] URI or a bare host[:port] string.

    Accepted forms (scheme is case-insensitive, optional)::

        ssh://host
        ssh://host:830
        ssh://[2001:db8::1]:830
        tcp://router.example.com
        host.example.com
        host.example.com:830
        [2001:db8::1]:830
        2001:db8::1                (bare IPv6, no port)

    Returns ``(scheme, host, port)`` where *scheme* and *port* are
    ``None`` if absent; IPv6 brackets are stripped from *host*.

    Credentials (``user:pass@``) are **not** supported and are
    rejected explicitly -- callers pass them as separate parameters.

    Raises ``YInvalidArgumentError`` on any malformed input.
    """
    if not isinstance(endpoint, str):
        raise YInvalidArgumentError(
            "parse_endpoint: expected str, got %s"
            % type(endpoint).__name__)
    ep = endpoint.strip()
    if not ep:
        raise YInvalidArgumentError("parse_endpoint: endpoint is empty")

    scheme = None
    rest = ep
    if "://" in ep:
        scheme_raw, _, rest = ep.partition("://")
        scheme = scheme_raw.lower()
        if scheme not in _SUPPORTED_SCHEMES:
            raise YInvalidArgumentError(
                "parse_endpoint: unsupported scheme %r "
                "(expected one of %s)"
                % (scheme_raw, sorted(_SUPPORTED_SCHEMES)))
        if not rest:
            raise YInvalidArgumentError(
                "parse_endpoint: %r has no host component" % endpoint)

    if "@" in rest:
        raise YInvalidArgumentError(
            "parse_endpoint: userinfo (user:pass@) is not supported "
            "in the address; pass credentials as separate parameters")

    host_part = rest
    port_part = None

    if host_part.startswith("["):
        close = host_part.find("]")
        if close < 0:
            raise YInvalidArgumentError(
                "parse_endpoint: IPv6 literal in %r "
                "missing closing bracket" % endpoint)
        bracketed = host_part[:close + 1]
        remainder = host_part[close + 1:]
        if remainder:
            if remainder[0] != ":":
                raise YInvalidArgumentError(
                    "parse_endpoint: expected ':' after IPv6 "
                    "bracket in %r" % endpoint)
            port_part = remainder[1:]
        host_part = bracketed
    elif host_part.count(":") == 1:
        host_part, _, port_part = host_part.partition(":")
    # else: zero colons -> bare host, no port
    #       >=2 colons -> bare IPv6 literal without brackets, no port

    host = _coerce_host(host_part, "parse_endpoint")
    port = None
    if port_part is not None:
        if port_part == "":
            raise YInvalidArgumentError(
                "parse_endpoint: %r has trailing ':' but no port"
                % endpoint)
        port = _coerce_port(port_part, "parse_endpoint")

    return scheme, host, port


def validate_endpoint(address, port, protocol,
                      default_port=_NETCONF_DEFAULT_PORT):
    """Validate and normalise NETCONF constructor endpoint arguments.

    :param address:  host string (hostname, IPv4, or IPv6 literal).
                     Brackets/whitespace/trailing-dot are stripped.
    :param port:     ``int`` or numeric ``str``; ``None`` falls back
                     to *default_port* (matching existing behaviour).
    :param protocol: ``"ssh"`` or ``"tcp"`` (case-insensitive). The
                     value is validated but not transformed -- the
                     caller passes its original ``protocol`` string
                     to the underlying C++ layer unchanged.
    :returns: ``(host, port)`` normalised values.
    :raises YInvalidArgumentError: on any invalid input.
    """
    host = _coerce_host(address, "address")

    if protocol is not None:
        if not isinstance(protocol, str):
            raise YInvalidArgumentError(
                "protocol: expected str, got %s"
                % type(protocol).__name__)
        if protocol.lower() not in _SUPPORTED_SCHEMES:
            raise YInvalidArgumentError(
                "protocol: %r is not supported (expected one of %s)"
                % (protocol, sorted(_SUPPORTED_SCHEMES)))

    if port is None:
        return host, int(default_port)
    return host, _coerce_port(port, "port")
