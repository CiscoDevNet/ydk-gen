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
#include <string.h>

#include <fstream>

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
    struct stat st;
    memset(&st, 0, sizeof(struct stat));

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
            YLOG_ERROR("Could not create repository in '{}'", path);
            throw(YIllegalStateError{"Could not create repository in "+path});
        }
    }
}

}	// namespace ydk

ydk::path::RepositoryPtr::RepositoryPtr (path::ModelCachingOption caching_option)
  : using_temp_directory(true), caching_option(caching_option)
{
    path = get_models_download_path();
    set_libyang_logging_callback();
}


ydk::path::RepositoryPtr::RepositoryPtr(const std::string& search_dir, path::ModelCachingOption caching_option)
  : path{search_dir}, using_temp_directory(false), caching_option(caching_option)
{

    if (!file_exists(path))
    {
        YLOG_ERROR("Path '{}' is not a valid directory.", search_dir);
        throw(YInvalidArgumentError{"path "+search_dir+" is not a valid directory"});
    }

    set_libyang_logging_callback();
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
                YLOG_INFO("Cannot write to file '{}'", filename);
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
                YLOG_ERROR("Could not download model: {}", model_name);
                throw(YIllegalStateError{"Could not download model: " + model_name});
            }
            memcpy(enlarged_data, data, len);
            enlarged_data[len] = '\0';
            enlarged_data[len + 1] = '\0';
            return enlarged_data;
        }

        extern "C" char* get_module_callback(const char* module_name, const char* module_rev,
                                             const char *submod_name, const char *sub_rev,
                                             void* user_data, LYS_INFORMAT* format,
                                             void (**free_module_data)(void *model_data))
        {
            if (submod_name) {
                YLOG_DEBUG("Getting module '{}' submodule '{}' with revision {}",
                           module_name, submod_name, (sub_rev ? sub_rev : "none"));
            }
            else {
                YLOG_DEBUG("Getting module '{}' with revision {}",
                           module_name, (module_rev ? module_rev : "none"));
            }
            *free_module_data = c_free_data;

            if (user_data != nullptr) {
                ModelProvider::Format m_format = ModelProvider::Format::YANG;
                *format = LYS_IN_YANG;
                auto repo = reinterpret_cast<const RepositoryPtr*>(user_data);

                // first check the directory for a file name <module-module_name>@<module_rev-date>.yang
                // YLOG_DEBUG("Looking for file in folder: {}", repo->path);
                std::string yang_file_path{repo->path};
                yang_file_path += '/';
                yang_file_path += (submod_name ? submod_name : module_name);
                std::string yang_file_path_no_revision = yang_file_path + ".yang";
                bool with_revision = false;

                if (submod_name) {
                    if (sub_rev) {
                        yang_file_path += "@";
                        yang_file_path += sub_rev;
                        with_revision = true;
                    }
                }
                else if (module_name && module_rev) {
                    yang_file_path += "@";
                    yang_file_path += module_rev;
                    with_revision = true;
                }
                yang_file_path += ".yang";
                // YLOG_DEBUG("Opening file '{}'", yang_file_path);

                if (file_exists(yang_file_path) || file_exists(yang_file_path_no_revision)) {
                    if (with_revision && file_exists(yang_file_path))
                        YLOG_DEBUG("File found with revision: {}", yang_file_path);
                    else {
                        YLOG_DEBUG("File found with no revision: {}", yang_file_path_no_revision);
                        yang_file_path = yang_file_path_no_revision;
                    }

                    // open the file and read the data
                    std::string model_data {""};
                    std::ifstream yang_file {yang_file_path};
                    if (yang_file.is_open()) {
                        std::string line;
                        while (std::getline(yang_file, line)) {
                            model_data += line;
                            model_data += '\n';
                        }
                        yang_file.close();
                        return get_enlarged_data(model_data, yang_file_path);
                    }
                    else {
                        YLOG_ERROR("Cannot open file '{}'", yang_file_path);
                        throw(YIllegalStateError("Cannot open file " + yang_file_path));
                    }
                }
                else {
                    YLOG_DEBUG("File '{}' is not found in repository", yang_file_path_no_revision);
                }

                for (auto model_provider : repo->get_model_providers()) {
                    std::string model_data{};
                    if (submod_name)
                    {
                        YLOG_DEBUG("Getting submodule '{}' using get-schema", submod_name);
                        model_data = model_provider->get_model(submod_name, sub_rev ? sub_rev : "", m_format);
                    }
                    else
                    {
                        YLOG_DEBUG("Getting module '{}' using get-schema", module_name);
                        model_data = model_provider->get_model(module_name, module_rev ? module_rev : "", m_format);
                    }
                    if (!model_data.empty()) {
                        sink_to_file(yang_file_path, model_data);
                        return get_enlarged_data(model_data, yang_file_path);
                    }
                }
            }
            YLOG_ERROR("Cannot find model with module name '{}'", module_name);
            //throw(YServiceProviderError("Cannot find model with module name: " + std::string(module_name)));
            return nullptr;
        }
    }
}

ly_ctx* ydk::path::RepositoryPtr::create_ly_context()
 {
    create_if_does_not_exist(path);

    if(using_temp_directory)
    {
        if (caching_option == ModelCachingOption::PER_DEVICE)
        {
            for(auto model_provider : get_model_providers())
            {
                //path+="/"+model_provider->get_hostname_port();
                auto hostname_port = model_provider->get_hostname_port();
                auto underscore_pos = hostname_port.rfind("_");
                if (underscore_pos != std::string::npos)
                    hostname_port = hostname_port.substr(0, underscore_pos);
                path += "/" + hostname_port;
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
    struct ly_ctx* ctx = ly_ctx_new(path.c_str(), LY_CTX_ALLIMPLEMENTED);

    if(!ctx) {
        YLOG_ERROR("Could not create repository in: {}", path);
        throw(YIllegalStateError{"Could not create repository in: " + path});
    }

    //set module callback (only if there is a model provider)
    if(!model_providers.empty())
    {
        ly_ctx_set_module_imp_clb(ctx, get_module_callback, this);
    }

    return ctx;
}

std::shared_ptr<ydk::path::RootSchemaNode>
ydk::path::RepositoryPtr::create_root_schema(const std::unordered_map<std::string, path::Capability>& lookup_table,
                                             const std::vector<path::Capability>& caps_to_load)
{
    ly_verb(LY_LLVRB); // enable libyang logging
    ly_ctx* ctx = create_ly_context();

    load_module_from_capabilities(ctx, caps_to_load);

    RootSchemaNodeImpl* rs = new RootSchemaNodeImpl{ctx, shared_from_this(), lookup_table};
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

static bool contains_only_numbers(const std::string & module_name)
{
    bool ret = false;
    for(char c:module_name)
    {
        if(c >= '0' && c <= '9')
        {
            ret = true;
        }
        else
        {
            ret = false;
            break;
        }
    }
    return ret;
}

static bool contains_letters_dashes_colon_dot_slash(const std::string & module_name)
{
    bool ret = false;
    for(char c:module_name)
    {
        if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c=='-' || c==':' || c=='.' || c=='/')
        {
            ret = true;
        }
        else
        {
            ret = false;
            break;
        }
    }
    return ret;
}

std::vector<const lys_module*>
ydk::path::RepositoryPtr::get_new_ly_modules_from_lookup(ly_ctx* ctx,
                                                         const std::unordered_set<std::string>& namespace_module_names,
                                                         const std::unordered_map<std::string, path::Capability>& lookup_table)
{
    std::vector<const lys_module*> new_modules;

    for (auto k: namespace_module_names)
    {
        // k could be module namespace, which contains extra '/', ':', '.', need 
        // to add them to predicate function
        if(contains_letters_dashes_colon_dot_slash(k) && !contains_only_numbers(k))
        {
            bool new_module = true;
            auto module_name = k;

            // if k is module namespace, then we update module_name from lookup_table, and load deviation module if necessary
            auto kit = lookup_table.find(k);
            if (kit != lookup_table.end())
            {
                module_name = kit->second.module;
            }
            else if (k.length() > strlen("http://") && k.substr(0, strlen("http://")) == "http://")
            {
                YLOG_INFO("Failed to find namespace '{}' in lookup table", k);
                continue;
            }
            else {
                auto colon_pos = k.rfind(":");
                if (colon_pos != std::string::npos) {
                    module_name = k.substr(colon_pos+1);
                }
            }

            auto m = load_module(ctx, module_name, new_module);

            if (m && new_module)
            {
                YLOG_DEBUG("Added new libyang module '{}'", std::string(m->name));
                new_modules.emplace_back(m);
            }

            // resolve deviation module after main module
            if (kit != lookup_table.end())
            {
                auto capability = kit->second;
                for (auto& d: capability.deviations)
                {
                    new_module = true;
                    auto m = load_module(ctx, d, new_module);
    
                    if (m && new_module)
                    {
                        YLOG_DEBUG("Added new libyang deviation module '{}'", std::string(m->name));
                        new_modules.emplace_back(m);
                    }
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
    std::string revision = "";
    ydk::path::Capability mod_cap = ydk::path::Capability{module_name, revision};
    get_module_capabilities(mod_cap);
    return load_module(ctx, mod_cap, new_module);
}

const lys_module*
ydk::path::RepositoryPtr::load_module(ly_ctx* ctx, const std::string& module_name, bool& new_module)
{
    std::string revision = "";
    ydk::path::Capability mod_cap = ydk::path::Capability{module_name, revision};
    get_module_capabilities(mod_cap);
    return load_module(ctx, mod_cap, new_module);
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
    ydk::path::Capability mod_cap = ydk::path::Capability{module_name, revision};
    get_module_capabilities(mod_cap);
    return load_module(ctx, mod_cap, new_module);
}

void
ydk::path::RepositoryPtr::collect_features_from_imported_modules(const lys_module* module, std::set<std::pair<lys_module*, std::string>>& features)
{
    for (int i=0; i<module->imp_size; i++) {
        const lys_module* imp_module = module->imp[i].module;
        ydk::path::Capability mod_cap = ydk::path::Capability{imp_module->name, ""};
        get_module_capabilities(mod_cap);
        for (auto f : mod_cap.features) {
            features.insert(make_pair((lys_module*)imp_module, f));
        }
        collect_features_from_imported_modules(imp_module, features);
    }
}

const lys_module*
ydk::path::RepositoryPtr::load_module(ly_ctx* ctx, const std::string& module, const std::string& revision, const std::vector<std::string>& features, bool& new_module)
{

    YLOG_DEBUG("Loading module '{}', revision '{}'", module.c_str(), revision.c_str());

    auto p = ly_ctx_get_module(ctx, module.c_str(), revision.empty() ? NULL : revision.c_str(), 1);

    if (!p) {
        p = ly_ctx_load_module(ctx, module.c_str(), revision.empty() ? NULL : revision.c_str());
        if (!p) {
            YLOG_WARN("Unable to parse module '{}'. This model cannot be used with YDK", module);
            return p;
       }
    }
    else {
        const struct lys_node *last = nullptr;
        auto q = lys_getnext(last, nullptr, p, 0);
        if (q && q->priv) {
            YLOG_DEBUG("The module '{}' schema has already been populated in YDK repository", module);
            new_module = false;
        }
        else {
            YLOG_DEBUG("The module '{}' schema present in Libyang repository, but not in YDK; consider populate", module);
        }
    }

    // Collect all module features
    std::set<std::pair<lys_module*, std::string>> all_features{};
    for (auto f : features) {
        all_features.insert(make_pair((lys_module*)p, f));
    }
    collect_features_from_imported_modules(p, all_features);

    for (auto elem : all_features) {
        if (lys_features_enable((const lys_module*)elem.first, elem.second.c_str())) {
            YLOG_DEBUG("Failed to enable feature '{}' in '{}'", elem.second, elem.first->name);
        }
        else {
            YLOG_DEBUG("Enabled feature '{}' in '{}'", elem.second, elem.first->name);
        }
    }
    return p;
}

void
ydk::path::RepositoryPtr::get_module_capabilities(ydk::path::Capability & mod_cap)
{
    for (auto & cap: server_caps) {
        if (cap.module == mod_cap.module &&
            (mod_cap.revision == "" || cap.revision == mod_cap.revision))
        {
            mod_cap.features = cap.features;
            mod_cap.deviations = cap.deviations;
            break;
        }
    }
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
    std::unordered_map<std::string, path::Capability> lookup_table;
    return m_priv_repo->create_root_schema(lookup_table, caps_to_load);
}

std::shared_ptr<ydk::path::RootSchemaNode>
ydk::path::Repository::create_root_schema(const std::unordered_map<std::string, path::Capability>& lookup_table,
                                          const std::vector<path::Capability>& caps_to_load)
{
    return m_priv_repo->create_root_schema(lookup_table, caps_to_load);
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

void
ydk::path::Repository::set_server_capabilities(std::vector<path::Capability> & serv_caps) {
	m_priv_repo->server_caps = serv_caps;
}
////////////////////////////////////////////////////////////////////////

