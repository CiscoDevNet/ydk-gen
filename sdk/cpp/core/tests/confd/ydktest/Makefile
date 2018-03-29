
usage:
	@echo "See README files for more instructions"
	@echo "make all     Build all example files"
	@echo "make clean   Remove all built and intermediary files"
	@echo "make start   Start CONFD daemon and example agent"
	@echo "make stop    Stop any CONFD daemon and example agent"

######################################################################
# Where is ConfD installed? Make sure CONFD_DIR points it out
CONFD_DIR ?= ../../../..

# Include standard ConfD build definitions and rules
include $(CONFD_DIR)/src/confd/build/include.mk

# In case CONFD_DIR is not set (correctly), this rule will trigger
$(CONFD_DIR)/src/confd/build/include.mk:
	@echo 'Where is ConfD installed? Set $$CONFD_DIR to point it out!'
	@echo ''

######################################################################
# Example specific definitions and rules

CONFD_FLAGS = --addloadpath $(CONFD_DIR)/etc/confd --addloadpath $(CONFD_DIR)/src/confd/yang --addloadpath $(CONFD_DIR)/src/confd/aaa
START_FLAGS ?=


all:  openconfig-platform.deviation\
 	ydktest-filterread.fxs ydktest-sanity-augm.fxs ydktest-sanity-types.fxs ydktest-sanity.fxs\
	ydktest-types.fxs openconfig-bgp-types.fxs openconfig-bgp-policy.fxs openconfig-extensions.fxs openconfig-policy-types.fxs\
	ietf-interfaces.fxs openconfig-types.fxs openconfig-routing-policy.fxs openconfig-platform.fxs openconfig-interfaces.fxs openconfig-bgp.fxs\
	openconfig-platform-types.fxs openconfig-platform-transceiver.fxs oc-pattern.fxs main.fxs main-aug1.fxs iana-if-type.fxs\
	openconfig-transport-types.fxs ietf-aug-base-1.fxs ietf-aug-base-2.fxs openconfig-terminal-device.fxs  openconfig-if-ethernet.fxs\
	ydktest-aug-ietf-1.fxs ydktest-aug-ietf-2.fxs ydktest-aug-ietf-4.fxs ydktest-aug-ietf-5.fxs ydktest-sanity-action.fxs\
		$(CDB_DIR) ssh-keydir
	@echo "Copying YDK customized aaa_init.xml to $(CDB_DIR)"
	cp ydk_aaa_init.xml $(CDB_DIR)/aaa_init.xml
	@echo "Build complete"

openconfig-platform.deviation : cisco-xr-openconfig-platform-deviations.yang \
 		openconfig-platform.yang openconfig-platform-transceiver.yang openconfig-transport-types.yang openconfig-platform-types.yang\
                openconfig-extensions.yang openconfig-interfaces.yang openconfig-terminal-device.yang openconfig-if-ethernet.yang
	$(CONFDC) $(FXS_WERR) $(EXTRA_LINK_FLAGS) -o $(basename $@).fxs -c $(basename $@).yang \
		--deviation $(word 1, $^)

iana-if-type.yang:
	cp ../../../../../../yang/ietf/iana-if-type.yang iana-if-type.yang

ietf-interfaces.yang:
	cp ../../models/ietf-interfaces\@2014-05-08.yang ietf-interfaces.yang

openconfig-bgp-common-multiprotocol.yang:
	cp ../../models/openconfig-bgp-common-multiprotocol.yang openconfig-bgp-common-multiprotocol.yang

openconfig-bgp-common-structure.yang:
	cp ../../models/openconfig-bgp-common-structure.yang openconfig-bgp-common-structure.yang

openconfig-bgp-common.yang:
	cp ../../models/openconfig-bgp-common.yang openconfig-bgp-common.yang

openconfig-bgp-global.yang:
	cp ../../models/openconfig-bgp-global.yang openconfig-bgp-global.yang

openconfig-bgp-neighbor.yang:
	cp ../../models/openconfig-bgp-neighbor.yang openconfig-bgp-neighbor.yang

openconfig-bgp-peer-group.yang:
	cp ../../models/openconfig-bgp-peer-group.yang openconfig-bgp-peer-group.yang

openconfig-bgp-policy.yang: openconfig-routing-policy.yang openconfig-policy-types.yang
	cp ../../models/openconfig-bgp-policy.yang openconfig-bgp-policy.yang

openconfig-bgp-types.yang: openconfig-types.yang openconfig-extensions.yang
	cp ../../models/openconfig-bgp-types.yang openconfig-bgp-types.yang

cisco-xr-openconfig-platform-deviations.yang:
	cp ../../models/cisco-xr-openconfig-platform-deviations.yang cisco-xr-openconfig-platform-deviations.yang

openconfig-bgp.yang: openconfig-bgp-common.yang openconfig-bgp-common-multiprotocol.yang \
				openconfig-bgp-common-structure.yang openconfig-bgp-peer-group.yang \
			openconfig-bgp-neighbor.yang openconfig-bgp-global.yang
	cp ../../models/openconfig-bgp.yang openconfig-bgp.yang

openconfig-extensions.yang:
	cp ../../models/openconfig-extensions.yang openconfig-extensions.yang

openconfig-interfaces.yang: ietf-interfaces.yang
	cp ../../models/openconfig-interfaces.yang openconfig-interfaces.yang

openconfig-routing-policy.yang: openconfig-interfaces.yang
	cp ../../models/openconfig-routing-policy.yang openconfig-routing-policy.yang

openconfig-types.yang:
	cp ../../models/openconfig-types.yang openconfig-types.yang

openconfig-policy-types.yang:
	cp ../../models/openconfig-policy-types.yang openconfig-policy-types.yang

openconfig-platform.yang: openconfig-platform-types.yang openconfig-platform-transceiver.yang \
				openconfig-transport-types.yang
	cp ../../models/openconfig-platform.yang openconfig-platform.yang

openconfig-terminal-device.yang: openconfig-platform.yang openconfig-if-ethernet.yang
	cp ../../models/openconfig-terminal-device.yang openconfig-terminal-device.yang

openconfig-if-ethernet.yang:
	cp ../../models/openconfig-if-ethernet.yang openconfig-if-ethernet.yang

openconfig-platform-types.yang:
	cp ../../models/openconfig-platform-types.yang openconfig-platform-types.yang

openconfig-platform-transceiver.yang:
	cp ../../models/openconfig-platform-transceiver.yang openconfig-platform-transceiver.yang

openconfig-transport-types.yang:
	cp ../../models/openconfig-transport-types.yang openconfig-transport-types.yang

ydktest-filterread.yang:
	cp ../../models/ydktest-filterread\@2015-11-17.yang ydktest-filterread.yang

ydktest-sanity-augm.yang: ydktest-sanity.yang
	cp ../../models/ydktest-sanity-augm\@2015-11-17.yang ydktest-sanity-augm.yang

ydktest-sanity-submodule.yang:
	cp ../../models/ydktest-sanity-submodule\@2016-04-25.yang ydktest-sanity-submodule.yang

ydktest-sanity-types.yang:
	cp ../../models/ydktest-sanity-types\@2016-04-11.yang ydktest-sanity-types.yang

ydktest-sanity.yang: ydktest-types.yang ydktest-sanity-submodule.yang
	cp ../../models/ydktest-sanity\@2015-11-17.yang ydktest-sanity.yang

ydktest-types.yang:
	cp ../../models/ydktest-types@2016-05-23.yang ydktest-types.yang

ydktest-sanity-action.yang:
	cp ../../models/ydktest-sanity-action.yang ydktest-sanity-action.yang

main.yang:
	cp ../../models/main@2015-11-17.yang main.yang

main-aug1.yang:
	cp ../../models/main-aug1@2015-11-17.yang main-aug1.yang

ietf-aug-base-1.yang:
	cp ../../../../../../yang/ydktest-augmentation/ietf-aug-base-1\@2016-07-01.yang ietf-aug-base-1.yang

ietf-aug-base-2.yang:
	cp ../../../../../../yang/ydktest-augmentation/ietf-aug-base-2\@2016-07-01.yang ietf-aug-base-2.yang

oc-pattern.yang:
	cp ../../models/oc-pattern\@2015-11-17.yang oc-pattern.yang

ydktest-aug-ietf-1.yang:
	cp ../../../../../../yang/ydktest-augmentation/ydktest-aug-ietf-1\@2016-06-17.yang ydktest-aug-ietf-1.yang

ydktest-aug-ietf-2.yang:
	cp ../../../../../../yang/ydktest-augmentation/ydktest-aug-ietf-2\@2016-06-22.yang ydktest-aug-ietf-2.yang

ydktest-aug-ietf-4.yang:
	cp ../../../../../../yang/ydktest-augmentation/ydktest-aug-ietf-4\@2016-06-27.yang ydktest-aug-ietf-4.yang

ydktest-aug-ietf-5.yang:
	cp ../../../../../../yang/ydktest-augmentation/ydktest-aug-ietf-5\@2017-07-26.yang ydktest-aug-ietf-5.yang


######################################################################
clean:	iclean
	-rm -rf *_ns.py *.pyc *.yang 2> /dev/null || true

######################################################################
start:  stop start_confd

start_confd:
	$(CONFD) -c confd.conf $(CONFD_FLAGS)


######################################################################



######################################################################
stop:
	### Killing any confd daemon or DHCP confd agents
	$(CONFD) --stop &> /dev/null   || true

######################################################################
