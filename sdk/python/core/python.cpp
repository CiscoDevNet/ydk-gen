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


#include <boost/python.hpp>

#include "ydk/path_api.hpp"
#include "ydk/netconf_provider.hpp"

using namespace boost::python;
using namespace std;

typedef vector<string> StringVec;

namespace ydk
{
namespace path
{
class ServiceProviderWrap : public ServiceProvider, public wrapper<ServiceProvider>
{
public:
	RootSchemaNode* get_root_schema() const
	{
		return this->get_override("get_root_schema")();
	}
	DataNode* invoke(Rpc* rpc) const
	{
		return this->get_override("invoke")();
	}
};
}

}
BOOST_PYTHON_MODULE(path)
{
	class_<ydk::path::Capability>("Capability", init<const string &, const string &>());
	class_<ydk::path::Annotation>("Annotation", init<const string &, const string &, const string &>());
	//class_<ydk::path::DataNode>("DataNode");
	//class_<ydk::path::RootSchemaNode>("RootSchemaNode");
//	class_<ydk::path::ServiceProviderWrap, boost::noncopyable>("ServiceProvider")
//			.def("get_root_schema", pure_virtual(&ydk::path::ServiceProvider::get_root_schema))
//			.def("invoke", pure_virtual(&ydk::path::ServiceProvider::invoke));

};
