#  ----------------------------------------------------------------
# Copyright 2016 Cisco Systems
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#------------------------------------------------------------------

SDK_DIR         = `pwd`
MODELS_DIR      = $(SDK_DIR)/ydk/models
BGP_MODEL       = $(MODELS_DIR)/bgp/bgp.cpp
PROVIDERS       = $(SDK_DIR)/ydk/providers.cpp
SERVICES        = $(SDK_DIR)/ydk/services.cpp

MODELS_DIRS     = ${shell find ${MODELS_DIR} -type d -print}
MODELS_LIST     = $(foreach dir,$(MODELS_DIRS),$(wildcard $(dir)/*.cpp))

CXX             = g++
CXXFLAGS        = -Wall -std=c++0x
LDFLAGS         = -shared  -fPIC
ARCHIVE         = ar
ARFLAGS         = -cvq
 
default: all

all : $(MODELS_LIST)
	@$(CXX) $(CXXFLAGS) $(LDFLAGS)  -I/$(SDK_DIR) -o ydk_cpp.so $^

bgp_sample : samples/bgp_sample.cpp
	@$(CXX) $(CXXFLAGS) -o bgp_sample $(PROVIDERS) $(SERVICES) $(BGP_MODEL) $<

netconf_client : ydk/netconf_client.cpp ydk/netconf_client.h Makefile
	@printf "Compiling %-40s\n\n" $<
	$(CXX) $(CXXFLAGS) -I/usr/include/python2.7 -c $<
	$(ARCHIVE) $(ARFLAGS) libydk_client.a *.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -I/usr/include/python2.7 -o ydk_client.so $<  -lnetconf  -lpython2.7 -lboost_python 

clean :
	@rm bgp_sample
