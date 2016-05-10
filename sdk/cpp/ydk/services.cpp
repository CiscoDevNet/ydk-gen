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

#include "ydk/services.h"
#include "ydk/providers.h"

using namespace std;

namespace ydk {
CRUDService::CRUDService()
{

}

bool CRUDService::create(ServiceProvider & provider, Entity & entity)
{
	string payload = provider.encode(entity);
	return provider.execute_payload(payload);
}

std::unique_ptr<Entity> CRUDService::read(ServiceProvider & provider, Entity & entity)
{
	string payload = provider.encode(entity);
	provider.execute_payload(payload);
	string reply="";
	return provider.decode(reply);
}

bool CRUDService::update(ServiceProvider & provider, Entity & entity)
{
	string payload = provider.encode(entity);
	return provider.execute_payload(payload);
}

bool CRUDService::del(ServiceProvider & provider, Entity & entity)
{
	string payload = provider.encode(entity);
	return provider.execute_payload(payload);
}
}
