#!/usr/bin/env python
#
# Copyright 2016 Cisco Systems, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# hello-ydk.py
# Read all data for model Cisco-IOS-XR-shellutil-oper and print system
# uptime.
#

# import providers, services and models
import ydk_client

if __name__ == "__main__":
    """Main execution path"""

    # create ydk_client
    client = ydk_client.NetconfClient("admin", "admin", "127.0.0.1", 12022, 0)
    result = client.connect()
    print 'Created native ydk_client and tried to connect to router with result: ', 'SUCCESS' if result == 0 else 'ERROR'
    client.close()
    print 'Native ydk_client test completed successfully'
    exit()
# End of script

