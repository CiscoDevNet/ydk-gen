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
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <unordered_set>

#include "path_private.hpp"
#include "../ydk_yang.hpp"
#include "../logger.hpp"


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

static std::string get_models_download_path()
{
    const char *homeDir = getenv("HOME");

    if (!homeDir) {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd)
           homeDir = pwd->pw_dir;
    }

    std::ostringstream models_path{};
    models_path << homeDir << "/.ydk";

    return models_path.str();
}

static void create_if_does_not_exist(const std::string & path)
{
    if(!file_exists(path))
    {
        if(mkdir(path.c_str(), 0700) != 0)
        {
            YLOG_ERROR("Could not create repository: {}", path);
            throw(YCPPIllegalStateError{"Could not create repository: "+path});
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
            YLOG_DEBUG("Libyang DEBUG: {}", error_message.str());
            break;
    }
}
}
}

ydk::path::RepositoryPtr::RepositoryPtr (path::ModelCachingOption caching_option)
  : using_temp_directory(true), caching_option(caching_option)
{
    path = get_models_download_path();
    ly_set_log_clb(libyang_log_callback, 1);
}


ydk::path::RepositoryPtr::RepositoryPtr(const std::string& search_dir, path::ModelCachingOption caching_option)
  : path{search_dir}, using_temp_directory(false), caching_option(caching_option)
{
    if (!file_exists(path))
    {
        YLOG_ERROR("Path {} is not a valid directory.", search_dir);
        throw(YCPPInvalidArgumentError{"path "+search_dir+" is not a valid directory"});
    }

    ly_set_log_clb(libyang_log_callback, 1);
}

ydk::path::RepositoryPtr::~RepositoryPtr()
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
                YLOG_INFO("Cannot sink to file {}", filename);
            }
        }

        char* get_enlarged_data(const std::string & buffer, const std::string & model_name)
        {
            char *enlarged_data = nullptr;
            /* enlarge data by 2 bytes for flex */
            auto data = buffer.c_str();
            auto len = std::strlen(data);
            enlarged_data = static_cast<char*>(std::malloc((len + 2) * sizeof *enlarged_data));
            if (!enlarged_data) {
                YLOG_ERROR("Could not get model: {}", model_name);
                throw(std::bad_alloc{});
            }
            memcpy(enlarged_data, data, len);
            enlarged_data[len] = enlarged_data[len + 1] = '\0';
            return enlarged_data;
        }

        extern "C" char* get_module_callback(const char* module_name, const char* module_rev, const char *submod_name, const char *sub_rev,
                                       void* user_data, LYS_INFORMAT* format, void (**free_module_data)(void *model_data))
        {
            YLOG_DEBUG("Getting module {} submodule {}", module_name, (submod_name?submod_name:"none"));
            *free_module_data = c_free_data;

            if(user_data != nullptr){
                ModelProvider::Format m_format = ModelProvider::Format::YANG;
                *format = LYS_IN_YANG;
                auto repo = reinterpret_cast<const RepositoryPtr*>(user_data);

                //first check our directory for a file of the form <module-module_name>@<module_rev-date>.yang
                YLOG_DEBUG("Looking for file in folder: {}", repo->path);
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
                YLOG_DEBUG("Opening file {}", yang_file_path);

                YLOG_DEBUG("Path found with rev: {}. Path without rev: {}",
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

                        return get_enlarged_data(model_data, yang_file_path);
                    } else {
                        YLOG_ERROR("Cannot open file {}", yang_file_path);
                        throw(YCPPIllegalStateError("Cannot open file"));
                    }

                }

                for(auto model_provider : repo->get_model_providers()) {
                    std::string model_data{};
                    if(submod_name)
                    {
                        YLOG_DEBUG("Getting submodule using get-schema {}", submod_name);
                        model_data = model_provider->get_model(submod_name, sub_rev != nullptr ? sub_rev : "", m_format);
                    }
                    else
                    {
                        YLOG_DEBUG("Getting module using get-schema {}", module_name);
                        model_data = model_provider->get_model(module_name, module_rev != nullptr ? module_rev : "", m_format);
                    }
                    if(!model_data.empty()){
                        sink_to_file(yang_file_path, model_data);
                        return get_enlarged_data(model_data, yang_file_path);
                    } else {
                        YLOG_DEBUG("Cannot find model with module_name: {} module_rev: {}", module_name, (module_rev !=nullptr ? module_rev : ""));
                        throw(YCPPIllegalStateError{"Cannot find model"});
                        return {};
                    }
                }
            }
            YLOG_DEBUG("Cannot find model with module_name: {}", module_name);
//            throw(YCPPIllegalStateError{"Cannot find model"});
            return {};
        }
    }

}

ly_ctx*
ydk::path::RepositoryPtr::create_ly_context() {
    create_if_does_not_exist(path);

    if(using_temp_directory)
    {
        if (caching_option == ModelCachingOption::PER_DEVICE)
        {
            for(auto model_provider : get_model_providers())
            {
                path+="/"+model_provider->get_hostname_port();
                break;
            }
        }
        else
        {
            path += "/common_cache";
        }
        create_if_does_not_exist(path);
        YLOG_INFO("Path where models are to be downloaded: {}", path);
    }
    YLOG_DEBUG("Creating libyang context in path: {}", path);
    struct ly_ctx* ctx = ly_ctx_new(path.c_str());

    if(!ctx) {
        YLOG_ERROR("Could not create repository in: {}", path);
        throw(std::bad_alloc{});
    }

    //set module callback (only if there is a model provider)
    if(!model_providers.empty())
    {
        ly_ctx_set_module_clb(ctx, get_module_callback, this);
    }

    return ctx;
}

std::shared_ptr<ydk::path::RootSchemaNode>
ydk::path::RepositoryPtr::create_root_schema(const std::vector<std::unordered_map<std::string, path::Capability>>& lookup_tables,
                                             const std::vector<path::Capability>& caps_to_load)
{

    ly_verb(LY_LLSILENT); //turn off libyang logging at the beginning
    ly_ctx* ctx = create_ly_context();

    load_module_from_capabilities(ctx, caps_to_load);

    ly_verb(LY_LLVRB); // enable libyang logging after model download has completed
    RootSchemaNodeImpl* rs = new RootSchemaNodeImpl{ctx, shared_from_this(), lookup_tables};
    return std::shared_ptr<RootSchemaNode>(rs);
}

void
ydk::path::RepositoryPtr::load_module_from_capabilities(ly_ctx* ctx, const std::vector<path::Capability>& caps)
{
    for (auto c : caps)
    {
        for (auto d: c.deviations)
        {
            load_module(ctx, d);
        }
        if(c.module == "ietf-yang-library")
            continue;

        load_module(ctx, c);
    }
}

std::vector<const lys_module*>
ydk::path::RepositoryPtr::get_new_ly_modules_from_lookup(ly_ctx* ctx,
                                                         const std::unordered_set<std::string>& keys,
                                                         const std::unordered_map<std::string, path::Capability>& lookup_table)
{
    std::vector<const lys_module*> new_modules;

    for (auto k: keys)
    {
        auto it = lookup_table.find(k);
        if (it != lookup_table.end())
        {
            auto capability = it->second;

            bool new_module = true;
            auto m = load_module(ctx, capability.module, new_module);

            if (m && new_module)
            {
                YLOG_DEBUG("Added new libyang module '{}'", std::string(m->name));
                new_modules.emplace_back(m);
            }

            for (auto& d: capability.deviations)
            {
                new_module = true;
                m = load_module(ctx, d, new_module);

                if (m && new_module)
                {
                    YLOG_DEBUG("Added new libyang deivation module '{}'", std::string(m->name));
                    new_modules.emplace_back(m);
                }
            }
        }
    }
    return new_modules;
}

std::vector<const lys_module*>
ydk::path::RepositoryPtr::get_new_ly_modules_from_path(ly_ctx* ctx,
                                                       const std::string& path,
                                                       const std::unordered_map<std::string, path::Capability>& lookup_table)
{
    auto module_names = path::segmentalize_module_names(path);
    return get_new_ly_modules_from_lookup(ctx, module_names, lookup_table);
}

const lys_module*
ydk::path::RepositoryPtr::load_module(ly_ctx* ctx, const std::string& module_name)
{
    bool new_module = true;
    return load_module(ctx, module_name, "", {}, new_module);
}

const lys_module*
ydk::path::RepositoryPtr::load_module(ly_ctx* ctx, const std::string& module_name, bool& new_module)
{
    return load_module(ctx, module_name, "", {}, new_module);
}

const lys_module*
ydk::path::RepositoryPtr::load_module(ly_ctx* ctx, ydk::path::Capability& capability)
{
    bool new_module = true;
    return load_module(ctx, capability, new_module);
}

const lys_module*
ydk::path::RepositoryPtr::load_module(ly_ctx* ctx, ydk::path::Capability& capability, bool& new_module)
{
    return load_module(ctx, capability.module, capability.revision, capability.features, new_module);
}

const lys_module*
ydk::path::RepositoryPtr::load_module(ly_ctx* ctx, const std::string& module_name, const std::string& revision)
{
    bool new_module = true;
    return load_module(ctx, module_name, revision, {}, new_module);
}

const lys_module*
ydk::path::RepositoryPtr::load_module(ly_ctx* ctx, const std::string& module, const std::string& revision, const std::vector<std::string>& features, bool& new_module)
{

    YLOG_DEBUG("Module '{}' Revision '{}'", module.c_str(), revision.c_str());

    auto p = ly_ctx_get_module(ctx, module.c_str(), revision.empty() ? 0 : revision.c_str());

    if(!p)
    {
        p = ly_ctx_load_module(ctx, module.c_str(), revision.empty() ? 0 : revision.c_str());
    } else {
        YLOG_DEBUG("Cache hit Module '{}' Revision '{}'", module, revision);
        new_module = false;
    }

    if (!p) {
        YLOG_WARN("Unable to parse module: '{}'. This model cannot be used with YDK", module);
    }

    for (auto f : features)
        lys_features_enable(p, f.c_str());

    return p;
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
ydk::path::RepositoryPtr::add_model_provider(ydk::path::ModelProvider* model_provider)
{
    model_providers.push_back(model_provider);
}

///
/// @brief Removes a model provider.
///
/// Removes the given model provider from this Repository.
///
void
ydk::path::RepositoryPtr::remove_model_provider(ydk::path::ModelProvider* model_provider)
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
std::vector<ydk::path::ModelProvider*> ydk::path::RepositoryPtr::get_model_providers() const
{
    return model_providers;
}

ydk::path::Repository::Repository (path::ModelCachingOption caching_option) : m_priv_repo(std::make_shared<RepositoryPtr>(caching_option))
{
}

ydk::path::Repository::Repository(const std::string& search_dir, path::ModelCachingOption caching_option) : m_priv_repo(std::make_shared<RepositoryPtr>(search_dir, caching_option))
{
}

ydk::path::Repository::~Repository ()
{
}

std::shared_ptr<ydk::path::RootSchemaNode>
ydk::path::Repository::create_root_schema(const std::vector<path::Capability>& caps_to_load)
{
    std::vector<std::unordered_map<std::string, path::Capability>> lookup_tables(2);
    return m_priv_repo->create_root_schema(lookup_tables, caps_to_load);
}

std::shared_ptr<ydk::path::RootSchemaNode>
ydk::path::Repository::create_root_schema(const std::vector<std::unordered_map<std::string, path::Capability>>& lookup_tables,
                                          const std::vector<path::Capability>& caps_to_load)
{
    return m_priv_repo->create_root_schema(lookup_tables, caps_to_load);
}

void
ydk::path::Repository::add_model_provider(ydk::path::ModelProvider* model_provider)
{
    m_priv_repo->add_model_provider(model_provider);
}

void
ydk::path::Repository::remove_model_provider(ydk::path::ModelProvider* model_provider)
{
    m_priv_repo->remove_model_provider(model_provider);
}

std::vector<ydk::path::ModelProvider*> ydk::path::Repository::get_model_providers() const
{
    return m_priv_repo->get_model_providers();
}

////////////////////////////////////////////////////////////////////////

