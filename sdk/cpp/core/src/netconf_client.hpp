/*  ----------------------------------------------------------------
 Copyright 2016 Cisco Systems

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ------------------------------------------------------------------*/

#ifndef _YDK_NETCONF_CLIENT_H_
#define _YDK_NETCONF_CLIENT_H_
#include <map>
#include <string>
#include <vector>

#include <libnetconf/netconf.h>

#include "errors.hpp"

struct nc_session;
typedef struct nc_msg nc_rpc;
typedef struct nc_msg nc_reply;
typedef struct ssh_session_struct* ssh_session;

typedef struct capabilities {
	capabilities(std::vector<std::string> cas){caps=cas;}

	std::vector<std::string> caps;
} capabilities;

namespace ydk
{

class NetconfClient
{

public:
	static std::map<std::pair<std::string, std::string>, std::string> password_lookup;

public:
	NetconfClient(std::string  username, std::string  password,
			std::string  server_ip, int port);

	~NetconfClient();

	int connect();
	std::string execute_payload(const std::string & payload);
	std::vector<std::string> get_capabilities();
	std::string get_hostname_port();

private:

	static void clb_print(NC_VERB_LEVEL level, const char* msg);
	static void clb_error_print(const char* tag, const char* type,
			const char* severity, const char* apptag, const char* path,
			const char* message, const char* attribute, const char* element,
			const char* ns, const char* sid);
	static char* clb_set_password(const char* username, const char* hostname);
	static char* clb_set_interactive(const char *name, const char *instruction, const char *prompt, int echo);
	static char* clb_set_passphrase(const char *username, const char *hostname, const char *priv_key_file);
	static int clb_ssh_host_authenticity_check(const char *hostname,
			ssh_session session);

	nc_rpc* build_rpc_request(const std::string & payload);
	std::string process_rpc_reply(int reply_type, const nc_reply* reply);
	void init_capabilities();
	void perform_session_check(std::string message);

private:
	struct nc_session *session;

	std::string username;
	std::string hostname;
	int port;
	std::vector<std::string> capabilities;
};



}

#endif /* _YDK_NETCONF_CLIENT_H_ */
