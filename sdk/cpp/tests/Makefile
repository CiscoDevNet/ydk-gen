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

TEST_SOURCE    = $(wildcard *.cpp)
TEST_BINARY    = ydk_test
LIBYDK_AR      = ../libydk_client.a

CXX             = g++
CXXFLAGS        = -Wall -std=c++0x -I./../
LXXFLAGS        = -lUnitTest++ -I./../.includes -L./../ -lydk_client -L./../.libs -lnetconf -lboost_python -lpython2.7 -lssh -lxml2 -lcurl -lssh_threads -lxslt -lpthread -ldl

default: all

ydk_test : $(TEST_SOURCE) Makefile
	$(CXX) $(CXXFLAGS) $(TEST_SOURCE) -o $(TEST_BINARY) $(LXXFLAGS)
	@touch $(TEST_BINARY)

all : $(LIBYDK_AR) $(TEST_BINARY)
	./$(TEST_BINARY)

$(LIBYDK_AR) : ../Makefile
	make -C .. netconf_client 

clean :
	rm -f $(TEST_BINARY) $(LIBYDK_AR)
