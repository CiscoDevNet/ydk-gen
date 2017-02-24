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

#include <iostream>

#include "args_parser.h"

using namespace std;
void show_usage(string name)
{
    cerr << "\nUsage:\n\t"<< name << " [http|ssh]://user:password@host[:port] [-v]" <<endl;
    cerr << "\t\tIf host is IPv6, enclose it in square brackets. E.g. [1234:1234:1234:1234::1234]"<<endl;
    cerr << "\t\tIf port is not specified, 830 will be used with ssh (netconf) or 80 with http (restconf)"<<endl<<endl;
}

vector<string> parse_args(int argc, char* argv[])
{
    vector<string> ret;
    string host, username, password, port;
    string arg;

    if (argc < 2)
    {
        goto fail;
    }
    if ((arg == "-h") || (arg == "--help"))
    {
        goto fail;
    }

    arg = argv[1];

    try
    {
        size_t s = arg.find("ssh://");
        if(s!=string::npos)
        {
            s += sizeof("ssh://")-1;
            port = "830";
        }
        else
        {
            s = arg.find("http://");
            if(s!=string::npos)
            {
                s += sizeof("http://")-1;
                port = "80";
            }
        }

        size_t col1 = arg.find(":",s);
        size_t amp = arg.find("@")-1;
        if(col1 == string::npos || amp == string::npos)
        {
            goto fail;
        }

        username = arg.substr(s,col1-s);
        password = arg.substr(col1+1, amp-col1);

        size_t open_bracket = arg.find("[");
        if(open_bracket != string::npos)
        {
            size_t close_bracket = arg.find("]");
            if(close_bracket!=string::npos)
            {
                host = arg.substr(amp+3, close_bracket-amp-3);
                size_t col2 = arg.find(":", close_bracket);
                if(col2 != string::npos)
                {
                    port = arg.substr(col2+1);
                }
            }
        }
        else
        {
            size_t col2 = arg.find(":",amp);
            if(col2 != string::npos)
            {
                host = arg.substr(amp+2, col2-amp-2);
                port = arg.substr(col2+1);
            }
            else
            {
                host = arg.substr(amp+2, arg.size()-amp-2);
            }
        }

        ret.push_back(username);
        ret.push_back(password);
        ret.push_back(host);
        ret.push_back(port);

        bool verb = false;
        if(argc == 3)
        {
            string v = argv[2];
            if(v=="-v")
                verb = true;
        }

        if(verb)
        {
            ret.push_back("--verbose");
        }
        else
        {
            ret.push_back("--silent");
        }
    }
    catch(...)
    {
        goto fail;
    }
    return ret;

    fail:
    {
        show_usage(argv[0]);
        return {};
    }
}

