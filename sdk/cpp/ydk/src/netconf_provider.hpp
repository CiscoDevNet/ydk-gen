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

#include "entity.hpp"
#include "core.hpp"

namespace ydk {


class NetconfClient;
    class NetconfServiceProvider : public core::ServiceProvider {
	public:
        NetconfServiceProvider(const core::Repository* repo,
                std::string address,
				std::string username,
				std::string password,
				int port);
		~NetconfServiceProvider();

	std::string encode(Entity & entity, const std::string & operation, bool read_config_only) const;
	std::string encode(Entity & entity, const std::string & operation) const;
	std::unique_ptr<Entity> decode(const std::string & payload) const;
	std::string execute_payload(const std::string & payload, const std::string & operation) const;
        
        virtual core::RootSchemaNode* get_root_schema();
        
        virtual core::DataNode* invoke(core::Rpc* rpc) const;
        
        
        static const char* WRITABLE_RUNNING;
        static const char* CANDIDATE;
        static const char* ROLLBACK_ON_ERROR;
        static const char* STARTUP;
        static const char* URL;
        static const char* XPATH;
        static const char* BASE_1_1;
        static const char* CONFIRMED_COMMIT_1_1;
        static const char* VALIDATE_1_1;
        static const char* NS;
        static const char* MODULE_NAME;
        

	private:
        core::DataNode*
        handle_create_delete(core::Rpc* rpc, core::Annotation ann) const;
        
        
        core::DataNode*
        handle_read(core::Rpc* rpc) const;
        
        
        const core::Repository* m_repo;
	std::unique_ptr<NetconfClient> client;
	std::unique_ptr<ydk::core::RootSchemaNode> root_schema;
	std::vector<ydk::core::Capability> capabilities;
        
        std::vector<std::string> client_caps;
        //crud related stuff
        core::SchemaNode* create_sn;
        core::SchemaNode* read_sn;
        core::SchemaNode* update_sn;
        core::SchemaNode* delete_sn;
        
};
}

#endif /*_NETCONF_PROVIDER_H_*/
