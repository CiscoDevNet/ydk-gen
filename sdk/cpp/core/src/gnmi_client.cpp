#include "gnmi_client.hpp"

using namespace std;
using namespace ydk;

using json = nlohmann::json;
using grpc::SslCredentialsOptions;

vector<string> capabilities;
gNMIClient::PathPrefixValueFlags flag;

gNMIClient::gNMIClient(shared_ptr<Channel> channel)
    : stub_(gNMI::NewStub(channel)){}

static bool check_capabilities_status(Status status);
static string parse_get_request_payload(string payload);
static vector<string> get_path_from_payload_filter(string payload_filter, vector<string> container);
static string check_if_path_has_value(string element);
static string format_notification_response(string prefix_to_prepend, string path_to_prepend, string value);


int gNMIClient::connect(std::string address) 
{
    // Authenticates server 
    std::string server_cert;
    std::ifstream rf("ems.pem");

    server_cert.assign((std::istreambuf_iterator<char>(rf)),(std::istreambuf_iterator<char>()));

    grpc::SslCredentialsOptions ssl_opts;
    grpc::ChannelArguments      args;

    ssl_opts.pem_root_certs = server_cert;
    args.SetSslTargetNameOverride("ems.cisco.com");

    /* ToDo Authenticate client at server
    std::ifstream kf("client.key");
    std::ifstream cf("client.pem");
    client_key.assign((std::istreambuf_iterator<char>(kf)),(std::istreambuf_iterator<char>()));
    client_cert.assign((std::istreambuf_iterator<char>(cf)),(std::istreambuf_iterator<char>()));
    ssl_opts = {server_cert, client_key, client_cert};
    */

    auto channel_creds = grpc::SslCredentials(grpc::SslCredentialsOptions(ssl_opts));
    grpc::CreateCustomChannel(address, channel_creds, args);
    get_capabilities();
    return EXIT_SUCCESS;
}

static bool check_capabilities_status(Status status) 
{
    if (!(status.ok())) 
    {
      YLOG_ERROR("Capabilities RPC Failed.");
      throw(YCPPError{"Capabilities RPC Failed"});
      return false;
    }
    return true;
}

bool gNMIClient::has_gnmi_version(::gnmi::CapabilityResponse* response) 
{
    if (!(response->gnmi_version().size() > 0)) 
    {
        YLOG_ERROR("Capabilities Received Without gNMI Version");
        throw(YCPPError{"Capabilities Received Without gNMI Version"});
        return false;
    }
    return true; 
}

void gNMIClient::parse_capabilities_modeldata(::gnmi::CapabilityResponse* response)
{
    ::gnmi::ModelData modeldata;
    string cap, name, organization, version; 

    for (int i = 0, n = response->supported_models_size(); i < n; i++) 
    {
        cap.clear();
        modeldata = response->supported_models(i);
        name = modeldata.name();
        organization = modeldata.organization();
        version = modeldata.version();
        if(!(modeldata.name().empty()))
        {
            YLOG_DEBUG("Name: {}", name.c_str());
            cap.append("?module=" + name);
        }
        if(!(modeldata.organization().empty())) 
            YLOG_DEBUG("Organization: {}", organization.c_str());
        if(!(modeldata.version().empty()))
        {
            YLOG_DEBUG("Version: {}", version.c_str());
            cap.append("&revision=" + version);
        }
        capabilities.push_back(cap);
        YLOG_DEBUG("              ------------       ");
        YLOG_DEBUG("");
    }   
}

void gNMIClient::parse_capabilities_encodings(::gnmi::CapabilityResponse* response)
{
    ::gnmi::Encoding encoding;
    string encoding_value;

    for (int i = 0, n = response->supported_encodings_size(); i < n; i++) 
    {
        encoding = response->supported_encodings(i);
        encoding_value = ::gnmi::Encoding_Name(encoding);
        YLOG_DEBUG("Encoding {}", encoding_value.c_str());
    }
}

bool gNMIClient::parse_capabilities(::gnmi::CapabilityResponse* response)
{
    YLOG_DEBUG("Capabilities Received:");
    YLOG_DEBUG("");
    YLOG_DEBUG("==============gNMI Version==============");
    YLOG_DEBUG("gNMI Version: {}", response->gnmi_version().c_str());

    YLOG_DEBUG("============Supported Models============");    
    parse_capabilities_modeldata(response);
    
    YLOG_DEBUG("===========Supported Encodings===========");
    parse_capabilities_encodings(response);
    
    YLOG_DEBUG("=========================================");
    YLOG_DEBUG("");
} 

vector<string> gNMIClient::get_capabilities() 
{
    CapabilityRequest request;
    CapabilityResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub_->Capabilities(&context, request, &response);

    check_capabilities_status(status);
    
    if(has_gnmi_version(&response)) 
    {
        parse_capabilities(&response);
    }
    return capabilities;
}

static vector<string> parse_get_request_payload(string payload, vector<string> path_container) 
{
    auto payload_to_parse = json::parse("{" + payload + "}");
    string payload_filter = payload_to_parse.value("/rpc/ietf-netconf:get-config/filter"_json_pointer, "Empty Filter");
    YLOG_DEBUG("payload_filter: {}", payload_filter);

    std::string path_elem;
    istringstream payload_filter_value(payload_filter);
    while(getline(payload_filter_value, path_elem, '"')) 
    {
        if(path_elem.find("{")==string::npos && path_elem.find("}")==string::npos) 
            path_container.push_back(path_elem);
    }
    return path_container;
}


::gnmi::GetRequest gNMIClient::populate_get_request(::gnmi::GetRequest request, string payload) 
{   
    vector<string> container;   
    vector<string> path_container = parse_get_request_payload(payload, container);
    
    ::gnmi::Path* path = request.add_path();  
    for(int i = 0; i < path_container.size(); ++i) 
        path->add_element(path_container[i]);
    path->add_element("interface[name=\"Loopback0\"]");
    request.set_type(::gnmi::GetRequest::CONFIG);
    request.set_encoding(::gnmi::Encoding::JSON_IETF);
    return request;
}

string gNMIClient::get_prefix_from_notification(::gnmi::Notification notification)
{
    ::gnmi::Path prefix = notification.prefix();
    string element;
    string prefix_to_prepend;

    int element_size = prefix.element_size();
    for(int j = 0; j < element_size; ++j)
    {
        element.append("\"" + prefix.element(j) + "\" ");
        prefix_to_prepend.append(prefix.element(j));
        prefix_to_prepend.append(":");
    }
    return prefix_to_prepend;
}

static string check_if_path_has_value(string element)
{
    if(element.find("[")==string::npos)
    {
        flag.path_has_value = false;
        return element;
    } else { 
        flag.path_has_value = true;
        return element.substr(0,element.find("["));
    }
}

string gNMIClient::get_path_from_update(::gnmi::Update update)
{
    string path_to_prepend;
    string path_element_to_add;

    if(update.has_path()) 
    {
        ::gnmi::Path path = update.path();
        int element_size = path.element_size();

        for(int l = 0; l < element_size - 1; ++l) 
        {
            path_element_to_add = check_if_path_has_value(path.element(l));
            path_to_prepend.append("\"" + path_element_to_add + "\":{");
        }
        path_element_to_add = check_if_path_has_value(path.element(element_size - 1));
        path_to_prepend.append("\"" + path_element_to_add + "\":");
    } else 
        path_to_prepend.clear();
    return path_to_prepend;
}

string gNMIClient::get_value_from_update(::gnmi::Update update)
{
    ::gnmi::TypedValue value;
    string value_for_payload;

    if(update.has_val()) 
        value = update.val();
    value_for_payload.append(value.json_ietf_val());
    return value_for_payload;
}

static string format_notification_response(string prefix_to_prepend, string path_to_prepend, string value)
{
    string reply_to_parse;

    // Update again when payload format from the server(IOS XR) is made consistent with different request paths
    if (flag.path_has_value)
        reply_to_parse.append("\"data\":{" + prefix_to_prepend + path_to_prepend + "[" + value + "]" + "}}");        
    else if (flag.prefix_has_value)
        reply_to_parse.append("\"data\":{" + prefix_to_prepend + ":[" + path_to_prepend + value + "]" + "}}");
    else if ((flag.prefix_has_value) && (flag.path_has_value))
        reply_to_parse.append("\"data\":{" + prefix_to_prepend + ":[" + path_to_prepend + "[" + value + "]]" + "}}");
    else 
        reply_to_parse.append("\"data\":{" + prefix_to_prepend + path_to_prepend + "{\"" + value + "" + "}}");

    return reply_to_parse;
}

string gNMIClient::parse_get_response(::gnmi::GetResponse* response) 
{
    ::gnmi::Notification notification;
    ::gnmi::Update update;
    string value;
    string reply_to_parse;
    string prefix_to_prepend;
    string path_to_prepend;

    int notification_size = response->notification_size();

    for(int i = 0; i < notification_size; ++i) 
    {
        notification = response->notification(i);

        if(notification.has_prefix()) prefix_to_prepend = get_prefix_from_notification(notification);
        else prefix_to_prepend.clear();
        
        if(notification.update_size() != 0) {
            for(int k = 0; k < notification.update_size(); ++k) 
            {
                update = notification.update(k);               
                path_to_prepend = get_path_from_update(update);
                value.append(get_value_from_update(update));
                reply_to_parse = format_notification_response(prefix_to_prepend, path_to_prepend, value);
            }   
        }
    }
    return reply_to_parse;
}

string gNMIClient::execute_wrapper(const string & payload)
{
    ::gnmi::GetRequest request;
    ::gnmi::GetResponse response;

    request = populate_get_request(request, payload);
    YLOG_INFO("\n===============Get Request Sent================\n{}\n", request.DebugString().c_str());
    string reply = execute_payload(request, &response);
    return reply;
}

string gNMIClient::execute_payload(const GetRequest& request, GetResponse* response)
{
    grpc::ClientContext context;
    grpc::Status status;

    status = stub_->Get(&context, request, response);
    YLOG_INFO("\n=============Get Response Received=============\n{}\n", response->DebugString().c_str());
    if (!(status.ok())) 
    {
        YLOG_ERROR("Get RPC Status not OK");
        throw(YCPPError{status.error_message()});
    } 
    else 
    {
        YLOG_INFO("Get RPC Passed");
        string reply = parse_get_response(response); 
        return reply;
    }
}

gNMIClient::~gNMIClient() {}
