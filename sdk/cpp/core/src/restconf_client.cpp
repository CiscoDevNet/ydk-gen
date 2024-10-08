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
 -------------------------------------------------------------------
 This file has been modified by Yan Gorelik, YDK Solutions.
 All modifications in original under CiscoDevNet domain
 introduced since October 2019 are copyrighted.
 All rights reserved under Apache License, Version 2.0.
------------------------------------------------------------------*/

#include <curl/curl.h>
#include <sstream>

#include "capabilities_parser.hpp"
#include "errors.hpp"
#include "restconf_client.hpp"
#include "types.hpp"
#include "logger.hpp"

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
                        int port, const string & encoding, unsigned ssl_options)
    : curl(NULL), header_options_list(NULL), encoding(encoding), ssl_options(ssl_options)
{
    protocol = (address.find("https://") == 0) ? "HTTPS" : "HTTP";
    initialize(address, username, password, port);
    YLOG_INFO("Ready to communicate with {} using {}", base_url, protocol);
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

    string output = execute("GET", url, "");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_options_list);
    curl_slist_free_all(capabilities_option);

    return output;
}

string RestconfClient::execute(const string & yfilter, const string & url, const string & payload) const
{
    long response_code;
    string response;

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, yfilter.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_URL, (base_url + url).c_str());

    YLOG_DEBUG( "Sending request: {}. Payload: {}. URL: {}", yfilter, payload, (base_url + url));

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        YLOG_ERROR("Connection failed: {}", curl_easy_strerror(res));
        throw(YClientError{curl_easy_strerror(res)});
    }
    get_debug_info(curl);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

    if(http_status_is_error(response_code))
    {
        ostringstream os;
        os <<  "Operation did not succeed. Got response: " << response_code << " : " << response;
        YLOG_ERROR(os.str().c_str());
        throw(YServiceProviderError{os.str()});
    }
    string response_to_print = response.length()==0 ? "NONE" : "\n"+response;
    YLOG_INFO( "Got response code: {}, data: {}", response_code, response_to_print);
    return response;
}

void RestconfClient::initialize(const string & address, const string & username,
        const string & password, int port)
{
    initialize_curl(username, password);

    string base;
    if (address.find("http://") == string::npos &&
        address.find("https://") == string::npos)
    {
        base = "http://" + address + ":" + std::to_string(port);
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
        throw(YClientError{"Unable to create curl environment"});
    }

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //optionally uncomment for debug

    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, (username+":"+password).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);

    if (protocol == "HTTPS")
    {
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
        if (!(ssl_options & SSL_PEER_VERIFICATION))
        {
            /*
             * If you want to connect to a site who isn't using a certificate that is
             * signed by one of the certs in the CA bundle you have, you can skip the
             * verification of the server's certificate. This makes the connection
             * A LOT LESS SECURE.
             *
             * If you have a CA cert for the server stored someplace else than in the
             * default bundle, then the CURLOPT_CAPATH option might come handy for
             * you.
             */
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            YLOG_INFO("SSL peer verification is disabled!");
        }
        if (!(ssl_options & SSL_HOSTNAME_VERIFICATION))
        {
            /*
             * If the site you're connecting to uses a different host name that what
             * they have mentioned in their server certificate's commonName (or
             * subjectAltName) fields, libcurl will refuse to connect. You can skip
             * this check, but this will make the connection less secure.
             */
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            YLOG_INFO("SSL host name verification is disabled!");
        }
    }

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

static bool token_found(size_t token)
{
    return token != string::npos;
}

/*
 * Comparator function for case-insensitive find (ci_find).
 */
static bool ci_equals(char l, char r)
{
    return (std::tolower(l) == std::tolower(r));
}

/*
 * Case-insensitive find.
 */
static size_t ci_find(std::string text, std::string search)
{
  std::string::iterator it = std::search(text.begin(), text.end(), search.begin(), search.end(), ci_equals);
  if (it != text.end())
  {
      return std::distance(text.begin(), it);
  }
  return std::string::npos;
}

static string parse_quoted_string(string str)
{
    size_t end_quote;
    size_t start_quote = str.find("\"");
    if(token_not_found(start_quote))
    {
        start_quote = str.find("'");
        if(token_not_found(start_quote))
        {
            return "";
        }
        end_quote = str.find("'", start_quote + 1);
    }
    else
    {
        end_quote = str.find("\"", start_quote + 1);
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

    return str.substr(start_quote + 1, (end_quote - start_quote - 1));
}

static string parse_restconf_root_response(string response)
{
    size_t equals_sign, link_start, rel_start;
    size_t end_bracket = 0;
    if(token_not_found((link_start = ci_find(response, "link")))) {
        return "";
    }

    // Find and parse `rel` with value of `restconf`
    while (token_found(link_start))
    {
        link_start += end_bracket;

        if(token_not_found((end_bracket = response.find(">", link_start)))
          || token_not_found((rel_start = response.find("rel", link_start)))
          || token_not_found((equals_sign = response.find("=", rel_start))))
        {
            return "";
        }

        if(parse_quoted_string(response.substr(equals_sign)) == "restconf")
        {
            // Found the `link` containing the `restconf` metadata
            break;
        }

        link_start = ci_find(response.substr(end_bracket), "link")
    }

    // Parse value of `href` property
    size_t root_start;
    if(token_not_found((root_start = response.find("href", link_start)))
        || token_not_found((equals_sign = response.find("=", root_start))))
    {
        return "";
    }

    return parse_quoted_string(response.substr(equals_sign));
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
        YLOG_INFO("Unable to retrieve restconf root. Assuming '/restconf' as the root");
        return "/restconf";
    }

    return root;
}

static void get_debug_info(CURL *curl)
{
    double speed_upload, total_time;
    curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed_upload);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
    YLOG_DEBUG("Speed of upload: {} bytes/sec", speed_upload);
    YLOG_DEBUG("Total time: {} seconds", total_time);
}
}
