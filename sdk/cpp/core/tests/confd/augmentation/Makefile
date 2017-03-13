
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

CONFD_FLAGS = --addloadpath $(CONFD_DIR)/etc/confd --addloadpath $(CONFD_DIR)/src/confd/yang
START_FLAGS ?=


all:  ydktest-sanity-augm.fxs ydktest-sanity-types.fxs ydktest-sanity.fxs\
	ydktest-types.fxs ietf-aug-base-1.fxs ietf-aug-base-2.fxs ydktest-aug-ietf-1.fxs ydktest-aug-ietf-2.fxs ydktest-aug-ietf-4.fxs \
		$(CDB_DIR) ssh-keydir
	@echo "Build complete"

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

ydktest-sanity-augm.yang: ydktest-sanity.yang
	cp ../../models/ydktest-sanity-augm\@2015-11-17.yang ydktest-sanity-augm.yang

ydktest-sanity-submodule.yang:
	cp ../../models/ydktest-sanity-submodule\@2016-04-25.yang ydktest-sanity-submodule.yang

ydktest-sanity-types.yang:
	cp ../../models/ydktest-sanity-types\@2016-04-11.yang ydktest-sanity-types.yang

ydktest-sanity.yang: ydktest-types.yang ydktest-sanity-submodule.yang
	cp ../../models/ydktest-sanity\@2015-11-17.yang ydktest-sanity.yang

ydktest-types.yang:
	cp ../../models/ydktest-types.yang ydktest-types.yang



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
