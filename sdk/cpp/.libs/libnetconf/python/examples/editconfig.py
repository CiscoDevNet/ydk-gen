#!/usr/bin/python3
# -*- coding:utf-8 -*-

from optparse import OptionParser
import netconf

# parse command line arguments
parser = OptionParser(usage="Usage: %prog [options] host",
					description="Example program executing NETCONF <edit-config> and <get-config> operations on specified NETCONF server.")
parser.add_option("-p", action="store", type="int", dest="port", default=830,
				help="Port to connect to on the NETCONF server host [default: %default].")
parser.add_option("-l", action="store", type="string", dest="username",
				help="The user to log in as on NETCONF server.")
parser.add_option("-c", action="store", type="string", dest="editconfig",
				help="XML for editconfig.")
(options, args) = parser.parse_args()
if len(args) == 0:
	parser.error("Missing \'host\' parameter.")
elif len(args) > 1:
	parser.error("Unknown parameters.")

if not options.editconfig:
	parser.error("Missing -c (XML for edit-config)")

# connect to the host
session = netconf.Session.connect(args[0], options.port, options.username)
session.editConfig(target=netconf.RUNNING, source=options.editconfig, defop=netconf.NC_EDIT_DEFOP_MERGE,
	erropt=netconf.NC_EDIT_ERROPT_NOTSET, testopt=netconf.NC_EDIT_TESTOPT_TESTSET)
print(session.getConfig(netconf.RUNNING))
