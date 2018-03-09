/// YANG Development Kit
// Copyright 2016 Cisco Systems. All rights reserved
//
////////////////////////////////////////////////////////////////
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//////////////////////////////////////////////////////////////////


#include <iostream>
#include <sstream>
#include <stdio.h>

#include <libnetconf.h>
#include <libnetconf_ssh.h>

#include "netconf_ssh_client.hpp"
#include "types.hpp"
#include "logger.hpp"

using namespace std;
using namespace ydk;
typedef vector<string> StringVec;


namespace ydk
{

map<pair<string, string>, string> NetconfSSHClient::password_lookup;

NetconfSSHClient::NetconfSSHClient(
    string username,
    string password,
    string hostname,
    int port,
    int timeout) :
        NetconfClient(),
        username(username),
        hostname(hostname),
        port(port),
        timeout(timeout)
{
    nc_verbosity(NC_VERB_DEBUG);
    nc_callback_print(clb_print);
    nc_callback_sshauth_password(clb_set_password);
    nc_callback_sshauth_interactive(clb_set_interactive);
    nc_callback_sshauth_passphrase(clb_set_passphrase);
    nc_callback_ssh_host_authenticity_check(clb_ssh_host_authenticity_check);
    nc_ssh_pref(NC_SSH_AUTH_PASSWORD, 100);
    nc_session_transport(NC_TRANSPORT_SSH);

    password_lookup.insert(make_pair(make_pair(username, hostname), password));
    session=NULL;
}

NetconfSSHClient::NetconfSSHClient(
    string username,
    string private_key_path,
    string public_key_path,
    string server_ip,
    int port,
    int timeout
):
    NetconfClient(),
    username(username),
    private_key_path(private_key_path),
    public_key_path(public_key_path),
    hostname(server_ip),
    port(port),
    timeout(timeout)
{
    nc_verbosity(NC_VERB_DEBUG);
    nc_callback_print(clb_print);
    nc_callback_sshauth_password(clb_set_password);
    nc_callback_sshauth_interactive(clb_set_interactive);
    nc_callback_sshauth_passphrase(clb_set_passphrase);
    nc_callback_ssh_host_authenticity_check(clb_ssh_host_authenticity_check);
    nc_ssh_pref(NC_SSH_AUTH_PUBLIC_KEYS, 100);
    nc_session_transport(NC_TRANSPORT_SSH);

    nc_set_keypair_path(private_key_path.c_str(), public_key_path.c_str());
    session=NULL;
}


int NetconfSSHClient::connect()
{
    session = nc_session_connect(hostname.c_str(), port, username.c_str(), NULL);
    perform_session_check("Could not connect to " + hostname);
    init_capabilities();
    return EXIT_SUCCESS;
}

std::string NetconfSSHClient::get_hostname_port()
{
    ostringstream os;
    os<<hostname<<"_"<<port;
    return os.str();
}

void NetconfSSHClient::init_capabilities()
{
    struct nc_cpblts* capabilities_list;
    const char *cap;

    capabilities_list = nc_session_get_cpblts(session);
    nc_cpblts_iter_start(capabilities_list);

    while ((cap = nc_cpblts_iter_next(capabilities_list)) != NULL)
    {
        capabilities.push_back(cap);
    }
}

string NetconfSSHClient::execute_payload(const string & payload)
{
    perform_session_check("Could not execute payload. Not connected to " + hostname);

    nc_reply *reply;
    nc_rpc *rpc = build_rpc_request(payload);

    YLOG_DEBUG("Netconf SSH Client: sending rpc");
    const nc_msgid msgid = nc_session_send_rpc(session, rpc);
    if (msgid == NULL) {
        YLOG_ERROR("Could not send payload {}", payload );
        throw(YClientError{"Could not send payload"});
    }

    YLOG_DEBUG("Netconf SSH Client: receiving rpc");
    NC_MSG_TYPE msg_type = nc_session_recv_reply(session, timeout, &reply);
    YLOG_DEBUG("Netconf SSH Client: processing reply");
    string receive_msg = process_rpc_reply(msg_type, reply);

    nc_reply_free(reply);
    nc_rpc_free(rpc);

    return receive_msg;
}

NetconfSSHClient::~NetconfSSHClient()
{
    nc_session_free(session);
}

nc_rpc* NetconfSSHClient::build_rpc_request(const string & payload)
{
    nc_rpc* rpc = nc_rpc_build(payload.c_str(), session);

    if (rpc == NULL)
    {
        YLOG_ERROR("Could not build rpc payload: {}", payload );
        throw(YClientError{"Could not build payload"});
    }
    return rpc;
}

string NetconfSSHClient::process_rpc_reply(int msg_type, const nc_rpc* reply)
{
    switch (msg_type)
    {
        case NC_MSG_REPLY:
            return nc_rpc_dump(reply);
        case NC_MSG_HELLO:
            return nc_rpc_dump(reply);

        default:
        case NC_MSG_WOULDBLOCK:
            YLOG_ERROR("Connection timed out");
            throw(YClientError{"Connection timed out"});
        case NC_MSG_UNKNOWN:
            YLOG_ERROR("RPC error occurred");
            throw(YClientError{"RPC error occured"});
    }
    return {};
}

StringVec NetconfSSHClient::get_capabilities()
{
    return capabilities;
}

void NetconfSSHClient::clb_print(NC_VERB_LEVEL level, const char* msg)
{
    switch (level)
    {
    case NC_VERB_ERROR:
         YLOG_ERROR("Connection error occurred: {}", msg);
        break;
    case NC_VERB_WARNING:
    case NC_VERB_VERBOSE:
    case NC_VERB_DEBUG:
        YLOG_DEBUG("Trace: {}", msg);
        break;
    }
}

char* NetconfSSHClient::clb_set_password(const char* user_name,
        const char* host_name)
{
    string password_string = password_lookup[make_pair(user_name, host_name)];
    char* password_buffer = (char*) malloc(
            sizeof(char) * (password_string.size() + 1));
    snprintf(password_buffer, password_string.size() + 1, "%s",
            password_string.c_str());

    return password_buffer;
}

char* NetconfSSHClient::clb_set_interactive(const char *name, const char *instruction, const char *prompt, int echo)
{
    string password_string{};
    for(const auto & entry : password_lookup)
    {
        password_string = entry.second;
        break;
    }
    char* password_buffer = (char*) malloc(sizeof(char) * (password_string.size() + 1));
    snprintf(password_buffer, password_string.size() + 1, "%s", password_string.c_str());

    YLOG_DEBUG("looked up password for interactive: {}", password_buffer);
    return password_buffer;
}

char* NetconfSSHClient::clb_set_passphrase(const char *user_name, const char *host_name, const char *priv_key_file)
{
    string password_string = password_lookup[make_pair(user_name, host_name)];
    char* password_buffer = (char*) malloc(sizeof(char) * (password_string.size() + 1));
    snprintf(password_buffer, password_string.size() + 1, "%s", password_string.c_str());

    YLOG_DEBUG("looked up password for passphrase: {}", password_buffer);
    return password_buffer;
}

int NetconfSSHClient::clb_ssh_host_authenticity_check(const char *hostname,
        ssh_session session)
{
    return EXIT_SUCCESS;
}

void NetconfSSHClient::perform_session_check(const string & message)
{
    if (session == NULL)
    {
        YLOG_ERROR(message.c_str());
        throw(YClientError{message});
    }
}


}
