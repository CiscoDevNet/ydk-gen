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
	cerr << "\nUsage:\n\t"<< name << " [http|ssh]://user:password@host:port [-v]" <<endl<<endl;
}

vector<string> parse_args(int argc, char* argv[])
{
	if (argc < 2)
	{
		show_usage(argv[0]);
		return {};
	}
	string arg = argv[1];
	if ((arg == "-h") || (arg == "--help"))
	{
		show_usage(argv[0]);
		return {};
	}
	vector<string> ret;
	size_t s = arg.find("ssh://") + sizeof("ssh://")-1;
	if(s==string::npos)
	{
		s = arg.find("http://") + sizeof("http://")-1;
	}
	size_t col1 = arg.find(":",s);
	size_t amp = arg.find("@")-1;
	size_t col2 = arg.find(":",amp);
	ret.push_back(arg.substr(s,col1-s));
	ret.push_back(arg.substr(col1+1, amp-col1));
	ret.push_back(arg.substr(amp+2, col2-amp-2));
	ret.push_back(arg.substr(col2+1));

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
	return ret;
}
