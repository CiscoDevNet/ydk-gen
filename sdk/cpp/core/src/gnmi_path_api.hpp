//
// @file path_api.hpp
// @brief The main ydk public header.
//
// YANG Development Kit
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

#ifndef YDK_GNMI_CORE_HPP
#define YDK_GNMI_CORE_HPP

#include "path_api.hpp"

namespace grpc {
    class ChannelCredentials;
    class ChannelArguments;
}

namespace ydk {

class gNMIClient;

namespace path {

class gNMISession : public Session {
public:
    typedef struct SecureChannelArguments
    {
        std::shared_ptr<grpc::ChannelCredentials> channel_creds;
        std::shared_ptr<grpc::ChannelArguments> args;
    } SecureChannelArguments;

    gNMISession(Repository & repo,
                   const std::string& address,
                   const std::string& username,
                   const std::string& password,
                   int port = 57400);

    gNMISession(const std::string& address,
                   const std::string& username,
                   const std::string& password,
                   int port = 57400);

    gNMISession(Repository & repo,
                   const std::string& address,
                   int port = 57400);

    ~gNMISession();

    RootSchemaNode& get_root_schema() const;
    std::shared_ptr<DataNode> invoke(Rpc& rpc) const;
    std::shared_ptr<DataNode> invoke(DataNode& rpc) const;
    void invoke(Rpc& rpc, std::function<void(const std::vector<std::string> &)> func) const;

    std::vector<std::string> get_capabilities() const;
    EncodingFormat get_encoding() const;

    std::shared_ptr<path::DataNode> handle_get_capabilities() const;

    std::shared_ptr<path::DataNode> handle_get_reply(std::vector<std::string> reply_val) const;
    gNMIClient & get_client() const;

private:
    void initialize(Repository& repo, const std::string& address, const std::string& username, const std::string& password, int port);
    bool handle_set(path::Rpc& ydk_rpc) const;
    std::shared_ptr<path::DataNode> handle_get(path::Rpc& ydk_rpc) const;

    void print_root_paths(ydk::path::RootSchemaNode& rsn) const;
    void print_paths(ydk::path::SchemaNode& sn) const;

private:
    std::unique_ptr<gNMIClient> client;
    std::shared_ptr<RootSchemaNode> root_schema;
    std::vector<std::string> server_capabilities;
    bool is_secure;
};

}	// namespace path

}	// namespace ydk

#endif /* YDK_GNMI_CORE_HPP */
