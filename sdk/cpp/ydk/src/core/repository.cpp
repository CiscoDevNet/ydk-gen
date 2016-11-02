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


#include "core_private.hpp"
#include "../ydk_yang.hpp"
#include <boost/log/trivial.hpp>
#include <fstream>

namespace fs = boost::filesystem;

//////////////////////////////////////////////////////////////////////////
// class ydk::Repository
//////////////////////////////////////////////////////////////////////////

ydk::core::Repository::Repository()
{
    path = fs::temp_directory_path();
}


ydk::core::Repository::Repository(const std::string& search_dir) : path{search_dir}
{
    if(!fs::exists(path) || !fs::is_directory(path)) {
        BOOST_LOG_TRIVIAL(debug) << "Path " << search_dir << " is not a valid directory.";
        throw YDKInvalidArgumentException{"path is not a valid directory"};
    }

    ly_verb(LY_LLDBG);
}


namespace ydk {
    namespace core {

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
                BOOST_LOG_TRIVIAL(debug) << "Cannot sink to file " << filename;
            }
        }

        extern "C" char* get_module_callback(const char* name, const char* revision, void* user_data,
                                       LYS_INFORMAT* format, void (**free_module_data)(void *model_data))
        {
            BOOST_LOG_TRIVIAL(trace) << "Getting module " << name;

            if(user_data != nullptr){
                ModelProvider::Format m_format = ModelProvider::Format::YANG;
                *format = LYS_IN_YANG;
                auto pair = reinterpret_cast<std::pair<struct ly_ctx* , const ydk::core::Repository*>*>(user_data);
                struct ly_ctx* ctx = pair->first;
                const Repository* repo = pair->second;

                //check if the module is avaliable
                const struct lys_module* module = ly_ctx_get_module(ctx, name, revision);
                if(module != nullptr){
                    char* strp = nullptr;
                    if(!lys_print_mem(&strp, module, LYS_OUT_YANG, nullptr)){
                        *free_module_data = nullptr;
                        return strp;
                    }

                }

                //first check our directory for a file of the form <module-name>@<revision-date>.yang

                std::string yang_file_path_str{repo->path.string()};
                std::string yang_file_path_no_revision_str{repo->path.string()};
                yang_file_path_str += '/';
                yang_file_path_str += name;
                yang_file_path_no_revision_str += yang_file_path_str;
                if(revision){
                    yang_file_path_str += "@";
                    yang_file_path_str += revision;
                }
                yang_file_path_str += ".yang";
                BOOST_LOG_TRIVIAL(debug) << "Opening file " << yang_file_path_str;

                fs::path yang_file_path{yang_file_path_str};
                fs::path yang_file_path_no_revision{yang_file_path_no_revision_str};
                if(fs::is_regular_file(yang_file_path) || fs::is_regular_file(yang_file_path_no_revision)) {
                    //open the file read the data and return it
                    std::string model_data {""};
                    std::ifstream yang_file {yang_file_path.string()};
                    if(yang_file.is_open()) {
                        std::string line;
                        while(std::getline(yang_file, line)){
                            model_data+=line;
                            model_data+='\n';
                        }

                        yang_file.close();

                        *free_module_data = c_free_data;
                        char *enlarged_data = nullptr;
                        /* enlarge data by 2 bytes for flex */
                        auto data = model_data.c_str();
                        auto len = std::strlen(data);
                        enlarged_data = static_cast<char*>(std::malloc((len + 2) * sizeof *enlarged_data));
                        if (!enlarged_data) {
                            throw std::bad_alloc{};
                        }
                        memcpy(enlarged_data, data, len);
                        enlarged_data[len] = enlarged_data[len + 1] = '\0';
                        return enlarged_data;
                    } else {
                        BOOST_LOG_TRIVIAL(debug) << "Cannot open file " << yang_file_path_str;
                        throw YDKIllegalStateException("Cannot open file");
                    }

                }


                for(auto model_provider : repo->get_model_providers()) {
                    std::string model_data = model_provider->get_model(name, revision != nullptr ? revision : "", m_format);
                    if(!model_data.empty()){

                        sink_to_file(yang_file_path_str, model_data);
                        *free_module_data = c_free_data;
                        char *enlarged_data = nullptr;
                        /* enlarge data by 2 bytes for flex */
                        auto data = model_data.c_str();
                        auto len = std::strlen(data);
                        enlarged_data = static_cast<char*>(std::malloc((len + 2) * sizeof *enlarged_data));
                        if (!enlarged_data) {
                            throw std::bad_alloc{};
                        }
                        memcpy(enlarged_data, data, len);
                        enlarged_data[len] = enlarged_data[len + 1] = '\0';
                        return enlarged_data;
                    } else {
                        BOOST_LOG_TRIVIAL(error) << "Cannot find model with name:- " << name << " revision:-" << (revision !=nullptr ? revision : "");
//                        throw YDKIllegalStateException{"Cannot find model"};
                        return "";
                    }
                }
            }
//            BOOST_LOG_TRIVIAL(error) << "Cannot find model with name:- " << name;
//            throw YDKIllegalStateException{"Cannot find model"};
            return "";
        }
    }

}

ydk::core::RootSchemaNode*
ydk::core::Repository::create_root_schema(const std::vector<core::Capability> & capabilities) const
{
    std::string path_str = path.string();
    BOOST_LOG_TRIVIAL(trace) << "creating libyang context in path "<<path_str;
    struct ly_ctx* ctx = ly_ctx_new(path_str.c_str());

    if(!ctx) {
        throw std::bad_alloc{};
    }
    auto pair = new std::pair<struct ly_ctx* , const ydk::core::Repository*>{};
    pair->first = ctx;
    pair->second = this;

    //set module callback (only if there is a model provider)
    if(!model_providers.empty())
    {
        ly_ctx_set_module_clb(ctx, get_module_callback, pair);
    }

    for (auto c : capabilities)
    {
        if(c.module == "ietf-yang-library")
            continue;
        BOOST_LOG_TRIVIAL(trace) << "Module " << c.module.c_str() << " Revision " << c.revision.c_str();
        auto p = ly_ctx_get_module(ctx, c.module.c_str(), c.revision.empty() ? 0 : c.revision.c_str());

        if(!p) {
            p = ly_ctx_load_module(ctx, c.module.c_str(), c.revision.empty() ? 0 : c.revision.c_str());
        } else {
            BOOST_LOG_TRIVIAL(trace) << "Cache hit module name:-" << c.module;
        }

        if (!p) {
            BOOST_LOG_TRIVIAL(debug) << "Unable to parse module " << c.module;
            continue;
        }
        for (auto f : c.features)
            lys_features_enable(p, f.c_str());

    }

    RootSchemaNodeImpl* rs = new RootSchemaNodeImpl{ctx};
    return rs;
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
ydk::core::Repository::add_model_provider(ydk::core::ModelProvider* model_provider)
{
    model_providers.push_back(model_provider);
}

///
/// @brief Removes a model provider.
///
/// Removes the given model provider from this Repository.
///
void
ydk::core::Repository::remove_model_provider(ydk::core::ModelProvider* model_provider)
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
std::vector<ydk::core::ModelProvider*> ydk::core::Repository::get_model_providers() const
{
    return model_providers;
}


////////////////////////////////////////////////////////////////////////

