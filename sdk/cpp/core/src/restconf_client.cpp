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

#include <boost/log/trivial.hpp>
#include <curl/curl.h>

#include "capabilities_parser.hpp"
#include "errors.hpp"
#include "restconf_client.hpp"


using namespace std;

namespace ydk
{
static string get_restconf_root(CURL *curl, string base_url);
static void get_debug_info(CURL *curl);
static size_t writeFunction(void *ptr, size_t size, size_t nmemb, string* data);

static bool http_status_is_error(long status);
static bool curl_returned_error(CURLcode return_code);
static bool token_not_found(size_t token);


RestconfClient::RestconfClient(const string & address, const string & username, const string & password,
						int port, const string & encoding)
	: curl(NULL), header_options_list(NULL), encoding(encoding)
{
	initialize(address, username, password, port);
	BOOST_LOG_TRIVIAL(debug) << "Ready to communicate with " << base_url << " using http";
}

RestconfClient::~RestconfClient()
{
	curl_slist_free_all(header_options_list);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

string RestconfClient::get_capabilities(const string & url, const std::string & encoding) const
{
	curl_slist *capabilities_option = NULL;
	vector<string> capabilities {};

	capabilities_option = curl_slist_append(capabilities_option, ("Content-Type: " + encoding).c_str());
	capabilities_option = curl_slist_append(capabilities_option, ("Application: " + encoding).c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, capabilities_option);

	string output{};
	output = execute("GET", url, "");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_options_list);
	curl_slist_free_all(capabilities_option);

	return output;
}

string RestconfClient::execute(const string & operation, const string & url, const string & payload) const
{
	long response_code;
	string response;

	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, operation.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_URL, (base_url + url).c_str());

	BOOST_LOG_TRIVIAL(trace) << "Sending request: " << operation << ". Payload: " << payload << ". URL: " << (base_url + url);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
    	BOOST_LOG_TRIVIAL(error) << "Connection failed: " << curl_easy_strerror(res);
    	BOOST_THROW_EXCEPTION(YCPPClientError{curl_easy_strerror(res)});
    }
    get_debug_info(curl);

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

	if(http_status_is_error(response_code))
	{
		ostringstream os;
		os <<  "Operation did not succeed. Got response: " << response_code << " : " << response;
		BOOST_LOG_TRIVIAL(error) << os.str();
		BOOST_THROW_EXCEPTION(YCPPServiceProviderError{os.str()});
	}
	BOOST_LOG_TRIVIAL(trace) << "Got response: " << response_code << " : " << response;
	return response;
}

void RestconfClient::initialize(const string & address, const string & username,
		const string & password, int port)
{
	initialize_curl(username, password);

	string base;
	if(address.find("http") == string::npos)
	{
		base = "http://" +address + ":" + std::to_string(port);
	}
	else
	{
		base = address + ":" + std::to_string(port);
	}

	base_url = base + get_restconf_root(curl, base);
}

void RestconfClient::initialize_curl(const string & username, const string & password)
{
	string encoding_string;
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(!curl)
	{
		BOOST_THROW_EXCEPTION(YCPPClientError{"Unable to create curl environment"});
	}

	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //optionally uncomment for debug

	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
	curl_easy_setopt(curl, CURLOPT_USERPWD, (username+":"+password).c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);

	header_options_list = curl_slist_append(header_options_list, ("Content-Type: " + encoding).c_str());
	header_options_list = curl_slist_append(header_options_list, ("Accept: " + encoding).c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_options_list);
}

static size_t writeFunction(void *ptr, size_t size, size_t nmemb, string* data)
{
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

static bool http_status_is_error(long status)
{
	return status < 200
			|| status >= 300;
}

static bool curl_returned_error(CURLcode return_code)
{
	return return_code != CURLE_OK;
}

static bool token_not_found(size_t token)
{
	return token == string::npos;
}

static string parse_restconf_root_response(string response)
{
	size_t root_start, equals_sign;
	if(token_not_found((root_start = response.find("href")))
		|| token_not_found((equals_sign = response.find("=", root_start))))
	{
		return "";
	}

	size_t start_quote = response.find("\"", equals_sign);
	size_t end_quote;
	if(token_not_found(start_quote))
	{
		start_quote = response.find("'", equals_sign);
		if(token_not_found(start_quote))
		{
			return "";
		}
		end_quote = response.find("'", start_quote + 1);
	}
	else
	{
		end_quote = response.find("\"", start_quote + 1);
		if(token_not_found(end_quote))
		{
			return "";
		}
	}

	if(token_not_found(start_quote)
		|| token_not_found(end_quote))
	{
		return "";
	}

	return response.substr(start_quote + 1, (end_quote - start_quote - 1));
}

static string get_restconf_root(CURL *curl, string base)
{
	string response;
	long response_code;

	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_URL, (base+"/.well-known/host-meta").c_str());

	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

	string root{};
	if(curl_returned_error(res)
			|| http_status_is_error(response_code)
			|| (root = parse_restconf_root_response(response)).size() == 0)
	{
		BOOST_LOG_TRIVIAL(debug) << "Unable to retrieve restconf root. Assuming '/restconf' as the root";
		return "/restconf";
	}

	return root;
}

static void get_debug_info(CURL *curl)
{
    double speed_upload, total_time;
    curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed_upload);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
    BOOST_LOG_TRIVIAL(trace) <<"Speed of upload: " <<speed_upload<<" bytes/sec"<<endl;
    BOOST_LOG_TRIVIAL(trace) <<"Total time: " <<total_time<<" seconds"<<endl;
}
}
