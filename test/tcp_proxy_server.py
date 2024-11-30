#!/usr/bin/env python3
#  ----------------------------------------------------------------
# Copyright 2017 Cisco Systems
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

""" Simple proxy server forwarding messages between ConfD TCP session
and YDK TCP client.
"""
__author__ = "Xiaoqin Zhu"
__email__ = "xiaoqinz@cisco.com"
__usage__ = """ ./tcp_proxy_server.py -b 12307 -c 2023"""

import os
import sys
import time
import socket
import logging
import argparse

if sys.version_info < (3,):
    import SocketServer as socketserver
    from urlparse import urlparse
else:
    import socketserver
    from urllib.parse import urlparse

sys.tracebacklimit = 0

logging.basicConfig(level=logging.INFO,
                    format='%(name)s: %(message)s',
                    )

FOUR_k = 4096
EOM_10 = "]]>]]>"
EOM_11 = "\n##\n"
HELLO = """
<?xml version="1.0" encoding="UTF-8"?>
<hello xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
  <capabilities>
    <capability>urn:ietf:params:netconf:base:1.0</capability>
    <capability>urn:ietf:params:netconf:base:1.1</capability>
    </capabilities>
</hello>
"""


class DummyHandler(socketserver.BaseRequestHandler):
    """Dummy request handler"""

    def handle(self):
        """Nothing ..."""
        pass


class SimpleProxyServer(socketserver.TCPServer):
    """Simple proxy server: ConfD TCP <--> YDK TCP.

    Attribute:
        request_queue_size (int): maximum size of request queue acceptable
        confd_socket (socket.socket): socket for confd connection
        reset_confd (bool): reset ConfD connection if True
        logger (logging.Logger): logger.
    """

    def __init__(self, server_address, confd_address, handler_class=DummyHandler):
        self.logger = logging.getLogger('SimpleProxyServer')
        self.logger.debug('__init__')
        self.request_queue_size = 50

        self.reset_confd = True

        socketserver.TCPServer.__init__(self, server_address, handler_class)
        return

    def confd_connect(self):
        """Start connection, Send ConfD TCP header."""
        self.confd_socket = socket.create_connection(confd_address)
        confd_header = "[admin;%s;tcp;%d;%d;%s;%s;%s;]\n" % (confd_address[0],
                                                             os.getuid(),
                                                             os.getgid(),
                                                             "",
                                                             os.getenv("HOME", "/tmp"),
                                                             "")
        self.send_confd(confd_header)

    def serve_forever(self):
        """Hijacked socketserver.TCPServer.serve_forever. Main loop moved to
        SimpleProxyServer.process_request.
        """
        while True:
            try:
                self.handle_request()
            except Exception as e:
                pass
        return

    def process_request(self, request, client_address):
        """Ping-pang data between ConfD TCP session and YDK TCP client.

        Args:
            request(socket.socket): socket object for client connection.
            client_address(tuple of str): (hostname, port)
        """
        while True:
            self.logger.debug('Starting new send/recv request...')
            if self.reset_confd == True:
                self.confd_connect()
                self.finish_confd_connection(request)
            else:
                self.forward_client(request, EOM_11)
                time.sleep(0.1) # wait for reply
                self.forward(request, EOM_11)
            self.logger.debug('Finished one send/recv request...')

    def finish_confd_connection(self, request):
        """Finish ConfD connection:
            - Drop username sent from the client.
            - Drop password sent from the client.
            - Send hello request to ConfD.
            - Send hello reply back to the client.
            - Drop hello request sent from the client.
        """
        request.send("Username: ".encode('utf-8'))
        self.client_username = request.recv(FOUR_k)
        request.send("Password: ".encode('utf-8'))
        self.client_password = request.recv(FOUR_k)

        hello_requst = '\n#%d\n' % len(HELLO) + HELLO
        self.send_confd(hello_requst)
        self.send_confd(EOM_10)
        self.forward(request, EOM_10)
        # need to drop this hello request,
        # the hello message exchange order for ConfD is different from XR
        _ = self.recv_client(request, EOM_10)
        self.reset_confd = False

    def forward(self, request, eom):
        """ConfD --> Client."""
        self.send_client(request, self.recv_confd(eom))

    def forward_client(self, request, eom):
        """Client --> ConfD."""
        self.send_confd(self.recv_client(request, eom))

    def _recv(self, request, eom):
        """Receive data ends with eom through request socket.

        Args:
            request (socket.socket): request socket.
            eom (str): end of message marker.
        """
        data = []
        last_chunk = ""
        while True:
            last_chunk = request.recv(FOUR_k).decode('utf-8')
            if not last_chunk:
                request.close()
                self.reset_confd = True
                raise Exception("No data")
            elif last_chunk.endswith(eom):
                data.append(last_chunk)
                break
            data.append(last_chunk)

        data = ''.join(data)
        self.logger.debug("Receiving...\n\t{}".format(data))
        return data

    def _send(self, request, data):
        """Send data through request socket.

        Args:
            request (socket.socket): request socket.
            data (str): data payload.
        """
        self.logger.debug("Sending...\n\t{}".format(data))
        try:
            request.sendall(data.encode('utf-8'))
        except:
            request.close()
            self.reset_confd = True

    def recv_confd(self, eom):
        """Receive data from ConfD until eom.

        Args:
            eom (str): end of message marker.
        """
        return self._recv(self.confd_socket, eom)

    def send_confd(self, data):
        """Send data to ConfD.

        Args:
            data (str): data payload.
        """
        self._send(self.confd_socket, data)

    def recv_client(self, request, eom):
        """Receive data through client socket until eom.

        Args:
            request (socket.socket): client socket
            eom (str): end of message marker.
        """
        return self._recv(request, eom)

    def send_client(self, request, data):
        """Send data to client through request socket.

        Args:
            request (socket.socket): client request socket.
            data (str): data payload send to the client.
        """
        self._send(request, data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog="confd_tcp_proxy_server", usage="%(prog)s [options]")
    parser.add_argument("-v", "--verbose", help="verbose mode")
    parser.add_argument("-b", "--bind", dest='bind', type=int, help="binding port, 12307")
    parser.add_argument("-c", "--confd", dest='confd', type=int, help="ConfD address, 2023")

    args = parser.parse_args()
    bind_port = args.bind
    confd_port = args.confd

    binding_address = ('127.0.0.1', bind_port)
    confd_address = ('127.0.0.1', confd_port)

    server = SimpleProxyServer(binding_address, confd_address, DummyHandler)
    server.serve_forever()

    exit()
