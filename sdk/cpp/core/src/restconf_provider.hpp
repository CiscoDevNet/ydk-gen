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

#ifndef _RESTCONF_PROVIDER_H_
#define _RESTCONF_PROVIDER_H_

#include <map>
#include <memory>
#include <string>

#include "path_api.hpp"

namespace ydk {
static const std::string default_capabilities_url = "/ietf-restconf-monitoring:restconf-state/capabilities";

class CapabilitiesParser;
struct OpenDaylightNode;
class RestconfClient;


class RestconfServiceProvider : public path::ServiceProvider {
public:
        RestconfServiceProvider(path::Repository & repo,
                               const std::string & address,
                               const std::string & username,
                               const std::string & password,
                               int port = 80,
							   EncodingFormat encoding = EncodingFormat::JSON,
							   const std::string & config_url_root = "/data",
							   const std::string & state_url_root = "/data");

        RestconfServiceProvider(std::unique_ptr<RestconfClient> client,
        						std::unique_ptr<ydk::path::RootSchemaNode> root_schema,
								const std::string & edit_method,
								const std::string & config_url_root,
								const std::string & state_url_root,
								EncodingFormat encoding);

        ~RestconfServiceProvider();

        path::RootSchemaNode* get_root_schema() const;
        path::DataNode* invoke(path::Rpc* rpc) const;
        EncodingFormat get_encoding() const;

private:
        path::DataNode* handle_edit(path::Rpc* rpc, const std::string & operation) const;
        path::DataNode* handle_read(path::Rpc* rpc) const;
        void initialize(path::Repository & repo);

private:
        std::unique_ptr<RestconfClient> client;

        std::unique_ptr<path::ModelProvider> model_provider;
        std::unique_ptr<ydk::path::RootSchemaNode> root_schema;

        std::vector<std::string> server_capabilities;

        EncodingFormat encoding;
        std::string edit_method;
        std::string config_url_root;
        std::string state_url_root;
};
}

#endif /*_RESTCONF_PROVIDER_H_*/
