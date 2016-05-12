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
#ifndef _SERVICES_H_
#define _SERVICES_H_

#include <memory>

namespace ydk {

class Service {

};

class ServiceProvider;
class Entity;

class CRUDService : public Service {
	public:
		CRUDService();

		bool create(ServiceProvider & provider, Entity & entity);
		std::unique_ptr<Entity> read(ServiceProvider & provider, Entity & entity);
		bool update(ServiceProvider & provider, Entity & entity);
		bool del(ServiceProvider & provider, Entity & entity);
};

}

#endif /*_SERVICES_H_*/
