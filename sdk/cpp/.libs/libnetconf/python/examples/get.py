#!/usr/bin/python3
# -*- coding:utf-8 -*-

from optparse import OptionParser
import netconf

# parse command line arguments
parser = OptionParser(usage="Usage: %prog [options] host",
					description="Example program executing NETCONF <get> operation on specified NETCONF server.")
parser.add_option("-p", action="store", type="int", dest="port", default=830,
				help="Port to connect to on the NETCONF server host [default: %default].")
parser.add_option("-l", action="store", type="string", dest="username",
				help="The user to log in as on NETCONF server.")
parser.add_option("-f", action="store", type="string", dest="filter",
				help="NETCONF Subtree filter.")
(options, args) = parser.parse_args()
if len(args) == 0:
	parser.error("Missing \'host\' parameter.")
elif len(args) > 1:
	parser.error("Unknown parameters.")

# connect to the host
session = netconf.Session.connect(args[0], options.port, options.username)

# perform <get> and print result
print(session.get(options.filter))
