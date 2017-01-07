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

#ifndef _RESTCONF_CLIENT_H_
#define _RESTCONF_CLIENT_H_

#include <string>


typedef void CURL;
struct curl_slist;

namespace ydk
{
class RestconfClient
{
	public:
		RestconfClient(const std::string & address, const std::string & username, const std::string & password,
					int port, const std::string & encoding);
		~RestconfClient();

        std::string execute(const std::string & operation, const std::string & url, const std::string & payload) const;
    	std::string get_capabilities(const std::string & url, const std::string & encoding) const;

	private:
    	void initialize(const std::string & address, const std::string & username,
    			const std::string & password, int port);
    	void initialize_curl(const std::string & username, const std::string & password);

	private:
		CURL *curl;
		curl_slist *header_options_list;
		std::string base_url;
		std::string encoding;
};
}

#endif /*_RESTCONF_CLIENT_H_*/
