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
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fstream>

#include "path_private.hpp"
#include "../logger.hpp"
#include "../ydk_yang.hpp"
#include "core_info.hpp"


//////////////////////////////////////////////////////////////////////////
// class ydk::Repository
//////////////////////////////////////////////////////////////////////////
namespace ydk
{
static bool file_exists(const std::string & path)
{
    struct stat st = {0};

    return stat(path.c_str(), &st) == 0;
}

static void create_if_does_not_exist(const std::string & path)
{
    if(!file_exists(path))
    {
        if(mkdir(path.c_str(), 0700) != 0)
        {
            YLOG_ERROR("Could not create repository");
            throw(YCPPIllegalStateError{"Could not create repository"});
        }
    }
}

namespace path
{
void libyang_log_callback(LY_LOG_LEVEL level, const char *msg, const char *path)
{
    std::ostringstream error_message{};
    error_message <<msg;
    if(path)
    {
        error_message << " " << "Path: '" << path<<"'";
    }
    switch(level)
    {
        case LY_LLERR:
            if(error_message.str().find("Invalid value")!= std::string::npos
                    || error_message.str().find("Failed to resolve")!= std::string::npos
                    || error_message.str().find("Unexpected character")!= std::string::npos
                    || error_message.str().find("does not satisfy the constraint")!= std::string::npos)
            {
                YLOG_ERROR("Libyang ERROR: {}", error_message.str());
                throw(YCPPModelError{error_message.str()});
            }
            YLOG_ERROR("Libyang ERROR: {}", error_message.str());
            break;
        case LY_LLSILENT:
        case LY_LLWRN:
        case LY_LLVRB:
        case LY_LLDBG:
            YLOG_TRACE("Libyang TRACE: {}", error_message.str());
            break;
    }
}
}
}

ydk::path::Repository::Repository()
  : using_temp_directory(true)
{
    path = ydk_models_path;
    ly_set_log_clb(libyang_log_callback, 1);
}


ydk::path::Repository::Repository(const std::string& search_dir)
  : path{search_dir}, using_temp_directory(false)
{
    if (!file_exists(path))
    {
        YLOG_ERROR("Path {} is not a valid directory.", search_dir);
        throw(YCPPInvalidArgumentError{"path "+search_dir+" is not a valid directory"});
    }

    ly_set_log_clb(libyang_log_callback, 1);
}

ydk::path::Repository::~Repository()
{
}

namespace ydk {
    namespace path {

        extern "C" void cpp_free_data(void *model_data)
        {
            delete [] static_cast<char*>(model_data);
        }

        extern "C" void c_free_data(void *model_data)
        {
            std::free(model_data);
        }

        void sink_to_file(const std::string& filename, const std::string& contents)
        {
            std::ofstream sink {filename, std::ios::binary};
            if (sink.is_open()) {
                sink << contents ;
                sink.close();
            } else {
                YLOG_DEBUG("Cannot sink to file {}", filename);
            }
        }

        char* get_enlarged_data(const std::string & buffer)
        {
        	char *enlarged_data = nullptr;
			/* enlarge data by 2 bytes for flex */
			auto data = buffer.c_str();
			auto len = std::strlen(data);
			enlarged_data = static_cast<char*>(std::malloc((len + 2) * sizeof *enlarged_data));
			if (!enlarged_data) {
				throw(std::bad_alloc{});
			}
			memcpy(enlarged_data, data, len);
			enlarged_data[len] = enlarged_data[len + 1] = '\0';
			return enlarged_data;
        }

        extern "C" char* get_module_callback(const char* module_name, const char* module_rev, const char *submod_name, const char *sub_rev,
                                       void* user_data, LYS_INFORMAT* format, void (**free_module_data)(void *model_data))
        {
            YLOG_TRACE("Getting module {} submodule {}", module_name, (submod_name?submod_name:"none"));
            *free_module_data = c_free_data;

            if(user_data != nullptr){
                ModelProvider::Format m_format = ModelProvider::Format::YANG;
                *format = LYS_IN_YANG;
                auto repo = reinterpret_cast<const Repository*>(user_data);

                //first check our directory for a file of the form <module-module_name>@<module_rev-date>.yang
                YLOG_TRACE("Looking for file in folder: {}", repo->path);
                std::string yang_file_path{repo->path};
                std::string yang_file_path_no_revision{repo->path};
                yang_file_path += '/';
                yang_file_path += (submod_name?submod_name:module_name);
                yang_file_path_no_revision += yang_file_path;
                if(module_rev){
                    yang_file_path += "@";
                    yang_file_path += module_rev;
                }
                else if(sub_rev){
                    yang_file_path += "@";
                    yang_file_path += sub_rev;
                }
                yang_file_path += ".yang";
                YLOG_TRACE("Opening file {}", yang_file_path);

                YLOG_TRACE("Path found with rev: {}. Path without rev: {}",
                        file_exists(yang_file_path), file_exists(yang_file_path_no_revision));
                if(file_exists(yang_file_path) || file_exists(yang_file_path_no_revision)) {
                    //open the file read the data and return it
                    std::string model_data {""};
                    std::ifstream yang_file {yang_file_path};
                    if(yang_file.is_open()) {
                        std::string line;
                        while(std::getline(yang_file, line)){
                            model_data+=line;
                            model_data+='\n';
                        }

                        yang_file.close();

                        return get_enlarged_data(model_data);
                    } else {
                        YLOG_ERROR("Cannot open file {}", yang_file_path);
                        throw(YCPPIllegalStateError("Cannot open file"));
                    }

                }


                for(auto model_provider : repo->get_model_providers()) {
                    std::string model_data{};
                    if(submod_name)
                    {
                        YLOG_TRACE("Getting submodule using get-schema {}", submod_name);
                        model_data = model_provider->get_model(submod_name, sub_rev != nullptr ? sub_rev : "", m_format);
                    }
                    else
                    {
                        YLOG_TRACE("Getting module using get-schema {}", module_name);
                        model_data = model_provider->get_model(module_name, module_rev != nullptr ? module_rev : "", m_format);
                    }
                    if(!model_data.empty()){

                        sink_to_file(yang_file_path, model_data);
                        return get_enlarged_data(model_data);
                    } else {
                        YLOG_TRACE("Cannot find model with module_name: {} module_rev: {}", module_name, (module_rev !=nullptr ? module_rev : ""));
//                        throw(YCPPIllegalStateError{"Cannot find model"});
                        return {};
                    }
                }
            }
            YLOG_TRACE("Cannot find model with module_name: {}", module_name);
//            throw(YCPPIllegalStateError{"Cannot find model"});
            return {};
        }
    }

}

std::unique_ptr<ydk::path::RootSchemaNode>
ydk::path::Repository::create_root_schema(const std::vector<path::Capability> & capabilities)
{
    ly_verb(LY_LLSILENT); //turn off libyang logging at the beginning
    create_if_does_not_exist(path);

    if(using_temp_directory)
    {
        for(auto model_provider : get_model_providers())
        {
            path+="/"+model_provider->get_hostname_port();
            break;
        }
        create_if_does_not_exist(path);
        YLOG_DEBUG("Path where models are to be downloaded: {}", path);
    }
    YLOG_TRACE("Creating libyang context in path {}", path);
    struct ly_ctx* ctx = ly_ctx_new(path.c_str());

    if(!ctx) {
        throw(std::bad_alloc{});
    }

    //set module callback (only if there is a model provider)
    if(!model_providers.empty())
    {
        ly_ctx_set_module_clb(ctx, get_module_callback, this);
    }

    for (auto c : capabilities)
    {
        if(c.module == "ietf-yang-library")
            continue;
        YLOG_TRACE("Module {} Revision ", c.module.c_str(), c.revision.c_str());
        auto p = ly_ctx_get_module(ctx, c.module.c_str(), c.revision.empty() ? 0 : c.revision.c_str());

        if(!p)
        {
            p = ly_ctx_load_module(ctx, c.module.c_str(), c.revision.empty() ? 0 : c.revision.c_str());
        } else {
            YLOG_TRACE("Cache hit module name: {}", c.module);
        }

        if (!p) {
            YLOG_TRACE("Unable to parse module: {}", c.module);
            continue;
        }
        for (auto f : c.features)
            lys_features_enable(p, f.c_str());

    }

    ly_verb(LY_LLVRB); // enable libyang logging after model download has completed
    RootSchemaNodeImpl* rs = new RootSchemaNodeImpl{ctx};
    return std::unique_ptr<RootSchemaNode>(rs);
}

///
/// @brief Adds a model provider.
///
/// Adds a model provider to this Repository.
/// If the repository does not find a model while trying to create
/// a SchemaTree it calls on the model_provider to see if the said model
/// can be downloaded by one of them. If that fails it tries the next
///
/// @param[in] module_provider The Module Provider to add
///
void
ydk::path::Repository::add_model_provider(ydk::path::ModelProvider* model_provider)
{
    model_providers.push_back(model_provider);
}

///
/// @brief Removes a model provider.
///
/// Removes the given model provider from this Repository.
///
void
ydk::path::Repository::remove_model_provider(ydk::path::ModelProvider* model_provider)
{
    auto item = std::find(model_providers.begin(), model_providers.end(), model_provider);
    if(item != model_providers.end()) {
        model_providers.erase(item);
    }
}

///
/// @brief Get model providers
///
/// Gets all model providers registered with this repo.
///
/// @return vector of ModelProvider's
///
std::vector<ydk::path::ModelProvider*> ydk::path::Repository::get_model_providers() const
{
    return model_providers;
}


////////////////////////////////////////////////////////////////////////

