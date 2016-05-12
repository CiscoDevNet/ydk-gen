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

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "ydk/providers.h"

using namespace std;

namespace ydk {
NetconfServiceProvider::NetconfServiceProvider(std::initializer_list<std::string> args)
{
	if(args.size()==0)
	{
		cout<<"- address - The address of the netconf server"
            "- port  - The port to use default is 830"
            "- username - The name of the user"
            "- password - The password to use"
            "- protocol - one of either ssh or tcp"
            "- timeout  - Default to 60"<<endl;
		return;
	}

	std::vector<std::string> argsv;
	argsv.insert(argsv.end(), args.begin(), args.end());


	username = argsv[2];
	address = argsv[0];
	port = argsv[1];
	password = argsv[3];
	protocol = argsv[4];
	timeout = argsv[5];
}

std::string NetconfServiceProvider::encode(Entity & entity)
{
	return "";
}

std::unique_ptr<Entity> NetconfServiceProvider::decode(std::string & payload)
{
	return nullptr;
}

bool NetconfServiceProvider::execute_payload(std::string & payload)
{
	char connect_command[500];

	ofstream myfile;
	myfile.open ("payload.xml");
	myfile << payload;
	myfile.close();

	snprintf(connect_command,sizeof(connect_command),
			"ssh %s@%s -p %s -s netconf < payload.xml > debug.log",
			username.c_str(),
			address.c_str(),
			port.c_str()
			);

	int res=system(connect_command);
	cout << "Connected to device with command: "<< connect_command<<". Result: " << res << endl;
	cout << "Debug in debug.log"<<endl;
	return res==0;
}

}
