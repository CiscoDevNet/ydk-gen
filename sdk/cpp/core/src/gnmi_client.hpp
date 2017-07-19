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

#ifndef _YDK_GNMI_CLIENT_H_
#define _YDK_GNMI_CLIENT_H_
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "errors.hpp"
#include "json.hpp"
#include "logger.hpp"

#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/grpc++.h>
#include <libgnmi/gnmi.grpc.pb.h>

using gnmi::CapabilityRequest;
using gnmi::CapabilityResponse;
using grpc::Channel;
using grpc::ClientContext;
using gnmi::GetRequest;
using gnmi::GetResponse;
using gnmi::gNMI;
using grpc::Status;

namespace ydk 
{
    class gNMIClient 
    {
    public:
        typedef struct PathPrefixValueFlags
        {
            bool path_has_value;
            bool prefix_has_value;
        } PathPrefixValueFlags;

        gNMIClient(std::shared_ptr<Channel> channel);
        ~gNMIClient();

        int connect(std::string address);
        std::string execute_wrapper(const std::string & payload);
        std::string execute_payload(const GetRequest& request, GetResponse* response);
        std::vector<std::string> get_capabilities();
    
    private:
        std::string get_path_from_update(::gnmi::Update update);
        std::string get_prefix_from_notification(::gnmi::Notification notification);
        std::string get_value_from_update(::gnmi::Update update);
        bool has_gnmi_version(CapabilityResponse* response);
        void parse_capabilities_modeldata(CapabilityResponse* response);
        void parse_capabilities_encodings(CapabilityResponse* response);
        bool parse_capabilities(CapabilityResponse* response);
        std::string parse_get_response(GetResponse* response); 
        GetRequest populate_get_request(GetRequest request, std::string payload);
        std::unique_ptr<gNMI::Stub> stub_;
    };
}

#endif /* _YDK_GNMI_CLIENT_H_ */
