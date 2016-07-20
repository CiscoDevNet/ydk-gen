#!/usr/bin/python3
# -*- coding:utf-8 -*-

import netconf

# accept NETCONF connection
session = netconf.Session.accept()

# process incoming requests until the connection is closed
while session.isActive() == True:
	session.processRequest()
