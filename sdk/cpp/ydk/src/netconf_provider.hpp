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

#ifndef _NETCONF_PROVIDER_H_
#define _NETCONF_PROVIDER_H_

#include <memory>
#include <string>

#include "core.hpp"

namespace ydk {

class NetconfClient;

class NetconfServiceProvider : public core::ServiceProvider {
public:
        NetconfServiceProvider(core::Repository* repo,
                                std::string address,
                               std::string username,
                               std::string password,
                               int port);
        NetconfServiceProvider(std::string address,
                               std::string username,
                               std::string password,
                               int port);
        ~NetconfServiceProvider();

        virtual core::RootSchemaNode* get_root_schema() const;

        virtual core::DataNode* invoke(core::Rpc* rpc) const;

        static const char* CANDIDATE;
        static const char* MODULE_NAME;

private:
        core::DataNode* handle_edit(core::Rpc* rpc, core::Annotation ann) const;
        core::DataNode* handle_read(core::Rpc* rpc) const;
        void initialize();

private:
        std::unique_ptr<core::Repository> m_repo_ptr;
        core::Repository* m_repo;
        std::unique_ptr<NetconfClient> client;
        std::unique_ptr<core::ModelProvider> model_provider;
        std::unique_ptr<ydk::core::RootSchemaNode> root_schema;

        std::vector<std::string> server_capabilities;

        bool ietf_nc_monitoring_available = false;

};
}

#endif /*_NETCONF_PROVIDER_H_*/
